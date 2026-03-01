#ifndef EPISODE_PAGE_H
#define EPISODE_PAGE_H

#include "episode_ui.h"
#include <QListView>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include "main_page.h"

class EpisodeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EpisodeDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void episodeClicked(const QJsonObject &episodeData);
};

class EpisodeOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit EpisodeOverlay(QWidget *parent = nullptr);
    void setManagers(DatabaseManager *db, BangumiAPI *api);
    void showWithCollectionData(const CollectionData &collData);
    void closeOverlay();
    enum EpisodeRole {EpisodeDataRole = Qt::UserRole + 1};

signals:
    void episodeClicked(const QJsonObject &collectionData, const QJsonObject &episodeData);
    void overlayClosed();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void loadEpisodesData();
    void loadVolEpData();
    void updateEpisodeView();
    void onEpisodeItemClicked(const QJsonObject &episodeData);
    void onMarkAllWatchedClicked();
    void onUpdateClicked() const;
    Ui::EpisodePage ui{};
    DatabaseManager *dbManager = nullptr;
    BangumiAPI *bangumiAPI = nullptr;
    QWidget *episodeContainer = nullptr;
    QJsonObject collectionData;
    QJsonArray episodes;
    QLabel *noEpisodesLabel = nullptr;
    QListView *episodeListView = nullptr;
    QStandardItemModel *episodeModel = nullptr;
    EpisodeDelegate *episodeDelegate = nullptr;
    const int BUTTON_SIZE = 40;
    const int SPACING = 7;
    const int COLUMNS = 12;
    const int MAX_CONTAINER_HEIGHT = 900;
    const int MIN_CONTAINER_HEIGHT = 200;
    QLineEdit *volEdit = nullptr;
    QLineEdit *epEdit = nullptr;
};

#endif // EPISODE_PAGE_H