#include "bangumi_oauth.h"
#include <QTimer>
#include <QUrlQuery>
#include <QTcpServer>
#include <QJsonObject>
#include <QNetworkReply>
#include "../config.h"

BangumiOAuth::BangumiOAuth(QObject *parent) : QObject(parent), server(nullptr) {getInformation();}

void BangumiOAuth::getInformation()
{   // 获取信息
    clientId = getConfig("Bangumi/client_id").toString();
    clientSecret = getConfig("Bangumi/client_secret").toString();
    redirectUri = getConfig("Bangumi/redirect_uri").toString();
}

QString BangumiOAuth::generateAuthUrl()
{   // 获取授权code
    getInformation();
    const QString baseUrl = "https://bgm.tv/oauth/authorize";
    QUrlQuery params;
    params.addQueryItem("client_id", clientId);
    params.addQueryItem("response_type", "code");
    params.addQueryItem("redirect_uri", redirectUri);
    return baseUrl + "?" + params.toString();
}

QString BangumiOAuth::listenForCode(const int timeout)
{   // 获取code
    const QUrl redirectUrl(redirectUri);
    const QString hostname = redirectUrl.host();
    const int port = redirectUrl.port();
    server = new QTcpServer(this);
    if (hostname == "localhost") server->listen(QHostAddress::LocalHost, port);
    connect(server, &QTcpServer::newConnection, this, [this] {
        QTcpSocket *socket = server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
            const QString request = socket->readAll();
            const QString firstLine = request.split("\r\n")[0];
            const QString path = firstLine.split(" ")[1];
            const QUrlQuery query(QUrl("http://localhost" + path));
            authCode = query.queryItemValue("code");
            const QString response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html; charset=utf-8\r\n"
                               "\r\n"
                               "<html><head><title>授权成功</title></head><body style=\"text-align: center; padding-top: 50px;\"><p>授权成功，您可以关闭此窗口</p></body></html>";
            socket->write(response.toUtf8());
            socket->disconnectFromHost();
            emit authCodeReceived(authCode);
        });
    });
    QTimer timer;
    timer.setInterval(timeout * 1000);
    timer.setSingleShot(true);
    QEventLoop loop;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(this, &BangumiOAuth::authCodeReceived, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
    if (server) {
        server->close();
        server->deleteLater();
        server = nullptr;
    }
    return authCode;
}

bool BangumiOAuth::exchangeCodeForToken(const QString &code, const QString &refreshToken)
{   // 换取token
    const QString tokenUrl = "https://bgm.tv/oauth/access_token";
    QNetworkRequest request(tokenUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setRawHeader("User-Agent", "HYM408/ACGN_HYM (https://github.com/HYM408/ACGN_HYM)");
    QUrlQuery postData;
    if (!refreshToken.isEmpty()) {
        postData.addQueryItem("grant_type", "refresh_token");
        postData.addQueryItem("client_id", clientId);
        postData.addQueryItem("client_secret", clientSecret);
        postData.addQueryItem("refresh_token", refreshToken);
    } else {
        postData.addQueryItem("grant_type", "authorization_code");
        postData.addQueryItem("client_id", clientId);
        postData.addQueryItem("client_secret", clientSecret);
        postData.addQueryItem("code", code);
        postData.addQueryItem("redirect_uri", redirectUri);
    }
    QNetworkReply *reply = networkManager.post(request, postData.toString(QUrl::FullyEncoded).toUtf8());
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    if (reply->error() == QNetworkReply::NoError) {
        const QByteArray response = reply->readAll();
        const QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject tokenData = doc.object();
        const int expiresIn = tokenData["expires_in"].toInt(604800);
        const qint64 expiresAt = QDateTime::currentSecsSinceEpoch() + expiresIn;
        setConfig("Bangumi/access_token", tokenData["access_token"].toString());
        setConfig("Bangumi/refresh_token", tokenData["refresh_token"].toString());
        setConfig("Bangumi/expires_at", expiresAt);
        if (tokenData.contains("user_id")) setConfig("Bangumi/user_id", tokenData["user_id"].toVariant());
        reply->deleteLater();
        return true;
    }
    reply->deleteLater();
    return false;
}