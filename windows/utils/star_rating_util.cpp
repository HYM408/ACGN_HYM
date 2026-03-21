#include "star_rating_util.h"
#include <QPainter>
#include <QMouseEvent>

StarRatingWidget::StarRatingWidget(const int maxStars, QWidget *parent) : QWidget(parent), m_maxStars(maxStars)
{
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    setMouseTracking(true);
    const int totalWidth = m_maxStars * m_starSize + (m_maxStars - 1) * m_spacing + 10;
    setFixedWidth(totalWidth);
    m_descriptions = {"不忍直视 1 (请谨慎评价)", "很差 2", "差 3", "较差 4", "不过不失 5", "还行 6", "推荐 7", "力荐 8", "神作 9", "超神作 10 (请谨慎评价)"};
    m_hoverWindow = new QWidget(nullptr, Qt::ToolTip);
    m_hoverLabel = new QLabel(m_hoverWindow);
    m_hoverWindow->hide();
}

StarRatingWidget::~StarRatingWidget()
{
    delete m_hoverWindow;
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
    const int index = starAtPosition(event->pos().x(), event->pos().y());
    if (index == -1) {
        if (m_hoverIndex == -1) return;
        m_hoverIndex = -1;
        m_hoverWindow->hide();
        update();
        return;
    }
    if (index == m_hoverIndex) return;
    m_hoverIndex = index;
    m_hoverLabel->setText(index < m_descriptions.size() ? m_descriptions.at(index) : QString());
    const QString color = index == 0 || index == 9 ? "red" : "white";
    m_hoverLabel->setStyleSheet(QString("QLabel {background-color: rgba(0, 0, 0, 100); color: %1; border-radius: 6px; padding: 6px 12px; font-size: 14px}").arg(color));
    m_hoverLabel->adjustSize();
    m_hoverWindow->resize(m_hoverLabel->size());
    const QRect starRect((width() - (m_maxStars * (m_starSize + m_spacing) - m_spacing)) / 2 + index * (m_starSize + m_spacing), (height() - m_starSize) / 2, m_starSize, m_starSize);
    const auto globalStarRect = QRect(mapToGlobal(starRect.topLeft()), starRect.size());
    m_hoverWindow->move(globalStarRect.center().x() - m_hoverWindow->width() / 2, globalStarRect.top() - m_hoverWindow->height());
    m_hoverWindow->show();
    m_hoverWindow->raise();
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
{   // 鼠标离开事件
    m_hoverIndex = -1;
    m_hoverWindow->hide();
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