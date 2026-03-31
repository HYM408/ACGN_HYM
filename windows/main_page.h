#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "main_ui.h"
#include "sql/data_structs.h"

class QLabel;
class BangumiAPI;
class CacheImageUtil;
class DatabaseManager;
class GameMonitorUtil;

class MainPageManager : public QObject
{
    Q_OBJECT

public:
    explicit MainPageManager(Ui::MainWindow *mainWindow, CacheImageUtil *cacheImageUtil, BangumiAPI *bangumiAPI, DatabaseManager *dbManager, GameMonitorUtil *gameMonitor);
    void applyTheme();
    [[nodiscard]] int getCurrentSubjectType() const {return m_currentSubjectType;}
    [[nodiscard]] int getCurrentStatusType() const {return m_currentStatusType;}

signals:
    void showDetailPageRequested(int subjectId, const QString &progressText);
    void showEpisodePageRequested(const SubjectsData &subjectsData);
    void showMainPageRequested();

public slots:
    void onSearchTextChanged(const QString &text, bool resetToFirstPage, bool displayPage);
    void previousPage();
    void nextPage();
    void loadCollections(int subjectType, int statusType, bool resetToFirstPage);

private:
    void setupConnections();
    void initStatus();
    void switchCategory(int subjectType, const QString &title);
    void updatePageInfo() const;
    void displayCurrentPage();
    QFrame* createCardComponents(SubjectsData &subjectsData);
    bool eventFilter(QObject *obj, QEvent *event) override;
    Ui::MainWindow *mainWindow = nullptr;
    DatabaseManager *dbManager = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    GameMonitorUtil *gameMonitor = nullptr;
    QVector<QPushButton*> m_statusButtons;
    QVector<QLabel*> m_statusLabels;
    QVector<QLabel*> m_statusCountLabels;
    int m_currentSubjectType = 2;
    int m_currentStatusType = 3;
    int m_currentPage = 1;
    QVector<SubjectsData> m_allCollections;
    QVector<SubjectsData> m_filteredCollections;
    QMap<int, GameData> m_gameData;
    QVector<QWidget*> m_placeholderWidgets;
    QJsonObject m_airdatesJson;
    QHash<int, QFrame*> m_gameCards;
    QColor m_color2;
    QColor m_color3;
};

#endif // MAIN_PAGE_H