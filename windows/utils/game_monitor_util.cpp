#include "game_monitor_util.h"
#include <QTimer>
#include <QProcess>
#include <QWinEventNotifier>
#include <tlhelp32.h>
#include <QFileDialog>
#include "../config.h"
#include "../sql/sql.h"
#include "global_hotkey_manager.h"

GameMonitorUtil::GameMonitorUtil(GlobalHotkeyManager *hotkeyManager, QObject *parent) : QObject(parent), hotkeyManager(hotkeyManager) {}

GameMonitorUtil::~GameMonitorUtil()
{
    resumeAllSuspendedProcess(false);
    for (auto it = processHandles.begin(); it != processHandles.end(); ++it) if (it.value() && it.value() != INVALID_HANDLE_VALUE) CloseHandle(it.value());
    for (auto it = processNotifiers.begin(); it != processNotifiers.end(); ++it) {
        if (it.value()) {
            it.value()->setEnabled(false);
            delete it.value();
        }
    }
    processHandles.clear();
    processNotifiers.clear();
}

QMap<qint64, QString> GameMonitorUtil::getAllProcesses()
{   // 获取所有进程
    QMap<qint64, QString> processes;
    EnumWindows([](HWND hwnd, const LPARAM lParam) -> BOOL {
        if (!IsWindowVisible(hwnd)) return TRUE;
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == 0 || pid == GetCurrentProcessId()) return TRUE;
        if (HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid)) {
            wchar_t path[MAX_PATH];
            DWORD size = MAX_PATH;
            if (QueryFullProcessImageNameW(hProcess, 0, path, &size)) reinterpret_cast<QMap<qint64, QString>*>(lParam)->insert(pid, QString::fromWCharArray(path));
            CloseHandle(hProcess);
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&processes));
    return processes;
}

void GameMonitorUtil::updateHotkey(const int newKeyCode)
{   // 更新快捷键
    if (monitoredGames.isEmpty()) return;
    hotkeyManager->unregisterHotKey(1);
    hotkeyManager->registerHotKey(1, 0, newKeyCode, [this] {onFreezeOrResume();});
}

void GameMonitorUtil::addExternalProcess(const qint64 pid, const QString &processName, const qint64 startTime, const int idOverride)
{   // 添加外部进程
    for (auto it = monitoredGames.begin(); it != monitoredGames.end(); ++it) if (it.value() == pid) return;
    HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(pid));
    int id = idOverride;
    if (id <= 0) id = nextExternalId--;
    externalProcessNames.insert(id, processName);
    registerMonitoredProcess(id, pid, hProcess, startTime, processName);
}

void GameMonitorUtil::startGame(const int subjectId, const GameData &gameData)
{   // 启动游戏
    m_gameData = gameData;
    QString launchPath = m_gameData.launchPath;
    if (launchPath.isEmpty() || !QFile::exists(launchPath)) {
        launchPath = QFileDialog::getOpenFileName(nullptr, "选择启动文件", QString(), "可执行文件 (*.exe);;所有文件 (*)");
        if (launchPath.isEmpty()) return;
        DatabaseManager::updateGameData(subjectId, {{"launch_path", launchPath}});
    }
    qint64 pid = 0;
    const QFileInfo fileInfo(launchPath);
    if (!QProcess::startDetached(launchPath, {}, fileInfo.absolutePath(), &pid)) return;
    HANDLE hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(pid));
    registerMonitoredProcess(subjectId, pid, hProcess, QDateTime::currentMSecsSinceEpoch(), QString());
    emit gameStarted(subjectId, launchPath);
}

void GameMonitorUtil::registerMonitoredProcess(int id, const qint64 pid, const HANDLE hProcess, const qint64 startTime, const QString &name)
{   // 注册监控进程
    monitoredGames.insert(id, pid);
    gameStartTimes.insert(id, startTime);
    processHandles.insert(id, hProcess);
    gameSuspended.insert(id, false);
    auto *notifier = new QWinEventNotifier(hProcess, this);
    connect(notifier, &QWinEventNotifier::activated, this, [this, id] {onProcessExited(id);});
    notifier->setEnabled(true);
    processNotifiers.insert(id, notifier);
    if (!name.isEmpty()) qDebug() << name << "ID" << id << "PID" << pid;
    else qDebug() << "ID:" << id << "PID:" << pid;
    if (monitoredGames.size() == 1) hotkeyManager->registerHotKey(1, 0, getConfig("Shortcut/suspendProcess", 27).toInt(), [this] { onFreezeOrResume(); });
}

