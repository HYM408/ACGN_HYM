#include "crawler.h"
#include <QFile>
#include <QProcess>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>
#include "../utils/xml_util.h"
#include "../utils/captcha_utils/captcha_util.h"

static QJsonObject loadConfig()
{   // 加载配置文件
    QFile configFile("XPath.json");
    if (!configFile.open(QIODevice::ReadOnly)) return {};
    return QJsonDocument::fromJson(configFile.readAll()).object();
}

QJsonObject Crawler::loadSiteConfig(const QString &site_id) {return loadConfig()["site_configs"].toObject()[site_id].toObject();}

QStringList Crawler::getAllSiteIds() {return loadConfig()["site_configs"].toObject().keys();}

QJsonObject Crawler::loadBTConfig(const QString &site_id) {return loadConfig()["bt_configs"].toObject()[site_id].toObject();}

QStringList Crawler::getAllBTSiteIds() {return loadConfig()["bt_configs"].toObject().keys();}

QJsonObject Crawler::loadAPIConfig(const QString &site_id) {return loadConfig()["api_configs"].toObject()[site_id].toObject();}

QStringList Crawler::getAllAPISiteIds() {return loadConfig()["api_configs"].toObject().keys();}

QString Crawler::sendRequest(const QString &url, const AbortFlag &abortFlag)
{   // 发送请求
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    return sendRequestHtml(manager, request, "GET", {}, 1, nullptr, nullptr, abortFlag);
}

QList<SearchResult> Crawler::extractSearchResults(const QString &site_id, const QString &html)
{   // 提取搜索结果
    QList<SearchResult> results;
    QJsonObject config = loadSiteConfig(site_id);
    QStringList titles = XmlUtil::xpath(XmlUtil::parseHtml(html).data(), config["title"].toString());
    QStringList links = XmlUtil::xpath(XmlUtil::parseHtml(html).data(), config["link"].toString());
    const QString baseUrl = config["base_url"].toString();
    const qsizetype count = qMin(titles.size(), links.size());
    for (int i = 0; i < count; ++i) {
        SearchResult sr;
        sr.title = titles[i].trimmed();
        sr.link = XmlUtil::normalizeUrl(XmlUtil::joinUrl(baseUrl, links[i]));
        sr.site = site_id;
        results.append(sr);
    }
    return results;
}

QList<RouteInfo> Crawler::getRoutes(const QString &page_url, const QString &site_id, const AbortFlag &abortFlag)
{   // 线路和集数
    QList<RouteInfo> routes;
    QJsonObject config = loadSiteConfig(site_id);
    const QString html = sendRequest(page_url, abortFlag);
    if (html.isEmpty()) return routes;
    const auto doc = XmlUtil::parseHtml(html);
    QStringList routeNames = XmlUtil::xpath(doc.data(), config["route_tabs"].toString());
    QList<xmlNodePtr> containers = XmlUtil::xpathNodes(doc.data(), config["episode_containers"].toString());
    const qsizetype routeCount = qMin(routeNames.size(), containers.size());
    for (int i = 0; i < routeCount; ++i) {
        RouteInfo ri;
        ri.route = routeNames[i].replace("&nbsp;", " ").trimmed();
        QList<xmlNodePtr> episodeNodes = XmlUtil::xpathNodes(containers[i], config["episode_items"].toString());
        for (xmlNode *node : episodeNodes) {
            EpisodeInfo ep;
            ep.name = XmlUtil::nodeContent(node).trimmed();
            QString href = XmlUtil::nodeAttribute(node, "href");
            if (!href.isEmpty()) ep.link = XmlUtil::normalizeUrl(XmlUtil::joinUrl(page_url, href));
            if (!ep.name.isEmpty() && !ep.link.isEmpty()) ri.episodes.append(ep);
        }
        if (!ri.episodes.isEmpty()) routes.append(ri);
    }
    return routes;
}

