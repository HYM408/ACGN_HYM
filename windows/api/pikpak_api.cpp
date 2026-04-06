#include "pikpak_api.h"
#include <QUrlQuery>
#include <QJsonArray>
#include <QJsonObject>
#include "../config.h"
#include "../utils/network_util.h"

static const QString CLIENT_ID = "YNxT9w7GMdWvEOKa";
static const QString CLIENT_SECRET = "dbw2OtmVEeuUvIptb1Coyg";

PikPakApi::PikPakApi(QObject *parent) : QObject(parent)
{
    getInformation();
}

void PikPakApi::getInformation()
{   // 获取信息
    m_username = getConfig("PikPak/username").toString();
    m_password = getConfig("PikPak/password").toString();
    m_accessToken = getConfig("PikPak/access_token").toString();
    m_refreshToken = getConfig("PikPak/refresh_token").toString();
    m_deviceId = QString::fromLatin1(QCryptographicHash::hash((m_username + m_password).toUtf8(), QCryptographicHash::Md5).toHex());
}

void PikPakApi::requestWithAuth(const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, const JsonCallback &callback)
{   // 发送请求
    QNetworkRequest req = request;
    req.setRawHeader("Authorization", QString("Bearer %1").arg(m_accessToken).toUtf8());
    req.setRawHeader("X-Device-Id", m_deviceId.toUtf8());
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    new RequestHandler(m_manager, req, method, data, maxRetries, [callback](const QByteArray &rawData, const int statusCode, const QString &error) {
        callback(QJsonDocument::fromJson(rawData).object(), statusCode, error);
    }, [this, req, method, data, maxRetries, callback] {
        refreshAccessToken(maxRetries, [this, req, method, data, maxRetries, callback](const bool success) {
            if (success) requestWithAuth(req, method, data, maxRetries, callback);
            else callback(QJsonObject(), 401, "Token刷新失败");
        });
    });
}

