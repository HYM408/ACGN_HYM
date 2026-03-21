#include "score_chart_widget.h"
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>

static constexpr int BAR_COUNT = 10;
static constexpr int LEFT_MARGIN = 10;
static constexpr int RIGHT_MARGIN = 10;
static constexpr int TOP_MARGIN = 15;
static constexpr int BOTTOM_MARGIN = 25;
static constexpr int BAR_SPACING = 2;

ScoreChartWidget::ScoreChartWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    setFixedSize(300, 160);
    m_tooltipWindow = new QWidget(nullptr, Qt::ToolTip);
    m_tooltipLabel = new QLabel(m_tooltipWindow);
    m_tooltipLabel->setStyleSheet("QLabel {background-color: rgba(0, 0, 0, 100); color: white; border-radius: 6px; padding: 6px 12px; font-size: 12px}");
    m_tooltipWindow->hide();
}

ScoreChartWidget::~ScoreChartWidget()
{
    delete m_tooltipWindow;
}

void ScoreChartWidget::setData(const QVector<int> &scoreDetails, const int total)
{
    m_data = scoreDetails;
    m_total = total;
    m_hoveredDataIndex = -1;
    m_tooltipWindow->hide();
    update();
}

QRect ScoreChartWidget::barRect(const int index, int barWidth) const
{
    return {LEFT_MARGIN + index * (barWidth + BAR_SPACING), TOP_MARGIN, barWidth, height() - TOP_MARGIN - BOTTOM_MARGIN};
}

void ScoreChartWidget::paintEvent(QPaintEvent *)
{   // 绘制事件
    QPainter painter(this);
    const int maxVal = *std::max_element(m_data.constBegin(), m_data.constEnd());
    if (maxVal == 0) return;
    const int chartHeight = height() - TOP_MARGIN - BOTTOM_MARGIN;
    const int barWidth = (width() - LEFT_MARGIN - RIGHT_MARGIN) / BAR_COUNT - BAR_SPACING;
    const int bottomY = height() - BOTTOM_MARGIN;
    for (int i = 0; i < BAR_COUNT; ++i) {
        const int dataIndex = BAR_COUNT - 1 - i;
        int barHeight = m_data[dataIndex] * chartHeight / maxVal;
        if (barHeight == 0 && m_data[dataIndex] > 0) barHeight = 1;
        QRect rect = barRect(i, barWidth);
        painter.fillRect(rect.x(), rect.y() + chartHeight - barHeight, rect.width(), barHeight, QColor(153, 153, 153));
        QRect labelRect(rect.x() + rect.width() / 2 - 8, bottomY + 5, 16, 15);
        painter.drawText(labelRect, Qt::AlignCenter, QString::number(BAR_COUNT - i));
    }
}

void ScoreChartWidget::mouseMoveEvent(QMouseEvent *event)
{   // 鼠标移动事件
    const int barWidth = (width() - LEFT_MARGIN - RIGHT_MARGIN) / BAR_COUNT - BAR_SPACING;
    int hoveredIndex = -1;
    for (int i = 0; i < BAR_COUNT; ++i) {
        if (barRect(i, barWidth).contains(event->pos())) {
            hoveredIndex = i;
            break;
        }
    }
    const int newHovered = hoveredIndex != -1 ? BAR_COUNT - 1 - hoveredIndex : -1;
    if (newHovered == m_hoveredDataIndex) return;
    m_hoveredDataIndex = newHovered;
    if (m_hoveredDataIndex != -1 && m_total > 0) {
        const int count = m_data[m_hoveredDataIndex];
        m_tooltipLabel->setText(QString("%1%(%2人)").arg(100.0 * count / m_total, 0, 'f', 2).arg(count));
        m_tooltipLabel->adjustSize();
        m_tooltipWindow->adjustSize();
        const QRect rect = barRect(BAR_COUNT - 1 - m_hoveredDataIndex, barWidth);
        const QPoint globalPos = mapToGlobal(QPoint(rect.center().x(), rect.top()));
        m_tooltipWindow->move(globalPos.x() - m_tooltipWindow->width() / 2, globalPos.y() - m_tooltipWindow->height() - 4);
        m_tooltipWindow->show();
    } else m_tooltipWindow->hide();
    QWidget::mouseMoveEvent(event);
}

void ScoreChartWidget::leaveEvent(QEvent *event)
{   // 离开事件
    if (m_hoveredDataIndex != -1) {
        m_hoveredDataIndex = -1;
        m_tooltipWindow->hide();
    }
    QWidget::leaveEvent(event);
}