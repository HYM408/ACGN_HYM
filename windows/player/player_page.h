#ifndef PLAYER_PAGE_H
#define PLAYER_PAGE_H

#include "player_ui.h"
#include "../crawler/crawler.h"

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
    ~PlayerPage() override;
    void setManagers(DatabaseManager *db, CacheImageUtil *cacheImage, PikPakApi *pikpakapi);
    void setupControlOverlay();
    void fetchRoutes(const QJsonObject &collectionData, const QJsonObject &episodeData);

signals:
    void backButtonClicked();

private slots:
    void onRouteSelected(const QString &siteId, const QJsonObject &route);
    void onBTResultClicked(const QString &magnet, const QString &playLink);
    void handleSearchResult(const QString &siteId, const QList<SearchResult> &results);
    void handleBTSearchResult(const QString &siteId, const QList<BTResult> &results);
    void updatePlayerInfo() const;
    void toggleFullscreen();
    void onBackButtonClicked();

private:
    [[nodiscard]] QString getSiteIconUrl(const QString &siteId) const;
    void createSiteDetailTab(const QString &siteId);
    QWidget* createSiteCard(const QString &siteId, const QString &status, const QList<SearchResult> &results);
    void updateCardContent(const QWidget *card, const QString &status, const QList<SearchResult> &results);
    static void updateBTCardContent(const QWidget *card, const QString &status, const QList<BTResult> &results);
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
    QJsonObject currentEpisode;
    QMap<QString, QWidget*> siteWidgets;
    QMap<QString, QFrame*> siteDetailFrames;
    QTabWidget *detailTabWidget = nullptr;
    QStringList apiSiteIds;
    QStringList allSiteIds;
    QStringList btSiteIds;
    QTimer *updateTimer = nullptr;
    QWidget *original_parent = nullptr;
    QLayout *original_layout = nullptr;
    bool fullscreen_mode = false;
    QJsonObject m_episodeData;
};

#endif // PLAYER_PAGE_H