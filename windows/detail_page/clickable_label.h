#ifndef CLICKABLE_LABEL_H
#define CLICKABLE_LABEL_H

#include <QLabel>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(const QString &text, QWidget *parent = nullptr);

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLE_LABEL_H