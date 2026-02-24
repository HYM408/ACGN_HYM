#include "bangumi_api.h"
#include <QJsonArray>
#include <QJsonObject>
#include "../config.h"
#include "bangumi_oauth.h"
#include "../utils/network_util.h"

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
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "ACGN_HYM/1.0");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}

QJsonObject BangumiAPI::sendRequest(QNetworkAccessManager &manager, QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode)
{   // 发送请求
    QByteArray raw = sendRequestUtil(manager, request, method, data, maxRetries, statusCode, [this, &request] {
        refreshAndReload();
        request.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());});
    return QJsonDocument::fromJson(raw).object();
}

QJsonArray BangumiAPI::getUserCollections(bool getAll, int maxRetries)
{   // 获取全部收藏
    QJsonArray allItems;
    int offset = 0;
    while (true) {
        QNetworkAccessManager manager;
        QString url = QString("%1/v0/users/%2/collections?limit=50&offset=%3").arg(baseUrl, userId).arg(offset);
        QNetworkRequest request = createRequest(url);
        QJsonObject response = sendRequest(manager, request, "GET", QByteArray(), maxRetries);
        QJsonArray items = response["data"].toArray();
        qDebug() << "批次" << offset / 50 + 1;
        for (const auto &item : items) allItems.append(item);
        int total = response["total"].toInt();
        if (!getAll || offset + 50 >= total) return allItems;
        offset += 50;
    }
}

QJsonObject BangumiAPI::getUserCollection(int subjectId, int maxRetries)
{   // 获取单个收藏
    QNetworkAccessManager manager;
    QString url = QString("%1/v0/users/%2/collections/%3").arg(baseUrl, userId).arg(subjectId);
    QNetworkRequest request = createRequest(url);
    return sendRequest(manager, request, "GET", QByteArray(), maxRetries);
}

QJsonObject BangumiAPI::getSubjectInfo(int subjectId, int maxRetries)
{   // 获取条目信息
    QNetworkAccessManager manager;
    QString url = QString("%1/v0/subjects/%2").arg(baseUrl).arg(subjectId);
    QNetworkRequest request = createRequest(url);
    return sendRequest(manager, request, "GET", QByteArray(), maxRetries);
}

QJsonArray BangumiAPI::searchSubjects(const QString &keyword, const QString &tag, int subjectType, int maxRetries)
{   // 搜索
    QNetworkAccessManager manager;
    QString url = QString("%1/v0/search/subjects?limit=20").arg(baseUrl);
    QNetworkRequest request = createRequest(url);
    QJsonObject params;
    params["sort"] = "heat";
    QJsonObject filter;
    QJsonArray typeArray;
    typeArray.append(subjectType);
    filter["type"] = typeArray;
    if (!tag.isEmpty()) {
        QJsonArray tagArray;
        tagArray.append(tag);
        filter["tag"] = tagArray;
        params["filter"] = filter;
    } else {
        params["keyword"] = keyword;
        params["filter"] = filter;
    }
    QJsonDocument doc(params);
    QByteArray data = doc.toJson();
    QJsonObject response = sendRequest(manager, request, "POST", data, maxRetries);
    return response["data"].toArray();
}

QJsonArray BangumiAPI::getSubjectEpisodes(int subjectId, int maxRetries)
{   // 获取章节收藏
    QJsonArray allItems;
    int offset = 0;
    while (true) {
        QNetworkAccessManager manager;
        QString url = QString("%1/v0/users/-/collections/%2/episodes?limit=1000&offset=%3").arg(baseUrl).arg(subjectId).arg(offset);
        QNetworkRequest request = createRequest(url);
        QJsonObject response = sendRequest(manager, request, "GET", QByteArray(), maxRetries);
        QJsonArray items = response["data"].toArray();
        for (const auto &item : items) allItems.append(item);
        if (items.size() < 1000) return allItems;
        offset += 1000;
    }
}

bool BangumiAPI::createOrUpdateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries)
{   // 新增单个收藏条目
    QNetworkAccessManager manager;
    QString url = QString("%1/v0/users/-/collections/%2").arg(baseUrl).arg(subjectId);
    QNetworkRequest request = createRequest(url);
    QJsonDocument doc(collectionData);
    QByteArray data = doc.toJson();
    int statusCode = 0;
    sendRequest(manager, request, "POST", data, maxRetries, &statusCode);
    return statusCode == 202;
}

bool BangumiAPI::updateCollection(int subjectId, const QJsonObject &collectionData, int maxRetries)
{   // 修改单个收藏
    QNetworkAccessManager manager;
    QString url = QString("%1/v0/users/-/collections/%2").arg(baseUrl).arg(subjectId);
    QNetworkRequest request = createRequest(url);
    QJsonDocument doc(collectionData);
    QByteArray data = doc.toJson();
    int statusCode = 0;
    sendRequest(manager, request, "PATCH", data, maxRetries, &statusCode);
    return statusCode == 204;
}

bool BangumiAPI::updateSubjectEpisodes(int subjectId, const QJsonObject &episodesData, int maxRetries)
{   // 批量更新章节收藏信息
    QNetworkAccessManager manager;
    QString url = QString("%1/v0/users/-/collections/%2/episodes").arg(baseUrl).arg(subjectId);
    QNetworkRequest request = createRequest(url);
    QJsonDocument doc(episodesData);
    QByteArray data = doc.toJson();
    int statusCode = 0;
    sendRequest(manager, request, "PATCH", data, maxRetries, &statusCode);
    return statusCode == 204;
}