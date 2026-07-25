#ifndef BORDERWIDGET_H
#define BORDERWIDGET_H

#include <QWidget>

class BorderRateProvider;

class BorderWidget : public QWidget
{
public :
    enum Type{
        Vertical,
        Horizental
    };

public:
    explicit BorderWidget(QWidget *parent, BorderRateProvider *layout, Type type = Horizental);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    BorderRateProvider *m_provider = nullptr;
    Type type;
};


#endif // BORDERWIDGET_H
