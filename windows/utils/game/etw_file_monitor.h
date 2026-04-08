#ifndef ETW_FILE_MONITOR_H
#define ETW_FILE_MONITOR_H

#include <thread>
#include <QObject>
#include <windows.h>
#include <evntrace.h>

class EtwFileMonitor : public QObject
{
    Q_OBJECT

public:
    explicit EtwFileMonitor(QObject *parent = nullptr);
    ~EtwFileMonitor() override;
    bool startMonitoring(ULONG processId);
    void stopMonitoring(bool shouldStop);
    using FileAccessCallback = std::function<void(const QString &fileName)>;
    void setFileAccessCallback(FileAccessCallback callback) {m_callback = std::move(callback);}
    static QString convertDevicePathToDosPath(const QString &devicePath);

private:
    static void WINAPI eventCallback(PEVENT_RECORD eventRecord);
    TRACEHANDLE m_sessionHandle = INVALID_PROCESSTRACE_HANDLE;
    TRACEHANDLE m_traceHandle = INVALID_PROCESSTRACE_HANDLE;
    std::unique_ptr<std::thread> m_workerThread;
    ULONG m_targetPid = 0;
    FileAccessCallback m_callback{};
    std::atomic<bool> m_shouldStop{false};
};

#endif // ETW_FILE_MONITOR_H