#include "game_monitor_util.h"
#include <QTimer>
#include <QProcess>
#include <QFileDialog>
#include <windows.h>
#include <tlhelp32.h>
#include "../sql/sql.h"

GameMonitorUtil::GameMonitorUtil(DatabaseManager *dbManager, QWidget *parentWidget, QObject *parent) : QObject(parent), dbManager(dbManager), parentWidget(parentWidget), gameMonitorTimer(new QTimer(this))
{
    connect(gameMonitorTimer, &QTimer::timeout, this, &GameMonitorUtil::checkGamesStatus);
}

GameMonitorUtil::~GameMonitorUtil()
{
    if (gameMonitorTimer->isActive()) gameMonitorTimer->stop();
}

void GameMonitorUtil::startGame(int subjectId)
{   // 启动游戏
    QVector<GameData> gameDataList = DatabaseManager::getGameData({subjectId});
    QString launchPath = gameDataList.isEmpty() ? QString() : gameDataList.first().launch_path;
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
    emit gameStarted();
    if (!gameMonitorTimer->isActive()) gameMonitorTimer->start(2000);
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
                const qint64 elapsedSec = (now - gameStartTimes.take(subjectId)) / 1000;
                QVector<GameData> current = DatabaseManager::getGameData({subjectId});
                qint64 total = current.isEmpty() ? 0 : static_cast<qint64>(current.first().play_duration);
                total += elapsedSec;
                DatabaseManager::updateGameData(subjectId, {{"play_duration", total}});
                qDebug() << subjectId << "已退出，运行:" << elapsedSec << "秒，总计:" << total << "秒";
                emit gameExited();
            }
        }
    }
    monitoredGames = stillRunning;
    if (monitoredGames.isEmpty()) gameMonitorTimer->stop();
}

bool GameMonitorUtil::isProcessRunning(const qint64 pid)
{   // 检查进程运行状态
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
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
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
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