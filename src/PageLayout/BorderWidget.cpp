#include "BorderWidget.h"
#include "HorizentalLayout.h"

#include <QPainter>

const QColor kBorderColor(0x3d, 0x3d, 0x3d);

BorderWidget::BorderWidget(QWidget *parent, BorderRateProvider *layout)
    : QWidget(parent)
    , m_provider(layout)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void BorderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setPen(QPen(kBorderColor, 1));

    QList<double> m_rates = m_provider->borderRate();

    for (int i = 0; i < m_rates.size(); ++i) {
        const int x = qRound(width() * m_rates[i]);
        painter.drawLine(x, 0, x, height());
    }
}