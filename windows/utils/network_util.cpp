#include "network_util.h"
#include <QTimer>
#include <QJsonObject>
#include <QElapsedTimer>

QByteArray sendRequestUtil(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure, const AbortFlag &abortFlag)
{   // 发送请求
    auto waitBeforeRetry = [&] {
        QEventLoop loop;
        QTimer::singleShot(15000, &loop, &QEventLoop::quit);
        QTimer exitCheckTimer;
        exitCheckTimer.start();
        QObject::connect(&exitCheckTimer, &QTimer::timeout, [&] {
            if (abortFlag && abortFlag->load()) loop.quit();
        });
        loop.exec();
    };
    for (int retry = 0; retry < maxRetries; ++retry) {
        QNetworkReply *reply = nullptr;
        if (method == "GET") reply = manager.get(request);
        else if (method == "POST") reply = manager.post(request, data);
        else if (method == "PATCH") reply = manager.sendCustomRequest(request, "PATCH", data);
        QEventLoop loop;
        QTimer checkTimer;
        checkTimer.start();
        QElapsedTimer elapsed;
        elapsed.start();
        bool timeout = false, aborted = false;
        auto abortAndQuit = [&] {
            reply->abort();
            checkTimer.stop();
            loop.quit();
        };
        QObject::connect(&checkTimer, &QTimer::timeout, [&] {
            if (elapsed.hasExpired(15000)) {
                timeout = true;
                abortAndQuit();
            } else if (abortFlag && abortFlag->load()) {
                aborted = true;
                abortAndQuit();
            }
        });
        QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();
        if (aborted) {
            reply->deleteLater();
            return {};
        }
        if (timeout) {
            reply->deleteLater();
            if (retry + 1 < maxRetries) {
                waitBeforeRetry();
                continue;
            }
            break;
        }
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode) *statusCode = httpStatus;
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            reply->deleteLater();
            return response;
        }
        bool authFailure = (httpStatus == 401);
        reply->deleteLater();
        if (authFailure && onAuthFailure) onAuthFailure();
        if (retry + 1 < maxRetries) {
            waitBeforeRetry();
            continue;
        }
        break;
    }
    return {};
}

QJsonObject sendRequestJson(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure, const AbortFlag &abortFlag)
{   // JSON包装
    QByteArray raw = sendRequestUtil(manager, request, method, data, maxRetries, statusCode, onAuthFailure, abortFlag);
    return QJsonDocument::fromJson(raw).object();
}

QString sendRequestHtml(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure, const AbortFlag &abortFlag)
{   // HTML包装
    QByteArray raw = sendRequestUtil(manager, request, method, data, maxRetries, statusCode, onAuthFailure, abortFlag);
    return QString::fromUtf8(raw);
}