#ifndef BASICPOLYGON_H
#define BASICPOLYGON_H

#include <QPolygonF>
#include "transform.h"
#include <QTransform>

class BasicPolygon
{
public:
    QPolygonF getPolygon() { return myPolygon; }
    void setTranslate(Translate* offset);
    void setRotation(RotationTransform* transform);
    void setMatrix(Matrix3x3* matrix);
    void applyTransformations( QList<AbstractTransform*> trans );
    void applyTransformations( QList< QList<AbstractTransform*> > trans );

protected:
    QPolygonF myPolygon;
};

#endif // BASICPOLYGON_H
