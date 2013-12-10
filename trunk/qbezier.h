#ifndef QBEZIER_H
#define QBEZIER_H

#include <QPolygonF>
#include <QtMath>
#include <QDebug>

#include "basicpolygon.h"

class QBezier : public BasicPolygon
{    
public:
    explicit QBezier(QVector<QPointF> points, int steps);


private:

    int binomialCoefficient(int n, int i);
    int factorial(int n);

};

#endif // QBEZIER_H
