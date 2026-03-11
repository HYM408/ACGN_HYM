#include "cache_image_util.h"
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>

CacheImageUtil::CacheImageUtil(QObject *parent) : QObject(parent)
{   // 初始化图片缓存管理器
    connect(&networkManager, &QNetworkAccessManager::finished, this, &CacheImageUtil::onDownloadFinished);
    connect(this, &CacheImageUtil::downloadComplete, this, &CacheImageUtil::onDownloadComplete);
    QDir().mkpath("data/images");
}

void CacheImageUtil::getImageAsync(const QString &url, const ImageCallback& callback, bool cacheToLocal, const QString &fileName)
{   // 获取图片
    QRunnable* task = QRunnable::create([this, url, callback, cacheToLocal, fileName] {
        if (cacheToLocal && !fileName.isEmpty()) {
            QString fullPath = "data/images/" + fileName;
            if (QFile::exists(fullPath)) {
                QMetaObject::invokeMethod(this, [callback, fullPath] {callback(QPixmap(fullPath));}, Qt::QueuedConnection);
                return;
            }
        }
        QMetaObject::invokeMethod(this, [this, url, callback, cacheToLocal, fileName] {
            pendingCallbacks[url].append(callback);
            if (pendingCallbacks[url].size() == 1) {
                QNetworkRequest request(url);
                request.setAttribute(QNetworkRequest::Attribute::User, url);
                request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), QVariant::fromValue(cacheToLocal));
                request.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 2), fileName);
                networkManager.get(request);
            }
        }, Qt::QueuedConnection);
    });
    QThreadPool::globalInstance()->start(task);
}

void CacheImageUtil::onDownloadFinished(QNetworkReply *reply)
{   // 下载完成处理
    const QString url = reply->request().attribute(QNetworkRequest::Attribute::User).toString();
    const bool cacheToLocal = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), true).toBool();
    const QString fileName = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 2)).toString();
    QPixmap pixmap;
    if (!reply->error()) {
        const QByteArray imageData = reply->readAll();
        if (pixmap.loadFromData(imageData)) {
            const QUrl finalUrl = reply->url();
            const bool isDefaultImage = finalUrl.toString() == "https://lain.bgm.tv/img/no_icon_subject.png";
            if (cacheToLocal && !fileName.isEmpty() && !isDefaultImage) {
                const QString fullPath = "data/images/" + fileName;
                const QFileInfo fileInfo(fullPath);
                QDir().mkpath(fileInfo.absolutePath());
                QFile file(fullPath);
                if (file.open(QIODevice::WriteOnly)) file.write(imageData);
            }
        }
    }
    emit downloadComplete(url, pixmap);
    reply->deleteLater();
}

void CacheImageUtil::onDownloadComplete(const QString &url, const QPixmap &pixmap)
{   // 下载完成
    const auto it = pendingCallbacks.find(url);
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