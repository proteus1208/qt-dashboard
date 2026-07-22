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
    registerVerticalDockBox(leftBox);

    layoutHorizentalLayout();
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
    this->dockboxs.append(dockbox);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    layoutHorizentalLayout();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if(this == obj && e->type() == QEvent::MouseMove){
        qDebug()<<"wer";
    }
}

void MainWindow::init()
{
    registerDock<DockWidget>();
}

template<class Dock>
Dock* MainWindow::registerDock()
{
    static_assert(
        std::is_base_of<DockWidget, Dock>::value,
        "Dock must inherit from DockWidget"
    );
    Dock *newDock = new Dock();
    newDock->show();

    connect(newDock, &DockWidget::onDrag, [this](QPoint pos){

    });
    connect(newDock, &DockWidget::onMoving, [this](QPoint pos){

    });
    connect(newDock, &DockWidget::onDrop, [this](QPoint pos){

    });
    return newDock;
}
