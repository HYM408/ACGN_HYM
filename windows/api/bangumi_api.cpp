#include "bangumi_api.h"
#include <QJsonArray>
#include <QJsonObject>
#include "../config.h"
#include "bangumi_oauth.h"
#include "../utils/network_util.h"

static const QString baseUrl = "https://api.bgm.tv";

BangumiAPI::BangumiAPI(QObject *parent) : QObject(parent)
{   // 获取信息
    userId = getConfig("Bangumi/user_id").toString();
    accessToken = getConfig("Bangumi/access_token").toString();
    refreshToken = getConfig("Bangumi/refresh_token").toString();
}

void BangumiAPI::refreshAndReload()
{   // 刷新token
    BangumiOAuth oauth;
    oauth.exchangeCodeForToken("", refreshToken);
    userId = getConfig("Bangumi/user_id").toString();
    accessToken = getConfig("Bangumi/access_token").toString();
    refreshToken = getConfig("Bangumi/refresh_token").toString();
}

QNetworkRequest BangumiAPI::createRequest(const QString &url) const
{   // 创建请求
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "HYM408/ACGN_HYM (https://github.com/HYM408/ACGN_HYM)");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}

void BangumiAPI::getUserCollections(const bool getAll, const int maxRetries, const ProgressCallback &progressCallback, const CollectionsCallback &completionCallback)
{   // 获取全部收藏
    fetchCollectionsPage(0, QJsonArray(), getAll, maxRetries, progressCallback, completionCallback);
}

void BangumiAPI::fetchCollectionsPage(int offset, QJsonArray allItems, bool getAll, int maxRetries, const ProgressCallback &progress, const CollectionsCallback &completion)
{   // 处理分页
    const QString url = QString("%1/v0/users/%2/collections?limit=50&offset=%3").arg(baseUrl, userId).arg(offset);
    const QNetworkRequest request = createRequest(url);
    sendRequestUtil(manager, request, "GET", QByteArray(), maxRetries, [this, offset, allItems, getAll, maxRetries, progress, completion]
        (const QByteArray &raw, const int statusCode, const QString &error) mutable {
            if (!error.isEmpty() || statusCode != 200) {
                completion(QJsonArray(), error.isEmpty() ? QString("HTTP %1").arg(statusCode) : error);
                return;
            }
            QJsonObject response = QJsonDocument::fromJson(raw).object();
            QJsonArray items = response["data"].toArray();
            const int total = response["total"].toInt();
            for (const auto &item : items) allItems.append(item);
            if (progress) progress(static_cast<int>(offset + items.size()), total);
            if (getAll && offset + 50 < total) fetchCollectionsPage(offset + 50, allItems, getAll, maxRetries, progress, completion);
            else completion(allItems, QString());
        }, [this] {refreshAndReload();}
    );
}

void BangumiAPI::getUserCollection(const int subjectId, const int maxRetries, const CollectionCallback &callback)
{   // 获取单个收藏
    const QString url = QString("%1/v0/users/%2/collections/%3").arg(baseUrl, userId).arg(subjectId);
    const QNetworkRequest request = createRequest(url);
    sendRequestJson(manager, request, "GET", QByteArray(), maxRetries, [callback](const QJsonObject &obj, int, const QString &error) {callback(obj, error);}, [this] {refreshAndReload();});
}

void BangumiAPI::getSubjectInfo(const int subjectId, const int maxRetries, const SubjectCallback &callback)
{   // 获取条目信息
    const QString url = QString("%1/v0/subjects/%2").arg(baseUrl).arg(subjectId);
    const QNetworkRequest request = createRequest(url);
    sendRequestJson(manager, request, "GET", QByteArray(), maxRetries, [callback](const QJsonObject &obj, int, const QString &error) {callback(obj, error);}, [this] {refreshAndReload();});
}

void BangumiAPI::searchSubjectsWithPost(const QString &keyword, const QString &tag, const int subjectType, const bool containsNsfw, const int maxRetries, const CollectionsCallback &callback)
{   // 搜索
    const QString url = QString("%1/v0/search/subjects?limit=20").arg(baseUrl);
    const QNetworkRequest request = createRequest(url);
    QJsonObject params;
    if (!keyword.isEmpty()) params["keyword"] = keyword;
    params["sort"] = "heat";
    QJsonObject filter;
    if (!tag.isEmpty()) filter["tag"] = QJsonArray{tag};
    filter["type"] = QJsonArray{subjectType};
    filter["nsfw"] = containsNsfw;
    params["filter"] = filter;
    sendRequestJson(manager, request, "POST", QJsonDocument(params).toJson(), maxRetries, [callback](const QJsonObject &obj, int, const QString &error) {
        if (!error.isEmpty()) callback(QJsonArray(), error);
        else callback(obj["data"].toArray(), QString());
    }, [this] {refreshAndReload();});
}

void BangumiAPI::getSubjectEpisodes(int subjectId, int maxRetries, const EpisodesCallback &callback)
{   // 获取章节收藏
    std::function<void(int, QJsonArray)> fetchPage = [this, subjectId, maxRetries, callback, &fetchPage]
    (int offset, QJsonArray allItems) {
        const QString url = QString("%1/v0/users/-/collections/%2/episodes?limit=1000&offset=%3").arg(baseUrl).arg(subjectId).arg(offset);
        const QNetworkRequest request = createRequest(url);
        sendRequestJson(manager, request, "GET", QByteArray(), maxRetries, [offset, allItems, callback, fetchPage]
        (const QJsonObject &obj, int, const QString &error) mutable {
            if (!error.isEmpty()) {
                callback(QJsonArray(), error);
                return;
            }
            QJsonArray items = obj["data"].toArray();
            for (const auto &item : items) allItems.append(item);
            if (items.size() < 1000) callback(allItems, QString());
            else fetchPage(offset + 1000, allItems);
        }, [this] {refreshAndReload();});
    };
    fetchPage(0, QJsonArray());
}

void BangumiAPI::createOrUpdateCollection(const int subjectId, const QJsonObject &collectionData, const int maxRetries, const BoolCallback &callback)
{   // 新增单个收藏条目
    const QString url = QString("%1/v0/users/-/collections/%2").arg(baseUrl).arg(subjectId);
    const QNetworkRequest request = createRequest(url);
    const QByteArray data = QJsonDocument(collectionData).toJson();
    sendRequestUtil(manager, request, "POST", data, maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 202, QString());
    }, [this] {refreshAndReload();});
}

void BangumiAPI::updateCollection(const int subjectId, const QJsonObject &collectionData, const int maxRetries, const BoolCallback &callback)
{   // 修改单个收藏
    const QString url = QString("%1/v0/users/-/collections/%2").arg(baseUrl).arg(subjectId);
    const QNetworkRequest request = createRequest(url);
    const QByteArray data = QJsonDocument(collectionData).toJson();
    sendRequestUtil(manager, request, "PATCH", data, maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 204, QString());
    }, [this] {refreshAndReload();});
}

void BangumiAPI::updateSubjectEpisodes(const int subjectId, const QJsonObject &episodesData, const int maxRetries, const BoolCallback &callback)
{   // 批量更新章节收藏信息
    const QString url = QString("%1/v0/users/-/collections/%2/episodes").arg(baseUrl).arg(subjectId);
    const QNetworkRequest request = createRequest(url);
    const QByteArray data = QJsonDocument(episodesData).toJson();
    sendRequestUtil(manager, request, "PATCH", data, maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 204, QString());
    }, [this] {refreshAndReload();});
}