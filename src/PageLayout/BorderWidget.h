#ifndef BORDERWIDGET_H
#define BORDERWIDGET_H

#include <QWidget>

class HorizentalLayout;

class BorderWidget : public QWidget
{
public:
    explicit BorderWidget(HorizentalLayout *layout);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    HorizentalLayout *m_layout = nullptr;
};


#endif // BORDERWIDGET_H
