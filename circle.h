#ifndef CIRCLE_H
#define CIRCLE_H

#include <QRectF>
#include <QSizeF>
#include <QtMath>
#include <QPolygonF>
#include <QDebug>

class Circle
{

public:
    Circle (QPointF center, int steps, double radius, double thickness = 0.25);


    double getWidth() { return thickness; }
    QPolygonF getPolygon() { return polygon; }

private:
    double thickness;
    QPolygonF polygon;

};

#endif // CIRCLE_H