QList<SearchResult> Crawler::searchSite(const QString &site_id, const QString &keyword, const AbortFlag &abortFlag)
{   // 搜索
    QList<SearchResult> results;
    QJsonObject config = loadSiteConfig(site_id);
    const QString baseUrl = config["base_url"].toString();
    QString searchPath = config["search_path"].toString();
    QString html;
    if (config["need_captcha"].toBool()) {
        html = performCaptcha(baseUrl, searchPath, keyword, abortFlag);
        if (html.isEmpty()) return results;
    } else {
        searchPath.replace("{keyword}", keyword);
        const QString url = XmlUtil::joinUrl(baseUrl, searchPath);
        html = sendRequest(url, abortFlag);
        if (html.isEmpty()) return results;
    }
    QList<SearchResult> searchResults = extractSearchResults(site_id, html);
    for (SearchResult &result : searchResults) {
        QList<RouteInfo> routes = getRoutes(result.link, site_id, abortFlag);
        QList<QJsonObject> routeObjs;
        for (const auto & [route, episodes] : routes) {
            QJsonObject routeObj;
            routeObj["route"] = route;
            QJsonArray epArray;
            for (const auto & [name, link] : episodes) {
                QJsonObject epObj;
                epObj["name"] = name;
                epObj["link"] = link;
                epArray.append(epObj);
            }
            routeObj["episodes"] = epArray;
            routeObjs.append(routeObj);
        }
        result.routes = routeObjs;
        results.append(result);
    }
    return results;
}

void Crawler::processVideoUrl(const QString &site_id, const QString &url, const std::function<void(const QString&)> &callback, const AbortFlag &abortFlag)
{   // 获取视频链接
    qDebug() << url;
    const QJsonValue video_type = loadSiteConfig(site_id)["video_type"];
    if (video_type.isNull()) return callback(url);
    if (video_type.isDouble()) {
        const int type = video_type.toInt();
        const QString videoUrl = extractVideoUrl(url, abortFlag);
        if (type == 1) return callback(QUrl::fromPercentEncoding(QByteArray::fromBase64(videoUrl.toLatin1())));
        return callback(videoUrl);
    }
    auto wrappedCallback = [callback](const QString &rawUrl) {callback(XmlUtil::extractNestedUrl(rawUrl, "url"));};
    videoStreamDetector(site_id, url, wrappedCallback);
}

QString Crawler::extractVideoUrl(const QString &url, const AbortFlag &abortFlag)
{   // 分析视频链接(video_type整数)
    QString html = sendRequest(url, abortFlag);
    if (html.isEmpty()) return {};
    const qsizetype pos = html.indexOf("player_aaaa");
    if (pos < 0) return {};
    const qsizetype braceStart = html.indexOf('{', pos + 11);
    if (braceStart < 0) return {};
    int level = 0;
    qsizetype endPos = -1;
    for (qsizetype i = braceStart; i < html.size(); ++i) {
        QChar c = html[i];
        if (c == '{') ++level;
        else if (c == '}') { --level; if (level == 0) {endPos = i; break;}}
    }
    if (endPos < 0) return {};
    QJsonParseError err;
    QJsonObject obj = QJsonDocument::fromJson(html.mid(braceStart, endPos - braceStart + 1).toUtf8(), &err).object();
    return err.error == QJsonParseError::NoError ? obj["url"].toString() : QString();
}

void Crawler::videoStreamDetector(const QString &site_id, const QString &url, const std::function<void(const QString&)> &onVideoUrl)
{   // 浏览器获取视频链接
    auto *proc = new QProcess;
    QObject::connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [proc, onVideoUrl](int, QProcess::ExitStatus) {
        const QByteArray output = proc->readAllStandardOutput();
        for (const auto& line : output.split('\n')) {
            if (line.startsWith("VIDEO_URL:")) {
                onVideoUrl(QString::fromUtf8(line.mid(10).trimmed()));
                break;
            }
        }
        proc->deleteLater();
    });
    QString workerPath = QCoreApplication::applicationDirPath() + "/videoStream";
#ifdef Q_OS_WIN
    workerPath += ".exe";
