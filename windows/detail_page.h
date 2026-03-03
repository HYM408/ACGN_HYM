#ifndef DETAIL_PAGE_H
#define DETAIL_PAGE_H

#include "detail_ui.h"
#include <QListWidget>
#include <QMouseEvent>
#include "sql.h"

class BangumiAPI;
class CacheImageUtil;

class DetailPage : public QWidget
{
    Q_OBJECT

public:
    explicit DetailPage(QWidget *parent = nullptr);
    void setCollectionData(const CollectionData &data);
    void setCollectionDataFromMap(const QVariantMap &data);
    void setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db);
    void resetUI() const;

signals:
    void backButtonClicked();
    void tagClicked(const QString &tag);
    void showEpisodePageRequested(const CollectionData &collectionData);

private slots:
    void onEpisodeClicked();
    void onOpenBangumiPage() const;
    void onStatusButtonClicked();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void setupConnections();
    void updateDetailPage(const QJsonObject &subjectData);
    void tagsDisplay(const QList<QPair<QString, int>> &tagPairs);
    static QString getTimeInfo(const QList<QPair<QString, int>> &tagPairs, const QString &dateStr);
    void clearLayout() const;
    void loadData();
    void onBackButtonClicked();
    Ui::DetailPage ui{};
    CollectionData currentData;
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    DatabaseManager *dbManager = nullptr;
    QMap<int, QMap<int, QString>> statusNamesMap;
    QListWidget *tagListWidget = nullptr;
};

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(const QString &text = "", QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DETAIL_PAGE_H