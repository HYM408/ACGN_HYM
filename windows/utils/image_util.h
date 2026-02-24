#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

class QLabel;
class QString;
class QPixmap;
class CacheImageUtil;

class ImageUtil {
public:
    static QPixmap createRoundedPixmap(const QPixmap& pixmap, int radius, bool allCorners = false);
    static void loadImageWithCache(CacheImageUtil* cacheImageUtil, const QString &url, QLabel* label, int radius = 0, bool allCorners = false, bool cacheToLocal = true);
};

#endif // IMAGE_UTIL_H
