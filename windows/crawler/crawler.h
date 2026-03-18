#ifndef CRAWLER_H
#define CRAWLER_H

#include "../utils/network_util.h"

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
    static QJsonObject loadSiteConfig(const QString &site_id);
    static QStringList getAllSiteIds();
    static QJsonObject loadBTConfig(const QString &site_id);
    static QStringList getAllBTSiteIds();
    static QJsonObject loadAPIConfig(const QString &site_id);
    static QStringList getAllAPISiteIds();
    static void sendRequest(const QString &url, const std::function<void(const QString &html, const QString &error)>& callback);
    using SearchSiteCallback = std::function<void(const QList<SearchResult>&, const QString& error)>;
    static void searchSite(const QString &site_id, const QString &keyword, const SearchSiteCallback& callback);
    static QList<SearchResult> extractSearchResults(const QString &site_id, const QString &html);
    static void getRoutes(const QString &page_url, const QString &site_id, const std::function<void(const QList<RouteInfo>&, const QString& error)>& callback);
    static void extractVideoUrl(const QString &url, const std::function<void(const QString& videoUrl, const QString& error)>& callback);
    static void processVideoUrl(const QString &site_id, const QString &url, const std::function<void(const QString&)> &callback);
    static void videoStreamDetector(const QString &site_id, const QString &url, const std::function<void(const QString&)> &onVideoUrl);
    static void searchBT(const QString &site_id, const QString &keyword, const std::function<void(const QList<BTResult>&, const QString& error)>& callback);
    static void searchAPI(const QString &site_id, const QString &keyword, const SearchSiteCallback& callback);
};

#endif // CRAWLER_H