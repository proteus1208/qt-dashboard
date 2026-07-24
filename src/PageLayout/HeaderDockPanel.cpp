#include "HeaderDockPanel.h"

#include "HeaderImage.h"
#include "VerticalDockBox.h"

#include <QResizeEvent>
#include <QtMath>

HeaderDockPanel::HeaderDockPanel(QWidget *parent)
    : QWidget(parent)
{
    setMinimumWidth(0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_header = new HeaderImage(this);
    m_header->installEventFilter(parent);
    m_body = new VerticalDockBox(this);
    m_body->installEventFilter(parent);
    m_body->setMinimumWidth(0);

    layoutChildren();

    setMouseTracking(true);
}

void HeaderDockPanel::layoutChildren()
{
    const int paneWidth = width();
    const int headerHeight = m_header->heightForWidth(paneWidth);

    m_header->setGeometry(0, 0, paneWidth, headerHeight);
    m_body->setGeometry(0, headerHeight, paneWidth, qMax(0, height() - headerHeight));
}

void HeaderDockPanel::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutChildren();
}
