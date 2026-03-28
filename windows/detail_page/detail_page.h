#ifndef DETAIL_PAGE_H
#define DETAIL_PAGE_H

#include "detail_ui.h"
#include <QStack>
#include "../sql/data_structs.h"

class BangumiAPI;
class QListWidget;
class CacheImageUtil;
class DatabaseManager;
class GameMonitorUtil;
class StarRatingWidget;
class ScoreChartWidget;
class RatingChartWidget;

class DetailPage : public QWidget
{
    Q_OBJECT

public:
    explicit DetailPage(QWidget *parent = nullptr);
    void setCollectionData(int subjectId, const QString &progressText);
    void setManagers(CacheImageUtil *cacheImage, BangumiAPI *api, DatabaseManager *db, GameMonitorUtil *gameMonitor);
    void clearHistory();
    void resetUI();

signals:
    void backButtonClicked();
    void tagClicked(const QString &tag, int subjectType);
    void showEpisodePageRequested(const SubjectsData &subjectsData);
    void refresh();

private slots:
    void onEpisodeClicked();
    void onOpenBangumiPage() const;
    void onStatusButtonClicked();
    void clickOnTab(int index);
    void onBackButtonClicked();

protected:
    void applyTheme();
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void setupConnections();
    void mergeSubjectData(const SubjectsData& subjectData);
    void updateDetailPage(const QString &progressText);
    void setupScoreChart(const QVector<int> &scoreDetails, int total);
    void tagsDisplay(const QList<QPair<QString, int>> &tagPairs);
    static QString getTimeInfo(const QList<QPair<QString, int>> &tagPairs, const QString &dateStr);
    void onRatingButtonClicked();
    template<typename DataType, typename CreateFunc>
    void setupTabTemplate(QWidget *content, const QVector<DataType> &data, CreateFunc createCardFunc);
    void onCharacterTab();
    void onRelationTab();
    void onStaffTab();
    void clearLayout() const;
    static void clearTab(const QWidget *content) ;
    Ui::DetailPage ui{};
    CacheImageUtil *cacheImageUtil = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    DatabaseManager *dbManager = nullptr;
    GameMonitorUtil *gameMonitorUtil = nullptr;
    QListWidget *tagListWidget = nullptr;
    StarRatingWidget *m_starRating = nullptr;
    ScoreChartWidget *m_scoreChartWidget = nullptr;
    QStack<int> m_historyStack;
    SubjectsData m_subjectData;
    QMap<int, QMap<int, QString>> statusNamesMap;
    QVector<CharacterData> m_characters;
    QVector<SubjectRelationData> m_relations;
    QVector<PersonData> m_persons;
    bool characterTabInitialized = false;
    bool relationTabInitialized = false;
    bool staffTabInitialized = false;
    QList<QPair<QString, int>> m_currentTagPairs;
    QColor m_color2;
    QColor m_color3;
};

#endif // DETAIL_PAGE_H