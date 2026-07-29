#include "HoverWidget.h"
#include "../Theme.h"

#include <QPainter>
#include <QCoreApplication>

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

    Theme theme = qApp->property("Theme").value<Theme>();

    QPainter painter(this);
    QColor color = theme.split_hover;
    color.setAlphaF(m_opacity);
    painter.fillRect(rect(), color);
}