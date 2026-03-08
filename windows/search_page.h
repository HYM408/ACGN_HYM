#ifndef SEARCH_PAGE_H
#define SEARCH_PAGE_H

#include "search_ui.h"
#include "sql/data_structs.h"

class QLabel;
class BangumiAPI;
class DetailPage;
class CacheImageUtil;
class DatabaseManager;

class SearchPage : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPage(QWidget *parent = nullptr);
    ~SearchPage() override;
    void setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db);
    void updateComboBoxByType(int currentType) const;
    void searchByTag(const QString &tag, int subjectType);

signals:
    void backButtonClicked();
    void showEpisodePageRequested(const CollectionData &collectionData);

private slots:
    void onSearchLineEditReturnPressed();
    void onBackButtonClicked();

private:
    void setupConnections();
    void doSearch(const QString &keyword, const QString &tag);
    void clearSearchResults();
    void showSearchStatus(const QString &text);
    QFrame *createResultFrame(const QVariantMap &result);
    bool eventFilter(QObject *watched, QEvent *event) override;
    Ui::SearchPage ui{};
    BangumiAPI *bangumiApi = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    DatabaseManager *dbManager = nullptr;
    QLabel *statusLabel = nullptr;
    DetailPage *detailPage = nullptr;
    QMap<int, QVariantMap> resultDataMap;
};

#endif // SEARCH_PAGE_H