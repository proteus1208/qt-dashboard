#ifndef BATTERY_H
#define BATTERY_H

/**
 * Battery level widget, mail to vivipoe2010@hotmail.com
 * 1. The battery level can be set, and the battery level change can be dynamically switched
 * 2. The battery power warning value can be set
 * 3. The color of normal battery level and alarm color can be set
 * 4. the border gradient color can be set
 * 5. You can set the step length of each movement when the battery changes
 * 6. the frame rounded angle/background progress rounded angle/head rounded angle can be set
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT Battery : public QWidget
#else
class Battery : public QWidget
#endif

{
    Q_OBJECT    
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(double value READ getValue WRITE setValue)
    Q_PROPERTY(double alarmValue READ getAlarmValue WRITE setAlarmValue)

    Q_PROPERTY(double step READ getStep WRITE setStep)
    Q_PROPERTY(int borderWidth READ getBorderWidth WRITE setBorderWidth)
    Q_PROPERTY(int borderRadius READ getBorderRadius WRITE setBorderRadius)
    Q_PROPERTY(int bgRadius READ getBgRadius WRITE setBgRadius)
    Q_PROPERTY(int headRadius READ getHeadRadius WRITE setHeadRadius)

    Q_PROPERTY(QColor borderColorStart READ getBorderColorStart WRITE setBorderColorStart)
    Q_PROPERTY(QColor borderColorEnd READ getBorderColorEnd WRITE setBorderColorEnd)

    Q_PROPERTY(QColor alarmColorStart READ getAlarmColorStart WRITE setAlarmColorStart)
    Q_PROPERTY(QColor alarmColorEnd READ getAlarmColorEnd WRITE setAlarmColorEnd)

    Q_PROPERTY(QColor normalColorStart READ getNormalColorStart WRITE setNormalColorStart)
    Q_PROPERTY(QColor normalColorEnd READ getNormalColorEnd WRITE setNormalColorEnd)

public:
    explicit Battery(QWidget *parent = 0);
    ~Battery();

protected:
    void paintEvent(QPaintEvent *);
    void drawBorder(QPainter *painter);
    void drawBg(QPainter *painter);
    void drawHead(QPainter *painter);

private slots:
    void updateValue();

private:    
    double minValue;                //Minimum
    double maxValue;                //Maximum
    double value;                   //Target battery
    double alarmValue;              //Battery level warning value

    double step;                    //The step length of each move
    int borderWidth;                //Border thickness
    int borderRadius;               //Border rounded angle
    int bgRadius;                   //Background progress rounded angle
    int headRadius;                 //Head fillet angle

    QColor borderColorStart;        //Border gradient start color
    QColor borderColorEnd;          //Border gradient end color

    QColor alarmColorStart;         //Gradient start color when battery is low
    QColor alarmColorEnd;           //The end color of the gradient when the battery is low

    QColor normalColorStart;        //The gradient start color when the battery is normal
    QColor normalColorEnd;          //The end color of the gradient when the battery is normal

    bool isForward;                 //Whether to move forward
    double currentValue;            //Current battery
    QRectF batteryRect;             //Battery main area
    QTimer *timer;                  //Draw timer

public:    
    double getMinValue()            const;
    double getMaxValue()            const;
    double getValue()               const;
    double getAlarmValue()          const;

    double getStep()                const;
    int getBorderWidth()            const;
    int getBorderRadius()           const;
    int getBgRadius()               const;
    int getHeadRadius()             const;

    QColor getBorderColorStart()    const;
    QColor getBorderColorEnd()      const;

    QColor getAlarmColorStart()     const;
    QColor getAlarmColorEnd()       const;

    QColor getNormalColorStart()    const;
    QColor getNormalColorEnd()      const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:
    //Set range value
    void setRange(double minValue, double maxValue);
    void setRange(int minValue, int maxValue);

    //Set the maximum and minimum
    void setMinValue(double minValue);
    void setMaxValue(double maxValue);

    //Set the battery level value
    void setValue(double value);
    void setValue(int value);

    //Set the battery level warning value
    void setAlarmValue(double alarmValue);
    void setAlarmValue(int alarmValue);

    //Set step size
    void setStep(double step);
    void setStep(int step);

    //Set the border thickness
    void setBorderWidth(int borderWidth);
    //Set the rounded corner angle of the border
    void setBorderRadius(int borderRadius);
    //Set the background fillet angle
    void setBgRadius(int bgRadius);
    //Set the head fillet angle
    void setHeadRadius(int headRadius);

    //Set the border gradient color
    void setBorderColorStart(const QColor &borderColorStart);
    void setBorderColorEnd(const QColor &borderColorEnd);

    //Set the gradient color when battery power alarm
    void setAlarmColorStart(const QColor &alarmColorStart);
    void setAlarmColorEnd(const QColor &alarmColorEnd);

    //Set the gradient color when the battery level is normal
    void setNormalColorStart(const QColor &normalColorStart);
    void setNormalColorEnd(const QColor &normalColorEnd);

Q_SIGNALS:
    void valueChanged(double value);
};

#endif // BATTERY_H
