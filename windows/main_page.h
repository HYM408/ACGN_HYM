#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "sql.h"
#include "main_ui.h"

class QLabel;
class BangumiAPI;
class CacheImageUtil;
class DatabaseManager;

struct CardComponents {
    QFrame *card = nullptr;
    QLabel *coverLabel = nullptr;
    QLabel *titleLabel = nullptr;
    QLabel *progressLabel = nullptr;
    QPushButton *moreButton = nullptr;
    QPushButton *episodeButton = nullptr;
    bool inUse = false;
};

class MainPageManager : public QObject
{
    Q_OBJECT

public:
    explicit MainPageManager(Ui::MainWindow *mainWindow, CacheImageUtil *cacheImageUtil, BangumiAPI *bangumiAPI);
    void setupConnections();
    [[nodiscard]] CacheImageUtil *getCacheImageUtil() const {return cacheImageUtil;}
    [[nodiscard]] int getCurrentSubjectType() const {return currentSubjectType;}
    [[nodiscard]] int getCurrentStatusType() const {return currentStatusType;}

signals:
    void showDetailPageRequested(const CollectionData &collectionData);
    void showEpisodePageRequested(const CollectionData &collectionData);
    void showMainPageRequested();

public slots:
    void onSearchTextChanged(const QString &text);
    void previousPage();
    void nextPage();
    void loadCollections(int subjectType, int statusType, bool resetToFirstPage = true);

private:
    struct StatusFrameInfo {
        QFrame *frame = nullptr;
        QPushButton *statusButton = nullptr;
        QPushButton *countButton = nullptr;
    };
    void initCardPool();
    void initStatusFrames();
    void switchCategory(int subjectType, const QString &title);
    void updatePageInfo() const;
    void displayCurrentPage();
    void clearDisplayArea();
    void setupCardComponents(int cardIndex, const CollectionData &collection);
    static void setProgressText(QLabel* label, const CollectionData& collection);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void showDetailPage(const CollectionData &collectionData);
    void showEpisodePage(const CollectionData &collectionData);
    Ui::MainWindow *mainWindow = nullptr;
    DatabaseManager *dbManager = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    int currentSubjectType = 2;
    int currentStatusType = 3;
    int currentPage = 1;
    int itemsPerPage = 12;
    QVector<CollectionData> allCollections;
    QVector<CollectionData> filteredCollections;
    QVector<QWidget*> placeholderWidgets;
    QVector<CardComponents> cardPool;
    QMap<int, QMap<int, QString>> statusNamesMap;
    QMap<int, StatusFrameInfo> statusFrames;
};

#endif // MAIN_PAGE_H