void PikPakApi::captchaInit(const QString &action, const QJsonObject &meta, const int maxRetries, const JsonCallback &callback)
{   // 验证码
    QNetworkRequest request(QUrl("https://user.mypikpak.com/v1/shield/captcha/init"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    const QJsonObject params = {
        {"client_id", CLIENT_ID},
        {"action", action},
        {"device_id", m_deviceId},
        {"meta", meta}
    };
    new RequestHandler(m_manager, request, "POST", QJsonDocument(params).toJson(QJsonDocument::Compact), maxRetries, [callback](const QByteArray &rawData, const int statusCode, const QString &error) {callback(QJsonDocument::fromJson(rawData).object(), statusCode, QString());}, nullptr);
}

void PikPakApi::loginPikPak(int maxRetries, const BoolCallback &callback)
{   // 登录
    getInformation();
    QString url = "https://user.mypikpak.com/v1/auth/signin";
    static QRegularExpression emailRegex(R"(\w+([-+.]\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*)");
    static QRegularExpression phoneRegex(R"(\d{11,18})");
    QJsonObject meta;
    if (emailRegex.match(m_username).hasMatch()) meta["email"] = m_username;
    else if (phoneRegex.match(m_username).hasMatch()) meta["phone_number"] = m_username;
    else meta["username"] = m_username;
    captchaInit("POST:" + url, meta, maxRetries,[this, url, maxRetries, callback](const QJsonObject &captchaResult, int, const QString &captchaError) {
        if (!captchaError.isEmpty()) {
            callback(false, "验证码初始化失败: " + captchaError);
            return;
        }
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        QUrlQuery query;
        query.addQueryItem("client_id", CLIENT_ID);
        query.addQueryItem("client_secret", CLIENT_SECRET);
        query.addQueryItem("password", m_password);
        query.addQueryItem("username", m_username);
        query.addQueryItem("captcha_token", captchaResult["captcha_token"].toString());
        new RequestHandler(m_manager, request, "POST", query.toString(QUrl::FullyEncoded).toUtf8(), maxRetries, [this, callback](const QByteArray &rawData, const int statusCode, const QString &error) {
            if (statusCode == 200 && error.isEmpty()) {
                QJsonObject userInfo = QJsonDocument::fromJson(rawData).object();
                setConfig("PikPak/access_token", userInfo["access_token"].toString());
                setConfig("PikPak/refresh_token", userInfo["refresh_token"].toString());
                getInformation();
                callback(true, QString());
            } else callback(false, error.isEmpty() ? QString("HTTP %1").arg(statusCode) : error);
        }, nullptr);
    });
}

void PikPakApi::refreshAccessToken(const int maxRetries, const std::function<void(bool success)> &callback)
{   // 刷新token
    if (m_isRefreshing) {
        m_pendingCallbacks.push_back(callback);
        return;
    }
    m_isRefreshing = true;
    const QString url = "https://user.mypikpak.com/v1/auth/token";
    QJsonObject jsonData;
    jsonData["client_id"] = CLIENT_ID;
    jsonData["refresh_token"] = m_refreshToken;
    jsonData["grant_type"] = "refresh_token";
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36");
    new RequestHandler(m_manager, request, "POST", QJsonDocument(jsonData).toJson(QJsonDocument::Compact), maxRetries, [this, callback](const QByteArray &rawData, const int statusCode, const QString &error) {
        const bool success = statusCode == 200 && error.isEmpty();
        if (success) {
            QJsonObject result = QJsonDocument::fromJson(rawData).object();
            m_accessToken = result["access_token"].toString();
            m_refreshToken = result["refresh_token"].toString();
            setConfig("PikPak/access_token", m_accessToken);
            setConfig("PikPak/refresh_token", m_refreshToken);
        }
        callback(success);
        for (auto &cb : m_pendingCallbacks) if (cb) cb(success);
        m_pendingCallbacks.clear();
        m_isRefreshing = false;
    }, nullptr);
}

void PikPakApi::getQuotaInfo(const int maxRetries, const BoolCallback &callback)
{   // 获取空间信息
    const QUrl url("https://api-drive.mypikpak.com/drive/v1/about");
    const QNetworkRequest request(url);
    requestWithAuth(request, "GET", QByteArray(), maxRetries, [callback](const QJsonObject &, const int statusCode, const QString &error) {
        if (!error.isEmpty()) callback(false, error);
        else callback(statusCode == 200, QString());
    });
}

void PikPakApi::getRecentFiles(const int maxRetries, const JsonCallback &callback)
{   // 获取最近添加文件
    QUrl url("https://api-drive.mypikpak.com/drive/v1/events");
    QUrlQuery query;
    query.addQueryItem("thumbnail_size", "SIZE_MEDIUM");
    query.addQueryItem("limit", "30");
    url.setQuery(query);
    const QNetworkRequest request(url);
    requestWithAuth(request, "GET", QByteArray(), maxRetries, callback);
}

void PikPakApi::getDownloadUrl(const QString &fileId, int maxRetries, const JsonCallback &callback)
{   // 获取下载链接
    captchaInit("GET:/drive/v1/files/" + fileId, {}, maxRetries, [this, fileId, maxRetries, callback](const QJsonObject &captchaResult, int, const QString &captchaError) {
        if (!captchaError.isEmpty()) {
            callback(QJsonObject(), 0, captchaError);
            return;
        }
        QNetworkRequest request("https://api-drive.mypikpak.com/drive/v1/files/" + fileId);
        request.setRawHeader("X-Captcha-Token", captchaResult["captcha_token"].toString().toUtf8());
        requestWithAuth(request, "GET", QByteArray(), maxRetries, callback);
    });
}

void PikPakApi::getShareInfo(const QString &shareId, const QString &parentId, const QString &passCode, const int maxRetries, const JsonCallback &callback)
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
    const QNetworkRequest request(url);
    requestWithAuth(request, "GET", QByteArray(), maxRetries, callback);
}

void PikPakApi::restoreShare(const QString &shareId, const QString &passCodeToken, const QStringList &fileIds, const int maxRetries, const JsonCallback &callback)
{   // 转存分享文件
    const QString url = "https://api-drive.mypikpak.com/drive/v1/share/restore";
    const QNetworkRequest request(url);
    QJsonObject jsonData;
    jsonData["share_id"] = shareId;
    jsonData["pass_code_token"] = passCodeToken;
    jsonData["file_ids"] = QJsonArray::fromStringList(fileIds);
    requestWithAuth(request, "POST", QJsonDocument(jsonData).toJson(QJsonDocument::Compact), maxRetries, callback);
}

void PikPakApi::transferShareLink(const QString &shareLink, const QString &passCode, int maxRetries, const BoolCallback &callback)
{   // 封装 转存分享文件
    static QRegularExpression re(R"(/s/([^/]+)(?:.*/([^/]+))?$)");
    const QRegularExpressionMatch match = re.match(shareLink);
    if (!match.hasMatch()) {
        callback(false, "正则匹配失败");
        return;
    }
    QString shareId = match.captured(1);
    getShareInfo(shareId, match.captured(2), passCode, maxRetries, [this, shareId, maxRetries, callback](const QJsonObject &shareInfo, const int code, const QString &error) {
        if (code != 200 || !error.isEmpty()) {
            callback(false, error);
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
            callback(false, "未找到文件");
            return;
        }
        restoreShare(shareId, shareInfo["pass_code_token"].toString(), fileIds, maxRetries, [callback](const QJsonObject &, const int restoreCode, const QString &restoreError) {callback(restoreCode == 200 && restoreError.isEmpty(), restoreError);});
    });
}

void PikPakApi::getFileList(const QString &parentId, const int limit, const QString &pageToken, const int maxRetries, const JsonCallback &callback)
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
    const QNetworkRequest request(url);
    requestWithAuth(request, "GET", QByteArray(), maxRetries, callback);
}