#ifndef PLAYER_PAGE_H
#define PLAYER_PAGE_H

#include "player_ui.h"
#include "../crawler/crawler.h"
#include "../sql/data_structs.h"

class VLCPlayer;
class PikPakApi;
class CacheImageUtil;
class ControlOverlay;
class DatabaseManager;

class PlayerPage : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerPage(QWidget *parent = nullptr);
    void setManagers(CacheImageUtil *cacheImage, PikPakApi *pikpakapi);
    void setupControlOverlay();
    void applyTheme() const;
    void fetchRoutes(const CollectionData& collectionData, const EpisodeData& episodeData);

signals:
    void backButtonClicked();

private slots:
    void reSearchSite(const QString &siteId);
    void onRouteSelected(const QString &siteId, const QJsonObject &route) const;
    void onBTResultClicked(const QString &magnet, const QString &playLink);
    void handleSearchResult(const QString &siteId, const QList<SearchResult> &results);
    void handleBTSearchResult(const QString &siteId, const QList<BTResult> &results);
    void toggleFullscreen();
    void onBackButtonClicked();

private:
    void setSiteLoadingState(const QString &siteId) const;
    void startSiteSearch(const QString &siteId);
    static QString getSiteIconUrl(const QString &siteId);
    void createSiteDetailTab(const QString &siteId);
    QWidget* createSiteCard(const QString &siteId);
    void updateCardContent(const QWidget *card, const QString &status, const QList<SearchResult> &results);
    static void updateBTCardContent(const QWidget *card, const QString &status, const QList<BTResult> &results);
    static void filterBTResults(const QList<BTResult> &results, QList<BTResult> &filtered, QList<BTResult> &excluded);
    QFrame* createRouteComponent(const QString &siteId, const QJsonObject &route, const QString &title);
    QFrame* createBTResultComponent(const BTResult &result);
    static void clearLayout(QLayout *layout);
    void cleanupPage();
    bool eventFilter(QObject *watched, QEvent *event) override;
    Ui::choicePage ui{};
    DatabaseManager *dbManager = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    PikPakApi *pikpakApi = nullptr;
    VLCPlayer *vlcPlayer = nullptr;
    ControlOverlay *controlOverlay = nullptr;
    QMap<QString, int> siteDetailTabIndex;
    QMap<QString, QWidget*> siteWidgets;
    QMap<QString, QFrame*> siteDetailFrames;
    QTabWidget *detailTabWidget = nullptr;
    QWidget *original_parent = nullptr;
    QLayout *original_layout = nullptr;
    bool fullscreen_mode = false;
    EpisodeData m_episodeData;
    QString m_keyword;
};

#endif // PLAYER_PAGE_H