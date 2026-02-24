#ifndef CRAWLER_H
#define CRAWLER_H

#include <QJsonObject>

struct SearchResult {
    QString title;
    QString link;
    QString site;
    QList<QJsonObject> routes;
};

struct EpisodeInfo {
    QString name;
    QString link;
};

struct RouteInfo {
    QString route;
    QList<EpisodeInfo> episodes;
};

struct BTResult {
    QString name;
    QString size;
    QString magnet_link;
    QString play_link;
};

class Crawler
{
public:
    static QString sendRequest(const QString &url);
    static QJsonObject loadBTConfig(const QString &site_id);
    static QJsonObject loadSiteConfig(const QString &site_id);
    static QStringList getAllSiteIds();
    static QStringList getAllBTSiteIds();
    static QList<SearchResult> searchSite(const QString &site_id, const QString &keyword);
    static QList<SearchResult> extractSearchResults(const QString &site_id, const QString &html);
    static QList<RouteInfo> getRoutes(const QString &page_url, const QString &site_id);
    static QString extractVideoUrl(const QString &url);
    static void processVideoUrl(const QString &site_id, const QString &url, const std::function<void(const QString&)> &callback = nullptr);
    static void videoStreamDetector(const QString &site_id, const QString &url, const std::function<void(const QString&)> &onVideoUrl = nullptr);
    static QList<BTResult> searchBT(const QString &site_id, const QString &keyword);
};

#endif // CRAWLER_H