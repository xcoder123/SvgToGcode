#ifndef CIRCLE_H
#define CIRCLE_H

#include <QRectF>
#include <QSizeF>
#include <QtMath>
#include <QPolygonF>
#include <QDebug>
#include "basicpolygon.h"

class Circle : public BasicPolygon
{

public:
    Circle (QPointF center, int steps, double rx, double ry, double thickness = 0.25);


    double getWidth() { return thickness; }

private:
    double thickness;


};

#endif // CIRCLE_H
