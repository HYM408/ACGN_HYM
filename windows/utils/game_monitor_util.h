#ifndef GAME_MONITOR_UTIL_H
#define GAME_MONITOR_UTIL_H

#include <windows.h>
#include "../sql/data_structs.h"

class QWinEventNotifier;
class GlobalHotkeyManager;

class GameMonitorUtil : public QObject
{
    Q_OBJECT

public:
    explicit GameMonitorUtil(GlobalHotkeyManager *hotkeyManager, QObject *parent = nullptr);
    ~GameMonitorUtil() override;
    static QMap<qint64, QString> getAllProcesses();
    void addExternalProcess(qint64 pid, const QString &processName, qint64 startTime, int idOverride);
    void startGame(int subjectId, const GameData &gameData);
    [[nodiscard]] bool isGameRunning(const int subjectId) const {return monitoredGames.contains(subjectId);}

public slots:
    void updateHotkey(int newKeyCode);

signals:
    void gameStarted(int subjectId, const QString &launchPath);
    void gameExited(int subjectId);

private:
    void registerMonitoredProcess(int id, qint64 pid, HANDLE hProcess, qint64 startTime, const QString &name);
    static qint64 findChildProcess(qint64 parentPid);
    static bool suspendOrResumeProcess(DWORD pid, bool suspend);
    static HWND findWindowByPid(DWORD pid);
    void onProcessExited(int id);
    void resumeAllSuspendedProcess(bool restoreWindow);
    void onFreezeOrResume();
    void cleanupProcessResources(int id);
    GlobalHotkeyManager *hotkeyManager = nullptr;
    GameData m_gameData;
    int nextExternalId = -1;
    QHash<int, qint64> monitoredGames;
    QHash<int, qint64> gameStartTimes;
    QHash<int, HANDLE> processHandles;
    QHash<int, QWinEventNotifier*> processNotifiers;
    QHash<int, QString> externalProcessNames;
    QHash<int, bool> gameSuspended;
    QHash<int, HWND> suspendedGameHwnd;
    QHash<int, QString> originalGameTitles;
};

#endif // GAME_MONITOR_UTIL_H