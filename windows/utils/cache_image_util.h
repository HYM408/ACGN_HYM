#ifndef CACHEIMAGEUTIL_H
#define CACHEIMAGEUTIL_H

#include <QHash>
#include <QObject>

class QPixmap;
class BangumiAPI;

class CacheImageUtil : public QObject
{
    Q_OBJECT

public:
    explicit CacheImageUtil(BangumiAPI *bangumiAPI, QObject *parent = nullptr);
    using ImageCallback = std::function<void(const QPixmap&)>;
    void getImage(const QString &url, const ImageCallback &callback, bool cacheToLocal, const QString &fileName);
    BangumiAPI *bangumiAPI = nullptr;

private:
    QHash<QString, QList<ImageCallback>> pendingCallbacks;
};

#endif // CACHEIMAGEUTIL_H