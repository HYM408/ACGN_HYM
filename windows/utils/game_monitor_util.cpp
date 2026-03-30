#include "game_monitor_util.h"
#include <QTimer>
#include <QProcess>
#include <tlhelp32.h>
#include <QFileDialog>
#include "../sql/sql.h"

HHOOK GameMonitorUtil::keyboardHook = nullptr;
GameMonitorUtil* GameMonitorUtil::instance = nullptr;

GameMonitorUtil::GameMonitorUtil(QWidget *parentWidget, QObject *parent) : QObject(parent), parentWidget(parentWidget), gameMonitorTimer(new QTimer(this))
{
    connect(gameMonitorTimer, &QTimer::timeout, this, &GameMonitorUtil::checkGamesStatus);
}

GameMonitorUtil::~GameMonitorUtil()
{
    if (gameMonitorTimer->isActive()) gameMonitorTimer->stop();
    resumeAllSuspendedProcess(false);
    uninstallKeyboardHook();
}

void GameMonitorUtil::startGame(const int subjectId, const GameData &gameData)
{   // 启动游戏
    m_gameData = gameData;
    QString launchPath = m_gameData.launchPath;
    if (launchPath.isEmpty() || !QFile::exists(launchPath)) {
        launchPath = QFileDialog::getOpenFileName(parentWidget, "选择启动文件", QString(), "可执行文件 (*.exe);;所有文件 (*)");
        if (launchPath.isEmpty()) return;
        DatabaseManager::updateGameData(subjectId, {{"launch_path", launchPath}});
    }
    qint64 pid = 0;
    const QFileInfo fileInfo(launchPath);
    if (!QProcess::startDetached(launchPath, {}, fileInfo.absolutePath(), &pid)) return;
    gameStartTimes[subjectId] = QDateTime::currentMSecsSinceEpoch();
    monitoredGames.insert(subjectId, pid);
    gameSuspended.insert(subjectId, false);
    emit gameStarted(subjectId, launchPath);
    if (!gameMonitorTimer->isActive()) gameMonitorTimer->start(2000);
    installKeyboardHook();
}

void GameMonitorUtil::checkGamesStatus()
{   // 检查游戏状态
    QHash<int, qint64> stillRunning;
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    for (auto it = monitoredGames.begin(); it != monitoredGames.end(); ++it) {
        int subjectId = it.key();
        qint64 pid = it.value();
        if (isProcessRunning(pid)) stillRunning.insert(subjectId, pid);
        else {
            qint64 childPid = findChildProcess(pid);
            if (childPid != 0) {
                stillRunning.insert(subjectId, childPid);
                qDebug() << subjectId << "PID:" << childPid;
            } else {
                const int elapsedSec = static_cast<int>((now - gameStartTimes.take(subjectId)) / 1000);
                int total = m_gameData.playDuration;
                total += elapsedSec;
                DatabaseManager::updateGameData(subjectId, {{"play_duration", total}});
                qDebug() << subjectId << "已退出，运行:" << elapsedSec << "秒，总计:" << total << "秒";
                emit gameExited(subjectId, total);
                gameSuspended.remove(subjectId);
                suspendedGameHwnd.remove(subjectId);
                originalGameTitles.remove(subjectId);
            }
        }
    }
    monitoredGames = stillRunning;
    if (monitoredGames.isEmpty()) {
        gameMonitorTimer->stop();
        uninstallKeyboardHook();
    }
}

bool GameMonitorUtil::isProcessRunning(const qint64 pid)
{   // 检查进程运行状态
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(pid));
    if (!hProcess) return false;
    DWORD exitCode;
    const bool running = GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE;
    CloseHandle(hProcess);
    return running;
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
    EnumWindows([](const HWND hWnd, const LPARAM lParam) -> BOOL {
        auto* pData = reinterpret_cast<EnumData*>(lParam);
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

void GameMonitorUtil::installKeyboardHook()
{   // 安装全局低级键盘钩子
    if (keyboardHook) return;
    instance = this;
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
}

void GameMonitorUtil::uninstallKeyboardHook()
{   // 卸载全局低级键盘钩子
    if (!keyboardHook) return;
    UnhookWindowsHookEx(keyboardHook);
    keyboardHook = nullptr;
    instance = nullptr;
}

void GameMonitorUtil::resumeAllSuspendedProcess(const bool restoreWindow)
{   // 恢复所有暂停的进程
    for (auto it = gameSuspended.begin(); it != gameSuspended.end(); ++it) {
        if (!it.value()) continue;
        const int subjectId = it.key();
        const HWND hwnd = suspendedGameHwnd.value(subjectId);
        if (hwnd && IsWindow(hwnd)) {
            suspendOrResumeProcess(static_cast<DWORD>(monitoredGames.value(subjectId)), false);
            if (originalGameTitles.contains(subjectId)) {
                const QString& originalTitle = originalGameTitles[subjectId];
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

LRESULT CALLBACK GameMonitorUtil::lowLevelKeyboardProc(const int nCode, const WPARAM wParam, const LPARAM lParam)
{   // 低级键盘钩子回调
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        const auto *p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (p->vkCode == VK_ESCAPE) {
            bool hasSuspended = false;
            for (auto it = instance->gameSuspended.begin(); it != instance->gameSuspended.end(); ++it) if (it.value()) {hasSuspended = true; break;}
            if (hasSuspended) {
                instance->resumeAllSuspendedProcess(true);
                return 1;
            }
            for (auto it = instance->monitoredGames.begin(); it != instance->monitoredGames.end(); ++it) {
                int subjectId = it.key();
                const auto pid = static_cast<DWORD>(it.value());
                HWND hwnd = findWindowByPid(pid);
                if (hwnd) {
                    wchar_t title[128];
                    GetWindowTextW(hwnd, title, 128);
                    instance->originalGameTitles[subjectId] = QString(title);
                    SetWindowTextW(hwnd, L"已冻结，按快捷键恢复");
                    instance->suspendedGameHwnd[subjectId] = hwnd;
                    ShowWindow(hwnd, SW_MINIMIZE);
                    suspendOrResumeProcess(pid, true);
                    instance->gameSuspended[subjectId] = true;
                }
            }
            return 1;
        }
    }
    return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}