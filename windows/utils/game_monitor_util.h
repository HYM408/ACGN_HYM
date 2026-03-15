#ifndef GAME_MONITOR_UTIL_H
#define GAME_MONITOR_UTIL_H

#include <QWidget>

class DatabaseManager;

class GameMonitorUtil : public QObject
{
    Q_OBJECT

public:
    explicit GameMonitorUtil(DatabaseManager *dbManager, QWidget *parentWidget = nullptr, QObject *parent = nullptr);
    ~GameMonitorUtil() override;
    void startGame(int subjectId);

signals:
    void gameStarted();
    void gameExited();

private slots:
    void checkGamesStatus();

private:
    static bool isProcessRunning(qint64 pid);
    static qint64 findChildProcess(qint64 parentPid);
    DatabaseManager *dbManager;
    QWidget *parentWidget;
    QTimer *gameMonitorTimer;
    QHash<int, qint64> gameStartTimes;
    QHash<int, qint64> monitoredGames;
};

#endif // GAME_MONITOR_UTIL_H