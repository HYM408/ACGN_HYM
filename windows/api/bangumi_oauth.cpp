#include "bangumi_oauth.h"
#include <QTimer>
#include <QUrlQuery>
#include <QTcpServer>
#include <QJsonObject>
#include "../config.h"
#include "../utils/network_util.h"

BangumiOAuth::BangumiOAuth(QObject *parent) : QObject(parent) {getInformation();}

void BangumiOAuth::getInformation()
{   // 获取信息
    m_clientId = getConfig("Bangumi/client_id").toString();
    m_clientSecret = getConfig("Bangumi/client_secret").toString();
    m_redirectUri = getConfig("Bangumi/redirect_uri").toString();
}

QString BangumiOAuth::generateAuthUrl()
{   // 获取授权code
    getInformation();
    const QString baseUrl = "https://bgm.tv/oauth/authorize";
    QUrlQuery params;
    params.addQueryItem("client_id", m_clientId);
    params.addQueryItem("response_type", "code");
    params.addQueryItem("redirect_uri", m_redirectUri);
    return baseUrl + "?" + params.toString();
}

void BangumiOAuth::listenForCode(std::function<void(const QString&)> callback)
{   // 获取code
    this->m_callback = std::move(callback);
    const QUrl redirectUrl(m_redirectUri);
    const int port = redirectUrl.port();
    if (port <= 0) {
        if (this->m_callback) this->m_callback({});
        cleanupListenCode();
        return;
    }
    m_server = new QTcpServer(this);
    if (!m_server->listen(QHostAddress::LocalHost, port)) {
        if (this->m_callback) this->m_callback({});
        cleanupListenCode();
        return;
    }
    connect(m_server, &QTcpServer::newConnection, this, [this] {
        if (m_socket) return;
        m_socket = m_server->nextPendingConnection();
        connect(m_socket, &QTcpSocket::readyRead, this, [this] {
            const QString response ="HTTP/1.1 200 OK\r\n"
                                    "Content-Type: text/html; charset=utf-8\r\n"
                                    "\r\n"
                                    "<html><head><title>授权成功</title></head><body style=\"text-align:center; padding-top:50px\"><p>授权成功，您可以关闭此窗口</p></body></html>";
            m_socket->write(response.toUtf8());
            m_socket->flush();
            m_socket->disconnectFromHost();
            if (this->m_callback) this->m_callback(QUrlQuery(QUrl("http://localhost" + QString::fromUtf8(m_socket->readAll()).split("\r\n")[0].split(" ")[1])).queryItemValue("code"));
            cleanupListenCode();
        });
    });
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, [this] {
        if (this->m_callback) this->m_callback({});
        cleanupListenCode();
    });
    m_timer->start(30000);
}

void BangumiOAuth::cleanupListenCode()
{   // 清理
    if (m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = nullptr;
    }
    if (m_socket) {
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
    }
    m_callback = nullptr;
}

void BangumiOAuth::exchangeCodeForToken(const QString &code, const QString &refreshToken, const std::function<void(bool)> &callback)
{   // 换取token
    const QString tokenUrl = "https://bgm.tv/oauth/access_token";
    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", "HYM408/ACGN_HYM (https://github.com/HYM408/ACGN_HYM)");
    QUrlQuery postData;
    if (!refreshToken.isEmpty()) {
        postData.addQueryItem("grant_type", "refresh_token");
        postData.addQueryItem("client_id", m_clientId);
        postData.addQueryItem("client_secret", m_clientSecret);
        postData.addQueryItem("refresh_token", refreshToken);
    } else {
        postData.addQueryItem("grant_type", "authorization_code");
        postData.addQueryItem("client_id", m_clientId);
        postData.addQueryItem("client_secret", m_clientSecret);
        postData.addQueryItem("code", code);
        postData.addQueryItem("redirect_uri", m_redirectUri);
    }
    new RequestHandler(m_networkManager, request, "POST", postData.toString(QUrl::FullyEncoded).toUtf8(), 0, [callback](const QByteArray &data, const int statusCode, const QString &error) {
        if (statusCode == 200 && error.isEmpty()) {
            QJsonObject tokenData = QJsonDocument::fromJson(data).object();
            setConfig("Bangumi/access_token", tokenData["access_token"].toString());
            setConfig("Bangumi/refresh_token", tokenData["refresh_token"].toString());
            setConfig("Bangumi/expires_at", QDateTime::currentSecsSinceEpoch() + tokenData["expires_in"].toInt(604800));
            if (tokenData.contains("user_id")) setConfig("Bangumi/user_id", tokenData["user_id"].toVariant());
            if (callback) callback(true);
        } else if (callback) callback(false);
    }, nullptr);
}