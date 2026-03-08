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
    static QString sendRequest(const QString &url, const AbortFlag& abortFlag);
    static QJsonObject loadSiteConfig(const QString &site_id);
    static QStringList getAllSiteIds();
    static QJsonObject loadBTConfig(const QString &site_id);
    static QStringList getAllBTSiteIds();
    static QJsonObject loadAPIConfig(const QString &site_id);
    static QStringList getAllAPISiteIds();
    static QList<SearchResult> searchSite(const QString &site_id, const QString &keyword, const AbortFlag& abortFlag);
    static QList<SearchResult> extractSearchResults(const QString &site_id, const QString &html);
    static QList<RouteInfo> getRoutes(const QString &page_url, const QString &site_id, const AbortFlag& abortFlag);
    static QString extractVideoUrl(const QString &url, const AbortFlag& abortFlag);
    static void processVideoUrl(const QString &site_id, const QString &url, const std::function<void(const QString&)> &callback, const AbortFlag& abortFlag);
    static void videoStreamDetector(const QString &site_id, const QString &url, const std::function<void(const QString&)> &onVideoUrl);
    static QList<BTResult> searchBT(const QString &site_id, const QString &keyword, const AbortFlag& abortFlag);
    static QList<SearchResult> searchAPI(const QString &site_id, const QString &keyword, const AbortFlag& abortFlag);
};

#endif // CRAWLER_H