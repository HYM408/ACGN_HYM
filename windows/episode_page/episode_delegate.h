#ifndef EPISODE_DELEGATE_H
#define EPISODE_DELEGATE_H

#include <QStyledItemDelegate>
#include "../sql/data_structs.h"

class EpisodeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EpisodeDelegate(QObject *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void episodeClicked(const EpisodeData &episodeData);
};

#endif // EPISODE_DELEGATE_H