#ifndef LIGHTBUTTON_H
#define LIGHTBUTTON_H

/**
 * Highlight button widget, mail to vivipoe2010@hotmail.com
 * 1. Text can be set and displayed in the center
 * 2. Text color can be set
 * 3. The outer border gradient color can be set
 * 4. Can set the gradient color of the inner border
 * 5. Can set background color
 * 6. Can directly call the built-in settings green/red/yellow/black/blue and other public slot functions
 * 7. Can be set whether it is movable in the container and used as an object
 * 8. Can set whether to display rectangle
 * 9. Alarm color + non-alarm color can be set
 * 10. It can control to start and stop the alarm, flashing when alarming
 */

#include <QWidget>

#ifdef quc
class Q_DECL_EXPORT LightButton : public QWidget
#else
class LightButton : public QWidget
#endif

{
    Q_OBJECT
    Q_PROPERTY(QString text READ getText WRITE setText)
    Q_PROPERTY(QColor textColor READ getTextColor WRITE setTextColor)
    Q_PROPERTY(QColor alarmColor READ getAlarmColor WRITE setAlarmColor)
    Q_PROPERTY(QColor normalColor READ getNormalColor WRITE setNormalColor)

    Q_PROPERTY(QColor borderOutColorStart READ getBorderOutColorStart WRITE setBorderOutColorStart)
    Q_PROPERTY(QColor borderOutColorEnd READ getBorderOutColorEnd WRITE setBorderOutColorEnd)
    Q_PROPERTY(QColor borderInColorStart READ getBorderInColorStart WRITE setBorderInColorStart)
    Q_PROPERTY(QColor borderInColorEnd READ getBorderInColorEnd WRITE setBorderInColorEnd)
    Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)

    Q_PROPERTY(bool canMove READ getCanMove WRITE setCanMove)
    Q_PROPERTY(bool showRect READ getShowRect WRITE setShowRect)
    Q_PROPERTY(bool showOverlay READ getShowOverlay WRITE setShowOverlay)
    Q_PROPERTY(QColor overlayColor READ getOverlayColor WRITE setOverlayColor)

public:
    explicit LightButton(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void paintEvent(QPaintEvent *);
    void drawBorderOut(QPainter *painter);
    void drawBorderIn(QPainter *painter);
    void drawBg(QPainter *painter);
    void drawText(QPainter *painter);
    void drawOverlay(QPainter *painter);

private:
    QString text;
    QColor textColor;
    QColor alarmColor;
    QColor normalColor;

    QColor borderOutColorStart;     //Outer border gradient start color
    QColor borderOutColorEnd;       //Outer border gradient end color
    QColor borderInColorStart;      //Inner border gradient start color
    QColor borderInColorEnd;        //Inner border gradient end color
    QColor bgColor;                 //background color

    bool showRect;                  //Display as rectangle
    bool canMove;                   //Can move
    bool showOverlay;               //Whether to show the mask layer
    QColor overlayColor;            //Mask layer color

    bool isAlarm;                   //Whether to call the police
    QTimer *timerAlarm;             //Timer switch color

public:
    QString getText()               const;
    QColor getTextColor()           const;
    QColor getAlarmColor()          const;
    QColor getNormalColor()         const;

    QColor getBorderOutColorStart() const;
    QColor getBorderOutColorEnd()   const;
    QColor getBorderInColorStart()  const;
    QColor getBorderInColorEnd()    const;
    QColor getBgColor()             const;

    bool getCanMove()               const;
    bool getShowRect()              const;
    bool getShowOverlay()           const;
    QColor getOverlayColor()        const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public Q_SLOTS:

    void setText(const QString &text);

    void setTextColor(const QColor &textColor);

    //Set alarm color + normal color
    void setAlarmColor(const QColor &alarmColor);
    void setNormalColor(const QColor &normalColor);

    //Set the outer border gradient color
    void setBorderOutColorStart(const QColor &borderOutColorStart);
    void setBorderOutColorEnd(const QColor &borderOutColorEnd);

    //Set the gradient color of the inner border
    void setBorderInColorStart(const QColor &borderInColorStart);
    void setBorderInColorEnd(const QColor &borderInColorEnd);

    //Set background color
    void setBgColor(const QColor &bgColor);

    //Set whether it can be moved
    void setCanMove(bool canMove);
    //Set whether to display the rectangle
    void setShowRect(bool showRect);
    //Set whether to display the mask layer
    void setShowOverlay(bool showOverlay);
    //Set the color of the mask layer
    void setOverlayColor(const QColor &overlayColor);

    void setGreen();
    void setRed();
    void setYellow();
    void setBlack();
    void setGray();
    void setBlue();
    void setLightBlue();
    void setLightRed();
    void setLightGreen();

    //Set alarm flashing
    void startAlarm();
    void stopAlarm();
    void alarm();
};

#endif // LIGHTBUTTON_H
