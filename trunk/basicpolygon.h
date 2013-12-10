#ifndef BASICPOLYGON_H
#define BASICPOLYGON_H

#include <QPolygonF>
#include "transform.h"
#include <QTransform>

class BasicPolygon
{
public:
    QPolygonF getPolygon() { return myPolygon; }
    void setTranslate(QPointF offset);
    void setRotation(rotationTransform transform);
    void setMatrix(matrix3x3_struct matrix);
    void applyTransformations( Transform trans );

protected:
    QPolygonF myPolygon;
};

#endif // BASICPOLYGON_H
