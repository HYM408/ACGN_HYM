#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "main_ui.h"
#include "sql/data_structs.h"

class QLabel;
class BangumiAPI;
class CacheImageUtil;
class DatabaseManager;
class GameMonitorUtil;

struct CardComponents {
    QFrame *card = nullptr;
    QLabel *coverLabel = nullptr;
    QLabel *titleLabel = nullptr;
    QLabel *progressLabel = nullptr;
    QPushButton *moreButton = nullptr;
    QPushButton *episodeButton = nullptr;
};

class MainPageManager : public QObject
{
    Q_OBJECT

public:
    explicit MainPageManager(Ui::MainWindow *mainWindow, CacheImageUtil *cacheImageUtil, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, GameMonitorUtil *gameMonitor);
    void applyTheme();
    void setupConnections();
    [[nodiscard]] CacheImageUtil *getCacheImageUtil() const {return cacheImageUtil;}
    [[nodiscard]] int getCurrentSubjectType() const {return currentSubjectType;}
    [[nodiscard]] int getCurrentStatusType() const {return currentStatusType;}

signals:
    void showDetailPageRequested(const int &subject_id, const CollectionData &searchData, const QString &progressText);
    void showEpisodePageRequested(const CollectionData &collectionData);
    void showMainPageRequested();

public slots:
    void onSearchTextChanged(const QString &text);
    void previousPage();
    void nextPage();
    void loadCollections(int subjectType, int statusType, bool resetToFirstPage);

private:
    struct StatusFrameInfo {
        QFrame *frame = nullptr;
        QLabel *statusButton = nullptr;
        QLabel *countButton = nullptr;
    };
    void initStatusFrames();
    void switchCategory(int subjectType, const QString &title);
    void updatePageInfo() const;
    void displayCurrentPage();
    void clearDisplayArea();
    void createCardComponents(CardComponents &card, const CollectionData &collection);
    bool eventFilter(QObject *obj, QEvent *event) override;
    Ui::MainWindow *mainWindow = nullptr;
    DatabaseManager *dbManager = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    GameMonitorUtil *gameMonitor = nullptr;
    int currentSubjectType = 2;
    int currentStatusType = 3;
    int currentPage = 1;
    int itemsPerPage = 12;
    QVector<CollectionData> allCollections;
    QVector<CollectionData> filteredCollections;
    QVector<QWidget*> placeholderWidgets;
    QMap<int, QMap<int, QString>> statusNamesMap;
    QMap<int, StatusFrameInfo> statusFrames;
    QJsonObject airdatesJson;
    QColor m_color2;
    QColor m_color3;
    QColor m_color6;
    QString m_color6A;
};

#endif // MAIN_PAGE_H