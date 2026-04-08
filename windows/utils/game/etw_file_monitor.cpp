#include "etw_file_monitor.h"
#include <tdh.h>

static constexpr GUID FILE_PROVIDER_GUID = {0xedd08927, 0x9cc4, 0x4e65, {0xb9, 0x70, 0xc2, 0x56, 0x0f, 0xb5, 0xc2, 0x89}};
static constexpr wchar_t SESSION_NAME[] = L"FileMonitorSession";

EtwFileMonitor::EtwFileMonitor(QObject *parent) : QObject(parent) {}

EtwFileMonitor::~EtwFileMonitor()
{
    stopMonitoring(false);
}

bool EtwFileMonitor::startMonitoring(const ULONG processId)
{   // 启动监控
    stopMonitoring(false);
    constexpr DWORD propsSize = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(SESSION_NAME);
    std::vector<BYTE> buffer(propsSize);
    ZeroMemory(buffer.data(), buffer.size());
    const auto props = reinterpret_cast<EVENT_TRACE_PROPERTIES*>(buffer.data());
    props->Wnode.BufferSize = propsSize;
    props->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
    wcscpy_s(reinterpret_cast<WCHAR*>(buffer.data() + props->LoggerNameOffset), _countof(SESSION_NAME), SESSION_NAME);
    props->LogFileMode = EVENT_TRACE_REAL_TIME_MODE;
    StartTraceW(&m_sessionHandle, SESSION_NAME, props);
    EnableTraceEx2(m_sessionHandle, &FILE_PROVIDER_GUID, EVENT_CONTROL_CODE_ENABLE_PROVIDER, TRACE_LEVEL_INFORMATION, 0, 0, 0, nullptr);
    EVENT_TRACE_LOGFILEW logFile = {};
    logFile.LoggerName = const_cast<LPWSTR>(SESSION_NAME);
    logFile.ProcessTraceMode = PROCESS_TRACE_MODE_REAL_TIME | PROCESS_TRACE_MODE_EVENT_RECORD;
    logFile.EventRecordCallback = &EtwFileMonitor::eventCallback;
    logFile.Context = this;
    m_traceHandle = OpenTraceW(&logFile);
    m_targetPid = processId;
    m_workerThread = std::make_unique<std::thread>([this] {ProcessTrace(&m_traceHandle, 1, nullptr, nullptr);});
    return true;
}

void EtwFileMonitor::stopMonitoring(const bool shouldStop)
{   // 停止监控
    m_shouldStop = shouldStop;
    if (m_traceHandle != INVALID_PROCESSTRACE_HANDLE) {
        CloseTrace(m_traceHandle);
        m_traceHandle = INVALID_PROCESSTRACE_HANDLE;
    }
    if (m_workerThread && m_workerThread->joinable()) {
        m_workerThread->join();
        m_workerThread.reset();
    }
    if (m_sessionHandle != INVALID_PROCESSTRACE_HANDLE) m_sessionHandle = INVALID_PROCESSTRACE_HANDLE;
    constexpr DWORD propsSize = sizeof(EVENT_TRACE_PROPERTIES) + sizeof(SESSION_NAME);
    std::vector<BYTE> buffer(propsSize);
    const auto props = reinterpret_cast<EVENT_TRACE_PROPERTIES*>(buffer.data());
    props->Wnode.BufferSize = propsSize;
    props->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
    StopTraceW(0, SESSION_NAME, props);
    m_targetPid = 0;
}

void WINAPI EtwFileMonitor::eventCallback(PEVENT_RECORD eventRecord)
{   // ETW事件回调
    const auto *self = static_cast<EtwFileMonitor*>(eventRecord->UserContext);
    if (!self || self->m_shouldStop.load()) return;
    if (eventRecord->EventHeader.ProcessId != self->m_targetPid || eventRecord->EventHeader.EventDescriptor.Id != 12) return;
    PROPERTY_DATA_DESCRIPTOR propDesc = {};
    propDesc.PropertyName = reinterpret_cast<ULONGLONG>(L"FileName");
    ULONG bufferSize = 0;
    TdhGetPropertySize(eventRecord, 0, nullptr, 1, &propDesc, &bufferSize);
    std::vector<BYTE> buffer(bufferSize);
    TdhGetProperty(eventRecord, 0, nullptr, 1, &propDesc, bufferSize, buffer.data());
    self->m_callback(QString::fromWCharArray(reinterpret_cast<LPCWSTR>(buffer.data())));
}

QString EtwFileMonitor::convertDevicePathToDosPath(const QString &devicePath)
{   // 设备路径->DOS路径
    const QString &normalized = devicePath;
    const qsizetype deviceEnd = normalized.indexOf('\\', 8);
    const QString deviceName = normalized.left(deviceEnd);
    const QString remaining = normalized.mid(deviceEnd);
    const DWORD drives = GetLogicalDrives();
    for (char driveLetter = 'A'; driveLetter <= 'Z'; ++driveLetter) {
        if (!(drives & 1 << (driveLetter - 'A'))) continue;
        const wchar_t driveRoot[] = {static_cast<wchar_t>(driveLetter), L':', L'\0'};
        wchar_t devicePathBuf[512];
        if (QueryDosDeviceW(driveRoot, devicePathBuf, 512) == 0) continue;
        if (QString::fromWCharArray(devicePathBuf).compare(deviceName, Qt::CaseInsensitive) == 0) return QString(driveRoot) + remaining;
    }
    return devicePath;
}