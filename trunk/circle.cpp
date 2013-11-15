#include "circle.h"

Circle::Circle(QPointF center, int steps, double radius, double thickness)
{
    this->thickness = thickness;

    double inc = (2.0 * M_PI) / (steps-1);
    double theta = 0.0;
    for(int i=0; i<steps-1; i++)
    {

        polygon << QPointF(
                       center.x() + radius*qCos(theta),
                       center.y() - radius*qSin(theta)
                       );

        theta += inc;
    }

    polygon << polygon[0];

    //qDebug() << polygon;
}

