#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <QNetworkReply>

void abortNetworkRequests();
QByteArray sendRequestUtil(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure);
QJsonObject sendRequestJson(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure);
QString sendRequestHtml(QNetworkAccessManager &manager, const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, int *statusCode, const std::function<void()> &onAuthFailure);

#endif // NETWORK_UTILS_H