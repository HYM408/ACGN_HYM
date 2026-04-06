#ifndef BANGUMI_OAUTH_H
#define BANGUMI_OAUTH_H

#include <QNetworkAccessManager>

class QTcpServer;

class BangumiOAuth : public QObject
{
    Q_OBJECT

public:
    explicit BangumiOAuth(QObject *parent = nullptr);
    void getInformation();
    [[nodiscard]] QString generateAuthUrl();
    void listenForCode(std::function<void(const QString &code)> callback);
    void exchangeCodeForToken(const QString &code, const QString &refreshToken, const std::function<void(bool)> &callback);

private:
    void cleanupListenCode();
    QTcpServer *m_server = nullptr;
    QTcpSocket *m_socket = nullptr;
    QTimer *m_timer = nullptr;
    std::function<void(const QString&)> m_callback;
    QNetworkAccessManager m_networkManager;
    QString m_clientId;
    QString m_clientSecret;
    QString m_redirectUri;
};

#endif // BANGUMI_OAUTH_H