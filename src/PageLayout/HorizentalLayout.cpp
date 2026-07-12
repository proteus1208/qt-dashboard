#include "HorizentalLayout.h"

#include <QMouseEvent>
#include <QPainter>
#include <QtMath>

const QColor HorizentalLayout::kBackgroundColor(0x1e, 0x1e, 0x1e);
const QColor HorizentalLayout::kBorderColor(0x3d, 0x3d, 0x3d);
const QColor HorizentalLayout::kBorderHoverColor(0x5a, 0x9f, 0xff, 0x99);

HorizentalLayout::HorizentalLayout(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAutoFillBackground(false);
}

void HorizentalLayout::addWidget(QWidget *widget)
{
    if (!widget || m_widgets.contains(widget)) {
        return;
    }

    widget->setParent(this);
    m_widgets.append(widget);
    m_rates.resize(m_widgets.size());

    const int count = m_widgets.size();
    const double equalRate = 1.0 / count;
    for (int i = 0; i < count; ++i) {
        m_rates[i] = equalRate;
    }
    m_rates.last() = 1.0 - equalRate * (count - 1);

    updateChildGeometries();
    update();
}

void HorizentalLayout::addWidget(QWidget &widget)
{
    addWidget(&widget);
}

void HorizentalLayout::updateChildGeometries()
{
    if (m_widgets.isEmpty()) {
        return;
    }

    const int totalWidth = width();
    const int totalHeight = height();
    const int gapCount = m_widgets.size() - 1;
    const int availableWidth = totalWidth - gapCount * kBorderHitWidth;
    int x = 0;

    for (int i = 0; i < m_widgets.size(); ++i) {
        int paneWidth = (i == m_widgets.size() - 1)
                            ? totalWidth - x
                            : qRound(m_rates[i] * availableWidth);
        m_widgets[i]->setGeometry(x, 0, paneWidth, totalHeight);
        x += paneWidth;
        if (i < gapCount) {
            x += kBorderHitWidth;
        }
    }
}

int HorizentalLayout::borderPixelPosition(int borderIndex) const
{
    const int gapCount = m_widgets.size() - 1;
    const int availableWidth = width() - gapCount * kBorderHitWidth;
    int position = 0;

    for (int i = 0; i <= borderIndex; ++i) {
        position += qRound(m_rates[i] * availableWidth);
        if (i < borderIndex) {
            position += kBorderHitWidth;
        }
    }

    return position + kBorderHitWidth / 2;
}

int HorizentalLayout::borderAtPosition(int x) const
{
    if (m_widgets.size() < 2) {
        return -1;
    }

    const int halfHit = kBorderHitWidth / 2;
    for (int i = 0; i < m_widgets.size() - 1; ++i) {
        const int borderX = borderPixelPosition(i);
        if (qAbs(x - borderX) <= halfHit) {
            return i;
        }
    }

    return -1;
}

void HorizentalLayout::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), kBackgroundColor);

    if (m_widgets.size() < 2) {
        return;
    }

    for (int i = 0; i < m_widgets.size() - 1; ++i) {
        const int borderX = borderPixelPosition(i);
        const bool hovered = (i == m_hoveredBorder) || (i == m_draggingBorder);

        if (hovered) {
            painter.fillRect(borderX - kBorderHitWidth / 2,
                             0,
                             kBorderHitWidth,
                             height(),
                             kBorderHoverColor);
        }

        painter.fillRect(borderX - kBorderLineWidth / 2,
                         0,
                         kBorderLineWidth,
                         height(),
                         hovered ? kBorderHoverColor : kBorderColor);
    }
}

void HorizentalLayout::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateChildGeometries();
}

void HorizentalLayout::mouseMoveEvent(QMouseEvent *event)
{
    if (m_draggingBorder >= 0) {
        const int deltaX = event->pos().x() - m_dragStartX;
        const int gapCount = m_widgets.size() - 1;
        const int availableWidth = width() - gapCount * kBorderHitWidth;
        const double deltaRate = static_cast<double>(deltaX) / availableWidth;

        const int leftIndex = m_draggingBorder;
        const int rightIndex = m_draggingBorder + 1;

        double newLeftRate = m_dragStartRates[leftIndex] + deltaRate;
        double newRightRate = m_dragStartRates[rightIndex] - deltaRate;

        if (newLeftRate >= kMinPaneRate && newRightRate >= kMinPaneRate) {
            m_rates[leftIndex] = newLeftRate;
            m_rates[rightIndex] = newRightRate;
            updateChildGeometries();
            update();
        }
        return;
    }

    const int border = borderAtPosition(event->pos().x());
    if (border != m_hoveredBorder) {
        m_hoveredBorder = border;
        update();
    }

    if (border >= 0) {
        setCursor(Qt::SplitHCursor);
    } else {
        unsetCursor();
    }
}

void HorizentalLayout::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    const int border = borderAtPosition(event->pos().x());
    if (border < 0) {
        return;
    }

    m_draggingBorder = border;
    m_dragStartX = event->pos().x();
    m_dragStartRates = m_rates;
    update();
}

void HorizentalLayout::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (m_draggingBorder >= 0) {
        m_draggingBorder = -1;
        m_dragStartRates.clear();
        update();
    }
}

void HorizentalLayout::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    if (m_draggingBorder < 0) {
        m_hoveredBorder = -1;
        unsetCursor();
        update();
    }
}
