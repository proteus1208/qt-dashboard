#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "PageLayout/HeaderDockPanel.h"
#include "PageLayout/HorizentalLayout.h"
#include "PageLayout/VerticalDockBox.h"
#include "env.h"

#include <QColor>
#include <QResizeEvent>
#include <QEvent>
#include <QPoint>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Theme defaultTheme;

    Theme testTheme("Test Theme");
    testTheme.bg = QColor("#ffff00");

    registerTheme(defaultTheme);
    registerTheme(testTheme);

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

    VerticalDockBox *rightBox = new VerticalDockBox(horizentalLayout);
    horizentalLayout->addWidget(rightBox);
    registerVerticalDockBox(rightBox);

    DockWidget* dock11 = registerDock<DockWidget>("Factory");
    DockWidget* dock12 = registerDock<DockWidget>("Statistic");
    DockWidget* dock21 = registerDock<DockWidget>("Total");
    DockWidget* dock22 = registerDock<DockWidget>("Graph");
    DockWidget* dock31 = registerDock<DockWidget>("Hello");
    DockWidget* dock32 = registerDock<DockWidget>("Test");

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

void MainWindow::setTheme(Theme theme)
{
    QVariant v;
    v.setValue(theme);
    qApp->setProperty("Theme", v);

    QEvent *e = new QEvent(QEvent::User);
    e->type();
    qApp->postEvent(qApp, e);
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
    setTheme("Default");
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

    connect(newDock, &DockWidget::onDrag, this, [this, newDock](QPoint pos) {
        startDockDrag(newDock, pos);
    });

    connect(newDock, &DockWidget::onMoving, this, [this, newDock](QPoint pos) {
        updateDockBoxPreview(newDock, pos);
    });

    connect(newDock, &DockWidget::onDrop, this, [this, newDock](QPoint pos) {
        finishDockDrag(newDock, pos);
    });
    return newDock;
}
