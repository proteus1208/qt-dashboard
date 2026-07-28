#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QEvent>

class Theme{

public:
    static const int EventType = 156128;
    Theme(){

    }
    Theme(QString name){
        this->name = name;
    }

    QString name = "Default";

};
Q_DECLARE_METATYPE(Theme);

class ThemeChangedEvent : QEvent{
public :
    Theme theme;
};

#endif // THEME_H
