#include "VerticalDockBox.h"

#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QtMath>
#include <QObject>
#include <QTimer>

const QColor kBackgroundColor(0x1e, 0x1e, 0x1e);
const QColor kBorderColor(0x3d, 0x3d, 0x3d);
const QColor kBorderHoverColor(0x5a, 0x9f, 0xff, 0x99);

VerticalDockBox::VerticalDockBox(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setAutoFillBackground(false);

    m_borderWidget = new BorderWidget(this, this, BorderWidget::Type::Vertical);
    m_hoverWidget = new HoverWidget(this);

    m_borderWidget->setGeometry(rect());
    m_borderWidget->show();

    m_hoverWidget->setVisible(false);
    // m_hoverWidget->setOpacity(0.0);

    m_borderWidget->raise();
    m_hoverWidget->raise();

    borderAnimation.setDuration(100);

    connect(
        &borderAnimation,
        &ListAnimation::changed,
        this,
        [this](QList<double> value)
        {
            if (value.count() != m_rates.count()) {
                return;
            }

            m_rates = value;
            updateChildGeometries();

            // Hide static borders while rates are animating.
            m_borderWidget->hide();
            // m_hoverWidget->hide();
        }
    );

    connect(
        &borderAnimation,
        &ListAnimation::finished,
        this,
        [this]()
        {
            m_borderWidget->show();
            m_borderWidget->raise();
            m_borderWidget->update();
        }
    );
}

QList<double> VerticalDockBox::borderRate() const
{
    QList<double> result;

    if (borderAnimation.now_animating) {
        return result;
    }

    for(int i = 1, len = m_docks.count(); i<len ; i ++){
        result<<m_docks[i]->geometry().top() - 1;
    }
    return result;
}

void VerticalDockBox::insertDock(DockWidget *dock, int index)
{
    if(index < 0) index = m_docks.count();
    dock->hide();
    dock->setParent(this);
    dock->show();

    if(m_docks.count() == 0){
        m_rates.append(0.);
        m_rates.append(1.);
        m_rates_target = m_rates;
    } else {
        m_rates_target = getTarget(index);
        const double top1 = index == 0 ? 0 : m_rates[index * 2 - 1];
        const double bottom1 = index == m_docks.count() ? 1 : m_rates[index * 2];

        const double top2 = index == 0 ? 0 : m_rates_target[index * 2 - 1];
        const double bottom2 = index == m_docks.count() ? 1 : m_rates_target[index * 2];

        m_rates.insert(index * 2, top1);
        m_rates.insert(index * 2 + 1, bottom1);

        m_rates_target.insert(index * 2, top2);
        m_rates_target.insert(index * 2 + 1, bottom2);

        borderAnimation.setCurrentList(m_rates);
        borderAnimation.setTargetList(m_rates_target);
        borderAnimation.start();
    }
    m_docks.insert(index, dock);
    dock->installEventFilter(this);
    m_rates_old = m_rates_target;
    updateChildGeometries();
}


void VerticalDockBox::removeDock(DockWidget *dock)
{
    const int index = m_docks.indexOf(dock);
    const double dockHeight_rate = m_rates_old[index * 2 + 1] - m_rates_old[index * 2];
    if(index == -1) return;
    m_docks.removeAt(index);

    m_rates.removeAt(index * 2 + 1);
    m_rates.removeAt(index * 2);

    m_rates_old.removeAt(index * 2 + 1);
    m_rates_old.removeAt(index * 2);

    m_rates_target.removeAt(index * 2 + 1);
    m_rates_target.removeAt(index * 2);

    if(m_docks.count() == 0){
        m_rates.clear();
        m_rates_old.clear();
        m_rates_target.clear();
    } else {
        borderAnimation.setCurrentList(m_rates);
        QList<double> targetAnimation;
        const double scale = 1 / (1 - dockHeight_rate);
        for(int i=0, len=m_docks.count() ; i<len ; i++){
            if (i < index){
                targetAnimation<<m_rates_old[i * 2] * scale;
                targetAnimation<<m_rates_old[i * 2 + 1] * scale;
            } else {
                targetAnimation<<1. - (1. - m_rates_old[i * 2]) * scale;
                targetAnimation<<1. - (1. - m_rates_old[i * 2 + 1]) * scale;
            }
        }
        m_rates_old = targetAnimation;
        m_rates_target = targetAnimation;
        borderAnimation.setCurrentList(m_rates);
        borderAnimation.setTargetList(targetAnimation);
        borderAnimation.start();
    }

    dock->removeEventFilter(this);
}

