#ifndef SAMPLEDASHBOARDDOCK_H
#define SAMPLEDASHBOARDDOCK_H

#include "DockWidget.h"

class Battery;
class LightButton;
class LineChartWidget;

class SampleDashboardDock : public DockWidget
{
    Q_OBJECT

public:
    explicit SampleDashboardDock(QString name);

    void Init() override;

protected:
    bool event(QEvent *e) override;

private:
    void applyTheme(const Theme &theme);

    Battery* m_battery = nullptr;
    LightButton* m_statusOk = nullptr;
    LightButton* m_statusWarn = nullptr;
    LightButton* m_statusErr = nullptr;
    LineChartWidget* m_chart = nullptr;
};

#endif // SAMPLEDASHBOARDDOCK_H
