#include "crawler.h"
#include <QFile>
#include <QProcess>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>
#include <QNetworkAccessManager>
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

void Crawler::sendRequest(const QString &url, const std::function<void(const QString &html, const QString &error)>& callback)
{   // 发送请求
    auto *manager = new QNetworkAccessManager;
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    sendRequestHtml(*manager, request, "GET", {}, 1, [manager, callback](const QString &html, int, const QString &error) {
        callback(html, error);
        manager->deleteLater();
    }, nullptr);
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

void Crawler::getRoutes(const QString &page_url, const QString &site_id, const std::function<void(const QList<RouteInfo>&, const QString& error)>& callback)
{   // 线路和集数
    sendRequest(page_url, [=](const QString &html, const QString &error) {
        if (!error.isEmpty() || html.isEmpty()) {
            callback({}, "获取线路失败:" + error);
            return;
        }
        QJsonObject config = loadSiteConfig(site_id);
        const auto doc = XmlUtil::parseHtml(html);
        QStringList routeNames = XmlUtil::xpath(doc.data(), config["route_tabs"].toString());
        auto containers = XmlUtil::xpathNodes(doc.data(), config["episode_containers"].toString());
        QList<RouteInfo> routes;
        const qsizetype routeCount = qMin(routeNames.size(), containers.size());
        for (int i = 0; i < routeCount; ++i) {
            RouteInfo ri;
            ri.route = routeNames[i].replace("&nbsp;", " ").trimmed();
            auto episodeNodes = XmlUtil::xpathNodes(containers[i], config["episode_items"].toString());
            for (const auto node : episodeNodes) {
                QString name = XmlUtil::nodeContent(node).trimmed();
                QString href = XmlUtil::nodeAttribute(node, "href");
                if (!name.isEmpty() && !href.isEmpty()) ri.episodes.append({name, XmlUtil::normalizeUrl(XmlUtil::joinUrl(page_url, href))});
            }
            if (!ri.episodes.isEmpty()) routes.append(ri);
        }
        callback(routes, QString());
    });
}

void Crawler::searchSite(const QString &site_id, const QString &keyword, const SearchSiteCallback &callback)
{   // 搜索
    QJsonObject config = loadSiteConfig(site_id);
    const QString baseUrl = config["base_url"].toString();
    QString searchPath = config["search_path"].toString();
    auto processHtml = [=](const QString &html, const QString &error) {
        if (!error.isEmpty() || html.isEmpty()) {
            callback({}, "搜索失败:" + error);
            return;
        }
        QList<SearchResult> baseResults = extractSearchResults(site_id, html);
        if (baseResults.isEmpty()) {
            callback({}, "无结果");
            return;
        }
        struct Context {
            QList<SearchResult> results;
            QAtomicInteger<qsizetype> pending;
            QStringList errors;
            SearchSiteCallback callback;
        };
        auto context = std::make_shared<Context>();
        context->results = baseResults;
        context->pending = baseResults.size();
        context->callback = callback;
        for (int i = 0; i < baseResults.size(); ++i) {
            const auto &sr = baseResults[i];
            getRoutes(sr.link, site_id, [context, i](const QList<RouteInfo> &routes, const QString &err) {
                if (!err.isEmpty()) context->errors.append(QString("获取路线失败 [%1]: %2").arg(context->results[i].title, err));
                else {
                    QList<QJsonObject> routeObjs;
                    for (const auto& [route, episodes] : routes) {
                        QJsonObject routeObj;
                        routeObj["route"] = route;
                        QJsonArray episodesArray;
                        for (const auto& [name, link] : episodes) episodesArray.append(QJsonObject{{"name", name}, {"link", link}});
                        routeObj["episodes"] = episodesArray;
                        routeObjs.append(routeObj);
                    }
                    context->results[i].routes = routeObjs;
                }
                if (--context->pending == 0) {
                    if (!context->errors.isEmpty()) qDebug() << "部分路线获取失败:" << context->errors;
                    context->callback(context->results, QString());
                }
            });
        }
    };
    if (config["need_captcha"].toBool()) performCaptchaAsync(baseUrl, searchPath, keyword, processHtml);
    else {
        const QString fullUrl = XmlUtil::joinUrl(baseUrl, searchPath.replace("{keyword}", keyword));
        sendRequest(fullUrl, processHtml);
    }
}

void Crawler::processVideoUrl(const QString &site_id, const QString &url, const std::function<void(const QString&)> &callback)
{   // 获取视频链接
    qDebug() << url;
    const QJsonValue videoType = loadSiteConfig(site_id)["video_type"];
    if (videoType.isNull()) {
        callback(url);
        return;
    }
    if (videoType.isDouble()) {
        extractVideoUrl(url, [callback, videoType](const QString &videoUrl, const QString &error) {
            if (!error.isEmpty() || videoUrl.isEmpty()) {
                qDebug() << "提取视频URL失败:" << error;
                callback(QString());
                return;
            }
            const int type = videoType.toInt();
            if (type == 1) callback(QUrl::fromPercentEncoding(QByteArray::fromBase64(videoUrl.toLatin1())));
            else callback(videoUrl);
        });
    } else videoStreamDetector(site_id, url, [callback](const QString &rawUrl) {callback(XmlUtil::extractNestedUrl(rawUrl, "url"));});
}

void Crawler::extractVideoUrl(const QString &url, const std::function<void(const QString& videoUrl, const QString& error)> &callback)
{   // 分析视频链接(video_type整数)
    sendRequest(url, [callback](const QString &html, const QString &error) {
        if (!error.isEmpty() || html.isEmpty()) {
            callback(QString(), "获取页面失败: " + error);
            return;
        }
        const qsizetype pos = html.indexOf("player_aaaa");
        if (pos < 0) {
            callback(QString(), "未找到player_aaaa");
            return;
        }
        const qsizetype braceStart = html.indexOf('{', pos + 11);
        if (braceStart < 0) {
            callback(QString(), "未找到JSON起始位置");
            return;
        }
        int level = 0;
        qsizetype braceEnd = -1;
        for (qsizetype i = braceStart; i < html.size(); ++i) {
            QChar c = html[i];
            if (c == '{') ++level;
            else if (c == '}' && --level == 0) {
                braceEnd = i;
                break;
            }
        }
        if (braceEnd < 0) {
            callback(QString(), "未找到JSON结束位置");
            return;
        }
        QJsonParseError err;
        QJsonObject obj = QJsonDocument::fromJson(html.mid(braceStart, braceEnd - braceStart + 1).toUtf8(), &err).object();
        callback(obj["url"].toString(), QString());
    });
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
    proc->start(workerPath, QStringList{url} + loadSiteConfig(site_id)["video_type"].toVariant().toStringList());
}

void Crawler::searchBT(const QString &site_id, const QString &keyword, const std::function<void(const QList<BTResult>&, const QString& error)>& callback)
{   // BT搜索
    QJsonObject config = loadBTConfig(site_id);
    const QString url = XmlUtil::joinUrl(config["base_url"].toString(), config["search_path"].toString().replace("{keyword}", keyword));
    sendRequest(url, [=](const QString &html, const QString &error) {
        if (!error.isEmpty() || html.isEmpty()) {
            callback({}, "搜索失败: " + error);
            return;
        }
        const auto doc = XmlUtil::parseHtml(html);
        auto rows = XmlUtil::xpathNodes(doc.data(), config["row_selector"].toString());
        QList<BTResult> results;
        results.reserve(rows.size());
        for (const auto row : rows) {
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
        callback(results, QString());
    });
}

void Crawler::searchAPI(const QString &site_id, const QString &keyword, const SearchSiteCallback &callback)
{   // API搜索
    QJsonObject config = loadAPIConfig(site_id);
    const QString searchUrl = config["search_api"].toString().replace("{keyword}", keyword);
    QString detailTemplate = config["detail_api"].toString();
    sendRequest(searchUrl, [=](const QString &jsonStr, const QString &error) {
        if (!error.isEmpty() || jsonStr.isEmpty()) {
            callback({}, "搜索API请求失败: " + error);
            return;
        }
        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
        QJsonArray rows = doc.object()["info"].toObject()["rows"].toArray();
        if (rows.isEmpty()) {
            callback({}, "未找到搜索结果");
            return;
        }
        struct Context {
            QList<SearchResult> results;
            QAtomicInteger<qsizetype> pending;
            QStringList errors;
            SearchSiteCallback callback;
        };
        const auto context = std::make_shared<Context>();
        context->results.resize(rows.size());
        context->pending = rows.size();
        context->callback = callback;
        for (int i = 0; i < rows.size(); ++i) {
            const int vod_id = rows[i].toObject()["vod_id"].toInt();
            QString detailUrl = QString(detailTemplate).replace("{vod_id}", QString::number(vod_id));
            const int index = i;
            sendRequest(detailUrl, [=](const QString &innerJsonStr, const QString &err) {
                if (!err.isEmpty() || innerJsonStr.isEmpty()) context->errors.append(QString("获取详情失败 (vod_id=%1): %2").arg(vod_id).arg(err));
                else {
                    QJsonParseError innerParseError;
                    const QJsonDocument innerDoc = QJsonDocument::fromJson(innerJsonStr.toUtf8(), &innerParseError);
                    if (innerParseError.error != QJsonParseError::NoError) context->errors.append(QString("详情JSON解析失败 (vod_id=%1): %2").arg(vod_id).arg(innerParseError.errorString()));
                    else {
                        QJsonObject info = innerDoc.object()["info"].toObject();
                        if (!info.isEmpty()) {
                            SearchResult sr;
                            sr.title = info["vod_name"].toString();
                            sr.site = site_id;
                            QStringList playFromList = info["vod_play_from"].toString().split("$$$", Qt::SkipEmptyParts);
                            QStringList playUrlList = info["vod_play_url"].toString().split("$$$", Qt::SkipEmptyParts);
                            for (qsizetype j = 0, n = qMin(playFromList.size(), playUrlList.size()); j < n; ++j) {
                                QString routeName = playFromList[j].trimmed();
                                if (routeName == "chs") routeName = "简中";
                                if (routeName == "cht") routeName = "繁中";
                                QJsonObject routeObj{{"route", routeName}};
                                QJsonArray episodes;
                                for (const QString &ep : playUrlList[j].split('#', Qt::SkipEmptyParts)) {
                                    QStringList parts = ep.split('$');
                                    if (parts.size() == 2) episodes.append(QJsonObject{{"name", parts[0].trimmed()}, {"link", parts[1].trimmed()}});
                                }
                                if (!episodes.isEmpty()) {
                                    routeObj["episodes"] = episodes;
                                    sr.routes.append(routeObj);
                                }
                            }
                            if (!sr.routes.isEmpty()) context->results[index] = sr;
                        }
                    }
                }
                if (--context->pending == 0) {
                    QList<SearchResult> finalResults;
                    for (const auto &sr : context->results) if (!sr.routes.isEmpty()) finalResults.append(sr);
                    if (!context->errors.isEmpty()) qDebug() << "部分API详情获取失败:" << context->errors;
                    context->callback(finalResults, QString());
                }
            });
        }
    });
}