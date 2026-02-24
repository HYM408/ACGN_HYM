#include "pikpak_api.h"
#include <QUrlQuery>
#include <QJsonArray>
#include "../config.h"
#include "../utils/network_util.h"

static const QString CLIENT_ID = "YNxT9w7GMdWvEOKa";
static const QString CLIENT_SECRET = "dbw2OtmVEeuUvIptb1Coyg";

PikPakApi::PikPakApi(QObject *parent) : QObject(parent)
{   // 获取信息
    username = getConfig("PikPak/username").toString();
    password = getConfig("PikPak/password").toString();
    accessToken = getConfig("PikPak/access_token").toString();
    refreshToken = getConfig("PikPak/refresh_token").toString();
    deviceId = QString::fromLatin1(QCryptographicHash::hash((username + password).toUtf8(), QCryptographicHash::Md5).toHex());
}

QJsonObject PikPakApi::sendRequest(QNetworkAccessManager &manager, QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode)
{   // 发送请求
    QByteArray raw = sendRequestUtil(manager, request, method, data, maxRetries, statusCode, [this, &request] {
        refreshAccessToken();
        request.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());});
    return QJsonDocument::fromJson(raw).object();
}

QJsonObject PikPakApi::captchaInit(const QString &action, QJsonObject meta)
{   // 验证码
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://user.mypikpak.com/v1/shield/captcha/init"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject params = {
        {"client_id", CLIENT_ID},
        {"action", action},
        {"device_id", deviceId},
        {"meta", meta}
    };
    QByteArray postData = QJsonDocument(params).toJson(QJsonDocument::Compact);
    return sendRequest(manager, request, "POST", postData, 3);
}

bool PikPakApi::loginPikPak()
{   // 登录
    const QString url = "https://user.mypikpak.com/v1/auth/signin";
    static const QRegularExpression emailRegex(R"(\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*)");
    static const QRegularExpression phoneRegex(R"(\d{11,18})");
    QJsonObject meta;
    if (emailRegex.match(username).hasMatch()) meta["email"] = username;
    else if (phoneRegex.match(username).hasMatch()) meta["phone_number"] = username;
    else meta["username"] = username;
    QString action = "POST:" + url;
    QJsonObject result = captchaInit(action, meta);
    QString captchaToken = result["captcha_token"].toString();
    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QUrlQuery query;
    query.addQueryItem("client_id", CLIENT_ID);
    query.addQueryItem("client_secret", CLIENT_SECRET);
    query.addQueryItem("password", password);
    query.addQueryItem("username", username);
    query.addQueryItem("captcha_token", captchaToken);
    QByteArray postData = query.toString(QUrl::FullyEncoded).toUtf8();
    int statusCode = 0;
    QJsonObject userInfo = sendRequest(manager, request, "POST", postData, 3, &statusCode);
    if (statusCode == 200) {
        setConfig("PikPak/access_token", userInfo["access_token"].toString());
        setConfig("PikPak/refresh_token", userInfo["refresh_token"].toString());
        return true;
    }
    return false;
}

void PikPakApi::refreshAccessToken()
{   // 刷新token
    const QString url = "https://user.mypikpak.com/v1/auth/token";
    QJsonObject jsonData;
    jsonData["client_id"] = CLIENT_ID;
    jsonData["refresh_token"] = refreshToken;
    jsonData["grant_type"] = "refresh_token";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    QByteArray postData = QJsonDocument(jsonData).toJson(QJsonDocument::Compact);
    QNetworkAccessManager manager;
    int statusCode = 0;
    QJsonObject result = sendRequest(manager, request, "POST", postData, 3, &statusCode);
    if (statusCode == 200) {
        QString newAccessToken = result["access_token"].toString();
        QString newRefreshToken = result["refresh_token"].toString();
        accessToken = newAccessToken;
        refreshToken = newRefreshToken;
        setConfig("PikPak/access_token", accessToken);
        setConfig("PikPak/refresh_token", refreshToken);
    }
}

