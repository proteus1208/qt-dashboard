#include "DockWidget.h"

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QCursor>
#include <QTimer>

constexpr int HEADER_HEIGHT = 32;
constexpr int BORDER_WIDTH = 1;
constexpr int BORDER_MOUSE_AREA_WIDTH = 5;
constexpr int INITIAL_WIDTH = 600;
constexpr int INITIAL_HEIGHT = 400;
constexpr int MIN_WIDTH = 200;
constexpr int MIN_HEIGHT = HEADER_HEIGHT;

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

    QTimer::singleShot(0, [=](){
        m_normalGeometry = this->geometry();
    });
}

DockWidget::~DockWidget()
{
}

VerticalDockBox *DockWidget::parentDockBox()
{
    return (VerticalDockBox *) this->parent();
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

void DockWidget::detachFromDock(QPoint pos)
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    hide();
    setParent(nullptr);
    setWindowFlags( Qt::FramelessWindowHint | Qt::Window );
    move(globalPos);
    int newX = pos.x() - m_normalGeometry.width() / 2;
    int newY = pos.y() - HEADER_HEIGHT / 2;
    setGeometry(
        newX,
        newY,
        m_normalGeometry.width(),
        m_normalGeometry.height()
    );

    m_dragOffset = pos - frameGeometry().topLeft();
    setMinimumSize( MIN_WIDTH, MIN_HEIGHT );
    show();
    raise();
    activateWindow();
}

int DockWidget::minimumDockHeight() const
{
    return HEADER_HEIGHT + 20; // header + content minimum
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

        if (m_pendingMaximizedDrag)
        {
            if ((pos - m_pendingDragPos).manhattanLength() > 3)
            {
                m_pendingMaximizedDrag = false;

                int newX = pos.x() - m_normalGeometry.width() / 2;
                int newY = pos.y() - HEADER_HEIGHT / 2;

                setGeometry(
                    newX,
                    newY,
                    m_normalGeometry.width(),
                    m_normalGeometry.height()
                );

                m_maximized = false;

                if (obj == m_header)
                {
                    QPoint globalPos = e->globalPosition().toPoint();
                    m_dragOffset = globalPos - frameGeometry().topLeft();
                    emit onDrag(globalPos);
                    return true;
                }

                m_dragging = true;
                emit onDrag(pos);
                QPoint currentPos = this->pos();
                setParent(nullptr);
                setWindowFlags(
                    Qt::FramelessWindowHint |
                    Qt::Window
                );
                move(pos - m_dragOffset);
            }
        }

        if (m_dragging)
        {
            QPoint newPos = pos - m_dragOffset;

            move(newPos);

            emit onMoving(e->globalPosition().toPoint());

            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);

        if (e->button() == Qt::LeftButton)
        {
            QPoint globalPos = e->globalPosition().toPoint();

            // Resize only when not maximized
            if (!m_maximized && parent() == nullptr)
            {
                m_resizeEdge = GetResizeEdge(globalPos);

                if (m_resizeEdge != None)
                {
                    m_resizing = true;
                    m_resizeStartPos = globalPos;
                    m_resizeStartGeometry = geometry();
                    return false;
                }
            }

            // Header drag
            if (obj == m_header)
            {
                if(parent() && ( e->pos().y() <= 3 || e->pos().x() <= 3 || e->pos().x() >= width() - 4 )){
                    return false;
                }
                m_dragOffset = globalPos - frameGeometry().topLeft();

                if (m_maximized)
                {
                    m_pendingMaximizedDrag = true;
                    m_pendingDragPos = globalPos;
                }

                m_dragging = true;
                emit onDrag(e->globalPosition().toPoint());
                return true;
            }
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);

        if (e->button() == Qt::LeftButton)
        {
            if (m_dragging)
            {
                emit onDrop(e->globalPosition().toPoint());
            }

            m_dragging = false;
            m_resizing = false;
            m_pendingMaximizedDrag = false;
            m_resizeEdge = None;

            return false;
        }
    }

    if (event->type() == QEvent::MouseButtonDblClick)
    {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        if (obj == m_header && e->button() == Qt::LeftButton)
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

    bool left = local.x() <= BORDER_MOUSE_AREA_WIDTH;
    bool right = local.x() >= r.width() - BORDER_MOUSE_AREA_WIDTH;
    bool top = local.y() <= BORDER_MOUSE_AREA_WIDTH;
    bool bottom = local.y() >= r.height() - BORDER_MOUSE_AREA_WIDTH;

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
    if (parent() != nullptr || m_maximized)
    {
        target->unsetCursor();
        return;
    }
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

    if (parent() != nullptr || m_maximized)
    {
        m_resizing = false;
        return;
    }

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

        AnimateGeometry(screen->availableGeometry());

        m_maximized = true;
    }
    else
    {
        AnimateGeometry(m_normalGeometry);
        m_maximized = false;
    }
}

void DockWidget::AnimateGeometry(const QRect &target)
{
    if (m_animation)
    {
        m_animation->stop();
        delete m_animation;
    }

    m_animation = new QPropertyAnimation(this, "geometry");
    m_animation->setDuration(200);
    m_animation->setStartValue(geometry());
    m_animation->setEndValue(target);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_animation, &QPropertyAnimation::finished, this, [this]()
            {
                delete m_animation;
                m_animation = nullptr;
            });

    m_animation->start();
}