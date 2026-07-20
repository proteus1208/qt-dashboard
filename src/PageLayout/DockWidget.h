#ifndef DOCKWIDGET_H
#define DOCKWIDGET_H

#include <QWidget>

class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QWidget;

class DockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DockWidget(const QString& title, QWidget* parent = nullptr);
    QWidget* contentWidget() const;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel* m_titleLabel;
    QWidget* m_rightWidget; // Empty widget at top-right
    QWidget* m_contentWidget; // The content area
    QHBoxLayout* m_headerLayout;
    QVBoxLayout* m_mainLayout;
};

#endif // DOCKWIDGET_H
