#include "LineChartWidget.h"
#include "SmoothCurve.h"
#include "../Theme.h"

#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QCoreApplication>

LineChartWidget::LineChartWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(60);
}

void LineChartWidget::setValues(const QVector<double> &values)
{
    m_values = values;
    update();
}

void LineChartWidget::setRange(double minValue, double maxValue)
{
    if (minValue >= maxValue) {
        return;
    }

    m_minValue = minValue;
    m_maxValue = maxValue;
    update();
}

bool LineChartWidget::event(QEvent *e)
{
    if (e->type() == Theme::EventType) {
        update();
    }
    return QWidget::event(e);
}

void LineChartWidget::paintEvent(QPaintEvent *)
{
    if (m_values.size() < 2) {
        return;
    }

    const Theme theme = qApp->property("Theme").value<Theme>();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int margin = 10;
    const QRectF plotRect(margin, margin, width() - margin * 2, height() - margin * 2);

    // Horizontal gridlines.
    painter.setPen(QPen(theme.dock_border, 1));
    for (int i = 0; i <= 3; ++i) {
        const double y = plotRect.top() + plotRect.height() * i / 3.0;
        painter.drawLine(QPointF(plotRect.left(), y), QPointF(plotRect.right(), y));
    }

    // Map values into plot-space points.
    const double range = qMax(0.0001, m_maxValue - m_minValue);
    QVector<QPointF> points;
    points.reserve(m_values.size());
    for (int i = 0; i < m_values.size(); ++i) {
        const double x = plotRect.left() + plotRect.width() * i / double(m_values.size() - 1);
        const double t = (m_values[i] - m_minValue) / range;
        const double y = plotRect.bottom() - t * plotRect.height();
        points << QPointF(x, y);
    }

    const QPainterPath curve = SmoothCurve::createSmoothCurve(points);

    // Gradient fill under the curve.
    QPainterPath fillPath = curve;
    fillPath.lineTo(points.last().x(), plotRect.bottom());
    fillPath.lineTo(points.first().x(), plotRect.bottom());
    fillPath.closeSubpath();

    const QColor accent = theme.split_hover;
    QColor fillTop = accent;
    fillTop.setAlpha(90);
    QColor fillBottom = accent;
    fillBottom.setAlpha(0);

    QLinearGradient gradient(0, plotRect.top(), 0, plotRect.bottom());
    gradient.setColorAt(0.0, fillTop);
    gradient.setColorAt(1.0, fillBottom);

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawPath(fillPath);

    // The curve itself.
    painter.setPen(QPen(accent, 2));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(curve);

    // Point markers.
    painter.setPen(Qt::NoPen);
    painter.setBrush(accent);
    for (const QPointF &pt : points) {
        painter.drawEllipse(pt, 2.5, 2.5);
    }
}