#endif
    proc->start(workerPath, QStringList{url} << loadSiteConfig(site_id)["video_type"].toVariant().toStringList());
}

QList<BTResult> Crawler::searchBT(const QString &site_id, const QString &keyword, const AbortFlag &abortFlag)
{   // BT搜索
    QList<BTResult> results;
    const QJsonObject config = loadBTConfig(site_id);
    const QString url = XmlUtil::joinUrl(config["base_url"].toString(), config["search_path"].toString().replace("{keyword}", keyword));
    const QString html = sendRequest(url, abortFlag);
    if (html.isEmpty()) return results;
    const auto doc = XmlUtil::parseHtml(html);
    QList<xmlNodePtr> rows = XmlUtil::xpathNodes(doc.data(), config["row_selector"].toString());
    for (xmlNode *row : rows) {
        BTResult res;
        auto nameNodes = XmlUtil::xpathNodes(row, config["name_selector"].toString());
        if (!nameNodes.isEmpty()) res.name = XmlUtil::nodeContent(nameNodes[0]).trimmed();
        auto sizeNodes = XmlUtil::xpathNodes(row, config["size_selector"].toString());
        if (!sizeNodes.isEmpty()) res.size = XmlUtil::nodeContent(sizeNodes[0]).trimmed();
        auto magnetNodes = XmlUtil::xpathNodes(row, config["magnet_selector"].toString());
        if (!magnetNodes.isEmpty()) res.magnet_link = XmlUtil::nodeAttribute(magnetNodes[0], qPrintable(config["magnet_attr"].toString()));
        auto playNodes = XmlUtil::xpathNodes(row, config["play_selector"].toString());
        if (!playNodes.isEmpty()) res.play_link = XmlUtil::nodeAttribute(playNodes[0], qPrintable(config["play_attr"].toString()));
        results.append(res);
    }
    return results;
}

QList<SearchResult> Crawler::searchAPI(const QString &site_id, const QString &keyword, const AbortFlag &abortFlag)
{   // API搜索
    QList<SearchResult> results;
    QJsonObject config = loadAPIConfig(site_id);
    const QString searchUrl = config["search_api"].toString().replace("{keyword}", keyword);
    QJsonArray rows = QJsonDocument::fromJson(sendRequest(searchUrl, abortFlag).toUtf8()).object()["info"].toObject()["rows"].toArray();
    if (rows.isEmpty()) return results;
    QString detailApi = config["detail_api"].toString();
    for (const auto &val : rows) {
        const int vod_id = val.toObject()["vod_id"].toInt();
        QJsonObject info = QJsonDocument::fromJson(sendRequest(QString(detailApi).replace("{vod_id}", QString::number(vod_id)), abortFlag).toUtf8()).object()["info"].toObject();
        if (info.isEmpty()) continue;
        SearchResult sr;
        sr.title = info["vod_name"].toString();
        sr.site = site_id;
        QStringList playFromList = info["vod_play_from"].toString().split("$$$", Qt::SkipEmptyParts);
        QStringList playUrlList = info["vod_play_url"].toString().split("$$$", Qt::SkipEmptyParts);
        for (qsizetype i = 0, n = qMin(playFromList.size(), playUrlList.size()); i < n; ++i) {
            QString routeName = playFromList[i].trimmed();
            if (routeName == "chs") routeName = "简中";
            if (routeName == "cht") routeName = "繁中";
            QJsonObject routeObj{{"route", routeName}};
            QJsonArray epArray;
            for (const QString &ep : playUrlList[i].split('#', Qt::SkipEmptyParts)) {
                QStringList parts = ep.split('$');
                if (parts.size() == 2) epArray.append(QJsonObject{{"name", parts[0].trimmed()}, {"link", parts[1].trimmed()}});
            }
            if (!epArray.isEmpty()) {
                routeObj["episodes"] = epArray;
                sr.routes.append(routeObj);
            }
        }
        if (!sr.routes.isEmpty()) results.append(sr);
    }
    return results;
}