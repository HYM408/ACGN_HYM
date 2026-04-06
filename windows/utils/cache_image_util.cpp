#include "cache_image_util.h"
#include <QDir>
#include <QPixmap>
#include "../config.h"
#include "../api/bangumi_api.h"

CacheImageUtil::CacheImageUtil(BangumiAPI *bangumiAPI, QObject *parent) : QObject(parent), bangumiAPI(bangumiAPI)
{
    (void)QDir().mkpath("data/images");
}

void CacheImageUtil::getImage(const QString &url, const ImageCallback &callback, const bool cacheToLocal, const QString &fileName)
{   // 获取图片
    if (cacheToLocal) {
        const QString fullPath = "data/images/" + fileName;
        if (QFile::exists(fullPath)) {
            callback(QPixmap(fullPath));
            return;
        }
    }
    pendingCallbacks[url].append(callback);
    if (pendingCallbacks[url].size() > 1) return;
    bangumiAPI->requestWithAuth(url, "GET", QByteArray(), 2, [this, url, cacheToLocal, fileName](const QByteArray &data, const int statusCode, const QString &error) {
        QPixmap pixmap;
        if (statusCode == 200 && error.isEmpty()) {
            pixmap.loadFromData(data);
            if (cacheToLocal && url != "https://lain.bgm.tv/img/no_icon_subject.png") {
                const QString fullPath = "data/images/" + fileName;
                (void)QDir().mkpath(QFileInfo(fullPath).absolutePath());
                QFile file(fullPath);
                if (file.open(QIODevice::WriteOnly)) file.write(data);
            }
        }
        const auto it = pendingCallbacks.find(url);
        if (it != pendingCallbacks.end()) {
            auto callbacks = std::move(it.value());
            pendingCallbacks.erase(it);
            for (const auto &cb : callbacks) cb(pixmap);
        }
    });
}