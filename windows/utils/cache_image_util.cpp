#include "cache_image_util.h"
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>

CacheImageUtil::CacheImageUtil(QObject *parent) : QObject(parent)
{   // 初始化图片缓存管理器
    connect(&networkManager, &QNetworkAccessManager::finished, this, &CacheImageUtil::onDownloadFinished);
    connect(this, &CacheImageUtil::downloadComplete, this, &CacheImageUtil::onDownloadComplete);
    QDir().mkpath("data/images");
}

QString CacheImageUtil::getCachePath(const QString &url)
{   // 获取缓存路径
    qsizetype lastSlash = url.lastIndexOf('/');
    if (lastSlash == -1) return "";
    QString fileName = url.mid(lastSlash + 1);
    qsizetype underscore = fileName.indexOf('_');
    if (underscore == -1) return "";
    QString subjectId = fileName.left(underscore);
    return QString("data/images/%1.jpg").arg(subjectId);
}

void CacheImageUtil::getImageAsync(const QString &url, const ImageCallback& callback, bool cacheToLocal)
{   // 获取图片
    QRunnable* task = QRunnable::create([this, url, callback, cacheToLocal] {
        if (cacheToLocal) {
            QString path = getCachePath(url);
            if (QFile::exists(path)) {
                QMetaObject::invokeMethod(this, [path, callback] {callback(QPixmap(path));}, Qt::QueuedConnection);
                return;
            }
        }
        QMetaObject::invokeMethod(this, [this, url, callback, cacheToLocal] {
            pendingCallbacks[url].append(callback);
            if (pendingCallbacks[url].size() == 1) {
                QNetworkRequest request(url);
                request.setAttribute(QNetworkRequest::Attribute::User, url);
                request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), cacheToLocal);
                networkManager.get(request);
            }
        }, Qt::QueuedConnection);
    });
    QThreadPool::globalInstance()->start(task);
}

void CacheImageUtil::onDownloadFinished(QNetworkReply *reply)
{   // 下载完成处理
    QString url = reply->request().attribute(QNetworkRequest::Attribute::User).toString();
    bool cacheToLocal = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), true).toBool();
    QPixmap pixmap;
    if (!reply->error()) {
        QByteArray imageData = reply->readAll();
        if (pixmap.loadFromData(imageData)) {
            if (cacheToLocal) {
                QFile file(getCachePath(url));
                if (file.open(QIODevice::WriteOnly)) file.write(imageData);
            }
        }
    }
    emit downloadComplete(url, pixmap);
    reply->deleteLater();
}

void CacheImageUtil::onDownloadComplete(const QString &url, const QPixmap &pixmap)
{   // 下载完成
    auto it = pendingCallbacks.find(url);
    if (it != pendingCallbacks.end()) {
        auto callbacks = std::move(*it);
        pendingCallbacks.erase(it);
        for (const auto &callback : callbacks) callback(pixmap);
    }
}

void CacheImageUtil::clearPendingDownloads()
{   // 清除待下载项目
    pendingCallbacks.clear();
}