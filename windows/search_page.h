#ifndef SEARCH_PAGE_H
#define SEARCH_PAGE_H

#include "search_ui.h"
#include "sql/data_structs.h"

class QLabel;
class BangumiAPI;
class DetailPage;
class CacheImageUtil;
class DatabaseManager;
class GameMonitorUtil;

class SearchPage : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPage(QWidget *parent = nullptr);
    void setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db, GameMonitorUtil *gameMonitor);
    void applyTheme();
    void updateComboBoxByType(int currentType) const;
    void searchByTag(const QString &tag, int subjectType);

public slots:
    void updateNsfwCheckBox(bool checked) const;

signals:
    void backButtonClicked();
    void showEpisodePageRequested(const SubjectsData &subjectsData);

private slots:
    void onSearchLineEditReturnPressed();
    void onBackButtonClicked();

private:
    void setupConnections();
    void searchSubject(const QString &keyword, const QString &tag);
    QPushButton *createResult(const QVariantMap &result);
    void showSearchStatus(const QString &text);
    void clearSearchResults();
    Ui::SearchPage ui{};
    BangumiAPI *bangumiApi = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    DatabaseManager *dbManager = nullptr;
    GameMonitorUtil *gameMonitorUtil = nullptr;
    QLabel *statusLabel = nullptr;
    DetailPage *detailPage = nullptr;
    QColor m_color2;
    QColor m_color3;
};

#endif // SEARCH_PAGE_H