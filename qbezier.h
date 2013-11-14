#ifndef QBEZIER_H
#define QBEZIER_H

#include <QPolygonF>
#include <QtMath>
#include <QDebug>

class QBezier
{    
public:
    explicit QBezier(QVector<QPointF> points, int steps);
    QPolygonF getPolygon() { return myPolygon; }

private:
    QPolygonF myPolygon;

    int binomialCoefficient(int n, int i);
    int factorial(int n);

};

#endif // QBEZIER_H
