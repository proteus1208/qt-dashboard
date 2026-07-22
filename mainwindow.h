#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "src/PageLayout/DockWidget.h"
#include "src/PageLayout/VerticalDockBox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class HorizentalLayout;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *e) override;

private:
    void init();

private:
    void layoutHorizentalLayout();

    Ui::MainWindow *ui;
    HorizentalLayout *horizentalLayout = nullptr;

private:

    QList<VerticalDockBox*> dockboxs;
    void registerVerticalDockBox(VerticalDockBox *);

    template <class Dock>
    Dock* registerDock();
    QList<DockWidget*> docks;

};
#endif // MAINWINDOW_H
