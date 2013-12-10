#include "circle.h"

Circle::Circle(QPointF center, int steps, double rx, double ry, double thickness)
{
    this->thickness = thickness;

    double inc = (2.0 * M_PI) / (steps-1);
    double theta = 0.0;
    for(int i=0; i<steps-1; i++)
    {

        myPolygon << QPointF(
                       center.x() + rx*qCos(theta),
                       center.y() - ry*qSin(theta)
                       );

        theta += inc;
    }

    myPolygon << myPolygon[0];

    //qDebug() << polygon;
}

