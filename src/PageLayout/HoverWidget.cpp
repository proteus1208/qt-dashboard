#include "HoverWidget.h"

#include <QPainter>

HoverWidget::HoverWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

qreal HoverWidget::opacity() const
{
    return m_opacity;
}

void HoverWidget::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    update();
}

void HoverWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QColor color(0x00, 0x8c, 0xff);
    color.setAlphaF(m_opacity);
    painter.fillRect(rect(), color);
}