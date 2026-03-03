#ifndef RSS_H
#define RSS_H

#include <QObject>

class BangumiAPI;

class Rss : public QObject
{
    Q_OBJECT
public:
    explicit Rss(BangumiAPI *api, QObject *parent = nullptr);
    void startRSS();

signals:
    void rssUpdated();

private slots:
    void updateRSS();

private:
    BangumiAPI *api;
    QTimer *timer;
};

#endif // RSS_H