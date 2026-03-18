#include "pikpak_api.h"
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include "../config.h"
#include "../utils/network_util.h"

static const QString CLIENT_ID = "YNxT9w7GMdWvEOKa";
static const QString CLIENT_SECRET = "dbw2OtmVEeuUvIptb1Coyg";

PikPakApi::PikPakApi(QObject *parent) : QObject(parent), m_manager(new QNetworkAccessManager(this)) {getInformation();}

void PikPakApi::getInformation()
{   // 获取信息
    username = getConfig("PikPak/username").toString();
    password = getConfig("PikPak/password").toString();
    accessToken = getConfig("PikPak/access_token").toString();
    refreshToken = getConfig("PikPak/refresh_token").toString();
    deviceId = QString::fromLatin1(QCryptographicHash::hash((username + password).toUtf8(), QCryptographicHash::Md5).toHex());
}

template<typename Func>
void PikPakApi::requestWithAuth(const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, Func callback)
{   // 发送请求
    QNetworkRequest req = request;
    req.setRawHeader("Authorization", QString("Bearer %1").arg(accessToken).toUtf8());
    req.setRawHeader("X-Device-Id", deviceId.toUtf8());
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    sendRequestJson(*m_manager, req, method, data, maxRetries,[this, request, method, data, maxRetries, callback](const QJsonObject &json, int statusCode, const QString &error) {
        if (statusCode == 401 && maxRetries > 1) {
            refreshAccessToken([this, request, method, data, maxRetries, callback, json, statusCode, error](const bool success) {
                if (success) requestWithAuth(request, method, data, maxRetries - 1, callback);
                else callback(json, statusCode, error);
            });
        } else callback(json, statusCode, error);
    }, nullptr);
}

