#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "PageLayout/HeaderDockPanel.h"
#include "PageLayout/HeaderImage.h"
#include "PageLayout/HorizentalLayout.h"
#include "PageLayout/VerticalDockBox.h"
#include "Widgets/Battery.h"
#include "Widgets/LightButton.h"
#include "Widgets/LineChartWidget.h"
#include "env.h"

#include <QColor>
#include <QCoreApplication>
#include <QResizeEvent>
#include <QEvent>
#include <QPoint>
#include <QRandomGenerator>

#include <QPushButton>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

#include <QDebug>

namespace {

// Builds a full Theme from a handful of hand-picked surface colors, reusing
// the Default theme's spacing/font sizes so every registered theme shares
// the same layout and only the palette changes.
Theme makeTheme(
    const QString &name,
    const QColor &dockBg,
    const QColor &headerBg,
    const QColor &itemBg,
    const QColor &itemHeaderBg,
    const QColor &contentBg,
    const QColor &accent,
    bool light = false)
{
    Theme t(name);

    const QColor textPrimary = light ? QColor("#1a1d26") : QColor("#eef0f5");
    const QColor textSecondary = light ? QColor("#3f4552") : QColor("#c7cbd6");
    const QColor border = light ? dockBg.darker(112) : dockBg.lighter(160);

    t.bg = light ? dockBg.darker(104) : dockBg.darker(140);
    t.dock_bg = dockBg;
    t.dock_border = border;
    t.dock_header_bg = headerBg;
    t.dock_header_text_color = textPrimary;

    t.dock_content_item_bg = itemBg;
    t.dock_content_item_header_bg = itemHeaderBg;
    t.dock_content_item_header_text_color = textSecondary;
    t.dock_content_item_content_bg = contentBg;
    t.dock_content_item_content_text = textPrimary;

    t.split_bg = t.bg;
    t.split_border = border;
    t.split_hover = accent;

    QColor preview = accent;
    preview.setAlpha(128);
    t.split_preview = preview;

    t.menu_bg = headerBg;
    t.menu_border = border;
    t.menu_text = textPrimary;
    t.menu_hover_bg = accent;
    t.menu_hover_text = QColor("#ffffff");

    return t;
}

QVector<double> randomSeries(int count, double lo, double hi)
{
    QVector<double> values;
    values.reserve(count);
    for (int i = 0; i < count; ++i) {
        values << lo + QRandomGenerator::global()->generateDouble() * (hi - lo);
    }
    return values;
}

QWidget* makeStatusRow()
{
    QWidget* row = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    LightButton* ok = new LightButton(row);
    ok->setText("OK");
    ok->setGreen();

    LightButton* warn = new LightButton(row);
    warn->setText("WARN");
    warn->setYellow();

    LightButton* err = new LightButton(row);
    err->setText("ERR");
    err->setRed();

    layout->addWidget(ok);
    layout->addWidget(warn);
    layout->addWidget(err);

    return row;
}

Battery* makeBattery(double value, const QColor &accent)
{
    Battery* battery = new Battery();
    battery->setRange(0, 100);
    battery->setAlarmValue(20);
    battery->setNormalColorStart(accent.lighter(110));
    battery->setNormalColorEnd(accent.darker(110));
    battery->setValue(value);
    return battery;
}

LineChartWidget* makeChart()
{
    LineChartWidget* chart = new LineChartWidget();
    chart->setRange(0, 100);
    chart->setValues(randomSeries(16, 20, 95));
    return chart;
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Without this, Windows' DWM still decorates the top-level frame (DWM
    // corner rounding + a native border) even in showFullScreen(), showing
    // up as a thin white/rounded gap around the app. Go frameless like
    // DockWidget's own floating windows already do.
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    Theme defaultTheme;

    registerTheme(defaultTheme);
    registerTheme(makeTheme("Midnight", QColor("#0f1420"), QColor("#141a2b"), QColor("#161d30"), QColor("#1c2440"), QColor("#0c1019"), QColor("#4f8cff")));
    registerTheme(makeTheme("Forest",   QColor("#0f1713"), QColor("#131f19"), QColor("#15211b"), QColor("#1b2b22"), QColor("#0b120e"), QColor("#34d399")));
    registerTheme(makeTheme("Sunset",   QColor("#1a140f"), QColor("#211a13"), QColor("#231c15"), QColor("#2c2318"), QColor("#140f0b"), QColor("#ff9d4d")));
    registerTheme(makeTheme("Crimson",  QColor("#170f13"), QColor("#1f1319"), QColor("#22141b"), QColor("#2c1a23"), QColor("#110b0e"), QColor("#fb5a72")));
    registerTheme(makeTheme("Violet",   QColor("#140f1e"), QColor("#1a1428"), QColor("#1c1630"), QColor("#251c3c"), QColor("#0f0b18"), QColor("#a78bfa")));
    registerTheme(makeTheme("Daylight", QColor("#f4f5f8"), QColor("#eceef3"), QColor("#ffffff"), QColor("#eef0f6"), QColor("#f7f8fb"), QColor("#4f6bed"), true));

    setWindowTitle(env::kProjectName);
    ui->centralwidget->setAutoFillBackground(true);
    QPalette palette = ui->centralwidget->palette();
    palette.setColor(QPalette::Window, QColor(0x1e, 0x1e, 0x1e));
    ui->centralwidget->setPalette(palette);

    horizentalLayout = new HorizentalLayout(ui->centralwidget);
    VerticalDockBox *leftBox = new VerticalDockBox(horizentalLayout);
    horizentalLayout->addWidget(leftBox);
    registerVerticalDockBox(leftBox);

    HeaderDockPanel* centerBox = new HeaderDockPanel(horizentalLayout);
    horizentalLayout->addWidget(centerBox);
    registerVerticalDockBox(centerBox->getDockBox());

    connect(centerBox->getHeader(), &HeaderImage::contextMenuRequested, this, [this](QPoint globalPos) {
        showHeaderContextMenu(globalPos);
    });

    VerticalDockBox *rightBox = new VerticalDockBox(horizentalLayout);
    horizentalLayout->addWidget(rightBox);
    registerVerticalDockBox(rightBox);

    DockWidget* dock11 = registerDock<DockWidget>("Factory");
    DockWidget* dock12 = registerDock<DockWidget>("Statistic");
    DockWidget* dock21 = registerDock<DockWidget>("Total");
    DockWidget* dock22 = registerDock<DockWidget>("Graph");
    DockWidget* dock31 = registerDock<DockWidget>("Hello");
    DockWidget* dock32 = registerDock<DockWidget>("Test");

    // Each group (left/center/right dock box) gets two docks, each showing
    // a different sample component: battery gauge, status row, line chart.
    dock11->AddRow();
    dock11->AddWidget("Battery", makeBattery(72, defaultTheme.split_hover), 1);
    dock11->AddRow();
    dock11->AddWidget("Trend", makeChart(), 3);
    dock11->AddWidget("Trend", makeChart(), 7);

    dock12->AddRow();
    dock12->AddWidget("Trend", makeChart(), 1);

    dock21->AddRow();
    dock21->AddWidget("Status", makeStatusRow(), 1);

    dock22->AddRow();
    dock22->AddWidget("Battery", makeBattery(45, defaultTheme.split_hover), 1);

    dock31->AddRow();
    dock31->AddWidget("Trend", makeChart(), 1);

    dock32->AddRow();
    dock32->AddWidget("Status", makeStatusRow(), 1);

    leftBox->insertDock(dock11);
    leftBox->insertDock(dock12);
    centerBox->getDockBox()->insertDock(dock21);
    centerBox->getDockBox()->insertDock(dock22);
    rightBox->insertDock(dock31);
    rightBox->insertDock(dock32);

    layoutHorizentalLayout();

    QTimer::singleShot(0, [=](){
        init();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::layoutHorizentalLayout()
{
    if (!horizentalLayout || !ui->centralwidget) {
        return;
    }

    horizentalLayout->setGeometry(ui->centralwidget->rect());
}

void MainWindow::registerVerticalDockBox(VerticalDockBox *dockbox)
{
    this->dockBoxes.append(dockbox);
}

void MainWindow::startDockDrag(DockWidget* dock, QPoint pos)
{
    if (!dock) return;

    m_draggingDock = dock;

    // Save original dock box
    m_sourceDockBox = dock->parentDockBox();

    if(m_sourceDockBox)
    {
        m_sourceDockBox->removeDock(dock);
        dock->detachFromDock(pos);
    }

    m_currentDockBox = nullptr;
}

void MainWindow::updateDockBoxPreview( DockWidget* dock, QPoint pos) {
    if (!dock) return;

    VerticalDockBox* hitBox = nullptr;

    // Find dock box under mouse
    for (auto box : dockBoxes)
    {
        QRect globalRect( box->mapToGlobal(QPoint(0, 0)), box->size() );

        if (globalRect.contains(pos))
        {
            hitBox = box;
            break;
        }
    }

    // Enter new dock box
    if (hitBox && hitBox != m_currentDockBox)
    {
        if (m_currentDockBox)
        {
            m_currentDockBox->onOutDock(dock, pos);
        }

        m_currentDockBox = hitBox;
        m_currentDockBox->onEnterDock(dock, pos);
    }

    // Move inside current dock box
    else if (hitBox && hitBox == m_currentDockBox)
    {
        m_currentDockBox->onMoveDock(dock, pos);
    }

    // Leave dock boxes
    else if (!hitBox && m_currentDockBox)
    {
        m_currentDockBox->onOutDock(dock, pos);
        m_currentDockBox = nullptr;
    }
}

void MainWindow::finishDockDrag( DockWidget* dock, QPoint pos) {
    if (!dock || dock != m_draggingDock)
        return;

    // Dropped on a dock box
    if (m_currentDockBox)
    {
        m_currentDockBox->onDropDock(dock, pos);
    }
    // Cleanup
    m_currentDockBox = nullptr;
    m_sourceDockBox = nullptr;
    m_draggingDock = nullptr;
}

void MainWindow::showHeaderContextMenu(QPoint globalPos)
{
    const Theme current = qApp->property("Theme").value<Theme>();

    QMenu menu(this);
    menu.setStyleSheet(current.menuStyleSheet());

    QMenu* themeMenu = menu.addMenu("Theme");
    for (const Theme &t : themes)
    {
        QAction* action = themeMenu->addAction(t.name);
        action->setCheckable(true);
        action->setChecked(t.name == current.name);
        connect(action, &QAction::triggered, this, [this, t]() {
            setTheme(t);
        });
    }

    QMenu* groupsMenu = menu.addMenu("Groups");
    for (DockWidget* d : docks)
    {
        QAction* action = groupsMenu->addAction(d->title());
        action->setCheckable(true);
        action->setChecked(d->isVisible());
        connect(action, &QAction::triggered, this, [this, d]() {
            toggleDockVisibility(d);
        });
    }

    menu.addSeparator();
    QAction* closeAction = menu.addAction("Close");
    connect(closeAction, &QAction::triggered, this, [=]() {
        qApp->quit();
        close();
    });

    menu.exec(globalPos);
}

void MainWindow::showDockContextMenu(DockWidget* dock, QPoint globalPos)
{
    if (!dock) return;

    const Theme current = qApp->property("Theme").value<Theme>();

    QMenu menu(this);
    menu.setStyleSheet(current.menuStyleSheet());

    // Flat list of every dock, checkable to toggle show/hide - no submenu,
    // just a separator marking it off from the actions below.
    for (DockWidget* d : docks)
    {
        QAction* action = menu.addAction(d->title());
        action->setCheckable(true);
        action->setChecked(d->isVisible());
        connect(action, &QAction::triggered, this, [this, d]() {
            toggleDockVisibility(d);
        });
    }

    menu.addSeparator();

    QAction* hideAction = menu.addAction("Hide");
    connect(hideAction, &QAction::triggered, this, [dock]() {
        dock->hideDock();
    });

    // QAction* fullScreenAction = menu.addAction("Full Screen");
    // fullScreenAction->setCheckable(true);
    // fullScreenAction->setChecked(isFullScreen());
    // connect(fullScreenAction, &QAction::triggered, this, [this]() {
    //     toggleFullScreen();
    // });

    menu.exec(globalPos);
}

void MainWindow::toggleDockVisibility(DockWidget* dock)
{
    if (!dock) return;

    if (dock->isVisible())
    {
        dock->hideDock();
    }
    else
    {
        dock->showDock();
    }
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen())
    {
        showNormal();
    }
    else
    {
        showFullScreen();
    }
}

void MainWindow::setTheme(Theme theme)
{
    QVariant v;
    v.setValue(theme);
    qApp->setProperty("Theme", v);

    if (!horizentalLayout) {
        return;
    }

    QEvent themeEvent(QEvent::Type(Theme::EventType));
    QCoreApplication::sendEvent(horizentalLayout, &themeEvent);

    const auto children = horizentalLayout->findChildren<QWidget*>();
    for (QWidget* child : children) {
        QCoreApplication::sendEvent(child, &themeEvent);
    }
}

void MainWindow::setTheme(QString name){
    for(auto theme: themes){
        if(theme.name == name) return setTheme(theme);
    }
    qDebug()<<"No Theme named"<<name;
}

void MainWindow::registerTheme(Theme theme)
{
    themes<<theme;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    layoutHorizentalLayout();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
}

void MainWindow::init()
{
    setTheme("Professional");
}

template<class Dock>
Dock* MainWindow::registerDock(QString nmae)
{
    static_assert(
        std::is_base_of<DockWidget, Dock>::value,
        "Dock must inherit from DockWidget"
    );
    Dock *newDock = new Dock(nmae);
    newDock->show();

    docks.append(newDock);

    connect(newDock, &DockWidget::onDrag, this, [this, newDock](QPoint pos) {
        startDockDrag(newDock, pos);
    });

    connect(newDock, &DockWidget::onMoving, this, [this, newDock](QPoint pos) {
        updateDockBoxPreview(newDock, pos);
    });

    connect(newDock, &DockWidget::onDrop, this, [this, newDock](QPoint pos) {
        finishDockDrag(newDock, pos);
    });

    connect(newDock, &DockWidget::headerContextMenuRequested, this, [this, newDock](QPoint globalPos) {
        showDockContextMenu(newDock, globalPos);
    });

    return newDock;
}
