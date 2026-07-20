// DockWidget.h

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QRect>

class QLabel;

class DockWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DockWidget(QWidget* parent = nullptr);
    ~DockWidget();

    virtual void Init();
    void InstallChildEventFilters(QWidget* widget);

    void AddRow();
    void AddWidget(QWidget* widget, int rate = 1);

protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    enum ResizeEdge
    {
        None = 0,
        Left = 1 << 0,
        Right = 1 << 1,
        Top = 1 << 2,
        Bottom = 1 << 3,

        TopLeft = Top | Left,
        TopRight = Top | Right,
        BottomLeft = Bottom | Left,
        BottomRight = Bottom | Right
    };

    ResizeEdge GetResizeEdge(const QPoint& pos);
    void UpdateCursor(QWidget* target, const QPoint& pos);
    void ResizeWindow(const QPoint& pos);
    void ToggleMaximize();

private:
    QVBoxLayout* m_rootLayout = nullptr;

    QWidget* m_header = nullptr;
    QLabel* m_headerLabel = nullptr;

    QWidget* m_content = nullptr;
    QVBoxLayout* m_contentLayout = nullptr;

    QHBoxLayout* m_currentRow = nullptr;

    bool m_dragging = false;
    bool m_resizing = false;
    bool m_maximized = false;

    QPoint m_dragOffset;

    QPoint m_resizeStartPos;
    QRect m_resizeStartGeometry;

    QRect m_normalGeometry;

    ResizeEdge m_resizeEdge = None;
};
