#ifndef HORIZENTALLAYOUT_H
#define HORIZENTALLAYOUT_H

#include <QVector>
#include <QWidget>

class HorizentalLayout : public QWidget
{
    Q_OBJECT

public:
    explicit HorizentalLayout(QWidget *parent = nullptr);

    void addWidget(QWidget *widget);
    void addWidget(QWidget &widget);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void updateChildGeometries();
    int borderAtPosition(int x) const;
    int borderPixelPosition(int borderIndex) const;

    static constexpr int kBorderHitWidth = 6;
    static constexpr int kBorderLineWidth = 1;
    static constexpr double kMinPaneRate = 0.05;

    static const QColor kBackgroundColor;
    static const QColor kBorderColor;
    static const QColor kBorderHoverColor;

    QVector<QWidget *> m_widgets;
    QVector<double> m_rates;

    int m_hoveredBorder = -1;
    int m_draggingBorder = -1;
    int m_dragStartX = 0;
    QVector<double> m_dragStartRates;
};

#endif // HORIZENTALLAYOUT_H
