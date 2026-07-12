#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "PageLayout/HorizentalLayout.h"
#include "PageLayout/VerticalDockBox.h"
#include "env.h"

#include <QColor>
#include <QResizeEvent>

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
    for (int i = 0; i < 3; ++i) {
        horizentalLayout->addWidget(new VerticalDockBox(horizentalLayout));
    }

    layoutHorizentalLayout();
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    layoutHorizentalLayout();
}

