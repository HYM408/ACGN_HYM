#ifndef PIKPAK_API_H
#define PIKPAK_API_H

#include <QJsonObject>
#include <QNetworkReply>

class PikPakApi : public QObject
{
    Q_OBJECT

public:
    explicit PikPakApi(QObject *parent = nullptr);
    bool loginPikPak();
    void refreshAccessToken();
    QJsonObject captchaInit(const QString &action, QJsonObject meta = {});
    QJsonObject getRecentFiles();
    QJsonObject getDownloadUrl(const QString &fileId);
    QJsonObject getShareInfo(const QString &shareId, const QString &parentId = QString(), const QString &passCode = QString());
    QJsonObject restoreShare(const QString &shareId, const QString &passCodeToken, const QStringList &fileIds);
    bool transferShareLink(const QString &shareLink, const QString &passCode);

private:
    QJsonObject sendRequest(QNetworkAccessManager &manager, QNetworkRequest &request, const QString &method = "GET", const QByteArray &data = QByteArray(), int maxRetries = 3, int *statusCode = nullptr);
    QString username;
    QString password;
    QString accessToken;
    QString refreshToken;
    QString deviceId;
};

#endif // PIKPAK_API_H