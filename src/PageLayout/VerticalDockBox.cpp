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
    borderAnimation.setDuration(100);
    connect(&borderAnimation, &ListAnimation::changed, [=](QList<double> value){
        if(value.count() != m_rates.count()) return;
        m_rates = value;
        updateChildGeometries();
    });
}

void VerticalDockBox::insertDock(DockWidget *dock, int index)
{
    dock->hide();
    dock->setParent(this);
    dock->show();

    if(m_docks.count() == 0){
        m_rates.append(0.);
        m_rates.append(1.);
        m_rates_target = m_rates;
    } else {
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
            QList<double> targetAnimation;
            const double newRate = 1.0 / (m_docks.count() + 1);
            const double scale = (1 - newRate);
            for(int i=0, len=m_docks.count() ; i<len ; i++){
                if (m_previewIndex <= i){
                    targetAnimation<<1. - (1. - m_rates_old[i * 2]) * scale;
                    targetAnimation<<1. - (1. - m_rates_old[i * 2 + 1]) * scale;
                } else {
                    targetAnimation<<m_rates_old[i * 2] * scale;
                    targetAnimation<<m_rates_old[i * 2 + 1] * scale;
                }
            }
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

int VerticalDockBox::borderPixelPosition(int index) const
{
    if (index < 0 || index >= m_docks.size() - 1) {
        return -1;
    }

    int borderCount = m_docks.size() - 1;

    int availableHeight =
        height() - borderCount * kBorderHitWidth;

    int y = 0;

    for (int i = 0; i <= index; ++i) {

        y += qRound(m_rates[i] * availableHeight);

        if (i < index) {
            y += kBorderHitWidth;
        }
    }

    return y + kBorderHitWidth / 2;
}


int VerticalDockBox::borderAtPosition(int y) const
{
    if (m_docks.size() < 2) {
        return -1;
    }

    int half =
        kBorderHitWidth / 2;

    for (int i = 0; i < m_docks.size() - 1; ++i) {

        int borderY =
            borderPixelPosition(i);

        if (qAbs(y - borderY) <= half) {
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


    for (int i = 0; i < m_docks.size() - 1; ++i) {

        int y =
            borderPixelPosition(i);

        bool hovered =
            (i == m_hoveredBorder ||
             i == m_draggingBorder);


        painter.fillRect(
            0,
            y - kBorderHitWidth / 2,
            width(),
            kBorderHitWidth,
            hovered
                ? kBorderHoverColor
                : kBorderColor
            );
    }

    if(m_previewIndex > -1){
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor("#0069D1")));
        if(m_docks.count() == 0){
            painter.drawRect(0,0,this->width(),this->height());
        } else {
            const double top = m_previewIndex == 0 ? 0 : m_rates[m_previewIndex * 2 - 1];
            const double bottom = m_previewIndex == m_docks.count() ? 1 : m_rates[m_previewIndex * 2];
            painter.drawRect(0,top * height(),this->width(),(bottom - top) * height());
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

        mouseMoveEvent(&converted);
    }
    return QWidget::eventFilter(
        obj,
        event
        );
}


void VerticalDockBox::mouseMoveEvent(QMouseEvent *event)
{

}


void VerticalDockBox::mousePressEvent(QMouseEvent *event)
{

}


void VerticalDockBox::mouseReleaseEvent(QMouseEvent *event)
{

}

void VerticalDockBox::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);


    if (m_draggingBorder < 0) {

        m_hoveredBorder = -1;

        unsetCursor();

        update();
    }
}

void VerticalDockBox::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    updateChildGeometries();
}