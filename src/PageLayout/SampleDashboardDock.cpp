#include "SampleDashboardDock.h"

#include "../Widgets/Battery.h"
#include "../Widgets/LightButton.h"
#include "../Widgets/LineChartWidget.h"

#include <QHBoxLayout>
#include <QRandomGenerator>
#include <QCoreApplication>

SampleDashboardDock::SampleDashboardDock(QString name)
    : DockWidget(name)
{
    // DockWidget's constructor already ran the base Init() (virtual dispatch
    // can't reach this class's override from there). Re-run through the
    // now-fully-constructed object so our Init() override actually applies.
    ReInit();
}

void SampleDashboardDock::Init()
{
    m_battery = new Battery();
    m_battery->setRange(0, 100);
    m_battery->setAlarmValue(20);
    m_battery->setValue(72);

    QWidget* statusPanel = new QWidget();
    QHBoxLayout* statusLayout = new QHBoxLayout(statusPanel);
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusLayout->setSpacing(8);

    m_statusOk = new LightButton(statusPanel);
    m_statusOk->setText("OK");
    m_statusOk->setGreen();

    m_statusWarn = new LightButton(statusPanel);
    m_statusWarn->setText("WARN");
    m_statusWarn->setYellow();

    m_statusErr = new LightButton(statusPanel);
    m_statusErr->setText("ERR");
    m_statusErr->setRed();

    statusLayout->addWidget(m_statusOk);
    statusLayout->addWidget(m_statusWarn);
    statusLayout->addWidget(m_statusErr);

    m_chart = new LineChartWidget();
    m_chart->setRange(0, 100);

    QVector<double> samples;
    for (int i = 0; i < 16; ++i) {
        samples << QRandomGenerator::global()->bounded(20, 95);
    }
    m_chart->setValues(samples);

    AddRow();
    AddWidget("Battery", m_battery, 5);
    AddWidget("Status", statusPanel, 4);

    AddRow();
    AddWidget("Trend", m_chart, 1);

    applyTheme(qApp->property("Theme").value<Theme>());
}

bool SampleDashboardDock::event(QEvent *e)
{
    if (e->type() == Theme::EventType) {
        applyTheme(qApp->property("Theme").value<Theme>());
    }
    return DockWidget::event(e);
}

void SampleDashboardDock::applyTheme(const Theme &theme)
{
    if (!m_battery) {
        return;
    }

    m_battery->setNormalColorStart(theme.split_hover.lighter(110));
    m_battery->setNormalColorEnd(theme.split_hover.darker(110));
    m_battery->setBorderColorStart(theme.dock_content_item_header_text_color);
    m_battery->setBorderColorEnd(theme.dock_border);
}
