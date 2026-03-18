#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <QNetworkRequest>

class QNetworkReply;
class QNetworkAccessManager;

class RequestHandler : public QObject {
    Q_OBJECT

public:
    RequestHandler(QNetworkAccessManager &manager, const QNetworkRequest &request, QString method, QByteArray data, int maxRetries, std::function<void(const QByteArray&, int, const QString&)> callback, std::function<void()> onAuthFailure);

private:
    void startRequest();
    void onReplyFinished();
    QNetworkAccessManager &m_manager;
    QNetworkRequest m_request;
    QString m_method;
    QByteArray m_data;
    int m_remainingTries;
    std::function<void(const QByteArray&, int, const QString&)> m_callback;
    std::function<void()> m_onAuthFailure;
    QNetworkReply *m_reply = nullptr;
    QTimer *m_timeoutTimer = nullptr;
};

void sendRequestUtil(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, const std::function<void(const QByteArray&, int statusCode, const QString &error)> &callback, std::function<void()> onAuthFailure = nullptr);
void sendRequestJson(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, const std::function<void(const QJsonObject&, int statusCode, const QString &error)> &callback, std::function<void()> onAuthFailure = nullptr);
void sendRequestHtml(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, const std::function<void(const QString&, int statusCode, const QString &error)> &callback, std::function<void()> onAuthFailure = nullptr);

#endif // NETWORK_UTILS_H