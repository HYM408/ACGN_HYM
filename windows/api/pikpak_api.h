#ifndef PIKPAK_API_H
#define PIKPAK_API_H

#include <QWidget>
class QNetworkRequest;
class QNetworkAccessManager;

class PikPakApi : public QObject
{
    Q_OBJECT

public:
    explicit PikPakApi(QObject *parent = nullptr);
    void getInformation();
    using LoginCallback = std::function<void(bool success, const QString &error)>;
    void loginPikPak(const LoginCallback& callback);
    using RefreshCallback = std::function<void(bool success)>;
    void refreshAccessToken(const RefreshCallback& callback);
    using JsonCallback = std::function<void(const QJsonObject&, int statusCode, const QString &error)>;
    void captchaInit(const QString &action, const QJsonObject &meta, const JsonCallback& callback);
    void getRecentFiles(const JsonCallback& callback);
    void getDownloadUrl(const QString &fileId, const JsonCallback& callback);
    void getShareInfo(const QString &shareId, const QString &parentId, const QString &passCode, const JsonCallback& callback);
    void restoreShare(const QString &shareId, const QString &passCodeToken, const QStringList &fileIds, const JsonCallback& callback);
    using TransferCallback = std::function<void(bool success)>;
    void transferShareLink(const QString &shareLink, const QString &passCode, const TransferCallback& callback);
    void getFileList(const QString &parentId, int limit, const QString &pageToken, const JsonCallback& callback);

private:
    template<typename Func>
    void requestWithAuth(const QNetworkRequest &request, const QString &method, const QByteArray &data, int maxRetries, Func callback);
    QNetworkAccessManager *m_manager;
    QString username;
    QString password;
    QString accessToken;
    QString refreshToken;
    QString deviceId;
};

#endif // PIKPAK_API_H