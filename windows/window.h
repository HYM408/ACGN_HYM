#ifndef WINDOW_H
#define WINDOW_H

#include "main_page.h"

class Rss;
class PikPakApi;
class DetailPage;
class SearchPage;
class PlayerPage;
class SettingsPage;
class DownloadPage;
class EpisodeOverlay;
class GameMonitorUtil;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    [[nodiscard]] DatabaseManager *getDatabaseManager() const {return dbManager;}
    [[nodiscard]] CacheImageUtil *getCacheImageUtil() const {return cacheImageUtil;}
    [[nodiscard]] BangumiAPI *getBangumiAPI() const {return bangumiAPI;}
    [[nodiscard]] PikPakApi *getPikPakApi() const {return pikpakApi;}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void minimizeWindow();
    void toggleMaximizeWindow();
    void onCloseButtonClicked();
    void onSearchButtonClicked();
    void onTagClicked(const QString &tag, int subjectType);
    void onSettingsButtonClicked();
    void onDownloadButtonClicked();
    void onBackButtonClicked();
    void onShowDetailPageRequested(const CollectionData &collectionData, const QString &progressText);
    void onShowEpisodePageRequested(const CollectionData &collectionData);
    void precreatePlayerPage();
    void onEpisodeClicked(const CollectionData &collectionData, const EpisodeData &episodeData);

private:
    void setupConnections();
    void checkCacheCleanup() const;
    void ensureSearchPage();
    QPoint dragPosition;
    bool isDragging = false;
    DatabaseManager *dbManager = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
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
    QList<QWidget*> pageHistory;
    GameMonitorUtil *gameMonitorUtil = nullptr;
};

#endif // WINDOW_H