qint64 GameMonitorUtil::findChildProcess(const qint64 parentPid)
{   // 查找子进程
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
    qint64 childPid = 0;
    if (Process32First(hSnapshot, &pe)) {
        do {
            if (pe.th32ParentProcessID == static_cast<DWORD>(parentPid)) {
                childPid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return childPid;
}

bool GameMonitorUtil::suspendOrResumeProcess(const DWORD pid, const bool suspend)
{   // 挂起/恢复进程
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return false;
    THREADENTRY32 te = {sizeof(THREADENTRY32)};
    if (Thread32First(hSnapshot, &te)) {
        do {
            if (te.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
                if (hThread) {
                    if (suspend) SuspendThread(hThread);
                    else ResumeThread(hThread);
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(hSnapshot, &te));
    }
    CloseHandle(hSnapshot);
    return true;
}

HWND GameMonitorUtil::findWindowByPid(const DWORD pid)
{   // 获取顶层窗口句柄
    struct EnumData {DWORD pid; HWND hwnd;} data = {pid, nullptr};
    EnumWindows([](HWND hWnd, const LPARAM lParam) -> BOOL {
        auto *pData = reinterpret_cast<EnumData*>(lParam);
        DWORD wndPid;
        GetWindowThreadProcessId(hWnd, &wndPid);
        if (wndPid == pData->pid && IsWindowVisible(hWnd)) {
            pData->hwnd = hWnd;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&data));
    return data.hwnd;
}

void GameMonitorUtil::onProcessExited(const int id)
{   // 进程退出处理
    if (!monitoredGames.contains(id)) return;
    const qint64 pid = monitoredGames.value(id);
    const qint64 startTime = gameStartTimes.value(id, 0);
    const qint64 childPid = findChildProcess(pid);
    if (childPid != 0) {
        qDebug() << "进程" << pid << "子进程" << childPid;
        const int gameId = id;
        cleanupProcessResources(id);
        addExternalProcess(childPid, QString("子进程_%1").arg(childPid), startTime, gameId);
        return;
    }
    if (id > 0) {
        const int elapsedSec = static_cast<int>((QDateTime::currentMSecsSinceEpoch() - startTime) / 1000);
        int total = m_gameData.playDuration + elapsedSec;
        DatabaseManager::updateGameData(id, {{"play_duration", total}});
        qDebug() << id << "已退出，运行:" << elapsedSec << "秒，总计:" << total << "秒";
        emit gameExited(id);
    } else qDebug() << "外部进程" << externalProcessNames.value(id) << "已退出，id" << id;
    cleanupProcessResources(id);
}

void GameMonitorUtil::resumeAllSuspendedProcess(const bool restoreWindow)
{   // 恢复所有暂停的进程
    for (auto it = gameSuspended.begin(); it != gameSuspended.end(); ++it) {
        if (!it.value()) continue;
        const int subjectId = it.key();
        HWND hwnd = suspendedGameHwnd.value(subjectId);
        if (hwnd && IsWindow(hwnd)) {
            suspendOrResumeProcess(static_cast<DWORD>(monitoredGames.value(subjectId)), false);
            if (originalGameTitles.contains(subjectId)) {
                const QString &originalTitle = originalGameTitles[subjectId];
                SetWindowTextW(hwnd, originalTitle.toStdWString().c_str());
                originalGameTitles.remove(subjectId);
            }
            if (restoreWindow) {
                ShowWindow(hwnd, SW_RESTORE);
                SetForegroundWindow(hwnd);
            }
        }
        gameSuspended[subjectId] = false;
        suspendedGameHwnd.remove(subjectId);
    }
}

void GameMonitorUtil::onFreezeOrResume()
{   // 冻结.恢复
    bool hasSuspended = false;
    for (auto it = gameSuspended.begin(); it != gameSuspended.end(); ++it) {
        if (it.value()) {
            hasSuspended = true;
            break;
        }
    }
    if (hasSuspended) {
        resumeAllSuspendedProcess(true);
        return;
    }
    for (auto it = monitoredGames.begin(); it != monitoredGames.end(); ++it) {
        int subjectId = it.key();
        const auto pid = static_cast<DWORD>(it.value());
        if (HWND hwnd = findWindowByPid(pid)) {
            wchar_t title[128];
            GetWindowTextW(hwnd, title, 128);
            originalGameTitles[subjectId] = QString::fromWCharArray(title);
            SetWindowTextW(hwnd, L"已冻结，按快捷键恢复");
            suspendedGameHwnd[subjectId] = hwnd;
            ShowWindow(hwnd, SW_MINIMIZE);
            suspendOrResumeProcess(pid, true);
            gameSuspended[subjectId] = true;
        }
    }
}

void GameMonitorUtil::cleanupProcessResources(const int id)
{   // 清理
    monitoredGames.remove(id);
    gameStartTimes.remove(id);
    externalProcessNames.remove(id);
    gameSuspended.remove(id);
    suspendedGameHwnd.remove(id);
    originalGameTitles.remove(id);
    if (processHandles.contains(id)) {
        const HANDLE h = processHandles.take(id);
        if (h && h != INVALID_HANDLE_VALUE) CloseHandle(h);
    }
    if (processNotifiers.contains(id)) {
        if (QWinEventNotifier *notifier = processNotifiers.take(id)) {
            notifier->setEnabled(false);
            delete notifier;
        }
    }
    if (monitoredGames.isEmpty()) hotkeyManager->unregisterHotKey(1);
}