QJsonObject PikPakApi::getRecentFiles()
{   // 获取最近添加文件
    QUrl url("https://api-drive.mypikpak.com/drive/v1/events");
    QUrlQuery query;
    query.addQueryItem("thumbnail_size", "SIZE_MEDIUM");
    query.addQueryItem("limit", "30");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());
    request.setRawHeader("X-Device-Id", deviceId.toUtf8());
    QNetworkAccessManager manager;
    int statusCode = 0;
    QJsonObject result = sendRequest(manager, request, "GET", QByteArray(), 3, &statusCode);
    return statusCode == 200 ? result : QJsonObject();
}

QJsonObject PikPakApi::getDownloadUrl(const QString &fileId)
{   // 获取下载链接
    QString action = "GET:/drive/v1/files/" + fileId;
    QJsonObject captchaResult = captchaInit(action);
    QString captchaToken = captchaResult["captcha_token"].toString();
    QNetworkRequest request("https://api-drive.mypikpak.com/drive/v1/files/" + fileId);
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());
    request.setRawHeader("X-Device-Id", deviceId.toUtf8());
    request.setRawHeader("X-Captcha-Token", captchaToken.toUtf8());
    QNetworkAccessManager manager;
    int statusCode = 0;
    QJsonObject result = sendRequest(manager, request, "GET", QByteArray(), 3, &statusCode);
    return statusCode == 200 ? result : QJsonObject();
}

QJsonObject PikPakApi::getShareInfo(const QString &shareId, const QString &parentId, const QString &passCode)
{   // 获取分享链接下内容
    QUrl url("https://api-drive.mypikpak.com/drive/v1/share");
    QUrlQuery query;
    query.addQueryItem("limit", "100");
    query.addQueryItem("thumbnail_size", "SIZE_LARGE");
    query.addQueryItem("order", "3");
    query.addQueryItem("share_id", shareId);
    if (!parentId.isEmpty()) query.addQueryItem("parent_id", parentId);
    if (!passCode.isEmpty()) query.addQueryItem("pass_code", passCode);
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());
    request.setRawHeader("X-Device-Id", deviceId.toUtf8());
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    QNetworkAccessManager manager;
    int statusCode = 0;
    QJsonObject result = sendRequest(manager, request, "GET", QByteArray(), 3, &statusCode);
    return statusCode == 200 ? result : QJsonObject();
}

QJsonObject PikPakApi::restoreShare(const QString &shareId, const QString &passCodeToken, const QStringList &fileIds)
{   // 转存分享文件
    const QString url = "https://api-drive.mypikpak.com/drive/v1/share/restore";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());
    request.setRawHeader("X-Device-Id", deviceId.toUtf8());
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    QJsonObject jsonData;
    jsonData["share_id"] = shareId;
    jsonData["pass_code_token"] = passCodeToken;
    jsonData["file_ids"] = QJsonArray::fromStringList(fileIds);
    QByteArray postData = QJsonDocument(jsonData).toJson(QJsonDocument::Compact);
    QNetworkAccessManager manager;
    int statusCode = 0;
    QJsonObject result = sendRequest(manager, request, "POST", postData, 3, &statusCode);
    return statusCode == 200 ? result : QJsonObject();
}

bool PikPakApi::transferShareLink(const QString &shareLink, const QString &passCode)
{   // 封装 转存分享文件
    static const QRegularExpression re(R"(/s/([^/]+)(?:.*/([^/]+))?$)");
    QRegularExpressionMatch match = re.match(shareLink);
    if (!match.hasMatch()) return false;
    QString shareId = match.captured(1);
    QString parentId = match.captured(2);
    QJsonObject shareInfo = getShareInfo(shareId, parentId, passCode);
    if (shareInfo.isEmpty()) return false;
    QString passCodeToken = shareInfo["pass_code_token"].toString();
    QStringList fileIds;
    QJsonArray filesArray = shareInfo["files"].toArray();
    for (const auto &fileValue : filesArray) {
        QJsonObject fileObj = fileValue.toObject();
        QString fileId = fileObj["id"].toString();
        if (!fileId.isEmpty()) fileIds.append(fileId);
    }
    if (fileIds.isEmpty()) return false;
    QJsonObject restoreResult = restoreShare(shareId, passCodeToken, fileIds);
    if (restoreResult.isEmpty()) return false;
    return true;
}