QList<double> VerticalDockBox::getTarget(int index)
{
    if(m_docks.count() == 0) return QList<double>({0.0, 1.0});
    QList<double> targetAnimation;
    const double newRate = 1.0 / (m_docks.count() + 1);
    const double scale = (1 - newRate);
    for(int i=0, len=m_docks.count() ; i<len ; i++){
        if (index <= i){
            targetAnimation<<1. - (1. - m_rates_old[i * 2]) * scale;
            targetAnimation<<1. - (1. - m_rates_old[i * 2 + 1]) * scale;
        } else {
            targetAnimation<<m_rates_old[i * 2] * scale;
            targetAnimation<<m_rates_old[i * 2 + 1] * scale;
        }
    }

    return targetAnimation;
}

void VerticalDockBox::updateRatesAfterInsert(int index)
{
    const double newRate = 1 / (m_rates.count() + 1);
    for(int i = 0, len = m_rates.count(); i < len ; i++){
        m_rates[i] *= (1 - newRate) / 1;
    }
    m_rates.insert(index, newRate);
}

void VerticalDockBox::normalizeRates()
{
}

void VerticalDockBox::updateChildGeometries()
{
    for(int i = 0, len = m_docks.count(); i < len ; i ++){
        m_docks[i]->setGeometry(0, m_rates[i * 2] * height(), width(), (m_rates[i * 2 + 1] - m_rates[i * 2]) * height());
    }
}

int VerticalDockBox::calculateDockIndex(QPoint globalPos) const
{
    if (m_docks.isEmpty()) {
        return 0;
    }

    const int y = mapFromGlobal(globalPos).y();

    for (int i = 0; i < m_docks.size(); ++i)
    {
        QRect geo = m_docks[i]->geometry();
        int center = geo.center().y();

        if (y < center)
        {
            return i;
        }
    }

    return m_docks.size();
}

void VerticalDockBox::onEnterDock(DockWidget *dock, QPoint pos)
{
    m_previewIndex = calculateDockIndex(pos);

    onMoveDock(dock, pos);
}


void VerticalDockBox::onMoveDock(DockWidget *dock, QPoint pos)
{
    m_previewIndex = calculateDockIndex(pos);
    if(m_previewIndex_old != m_previewIndex){
        if(m_docks.count() > 0){
            QList<double> targetAnimation = getTarget(m_previewIndex);

            m_rates_target = targetAnimation;
            borderAnimation.setCurrentList(m_rates);
            borderAnimation.setTargetList(targetAnimation);
            borderAnimation.start();
        }

        m_previewIndex_old = m_previewIndex;
    }
    update();
}


void VerticalDockBox::onOutDock(DockWidget *dock, QPoint pos)
{
    borderAnimation.setCurrentList(m_rates);
    borderAnimation.setTargetList(m_rates_old);
    borderAnimation.start();

    m_previewIndex_old = m_previewIndex = -1;
    update();
}


void VerticalDockBox::onDropDock(DockWidget *dock, QPoint pos)
{
    const int newIndex = calculateDockIndex(pos);
    insertDock(dock, newIndex);
    m_previewIndex_old = m_previewIndex = -1;
    update();
}


void VerticalDockBox::updatePreview(int index)
{

}


void VerticalDockBox::restorePreview()
{

}

void VerticalDockBox::hideBorderHover()
{
    m_hoverWidget->setVisible(false);
    // m_hoverWidget->setOpacity(0.0);
}

void VerticalDockBox::showBorderHover(int borderIndex)
{
    const int borderY =
        borderPixelPosition(borderIndex);

    m_hoverWidget->setGeometry(
        0,
        borderY - kBorderHitWidth / 2,
        width(),
        kBorderHitWidth
        );

    m_hoverWidget->raise();

    // m_hoverWidget->setOpacity(0.0);
    m_hoverWidget->show();

    // If you have the same animation
    // as HorizentalLayout:
    //
    // m_hoverAnimation->stop();
    // m_hoverAnimation->setStartValue(0.0);
    // m_hoverAnimation->setEndValue(1.0);
    // m_hoverAnimation->start();
}
int VerticalDockBox::borderPixelPosition(int index) const
{
    QList<double> borderRate = this->borderRate();

    if (borderRate.count() == 0 || index >= borderRate.count()) {
        return -1;
    }

    return borderRate[index];
}


