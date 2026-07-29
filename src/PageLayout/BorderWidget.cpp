#include "BorderWidget.h"
#include "HorizentalLayout.h"
#include "../Theme.h"

#include <QPainter>
#include <QCoreApplication>

BorderWidget::BorderWidget(QWidget *parent, BorderRateProvider *layout, Type type)
    : QWidget(parent)
    , m_provider(layout)
    , type(type)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void BorderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    Theme theme = qApp->property("Theme").value<Theme>();

    QPainter painter(this);
    painter.setPen(QPen(theme.split_border, theme.split_border_width));

    QList<double> m_rates = m_provider->borderRate();

    for (int i = 0; i < m_rates.size(); ++i) {
        int x;
        switch(type){
        case Type::Horizental:
            x = m_rates[i];
            painter.drawLine(x, 0, x, height());
            break;
        case Type::Vertical:
            x = m_rates[i];
            painter.drawLine(0, x, width(), x);
            break;
        }
    }
}