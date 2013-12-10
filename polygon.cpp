#include "polygon.h"

Polygon::Polygon(QVector<QPointF> points, bool close)
{
    foreach(QPointF p, points)
        myPolygon.push_back( p );

    if(close)
        myPolygon.push_back( points.at(0) );
}


