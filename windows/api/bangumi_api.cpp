#include "bangumi_api.h"
#include <QJsonArray>
#include <QJsonObject>
#include "../config.h"
#include "bangumi_oauth.h"
#include "../utils/network_util.h"

static const QString baseUrl = "https://api.bgm.tv";

BangumiAPI::BangumiAPI(BangumiOAuth *oauth, QObject *parent) : QObject(parent), bangumiOAuth(oauth)
{   // 获取信息
    m_userId = getConfig("Bangumi/user_id").toString();
    m_accessToken = getConfig("Bangumi/access_token").toString();
    m_refreshToken = getConfig("Bangumi/refresh_token").toString();
}

void BangumiAPI::requestWithAuth(const QString &url, const QString &method, const QByteArray &data, int maxRetries, const std::function<void(const QByteArray&, int, const QString&)> &callback)
{   // 发送请求
    if (m_accessToken.isEmpty() && m_refreshToken.isEmpty()) return;
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "HYM408/ACGN_HYM (https://github.com/HYM408/ACGN_HYM)");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", QString("Bearer %1").arg(m_accessToken).toUtf8());
    new RequestHandler(m_manager, req, method, data, maxRetries, [callback](const QByteArray &rawData, const int statusCode, const QString &error) {
        callback(rawData, statusCode, error);
    }, [this, url, method, data, maxRetries, callback] {
        refreshAndReload([this, url, method, data, maxRetries, callback](const bool success) {
            if (success) requestWithAuth(url, method, data, maxRetries, callback);
            else callback(QByteArray(), 401, "Token刷新失败");
        });
    });
}

void BangumiAPI::refreshAndReload(const std::function<void(bool)> &callback)
{   // 刷新token
    if (m_refreshToken.isEmpty()) return;
    if (m_isRefreshing) {
        m_pendingCallbacks.push_back(callback);
        return;
    }
    m_isRefreshing = true;
    bangumiOAuth->exchangeCodeForToken("", m_refreshToken, [this, callback](const bool success) {
        if (success) {
            m_userId = getConfig("Bangumi/user_id").toString();
            m_accessToken = getConfig("Bangumi/access_token").toString();
            m_refreshToken = getConfig("Bangumi/refresh_token").toString();
        }
        if (callback) callback(success);
        for (auto &cb : m_pendingCallbacks) if (cb) cb(success);
        m_pendingCallbacks.clear();
        m_isRefreshing = false;
    });
}

void BangumiAPI::getUser(const int maxRetries, const BoolCallback &callback)
{   // 获取用户信息
    const QString url = QString("%1/v0/me").arg(baseUrl);
    requestWithAuth(url, "GET", QByteArray(), maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 200, QString());
    });
}

void BangumiAPI::getUserCollections(int offset, QJsonArray allItems, bool getAll, int maxRetries, const std::function<void(int current, int total)> &progress, const CollectionsCallback &completion)
{   // 获取收藏
    const QString url = QString("%1/v0/users/%2/collections?limit=50&offset=%3").arg(baseUrl, m_userId).arg(offset);
    requestWithAuth(url, "GET", QByteArray(), maxRetries, [this, offset, allItems, getAll, maxRetries, progress, completion](const QByteArray &rawData, const int statusCode, const QString &error) mutable {
        if (!error.isEmpty() || statusCode != 200) {
            completion(QJsonArray(), error.isEmpty() ? QString("HTTP %1").arg(statusCode) : error);
            return;
        }
        QJsonObject response = QJsonDocument::fromJson(rawData).object();
        QJsonArray items = response["data"].toArray();
        const int total = response["total"].toInt();
        for (const auto &item : items) allItems.append(item);
        if (progress) progress(static_cast<int>(offset + items.size()), total);
        if (getAll && offset + 50 < total) getUserCollections(offset + 50, allItems, getAll, maxRetries, progress, completion);
        else completion(allItems, QString());
    });
}

