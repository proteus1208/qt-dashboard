#ifndef HORIZENTALLAYOUT_H
#define HORIZENTALLAYOUT_H

#include <QPropertyAnimation>
#include <QWidget>
#include <QVector>

#include "BorderRateProvider.h"

class BorderWidget;
class HoverWidget;

class HorizentalLayout : public QWidget, public BorderRateProvider
{
    Q_OBJECT
    friend class BorderWidget;

public:
    explicit HorizentalLayout(QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    void addWidget(QWidget &widget);

    QList<double> borderRate() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    void updateChildGeometries();
    int borderAtPosition(int x) const;
    int borderPixelPosition(int borderIndex) const;
    void showBorderHover(int borderIndex);
    void hideBorderHover();
    void onHoverAnimationFinished();

    static constexpr int kBorderHitWidth = 6;
    static constexpr int kBorderLineWidth = 1;
    static constexpr double kMinPaneRate = 0.05;

    QVector<QWidget *> m_widgets;
    QVector<double> m_rates;

    int m_hoveredBorder = -1;
    int m_draggingBorder = -1;
    int m_dragStartX = 0;

    BorderWidget *m_borderWidget = nullptr;
    HoverWidget *m_hoverWidget = nullptr;
    QPropertyAnimation *m_hoverAnimation = nullptr;
};

#endif