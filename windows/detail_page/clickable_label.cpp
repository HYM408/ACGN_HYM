#include "clickable_label.h"
#include <QMouseEvent>

ClickableLabel::ClickableLabel(const QString &text, QWidget *parent) : QLabel(text, parent)
{
    setTextFormat(Qt::RichText);
}

void ClickableLabel::mousePressEvent(QMouseEvent *event)
{   // 鼠标点击事件
    if (event->button() == Qt::LeftButton)
        emit clicked();
    QLabel::mousePressEvent(event);
}