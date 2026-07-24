#ifndef LISTANIMATION_H
#define LISTANIMATION_H

#include <QObject>
#include <QVariantAnimation>
#include <QList>

class ListAnimation : public QObject
{
    Q_OBJECT

public:
    explicit ListAnimation(QObject *parent = nullptr)
        : QObject(parent)
    {
        connect(&m_animation, &QVariantAnimation::valueChanged,
                this, [this](const QVariant &value)
                {
                    double progress = value.toDouble();

                    m_current.resize(m_start.size());

                    for (int i = 0; i < m_start.size(); ++i)
                    {
                        m_current[i] = m_start[i] +
                                       (m_target[i] - m_start[i]) * progress;
                    }

                    emit changed(m_current);
                });

        connect(&m_animation, &QVariantAnimation::finished,
                this, &ListAnimation::finished);

        m_animation.setStartValue(0.0);
        m_animation.setEndValue(1.0);
    }

    void setCurrentList(const QList<double> &list)
    {
        m_start = list;
        m_current = list;
    }

    void setTargetList(const QList<double> &list)
    {
        m_target = list;
    }

    QList<double> currentList() const
    {
        return m_current;
    }

    void setDuration(int duration)
    {
        m_animation.setDuration(duration);
    }

    void start()
    {
        m_animation.stop();
        if (m_start.size() != m_target.size())
            return;

        m_animation.start();
    }

    void stop()
    {
        m_animation.stop();
    }

signals:
    void changed(const QList<double> &list);
    void finished();

private:
    QList<double> m_start;
    QList<double> m_target;
    QList<double> m_current;

    QVariantAnimation m_animation;
};

#endif // LISTANIMATION_H