#ifndef CACHEIMAGEUTIL_H
#define CACHEIMAGEUTIL_H

#include <QHash>
#include <QObject>

class QPixmap;

using ImageCallback = std::function<void(const QPixmap&)>;

class CacheImageUtil : public QObject
{
    Q_OBJECT

public:
    explicit CacheImageUtil(QObject *parent = nullptr);
    void getImageAsync(const QString &url, const ImageCallback &callback, bool cacheToLocal, const QString &fileName);

private:
    QHash<QString, QList<ImageCallback>> pendingCallbacks;
};

#endif // CACHEIMAGEUTIL_H