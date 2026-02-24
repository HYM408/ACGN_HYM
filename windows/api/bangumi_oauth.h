#ifndef BANGUMI_OAUTH_H
#define BANGUMI_OAUTH_H

#include <QTcpServer>
#include <QNetworkAccessManager>

class BangumiOAuth : public QObject {
    Q_OBJECT

public:
    explicit BangumiOAuth(QObject *parent = nullptr);
    [[nodiscard]] QString generateAuthUrl() const;
    QString listenForCode(int timeout = 60);
    bool exchangeCodeForToken(const QString &code = "", const QString &refreshToken = "");

signals:
    void authCodeReceived(const QString &code);

private:
    QTcpServer *server;
    QString clientId;
    QString clientSecret;
    QString redirectUri;
    QString authCode;
    QNetworkAccessManager networkManager;
};

#endif // BANGUMI_OAUTH_H