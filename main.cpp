#include "mainwindow.h"

#include <QApplication>
#include "src/Theme.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<Theme>();

    MainWindow w;
    w.showFullScreen();
    return QApplication::exec();
}
