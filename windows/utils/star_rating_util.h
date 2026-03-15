#ifndef STAR_RATING_UTIL_H
#define STAR_RATING_UTIL_H

#include <QLabel>

class StarRatingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StarRatingWidget(int maxStars, QWidget *parent = nullptr);
    ~StarRatingWidget() override;
    void setRate(int rate);

signals:
    void ratingSelected(int rate);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    [[nodiscard]] int starAtPosition(int x, int y) const;
    int m_maxStars;
    int m_rating = 0;
    int m_hoverIndex = -1;
    int m_starSize = 30;
    int m_spacing = 2;
    QWidget* m_hoverWindow;
    QLabel* m_hoverLabel;
    QStringList m_descriptions;
};

#endif // STAR_RATING_UTIL_H