void BangumiAPI::getUserCollection(const int subjectId, const int maxRetries, const SubjectCallback &callback)
{   // 获取单个收藏
    const QString url = QString("%1/v0/users/%2/collections/%3").arg(baseUrl, m_userId).arg(subjectId);
    requestWithAuth(url, "GET", QByteArray(), maxRetries, [callback](const QByteArray &rawData, int, const QString &error) {callback(QJsonDocument::fromJson(rawData).object(), error);});
}

void BangumiAPI::getSubjectInfo(const int subjectId, const int maxRetries, const SubjectCallback &callback)
{   // 获取条目信息
    const QString url = QString("%1/v0/subjects/%2").arg(baseUrl).arg(subjectId);
    requestWithAuth(url, "GET", QByteArray(), maxRetries, [callback](const QByteArray &rawData, int, const QString &error) {callback(QJsonDocument::fromJson(rawData).object(), error);});
}

void BangumiAPI::searchSubjectsWithPost(const QString &keyword, const QString &tag, const int subjectType, const bool containsNsfw, const int maxRetries, const CollectionsCallback &callback)
{   // 搜索
    const QString url = QString("%1/v0/search/subjects?limit=20").arg(baseUrl);
    QJsonObject params;
    if (!keyword.isEmpty()) params["keyword"] = keyword;
    params["sort"] = "heat";
    QJsonObject filter;
    if (!tag.isEmpty()) filter["tag"] = QJsonArray{tag};
    filter["type"] = QJsonArray{subjectType};
    filter["nsfw"] = containsNsfw;
    params["filter"] = filter;
    requestWithAuth(url, "POST", QJsonDocument(params).toJson(), maxRetries, [callback](const QByteArray &rawData, int, const QString &error) {
        if (!error.isEmpty()) callback(QJsonArray(), error);
        else callback(QJsonDocument::fromJson(rawData)["data"].toArray(), QString());
    });
}

void BangumiAPI::getSubjectEpisodes(int subjectId, int maxRetries, const CollectionsCallback &callback)
{   // 获取章节收藏
    std::function<void(int, QJsonArray)> fetchPage = [this, subjectId, maxRetries, callback, &fetchPage](int offset, QJsonArray allItems) {
        const QString url = QString("%1/v0/users/-/collections/%2/episodes?limit=1000&offset=%3").arg(baseUrl).arg(subjectId).arg(offset);
        requestWithAuth(url, "GET", QByteArray(), maxRetries, [offset, allItems, callback, fetchPage](const QByteArray &rawData, int, const QString &error) mutable {
            if (!error.isEmpty()) {
                callback(QJsonArray(), error);
                return;
            }
            QJsonArray items = QJsonDocument::fromJson(rawData)["data"].toArray();
            for (const auto &item : items) allItems.append(item);
            if (items.size() < 1000) callback(allItems, QString());
            else fetchPage(offset + 1000, allItems);
        });
    };
    fetchPage(0, QJsonArray());
}

void BangumiAPI::createOrUpdateCollection(const int subjectId, const QJsonObject &collectionData, const int maxRetries, const BoolCallback &callback)
{   // 新增单个收藏条目
    const QString url = QString("%1/v0/users/-/collections/%2").arg(baseUrl).arg(subjectId);
    requestWithAuth(url, "POST", QJsonDocument(collectionData).toJson(), maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 202, QString());
    });
}

void BangumiAPI::updateCollection(const int subjectId, const QJsonObject &collectionData, const int maxRetries, const BoolCallback &callback)
{   // 修改单个收藏
    const QString url = QString("%1/v0/users/-/collections/%2").arg(baseUrl).arg(subjectId);
    requestWithAuth(url, "PATCH", QJsonDocument(collectionData).toJson(), maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 204, QString());
    });
}

void BangumiAPI::updateSubjectEpisodes(const int subjectId, const QJsonObject &episodesData, const int maxRetries, const BoolCallback &callback)
{   // 批量更新章节收藏信息
    const QString url = QString("%1/v0/users/-/collections/%2/episodes").arg(baseUrl).arg(subjectId);
    requestWithAuth(url, "PATCH", QJsonDocument(episodesData).toJson(), maxRetries, [callback](const QByteArray &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 204, QString());
    });
}