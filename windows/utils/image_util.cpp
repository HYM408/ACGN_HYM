#include "image_util.h"
#include <QLabel>
#include <QPainter>
#include <QPointer>
#include <QPainterPath>
#include "cache_image_util.h"

QPixmap ImageUtil::createRoundedPixmap(const QPixmap &pixmap, const int radius, const bool allCorners)
{   // 图片圆角
    QPixmap result(pixmap.size());
    result.fill(Qt::transparent);
    QPainter painter(&result);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    if (allCorners) path.addRoundedRect(0, 0, pixmap.width(), pixmap.height(), radius, radius);
    else {
        const int w = pixmap.width();
        const int h = pixmap.height();
        const int d = 2 * radius;
        path.moveTo(radius, 0);
        path.lineTo(w, 0);
        path.lineTo(w, h);
        path.lineTo(radius, h);
        path.arcTo(0, h - d, d, d, 270, -90);
        path.arcTo(0, 0, d, d, 180, -90);
        path.closeSubpath();
    }
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pixmap);
    return result;
}

void ImageUtil::loadImageWithCache(CacheImageUtil *cacheImageUtil, const QString &url, QLabel *label, int radius, bool allCorners, const bool cacheToLocal, const QString &fileName)
{   // 加载图片
    QPointer safeLabel(label);
    auto onLoaded = [safeLabel, radius, allCorners](const QPixmap& pixmap) {
        if (pixmap.isNull() || !safeLabel) return;
        QPixmap processed = pixmap;
        if (pixmap.height() >= pixmap.width()) {
            if (radius > 0) processed = createRoundedPixmap(pixmap, radius, allCorners);
            processed = processed.scaled(safeLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else processed = pixmap.scaledToWidth(safeLabel->width(), Qt::SmoothTransformation);
        safeLabel->setPixmap(processed);
    };
    cacheImageUtil->getImage(url, onLoaded, cacheToLocal, fileName);
}