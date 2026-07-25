#include "BorderWidget.h"
#include "HorizentalLayout.h"

#include <QPainter>

BorderWidget::BorderWidget(HorizentalLayout *layout)
    : QWidget(layout)
    , m_layout(layout)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void BorderWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setPen(QPen(m_layout->kBorderColor, 1));

    for (int i = 0; i < m_layout->m_widgets.size() - 1; ++i) {
        const int x = m_layout->borderPixelPosition(i);
        painter.drawLine(x, 0, x, height());
    }
}