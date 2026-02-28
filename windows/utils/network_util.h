#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <QNetworkReply>

using AbortFlag = std::shared_ptr<std::atomic<bool>>;
QByteArray sendRequestUtil(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure, const AbortFlag &abortFlag = AbortFlag());
QJsonObject sendRequestJson(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure, const AbortFlag &abortFlag = AbortFlag());
QString sendRequestHtml(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure, const AbortFlag &abortFlag = AbortFlag());

#endif // NETWORK_UTILS_H