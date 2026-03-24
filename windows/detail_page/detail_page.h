#ifndef DETAIL_PAGE_H
#define DETAIL_PAGE_H

#include "detail_ui.h"
#include "score_chart_widget.h"
#include "../sql/data_structs.h"

class BangumiAPI;
class QListWidget;
class CacheImageUtil;
class DatabaseManager;
class GameMonitorUtil;
class StarRatingWidget;
class RatingChartWidget;

class DetailPage : public QWidget
{
    Q_OBJECT

public:
    explicit DetailPage(QWidget *parent = nullptr);
    void setCollectionData(const CollectionData &data, const QString &progressText);
    void setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db, GameMonitorUtil *gameMonitor);
    void resetUI();

signals:
    void backButtonClicked();
    void tagClicked(const QString &tag, int subjectType);
    void showEpisodePageRequested(const CollectionData &collectionData);
    void refresh();

private slots:
    void onEpisodeClicked();
    void onOpenBangumiPage() const;
    void onStatusButtonClicked();
    void clickOnTab(int index);

protected:
    void applyTheme();
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupConnections();
    void updateDetailPage(const SubjectsData &subjectData);
    void setupScoreChart(const QVector<int>& scoreDetails, int total);
    void tagsDisplay(const QList<QPair<QString, int>> &tagPairs);
    static QString getTimeInfo(const QList<QPair<QString, int>> &tagPairs, const QString &dateStr);
    void onRatingButtonClicked();
    void onCharacterTab();
    void onStaffTab();
    void clearLayout() const;
    static void clearTab(const QWidget *content) ;
    void loadData();
    void onBackButtonClicked();
    Ui::DetailPage ui{};
    CollectionData currentData;
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    DatabaseManager *dbManager = nullptr;
    GameMonitorUtil *gameMonitorUtil = nullptr;
    QListWidget *tagListWidget = nullptr;
    StarRatingWidget *m_starRating = nullptr;
    ScoreChartWidget *m_scoreChartWidget = nullptr;
    QMap<int, QMap<int, QString>> statusNamesMap;
    QVector<CharacterData> m_characters;
    QVector<PersonData> m_persons;
    bool characterTabInitialized = false;
    bool staffTabInitialized = false;
    QList<QPair<QString, int>> m_currentTagPairs;
    QColor m_color2;
    QColor m_color3;
};

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(const QString &text, QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // DETAIL_PAGE_H