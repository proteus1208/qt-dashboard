#include "DockWidget.h"

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QCursor>

constexpr int HEADER_HEIGHT = 32;
constexpr int BORDER_WIDTH = 3;
constexpr int INITIAL_WIDTH = 600;
constexpr int INITIAL_HEIGHT = 400;
constexpr int MIN_WIDTH = 200;
constexpr int MIN_HEIGHT = 100;

DockWidget::DockWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setMouseTracking(true);
    setMinimumSize(MIN_WIDTH, MIN_HEIGHT);
    resize(INITIAL_WIDTH, INITIAL_HEIGHT);

    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setContentsMargins(BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH, BORDER_WIDTH);
    m_rootLayout->setSpacing(0);

    m_header = new QWidget(this);
    m_header->setFixedHeight(HEADER_HEIGHT);
    m_header->setStyleSheet("background:black;");
    m_header->setMouseTracking(true);

    m_headerLabel = new QLabel("DockWidget", m_header);
    m_headerLabel->setStyleSheet("color:white;");
    m_headerLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_headerLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    QHBoxLayout* headerLayout = new QHBoxLayout(m_header);
    headerLayout->setContentsMargins(10, 0, 0, 0);
    headerLayout->addWidget(m_headerLabel);

    m_content = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_content);
    m_contentLayout->setContentsMargins(5, 5, 5, 5);
    m_contentLayout->setSpacing(5);

    m_rootLayout->addWidget(m_header);
    m_rootLayout->addWidget(m_content);

    m_header->installEventFilter(this);

    m_content->installEventFilter(this);
    installEventFilter(this);


    Init();
    InstallChildEventFilters(m_content);
}

DockWidget::~DockWidget()
{
}

void DockWidget::Init()
{
    AddRow();
    AddWidget(new QPushButton("Button 1"), 7);
    AddWidget(new QPushButton("Button 2"), 3);

    AddRow();
    AddWidget(new QPushButton("Button 3"), 1);
}

inline void DockWidget::InstallChildEventFilters(QWidget *widget)
{
    widget->setMouseTracking(true);
    widget->installEventFilter(this);

    const auto children = widget->findChildren<QWidget*>();

    for (QWidget* child : children)
    {
        child->setMouseTracking(true);
        child->installEventFilter(this);
    }
}

void DockWidget::AddRow()
{
    m_currentRow = new QHBoxLayout();
    QWidget* rowWidget = new QWidget();
    rowWidget->setLayout(m_currentRow);
    rowWidget->installEventFilter(this);
    m_contentLayout->addWidget(rowWidget);
}

void DockWidget::AddWidget(QWidget* widget, int rate)
{
    if (!m_currentRow)
        AddRow();

    InstallChildEventFilters(widget);

    m_currentRow->addWidget(widget, rate);
}

void DockWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, BORDER_WIDTH));
    painter.drawRect(
        BORDER_WIDTH / 2,
        BORDER_WIDTH / 2,
        width() - BORDER_WIDTH,
        height() - BORDER_WIDTH
        );
}

bool DockWidget::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* e = dynamic_cast<QMouseEvent*>(event);
        if (!e)
            return false;
        QPoint pos = e->globalPosition().toPoint();

        if (m_resizing)
        {
            ResizeWindow(pos);
            return true;
        }

        UpdateCursor(this, pos);

        if (m_dragging)
        {
            move(pos - m_dragOffset);
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);

        if (e->button() == Qt::LeftButton)
        {
            QPoint globalPos = e->globalPosition().toPoint();

            m_resizeEdge = GetResizeEdge(globalPos);

            if (m_resizeEdge != None)
            {
                m_resizing = true;
                m_resizeStartPos = globalPos;
                m_resizeStartGeometry = geometry();
                return true;
            }

            if (obj == m_header)
            {
                m_dragging = true;
                m_dragOffset = globalPos - frameGeometry().topLeft();
                return true;
            }
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);

        if (e->button() == Qt::LeftButton)
        {
            m_dragging = false;
            m_resizing = false;
            m_resizeEdge = None;
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonDblClick)
    {
        if (obj == m_header)
        {
            ToggleMaximize();
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

DockWidget::ResizeEdge DockWidget::GetResizeEdge(const QPoint& pos)
{
    QRect r = rect();
    QPoint local = mapFromGlobal(pos);

    bool left = local.x() <= BORDER_WIDTH;
    bool right = local.x() >= r.width() - BORDER_WIDTH;
    bool top = local.y() <= BORDER_WIDTH;
    bool bottom = local.y() >= r.height() - BORDER_WIDTH;

    if (top && left) return TopLeft;
    if (top && right) return TopRight;
    if (bottom && left) return BottomLeft;
    if (bottom && right) return BottomRight;
    if (left) return Left;
    if (right) return Right;
    if (top) return Top;
    if (bottom) return Bottom;

    return None;
}

void DockWidget::UpdateCursor(QWidget* target, const QPoint& pos)
{
    switch (GetResizeEdge(pos))
    {
    case Left:
    case Right:
        target->setCursor(Qt::SizeHorCursor);
        break;

    case Top:
    case Bottom:
        target->setCursor(Qt::SizeVerCursor);
        break;

    case TopLeft:
    case BottomRight:
        target->setCursor(Qt::SizeFDiagCursor);
        break;

    case TopRight:
    case BottomLeft:
        target->setCursor(Qt::SizeBDiagCursor);
        break;

    default:
        target->unsetCursor();
        break;
    }
}

void DockWidget::ResizeWindow(const QPoint& pos)
{
    QRect g = m_resizeStartGeometry;
    QPoint delta = pos - m_resizeStartPos;

    if (m_resizeEdge & Left)
    {
        g.setLeft(g.left() + delta.x());
    }

    if (m_resizeEdge & Right)
    {
        g.setRight(g.right() + delta.x());
    }

    if (m_resizeEdge & Top)
    {
        g.setTop(g.top() + delta.y());
    }

    if (m_resizeEdge & Bottom)
    {
        g.setBottom(g.bottom() + delta.y());
    }

    if (g.width() < MIN_WIDTH)
        g.setWidth(MIN_WIDTH);

    if (g.height() < MIN_HEIGHT)
        g.setHeight(MIN_HEIGHT);

    setGeometry(g);
}

void DockWidget::ToggleMaximize()
{
    if (!m_maximized)
    {
        m_normalGeometry = geometry();

        QScreen* screen = QApplication::screenAt(QCursor::pos());

        if (!screen)
            screen = QApplication::primaryScreen();

        setGeometry(screen->availableGeometry());

        m_maximized = true;
    }
    else
    {
        setGeometry(m_normalGeometry);
        m_maximized = false;
    }
}