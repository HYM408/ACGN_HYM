#include "network_util.h"
#include <QTimer>
#include <QJsonObject>
#include <QNetworkReply>

RequestHandler::RequestHandler(QNetworkAccessManager &manager, const QNetworkRequest &request, QString method, QByteArray data, const int maxRetries, std::function<void(const QByteArray&, int, const QString&)> callback, std::function<void()> onAuthFailure) : QObject(nullptr), m_manager(manager), m_request(request), m_method(std::move(method)), m_data(std::move(data)), m_remainingTries(maxRetries), m_callback(std::move(callback)), m_onAuthFailure(std::move(onAuthFailure))
{
    startRequest();
}

void RequestHandler::startRequest()
{   // 开始请求
    if (m_method == "GET") m_reply = m_manager.get(m_request);
    else if (m_method == "POST") m_reply = m_manager.post(m_request, m_data);
    else if (m_method == "PATCH") m_reply = m_manager.sendCustomRequest(m_request, "PATCH", m_data);
    connect(m_reply, &QNetworkReply::finished, this, &RequestHandler::onReplyFinished);
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, [this] {m_reply->abort();});
    m_timeoutTimer->start(15000);
}

void RequestHandler::onReplyFinished()
{   // 网络请求完成
    if (!m_reply) return;
    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
        m_timeoutTimer->deleteLater();
        m_timeoutTimer = nullptr;
    }
    QNetworkReply *reply = m_reply;
    m_reply = nullptr;
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QNetworkReply::NetworkError err = reply->error();
    const QString errorMsg = err != QNetworkReply::NoError ? reply->errorString() : QString();
    if (httpStatus == 401 && m_onAuthFailure) {
        reply->deleteLater();
        m_onAuthFailure();
        deleteLater();
        return;
    }
    reply->deleteLater();
    if (err == QNetworkReply::NoError) {
        if (m_callback) m_callback(reply->readAll(), httpStatus, QString());
        deleteLater();
        return;
    }
    if (--m_remainingTries > 0) QTimer::singleShot(5000, this, [this] {startRequest();});
    else {
        if (m_callback) m_callback(QByteArray(), httpStatus, errorMsg);
        deleteLater();
    }
}