#ifndef SEARCH_PAGE_H
#define SEARCH_PAGE_H

#include "search_ui.h"

class QLabel;
class BangumiAPI;
class CacheImageUtil;

class SearchPage : public QWidget
{
    Q_OBJECT

public:
    explicit SearchPage(QWidget *parent = nullptr);
    ~SearchPage() override;
    void setManagers(CacheImageUtil *cacheImage, BangumiAPI *api);
    void updateComboBoxByType(int currentType) const;
    void searchByTag(const QString &tag);

signals:
    void backButtonClicked();
    void showDetailPage(const QVariantMap &data);

private slots:
    void onSearchLineEditReturnPressed();
    void onBackButtonClicked();

private:
    void setupConnections();
    void doSearch(const QString &keyword = "", const QString &tag = "");
    void clearSearchResults();
    void showSearchStatus(const QString &text);
    QFrame *createResultFrame(const QVariantMap &result);
    bool eventFilter(QObject *watched, QEvent *event) override;
    Ui::SearchPage ui{};
    BangumiAPI *bangumiApi = nullptr;
    CacheImageUtil *cacheImageUtil = nullptr;
    QLabel *statusLabel = nullptr;
};

#endif // SEARCH_PAGE_H