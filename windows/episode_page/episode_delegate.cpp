#include "episode_delegate.h"
#include "episode_overlay.h"
#include <QPainter>

EpisodeDelegate::EpisodeDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void EpisodeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{   // 绘制
    painter->setRenderHint(QPainter::Antialiasing);
    QRect rect = option.rect;
    auto ep = qvariant_cast<EpisodeData>(index.data(EpisodeOverlay::EpisodeDataRole));
    int epType = ep.episodeType;
    bool isHovered = option.state & QStyle::State_MouseOver;
    QColor bgColor, textColor;
    if (ep.collectionType == 2) {
        bgColor = isHovered ? QColor(66, 155, 70) : QColor(76, 175, 80);
        textColor = Qt::white;
    } else {
        bgColor = isHovered ? QColor(240, 240, 240) : Qt::white;
        textColor = Qt::black;
    }
    painter->setBrush(bgColor);
    painter->setPen(QPen(QColor(221, 221, 221), 1));
    painter->drawRoundedRect(rect, 5, 5);
    painter->setPen(textColor);
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(9);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QString::number(ep.sort));
    if (epType != 0) {
        QFont smallFont = painter->font();
        smallFont.setPointSize(6);
        painter->setFont(smallFont);
        QColor typeColor;
        QString typeText;
        switch (epType) {
            case 1: typeColor = QColor(255, 152, 0); typeText = "SP"; break;
            case 2: typeColor = QColor(33, 150, 243); typeText = "OP"; break;
            case 3: typeColor = QColor(156, 39, 176); typeText = "ED"; break;
            default: typeColor = QColor(255, 87, 34); typeText = "?"; break;
        }
        painter->setPen(typeColor);
        painter->drawText(rect.adjusted(-2, 2, -2, 2), Qt::AlignTop | Qt::AlignRight, typeText);
    }
}

QSize EpisodeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {return {40, 40};}

bool EpisodeDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{   // 鼠标事件
    if (event->type() == QEvent::MouseButtonRelease) {
        emit episodeClicked(qvariant_cast<EpisodeData>(index.data(EpisodeOverlay::EpisodeDataRole)));
        return true;
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}