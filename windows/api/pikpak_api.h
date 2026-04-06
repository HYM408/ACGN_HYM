#ifndef PIKPAK_API_H
#define PIKPAK_API_H

#include <QNetworkAccessManager>

class PikPakApi : public QObject
{
    Q_OBJECT

public:
    explicit PikPakApi(QObject *parent = nullptr);
    using BoolCallback = std::function<void(bool success, const QString &error)>;
    using JsonCallback = std::function<void(const QJsonObject&, int statusCode, const QString &error)>;
    void loginPikPak(int maxRetries, const BoolCallback &callback);
    void refreshAccessToken(int maxRetries, const std::function<void(bool success)> &callback);
    void captchaInit(const QString &action, const QJsonObject &meta, int maxRetries, const JsonCallback &callback);
    void getRecentFiles(int maxRetries, const JsonCallback &callback);
    void getQuotaInfo(int maxRetries, const BoolCallback &callback);
    void getDownloadUrl(const QString &fileId, int maxRetries, const JsonCallback &callback);
    void getShareInfo(const QString &shareId, const QString &parentId, const QString &passCode, int maxRetries, const JsonCallback& callback);
    void restoreShare(const QString &shareId, const QString &passCodeToken, const QStringList &fileIds, int maxRetries, const JsonCallback &callback);
    void transferShareLink(const QString &shareLink, const QString &passCode, int maxRetries, const BoolCallback &callback);
    void getFileList(const QString &parentId, int limit, const QString &pageToken, int maxRetries, const JsonCallback &callback);

private:
    void getInformation();
    void requestWithAuth(const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, const JsonCallback &callback);
    bool m_isRefreshing = false;
    std::vector<std::function<void(bool)>> m_pendingCallbacks;
    QNetworkAccessManager m_manager;
    QString m_username;
    QString m_password;
    QString m_accessToken;
    QString m_refreshToken;
    QString m_deviceId;
};

#endif // PIKPAK_API_H