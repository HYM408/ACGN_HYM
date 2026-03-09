#include "star_rating_util.h"
#include <QPainter>
#include <QMouseEvent>

StarRatingWidget::StarRatingWidget(const int maxStars, QWidget *parent) : QWidget(parent), m_maxStars(maxStars)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);
    const int totalWidth = m_maxStars * m_starSize + (m_maxStars - 1) * m_spacing + 10;
    const int totalHeight = m_starSize + 5;
    setFixedSize(totalWidth, totalHeight);
}

void StarRatingWidget::setRate(const int rate)
{   // 设置评分
    m_rating = rate;
    update();
}

void StarRatingWidget::paintEvent(QPaintEvent *)
{   // 绘制事件
    QPainter painter(this);
    const QFont font("Segoe UI Symbol", 25);
    painter.setFont(font);
    const int startX = (width() - (m_maxStars * (m_starSize + m_spacing) - m_spacing)) / 2;
    const int starY = (height() - m_starSize) / 2;
    for (int i = 0; i < m_maxStars; ++i) {
        const bool filled = m_hoverIndex >= 0 ? i <= m_hoverIndex : i < m_rating;
        QString starChar = filled ? "★" : "☆";
        painter.setPen(QColor(255, 215, 0));
        painter.drawText(QRect(startX + i * (m_starSize + m_spacing), starY, m_starSize, m_starSize), Qt::AlignCenter, starChar);
    }
}

void StarRatingWidget::mouseMoveEvent(QMouseEvent *event)
{   // 鼠标移动事件
    const QPoint pos = event->position().toPoint();
    const int index = starAtPosition(pos.x(), pos.y());
    if (index == m_hoverIndex) return;
    m_hoverIndex = index;
    update();
}

void StarRatingWidget::mousePressEvent(QMouseEvent *event)
{   // 鼠标点击事件
    if (event->button() == Qt::LeftButton) {
        const QPoint pos = event->position().toPoint();
        const int index = starAtPosition(pos.x(), pos.y());
        if (index >= 0) emit ratingSelected(index + 1);
    }
    close();
}

void StarRatingWidget::leaveEvent(QEvent *)
{   // 当鼠标离开事件
    m_hoverIndex = -1;
    update();
}

int StarRatingWidget::starAtPosition(const int x, const int y) const
{   // 星星位置
    const int totalWidth = m_maxStars * (m_starSize + m_spacing);
    const int startX = (width() - (totalWidth - m_spacing)) / 2;
    const int starY = (height() - m_starSize) / 2;
    if (x < startX || x >= startX + totalWidth || y < starY || y > starY + m_starSize) return -1;
    return (x - startX) / (m_starSize + m_spacing);
}