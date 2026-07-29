#ifndef LINECHARTWIDGET_H
#define LINECHARTWIDGET_H

#include <QWidget>
#include <QVector>

class LineChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LineChartWidget(QWidget *parent = nullptr);

    void setValues(const QVector<double> &values);
    void setRange(double minValue, double maxValue);

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *e) override;

private:
    QVector<double> m_values;
    double m_minValue = 0.0;
    double m_maxValue = 100.0;
};

#endif // LINECHARTWIDGET_H
