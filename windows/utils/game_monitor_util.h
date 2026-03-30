#ifndef GAME_MONITOR_UTIL_H
#define GAME_MONITOR_UTIL_H

#include <QObject>
#include <windows.h>
#include "../sql/data_structs.h"

class GameMonitorUtil : public QObject
{
    Q_OBJECT

public:
    explicit GameMonitorUtil(QWidget *parentWidget = nullptr, QObject *parent = nullptr);
    ~GameMonitorUtil() override;
    void startGame(int subjectId, const GameData &gameData);
    void resumeAllSuspendedProcess(bool restoreWindow);

signals:
    void gameStarted(int subjectId, const QString &launchPath);
    void gameExited(int subjectId, int totalSeconds);

private slots:
    void checkGamesStatus();

private:
    static bool isProcessRunning(qint64 pid);
    static qint64 findChildProcess(qint64 parentPid);
    static bool suspendOrResumeProcess(DWORD pid, bool suspend);
    static HWND findWindowByPid(DWORD pid);
    void installKeyboardHook();
    static void uninstallKeyboardHook();
    static LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    QWidget *parentWidget = nullptr;
    QTimer *gameMonitorTimer = nullptr;
    GameData m_gameData;
    static HHOOK keyboardHook;
    static GameMonitorUtil *instance;
    QHash<int, qint64> gameStartTimes;
    QHash<int, qint64> monitoredGames;
    QHash<int, bool> gameSuspended;
    QHash<int, HWND> suspendedGameHwnd;
    QHash<int, QString> originalGameTitles;
};

#endif // GAME_MONITOR_UTIL_H