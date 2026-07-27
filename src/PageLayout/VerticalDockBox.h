#ifndef VERTICALDOCKBOX_H
#define VERTICALDOCKBOX_H

#include <QWidget>
#include <QVector>
#include <QPoint>

#include "DockWidget.h"

#include "../ListAnimation.h"

#include "BorderRateProvider.h"
#include "BorderWidget.h"
#include "HoverWidget.h"

class QResizeEvent;
class QPaintEvent;
class QMouseEvent;
class QEvent;

class VerticalDockBox : public QWidget, BorderRateProvider
{
    Q_OBJECT

public:
    explicit VerticalDockBox(QWidget *parent = nullptr);

    VerticalDockBox(const VerticalDockBox &) = delete;
    VerticalDockBox(VerticalDockBox &&) = delete;
    VerticalDockBox &operator=(const VerticalDockBox &) = delete;
    VerticalDockBox &operator=(VerticalDockBox &&) = delete;
    QList<double> borderRate() const override;

    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void onEnterDock(DockWidget* dock, QPoint pos);
    void onMoveDock(DockWidget* dock, QPoint pos);
    void onOutDock(DockWidget* dock, QPoint pos);
    void onDropDock(DockWidget* dock, QPoint pos);

public:
    int dockCount() const
    {
        return m_docks.size();
    }

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    static constexpr int kBorderHitWidth = 6;
    static constexpr double kMinDockRate = 0.05;

private:
    void updateChildGeometries();

    int calculateDockIndex(QPoint pos) const;

public:
    void insertDock(DockWidget* dock, int index = -1);
    void removeDock(DockWidget* dock);

private:
    QList<double> getTarget(int index);

private:
    void updateRatesAfterInsert(int index);
    void normalizeRates();

    int borderPixelPosition(int index) const;
    int borderAtPosition(int y) const;

    void updatePreview(int index);
    void restorePreview();

private:
    QVector<DockWidget*> m_docks;

private:
    BorderWidget *m_borderWidget = nullptr;
    HoverWidget *m_hoverWidget = nullptr;

    QVector<double> m_rates, m_rates_old, m_rates_target;

    DockWidget* m_dragDock = nullptr;

    int m_previewIndex = -1, m_previewIndex_old = -1;

    QVector<float> m_preview_border_rates;

    int m_hoveredBorder = -1;
    int m_draggingBorder = -1;

    int m_dragStartY = 0;

    QVector<double> m_dragStartRates;

    ListAnimation borderAnimation;

private:
    void showBorderHover(int borderIndex);
    void hideBorderHover();

};

#endif // VERTICALDOCKBOX_H