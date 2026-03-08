#ifndef BANGUMI_OAUTH_H
#define BANGUMI_OAUTH_H

#include <QNetworkAccessManager>

class QTcpServer;

class BangumiOAuth : public QObject {
    Q_OBJECT

public:
    explicit BangumiOAuth(QObject *parent = nullptr);
    void getInformation();
    [[nodiscard]] QString generateAuthUrl();
    QString listenForCode(int timeout);
    bool exchangeCodeForToken(const QString &code, const QString &refreshToken);

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