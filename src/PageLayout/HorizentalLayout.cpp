#include "HorizentalLayout.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QtMath>
#include "HoverWidget.h"
#include "BorderWidget.h"

const QColor kBackgroundColor(0x1e, 0x1e, 0x1e);

HorizentalLayout::HorizentalLayout(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAutoFillBackground(false);

    m_borderWidget = new BorderWidget(this, this);
    m_hoverWidget = new HoverWidget(this);

    m_borderWidget->setGeometry(rect());
    m_borderWidget->show();
    m_hoverWidget->setVisible(false);
    m_hoverWidget->setOpacity(0.0);

    m_hoverAnimation = new QPropertyAnimation(m_hoverWidget, "opacity", this);
    m_hoverAnimation->setDuration(200);

    m_borderWidget->raise();
    m_hoverWidget->raise();
}

void HorizentalLayout::addWidget(QWidget *widget)
{
    if (!widget || m_widgets.contains(widget)) {
        return;
    }

    widget->installEventFilter(this);
    widget->setParent(this);
    m_widgets.append(widget);

    const int count = m_widgets.size();
    m_rates.resize(count + 1);

    for (int i = 0; i <= count; ++i) {
        m_rates[i] = static_cast<double>(i) / count;
    }

    updateChildGeometries();
    m_borderWidget->raise();
    m_hoverWidget->raise();
    update();
}

void HorizentalLayout::addWidget(QWidget &widget)
{
    addWidget(&widget);
}

QList<double> HorizentalLayout::borderRate() const
{
    QList<double> border_rate;
    for(int i = 1, len = m_widgets.count(); i<len ; i ++){
        border_rate<<m_widgets[i]->geometry().left();
    }
    return border_rate;
}

void HorizentalLayout::updateChildGeometries()
{
    if (m_widgets.isEmpty()) {
        return;
    }

    const int totalWidth = width();
    const int totalHeight = height();

    for (int i = 0; i < m_widgets.size(); ++i) {
        const int left = qRound(m_rates[i] * totalWidth);
        const int right = qRound(m_rates[i + 1] * totalWidth);
        m_widgets[i]->setGeometry(left, 0, right - left, totalHeight);
    }
}

int HorizentalLayout::borderPixelPosition(int borderIndex) const
{
    if (borderIndex < 0 || borderIndex >= m_widgets.size() - 1) {
        return -1;
    }

    return qRound(width() * m_rates[borderIndex + 1]);
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
}

void HorizentalLayout::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateChildGeometries();
    m_borderWidget->setGeometry(rect());

    if (m_hoveredBorder >= 0) {
        const int borderX = borderPixelPosition(m_hoveredBorder);
        m_hoverWidget->setGeometry(borderX - kBorderHitWidth / 2, 0, kBorderHitWidth, height());
    }
}

void HorizentalLayout::mouseMoveEvent(QMouseEvent *event)
{
    const int x = event->pos().x();

    if (m_draggingBorder >= 0) {
        const int rateIndex = m_draggingBorder + 1;
        double rate = static_cast<double>(x) / width();

        const double minRate = m_rates[rateIndex - 1] + kMinPaneRate;
        const double maxRate = m_rates[rateIndex + 1] - kMinPaneRate;

        rate = qBound(minRate, rate, maxRate);
        m_rates[rateIndex] = rate;

        updateChildGeometries();

        const int borderX = borderPixelPosition(m_draggingBorder);
        m_hoverWidget->move(borderX - kBorderHitWidth / 2, 0);

        return;
    }

    const int border = borderAtPosition(x);

    if (border != m_hoveredBorder) {
        if (border >= 0) {
            m_hoveredBorder = border;
            showBorderHover(border);
        } else {
            m_hoveredBorder = -1;
            hideBorderHover();
        }
    }

    if (border >= 0) {
        setCursor(Qt::SplitHCursor);
    } else {
        unsetCursor();
    }

    QWidget::mousePressEvent(event);
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
    setCursor(Qt::SplitHCursor);

    qApp->installEventFilter(this);
    QWidget::mousePressEvent(event);
}

void HorizentalLayout::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    if (m_draggingBorder >= 0) {
        m_draggingBorder = -1;
        unsetCursor();
        qApp->removeEventFilter(this);

        event->accept();
        return;
    }

    QWidget::mouseReleaseEvent(event);
}

void HorizentalLayout::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    if (m_draggingBorder < 0) {
        m_hoveredBorder = -1;
        hideBorderHover();
        unsetCursor();
    }
}

void HorizentalLayout::showBorderHover(int borderIndex)
{
    const int borderX = borderPixelPosition(borderIndex);

    m_hoverWidget->setGeometry(borderX - kBorderHitWidth / 2, 0, kBorderHitWidth, height());
    m_hoverWidget->raise();
    m_hoverWidget->setOpacity(0.0);
    m_hoverWidget->show();

    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(0.0);
    m_hoverAnimation->setEndValue(1.0);
    m_hoverAnimation->start();
}

void HorizentalLayout::onHoverAnimationFinished()
{
    if (m_hoverWidget->opacity() == 0.0) {
        m_hoverWidget->hide();
    }
}

void HorizentalLayout::hideBorderHover()
{
    m_hoverAnimation->stop();
    m_hoverWidget->setVisible(false);
    m_hoverWidget->setOpacity(0.0);
}

bool HorizentalLayout::eventFilter(QObject *obj, QEvent *event)
{
    // Border is currently being dragged.
    // Capture mouse movement and release globally.
    if (m_draggingBorder >= 0) {

        if (event->type() == QEvent::MouseMove) {
            auto *mouseEvent =
                static_cast<QMouseEvent *>(event);

            const QPoint pos =
                mapFromGlobal(
                    mouseEvent->globalPosition().toPoint()
                    );

            QMouseEvent translatedEvent(
                QEvent::MouseMove,
                pos,
                mouseEvent->button(),
                mouseEvent->buttons(),
                mouseEvent->modifiers()
                );

            mouseMoveEvent(&translatedEvent);

            // We handled the event.
            return true;
        }

        if (event->type() == QEvent::MouseButtonRelease) {
            auto *mouseEvent =
                static_cast<QMouseEvent *>(event);

            if (mouseEvent->button() == Qt::LeftButton) {
                mouseReleaseEvent(mouseEvent);

                // We handled the event.
                return true;
            }
        }
    }

    // Ignore events coming from the visual overlay widgets.
    if (obj == m_hoverWidget ||
        obj == m_borderWidget) {
        return false;
    }

    QWidget *widget =
        qobject_cast<QWidget *>(obj);

    if (!widget) {
        return QWidget::eventFilter(obj, event);
    }

    // Convert child-widget mouse coordinates
    // into HorizentalLayout coordinates.
    if (event->type() == QEvent::MouseMove) {
        auto *mouseEvent =
            static_cast<QMouseEvent *>(event);

        const QPoint pos =
            mapFromGlobal(
                mouseEvent->globalPosition().toPoint()
                );

        QMouseEvent translatedEvent(
            QEvent::MouseMove,
            pos,
            mouseEvent->button(),
            mouseEvent->buttons(),
            mouseEvent->modifiers()
            );

        mouseMoveEvent(&translatedEvent);
    }

    else if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent =
            static_cast<QMouseEvent *>(event);

        const QPoint pos =
            mapFromGlobal(
                mouseEvent->globalPosition().toPoint()
                );

        QMouseEvent translatedEvent(
            QEvent::MouseButtonPress,
            pos,
            mouseEvent->button(),
            mouseEvent->buttons(),
            mouseEvent->modifiers()
            );

        mousePressEvent(&translatedEvent);
    }

    return false;
}