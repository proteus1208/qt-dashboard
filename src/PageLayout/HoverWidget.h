#ifndef HOVERWIDGET_H
#define HOVERWIDGET_H

#include <QWidget>

class HoverWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit HoverWidget(QWidget *parent = nullptr);

    qreal opacity() const;
    void setOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_opacity = 0.0;
};

#endif