int VerticalDockBox::borderAtPosition(int y) const
{
    QList<double> borderRate = this->borderRate();

    if (borderRate.count() == 0) {
        return -1;
    }

    for (int i = 0, len = borderRate.count(); i < len ; ++i) {

        int borderY =
            borderPixelPosition(i);

        if (qAbs(y - borderY) <= 3) {
            return i;
        }
    }

    return -1;
}


void VerticalDockBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(
        rect(),
        kBackgroundColor
        );

    if (m_previewIndex > -1) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(
            QBrush(QColor("#0069D1"))
            );

        if (m_docks.isEmpty()) {
            painter.drawRect(
                0,
                0,
                width(),
                height()
                );
        } else {
            const double top =
                m_previewIndex == 0
                    ? 0
                    : m_rates[m_previewIndex * 2 - 1];

            const double bottom =
                m_previewIndex == m_docks.count()
                    ? 1
                    : m_rates[m_previewIndex * 2];

            painter.drawRect(
                0,
                top * height(),
                width(),
                (bottom - top) * height()
                );
        }
    }
}


bool VerticalDockBox::eventFilter(QObject *obj, QEvent *event)
{
    DockWidget* dock = qobject_cast<DockWidget*>(obj);

    if (!dock || !m_docks.contains(dock))
    {
        return QWidget::eventFilter(obj, event);
    }

    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mouse =
            static_cast<QMouseEvent*>(event);

        QPoint local =
            mapFromGlobal(
                mouse->globalPosition().toPoint()
                );

        QMouseEvent converted(
            QEvent::MouseMove,
            local,
            mouse->globalPosition(),
            mouse->button(),
            mouse->buttons(),
            mouse->modifiers()
            );

//        mouseMoveEvent(&converted);
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

        const int borderIndex = borderAtPosition(pos.y());
        if(borderIndex > -1){
            return true;
        }
    }
    return QWidget::eventFilter(
        obj,
        event
        );
}


void VerticalDockBox::mouseMoveEvent(QMouseEvent *event)
{
    const int y = event->pos().y();

    if (m_draggingBorder >= 0)
    {
        const int borderIndex = m_draggingBorder;

        const int rateIndex = borderIndex * 2 + 1;

        double rate =
            static_cast<double>(y) /
            static_cast<double>(height());

        const double minRateC = 32. / height();

        const double minRate =
            m_rates[rateIndex - 1] + minRateC;

        const double maxRate =
            m_rates[rateIndex + 2] - minRateC;

        rate = qBound(
            minRate,
            rate,
            maxRate
            );

        // Update both copies of the border rate.
        m_rates[rateIndex] = rate;
        m_rates[rateIndex + 1] = rate;

        m_rates_old = m_rates;

        updateChildGeometries();

        const int borderY =
            borderPixelPosition(borderIndex);

        m_hoverWidget->move(
            0,
            borderY - kBorderHitWidth / 2
            );

        return;
    }

    const int border =
        borderAtPosition(y);

    if (border != m_hoveredBorder)
    {
        if (border >= 0)
        {
            m_hoveredBorder = border;
            showBorderHover(border);
        }
        else
        {
            m_hoveredBorder = -1;
            hideBorderHover();
        }
    }

    if (border >= 0) {
        setCursor(Qt::SplitVCursor);
    }
    else {
        unsetCursor();
    }
}


void VerticalDockBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    const int border = borderAtPosition(event->pos().y());

    if (border < 0) {
        return;
    }

    m_draggingBorder = border;
    setCursor(Qt::SplitVCursor);

    // qApp->installEventFilter(this);
    QWidget::mousePressEvent(event);
}


void VerticalDockBox::mouseReleaseEvent(QMouseEvent *event)
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

void VerticalDockBox::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);

    if (m_draggingBorder < 0) {
        m_hoveredBorder = -1;

        hideBorderHover();

        unsetCursor();
    }
}

void VerticalDockBox::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    updateChildGeometries();

    m_borderWidget->setGeometry(rect());

    if (m_hoveredBorder >= 0) {
        const int borderY = borderPixelPosition(m_hoveredBorder);

        m_hoverWidget->setGeometry(
            0,
            borderY - kBorderHitWidth / 2,
            width(),
            kBorderHitWidth
        );
    }
}