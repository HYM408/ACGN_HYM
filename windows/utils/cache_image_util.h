#ifndef CACHEIMAGEUTIL_H
#define CACHEIMAGEUTIL_H

#include <QNetworkReply>

using ImageCallback = std::function<void(const QPixmap&)>;

class CacheImageUtil : public QObject
{
    Q_OBJECT

public:
    explicit CacheImageUtil(QObject *parent = nullptr);
    void getImageAsync(const QString &url, const ImageCallback& callback, bool cacheToLocal);
    void clearPendingDownloads();

signals:
    void downloadComplete(const QString &url, const QPixmap &pixmap);

private slots:
    void onDownloadFinished(QNetworkReply *reply);
    void onDownloadComplete(const QString &url, const QPixmap &pixmap);

private:
    static QString getCachePath(const QString &url);
    QNetworkAccessManager networkManager;
    QHash<QString, QList<ImageCallback>> pendingCallbacks;
};

#endif // CACHEIMAGEUTIL_H