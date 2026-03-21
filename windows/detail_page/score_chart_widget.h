#ifndef SCORE_CHART_WIDGET_H
#define SCORE_CHART_WIDGET_H

#include <QWidget>

class QLabel;

class ScoreChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScoreChartWidget(QWidget *parent = nullptr);
    ~ScoreChartWidget() override;
    void setData(const QVector<int> &scoreDetails, int total);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    [[nodiscard]] QRect barRect(int index, int barWidth) const;
    QVector<int> m_data;
    int m_total = 0;
    int m_hoveredDataIndex = -1;
    QWidget *m_tooltipWindow = nullptr;
    QLabel *m_tooltipLabel = nullptr;
};

#endif // SCORE_CHART_WIDGET_H