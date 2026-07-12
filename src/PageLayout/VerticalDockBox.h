#ifndef VERTICALDOCKBOX_H
#define VERTICALDOCKBOX_H

#include <QWidget>

class VerticalDockBox : public QWidget
{
    Q_OBJECT

public:
    explicit VerticalDockBox(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // VERTICALDOCKBOX_H
