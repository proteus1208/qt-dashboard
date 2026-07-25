#ifndef BORDERWIDGET_H
#define BORDERWIDGET_H

#include <QWidget>

class BorderRateProvider;

class BorderWidget : public QWidget
{
public:
    explicit BorderWidget(QWidget *parent, BorderRateProvider *layout);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    BorderRateProvider *m_provider = nullptr;
};


#endif // BORDERWIDGET_H
