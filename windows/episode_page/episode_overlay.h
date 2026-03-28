#ifndef EPISODE_OVERLAY_H
#define EPISODE_OVERLAY_H

#include "episode_ui.h"
#include "../sql/data_structs.h"
#include <QWidget>

class QListView;
class BangumiAPI;
class DatabaseManager;
class EpisodeDelegate;
class QStandardItemModel;

class EpisodeOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit EpisodeOverlay(QWidget *parent = nullptr);
    void setManagers(BangumiAPI *api, DatabaseManager *db);
    void applyTheme() const;
    void showWithSubjectsData(const SubjectsData &sData);
    void closeOverlay();
    enum EpisodeRole {EpisodeDataRole = Qt::UserRole + 1};

signals:
    void episodeClicked(const SubjectsData &subjectsData, const EpisodeData &episodeData);
    void overlayClosed();
    void collectionDataChanged();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onUpdateButtonClicked();

private:
    void loadEpisodesData();
    void loadVolEpData();
    void updateEpisodeView();
    void onEpisodeItemClicked(const EpisodeData &episodeData);
    void onMarkAllWatchedClicked();
    void onUpdateClicked();
    Ui::EpisodePage ui{};
    BangumiAPI *bangumiAPI = nullptr;
    DatabaseManager *dbManager = nullptr;
    QWidget *episodeContainer = nullptr;
    SubjectsData subjectsData;
    QVector<EpisodeData> episodes;
    QLabel *noEpisodesLabel = nullptr;
    QListView *episodeListView = nullptr;
    QStandardItemModel *episodeModel = nullptr;
    EpisodeDelegate *episodeDelegate = nullptr;
    QLineEdit *volEdit = nullptr;
    QLineEdit *epEdit = nullptr;
    QWidget *volContainer = nullptr;
};

#endif // EPISODE_OVERLAY_H