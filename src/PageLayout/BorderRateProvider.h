#ifndef BORDERRATEPROVIDER_H
#define BORDERRATEPROVIDER_H

#include <QList>

class BorderRateProvider
{
public:
    virtual ~BorderRateProvider() = default;

    virtual QList<double> borderRate() const = 0;
};

#endif // BORDERRATEPROVIDER_H