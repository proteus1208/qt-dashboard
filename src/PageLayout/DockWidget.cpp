#include "DockWidget.h"
#include "VerticalDockBox.h"

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QLabel>
#include <QCursor>
#include <QTimer>
#include <QContextMenuEvent>

constexpr int BORDER_MOUSE_AREA_WIDTH = 5;
constexpr int INITIAL_WIDTH = 600;
constexpr int INITIAL_HEIGHT = 400;
constexpr int MIN_WIDTH = 200;

DockWidget::DockWidget(QString name)
    : QWidget(nullptr),
    m_normalGeometry(0,0,600,400)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setMouseTracking(true);
    resize(INITIAL_WIDTH, INITIAL_HEIGHT);
    this->setAutoFillBackground(true);

    m_rootLayout = new QVBoxLayout(this);
    m_rootLayout->setSpacing(0);

    m_header = new QWidget(this);
    m_header->setMouseTracking(true);

    m_headerLabel = new QLabel(name, m_header);
    m_headerLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_headerLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    headerLayout = new QHBoxLayout(m_header);
    headerLayout->addWidget(m_headerLabel);

    m_content = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_content);

    m_rootLayout->addWidget(m_header);
    m_rootLayout->addWidget(m_content);

    m_header->installEventFilter(this);

    m_content->installEventFilter(this);
    installEventFilter(this);

    updateTheme();

    Init();
    InstallChildEventFilters(m_content);

    // QTimer::singleShot(0, [=](){
    //     m_normalGeometry = this->geometry();
    // });
}

DockWidget::~DockWidget()
{
}

QSize DockWidget::sizeHint() const
{
    return QSize(600, 400);
}

VerticalDockBox *DockWidget::parentDockBox()
{
    return (VerticalDockBox *) this->parent();
}

QString DockWidget::title() const
{
    return m_headerLabel->text();
}

void DockWidget::hideDock()
{
    VerticalDockBox* box = parentDockBox();
    if (box) box->removeDock(this);
    hide();
}

void DockWidget::showDock()
{
    VerticalDockBox* box = parentDockBox();
    if (box)
    {
        box->insertDock(this);
    }
    else
    {
        // Floating (detached, not currently owned by any dock box):
        // just restore the top-level window instead of trying to dock it.
        show();
        raise();
        activateWindow();
    }
}

void DockWidget::Init()
{
    // Intentionally empty: dock content is configured from the outside via
    // AddRow()/AddWidget() (see mainwindow.cpp) rather than hardcoded here.
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
    m_currentRow->setContentsMargins(0, 0, 0, 0);
    m_currentRow->setSpacing(theme.dock_content_gap);
    m_rows << m_currentRow;

    QWidget* rowWidget = new QWidget();
    rowWidget->setLayout(m_currentRow);
    rowWidget->installEventFilter(this);
    m_contentLayout->addWidget(rowWidget);
}

void DockWidget::AddWidget(QString title, QWidget* widget, int rate)
{
    if (!m_currentRow)
        AddRow();

    ItemFrame *newFrame = new ItemFrame(title,widget, this);
    newFrame->setTheme(theme);
    newFrame->show();
    items<<newFrame;
    InstallChildEventFilters(newFrame);
    m_currentRow->addWidget(newFrame, rate);
}

void DockWidget::detachFromDock(QPoint pos)
{
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    hide();
    setParent(nullptr);
    setWindowFlags( Qt::FramelessWindowHint | Qt::Window );
    move(globalPos);
    int newX = pos.x() - m_normalGeometry.width() / 2;
    int newY = pos.y() - theme.dock_header_height / 2;
    setGeometry(
        newX,
        newY,
        m_normalGeometry.width(),
        m_normalGeometry.height()
    );

    m_dragOffset = pos - frameGeometry().topLeft();
    setMinimumSize( MIN_WIDTH, theme.dock_header_height );
    show();
    raise();
    activateWindow();
}

int DockWidget::minimumDockHeight() const
{
    return theme.dock_header_height + 20; // header + content minimum
}

void DockWidget::paintEvent(QPaintEvent*)
{
    QVariant vt = qApp->property("Theme");
    Theme theme = vt.value<Theme>();

    QPainter painter(this);
    painter.setPen(QPen(theme.dock_border, theme.dock_padding + 2));
    painter.drawRect(
        theme.dock_padding / 2 - 1,
        theme.dock_padding / 2 - 1,
        width() - theme.dock_padding + 2,
        height() - theme.dock_padding + 2
        );
}

bool DockWidget::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == Theme::EventType){
        updateTheme();

    }
    if (obj == m_header && event->type() == QEvent::ContextMenu){
        QContextMenuEvent* e = static_cast<QContextMenuEvent*>(event);
        emit headerContextMenuRequested(e->globalPos());
        return true;
    }
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
                int newY = pos.y() - theme.dock_header_height / 2;

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
                if(parent() && ( e->pos().y() <= 3 || e->pos().y() >= height() - 4 || e->pos().x() <= 3 || e->pos().x() >= width() - 4 )){
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

void DockWidget::updateTheme()
{
    theme = qApp->property("Theme").value<Theme>();
    m_rootLayout->setContentsMargins(
        theme.dock_padding,
        theme.dock_padding,
        theme.dock_padding,
        theme.dock_padding
        );
    setMinimumSize(MIN_WIDTH, theme.dock_header_height);
    m_header->setFixedHeight(theme.dock_header_height);
    m_header->setStyleSheet(
        QString("background:%1;")
            .arg(theme.dock_bg.name(QColor::HexRgb))
        );
    headerLayout->setContentsMargins(
        theme.dock_content_padding,
        theme.dock_content_padding,
        theme.dock_content_padding,
        theme.dock_content_padding
        );
    m_headerLabel->setStyleSheet(
        QString("padding-left:%1px;color:%2; background:%3; font-size:%4px;")
            .arg(theme.dock_header_padding_left)
            .arg(theme.dock_header_text_color.name(QColor::HexRgb))
            .arg(theme.dock_header_bg.name(QColor::HexRgb))
            .arg(theme.dock_header_text_fontSize)
        );
    m_content->setStyleSheet(
        QString("background:%1;")
            .arg(theme.dock_bg.name(QColor::HexRgb))
        );
    m_contentLayout->setContentsMargins(
        theme.dock_content_padding,
        theme.dock_content_padding,
        theme.dock_content_padding,
        theme.dock_content_padding
        );
    m_contentLayout->setSpacing(theme.dock_content_gap);
    for(auto row: m_rows){
        row->setSpacing(theme.dock_content_gap);
    }
    for(auto item: items){
        item->setTheme(theme);
    }
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

    if (g.height() < theme.dock_header_height)
        g.setHeight(theme.dock_header_height);

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