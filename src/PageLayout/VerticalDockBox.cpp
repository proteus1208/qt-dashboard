#include "VerticalDockBox.h"

#include <QPainter>

VerticalDockBox::VerticalDockBox(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    setAutoFillBackground(false);
}

void VerticalDockBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(rect(), QColor(255, 255, 255, 40));
}
