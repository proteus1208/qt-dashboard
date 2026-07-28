#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "src/Theme.h"

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

    QList<VerticalDockBox*> dockBoxes;
    void registerVerticalDockBox(VerticalDockBox *);

    template <class Dock>
    Dock* registerDock(QString name);
    QList<DockWidget*> docks;

    DockWidget *m_draggingDock = nullptr;
    VerticalDockBox* m_currentDockBox = nullptr;
    VerticalDockBox* m_sourceDockBox = nullptr;
    void startDockDrag(DockWidget* dock, QPoint pos);
    void updateDockBoxPreview(DockWidget* dock, QPoint pos);
    void finishDockDrag(DockWidget* dock, QPoint pos);

private:
    void setTheme(Theme);
    void setTheme(QString);
    void registerTheme(Theme);
    QVector<Theme> themes;
};
#endif // MAINWINDOW_H
