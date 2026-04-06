#ifndef WINDOW_H
#define WINDOW_H

#include "main_page.h"
#include <QSystemTrayIcon>

class Rss;
class PikPakApi;
class DetailPage;
class SearchPage;
class PlayerPage;
class BangumiOAuth;
class SettingsPage;
class DownloadPage;
class EpisodeOverlay;
class GameMonitorUtil;
class GlobalHotkeyManager;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showCloseOptionDialog();
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private slots:
    void toggleMaximizeWindow();
    void onCloseButtonClicked();
    void onSearchButtonClicked();
    void onTagClicked(const QString &tag, int subjectType);
    void onSettingsButtonClicked();
    void onDownloadButtonClicked();
    void onBackButtonClicked();
    void onShowDetailPageRequested(int subjectId, const QString &progressText);
    void onShowEpisodePageRequested(const SubjectsData &subjectsData);
    void precreatePlayerPage();
    void onEpisodeClicked(const SubjectsData &subjectsData, const EpisodeData &episodeData);
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void closeOptionSelection(int result);
    void onGameStarted();
    void onGameFocusTimeout();
    void gameExited();

private:
    void initializeManagers();
    void setupTrayIcon() const;
    void setupConnections();
    void checkCacheCleanup() const;
    void refreshToken() const;
    void ensureSearchPage();
    DatabaseManager *dbManager = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiOAuth *bangumiOAuth = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    PikPakApi *pikpakApi = nullptr;
    Rss *rss = nullptr;
    SearchPage *searchPage = nullptr;
    SettingsPage *settingsPage = nullptr;
    DownloadPage *downloadPage = nullptr;
    DetailPage *detailPage = nullptr;
    PlayerPage *playerPage = nullptr;
    MainPageManager *mainPageManager = nullptr;
    EpisodeOverlay *episodeOverlay = nullptr;
    GameMonitorUtil *gameMonitorUtil = nullptr;
    GlobalHotkeyManager *hotkeyManager = nullptr;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayMenu = nullptr;
    QTimer *m_gameFocusTimer = nullptr;
    QList<QWidget*> m_pageHistory;
    QPoint m_dragPosition;
    bool m_isDragging = false;
    bool m_wasMaximized = false;
    QRect m_savedNormalGeometry = QRect();
};

#endif // WINDOW_H