void PikPakApi::captchaInit(const QString &action, const QJsonObject &meta, const JsonCallback& callback)
{   // 验证码
    QNetworkRequest request(QUrl("https://user.mypikpak.com/v1/shield/captcha/init"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    const QJsonObject params = {
        {"client_id", CLIENT_ID},
        {"action", action},
        {"device_id", deviceId},
        {"meta", meta}
    };
    const QByteArray postData = QJsonDocument(params).toJson(QJsonDocument::Compact);
    sendRequestJson(*m_manager, request, "POST", postData, 3, [callback](const QJsonObject &json, const int code, const QString &err) {callback(json, code, err);});
}

void PikPakApi::loginPikPak(const LoginCallback& callback)
{   // 登录
    getInformation();
    QString url = "https://user.mypikpak.com/v1/auth/signin";
    static QRegularExpression emailRegex(R"(\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*)");
    static QRegularExpression phoneRegex(R"(\d{11,18})");
    QJsonObject meta;
    if (emailRegex.match(username).hasMatch()) meta["email"] = username;
    else if (phoneRegex.match(username).hasMatch()) meta["phone_number"] = username;
    else meta["username"] = username;
    const QString action = "POST:" + url;
    captchaInit(action, meta, [this, url, callback](const QJsonObject &captchaResult, int, const QString &captchaError) {
        if (!captchaError.isEmpty()) {
            callback(false, "验证码初始化失败: " + captchaError);
            return;
        }
        const QString captchaToken = captchaResult["captcha_token"].toString();
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QUrlQuery query;
        query.addQueryItem("client_id", CLIENT_ID);
        query.addQueryItem("client_secret", CLIENT_SECRET);
        query.addQueryItem("password", password);
        query.addQueryItem("username", username);
        query.addQueryItem("captcha_token", captchaToken);
        const QByteArray postData = query.toString(QUrl::FullyEncoded).toUtf8();
        sendRequestJson(*m_manager, request, "POST", postData, 3, [this, callback](const QJsonObject &userInfo, const int statusCode, const QString &error) {
            if (statusCode == 200 && error.isEmpty()) {
                setConfig("PikPak/access_token", userInfo["access_token"].toString());
                setConfig("PikPak/refresh_token", userInfo["refresh_token"].toString());
                getInformation();
                callback(true, QString());
            } else callback(false, error.isEmpty() ? QString("HTTP %1").arg(statusCode) : error);
        });
    });
}

void PikPakApi::refreshAccessToken(const RefreshCallback& callback)
{   // 刷新token
    const QString url = "https://user.mypikpak.com/v1/auth/token";
    QJsonObject jsonData;
    jsonData["client_id"] = CLIENT_ID;
    jsonData["refresh_token"] = refreshToken;
    jsonData["grant_type"] = "refresh_token";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    const QByteArray postData = QJsonDocument(jsonData).toJson(QJsonDocument::Compact);
    sendRequestJson(*m_manager, request, "POST", postData, 3, [this, callback](const QJsonObject &result, const int statusCode, const QString &error) {
        if (statusCode == 200 && error.isEmpty()) {
            accessToken = result["access_token"].toString();
            refreshToken = result["refresh_token"].toString();
            setConfig("PikPak/access_token", accessToken);
            setConfig("PikPak/refresh_token", refreshToken);
            callback(true);
        } else callback(false);
    });
}

void PikPakApi::getRecentFiles(const JsonCallback& callback)
{   // 获取最近添加文件
    QUrl url("https://api-drive.mypikpak.com/drive/v1/events");
    QUrlQuery query;
    query.addQueryItem("thumbnail_size", "SIZE_MEDIUM");
    query.addQueryItem("limit", "30");
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    requestWithAuth(request, "GET", QByteArray(), 3, [callback](const QJsonObject &json, const int code, const QString &err) {callback(json, code, err);});
}

void PikPakApi::getDownloadUrl(const QString &fileId, const JsonCallback& callback)
{   // 获取下载链接
    const QString action = "GET:/drive/v1/files/" + fileId;
    captchaInit(action, {}, [this, fileId, callback](const QJsonObject &captchaResult, int, const QString &captchaError) {
        if (!captchaError.isEmpty()) {
            callback(QJsonObject(), 0, captchaError);
            return;
        }
        const QString captchaToken = captchaResult["captcha_token"].toString();
        QNetworkRequest request("https://api-drive.mypikpak.com/drive/v1/files/" + fileId);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("X-Captcha-Token", captchaToken.toUtf8());
        requestWithAuth(request, "GET", QByteArray(), 3, [callback](const QJsonObject &json, const int code, const QString &err) {callback(json, code, err);});
    });
}

void PikPakApi::getShareInfo(const QString &shareId, const QString &parentId, const QString &passCode, const JsonCallback& callback)
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
    requestWithAuth(request, "GET", QByteArray(), 3, [callback](const QJsonObject &json, const int code, const QString &err) {callback(json, code, err);});
}

void PikPakApi::restoreShare(const QString &shareId, const QString &passCodeToken, const QStringList &fileIds, const JsonCallback& callback)
{   // 转存分享文件
    const QString url = "https://api-drive.mypikpak.com/drive/v1/share/restore";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject jsonData;
    jsonData["share_id"] = shareId;
    jsonData["pass_code_token"] = passCodeToken;
    jsonData["file_ids"] = QJsonArray::fromStringList(fileIds);
    const QByteArray postData = QJsonDocument(jsonData).toJson(QJsonDocument::Compact);
    requestWithAuth(request, "POST", postData, 3, [callback](const QJsonObject &json, const int code, const QString &err) {callback(json, code, err);});
}

void PikPakApi::transferShareLink(const QString &shareLink, const QString &passCode, const TransferCallback& callback)
{   // 封装 转存分享文件
    static QRegularExpression re(R"(/s/([^/]+)(?:.*/([^/]+))?$)");
    const QRegularExpressionMatch match = re.match(shareLink);
    if (!match.hasMatch()) {
        callback(false);
        return;
    }
    QString shareId = match.captured(1);
    getShareInfo(shareId, match.captured(2), passCode, [this, shareId, callback](const QJsonObject &shareInfo, const int code, const QString &error) {
        if (code != 200 || error.isEmpty() == false) {
            callback(false);
            return;
        }
        QStringList fileIds;
        QJsonArray filesArray = shareInfo["files"].toArray();
        for (const auto &fileValue : filesArray) {
            QJsonObject fileObj = fileValue.toObject();
            QString fileId = fileObj["id"].toString();
            if (!fileId.isEmpty()) fileIds.append(fileId);
        }
        if (fileIds.isEmpty()) {
            callback(false);
            return;
        }
        restoreShare(shareId, shareInfo["pass_code_token"].toString(), fileIds, [callback](const QJsonObject &, const int restoreCode, const QString &restoreError) {callback(restoreCode == 200 && restoreError.isEmpty());});
    });
}

void PikPakApi::getFileList(const QString &parentId, const int limit, const QString &pageToken, const JsonCallback& callback)
{   // 获取目录下文件列表
    QUrl url("https://api-drive.mypikpak.com/drive/v1/files");
    QUrlQuery query;
    query.addQueryItem("thumbnail_size", "SIZE_MEDIUM");
    query.addQueryItem("limit", QString::number(limit));
    query.addQueryItem("parent_id", parentId);
    if (!pageToken.isEmpty()) query.addQueryItem("page_token", pageToken);
    QJsonObject filters;
    filters["trashed"] = QJsonObject{{"eq", false}};
    filters["phase"] = QJsonObject{{"eq", "PHASE_TYPE_COMPLETE"}};
    query.addQueryItem("filters", QString::fromUtf8(QJsonDocument(filters).toJson(QJsonDocument::Compact)));
    url.setQuery(query);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    requestWithAuth(request, "GET", QByteArray(), 3, [callback](const QJsonObject &json, const int code, const QString &err) {callback(json, code, err);});
}