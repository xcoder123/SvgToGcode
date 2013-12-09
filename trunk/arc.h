#ifndef ARC_H
#define ARC_H

#include <QPolygonF>
#include <QtMath>
#include <QDebug>
#include "transform.h"
#include <QTransform>

#define ARC_POINTS 64

class Arc
{
public:
    explicit Arc(QPointF radii, double rotation,
                 int largeArcFlag, int sweepFlag,
                 QPointF endPos, QPointF initPos);
    QPolygonF getPolygon() { return myPolygon; }
    void setTranslate(QPointF offset);
    void setRotation(rotationTransform transform);
    void setMatrix(matrix3x3_struct matrix);

private slots:
    void constructPolygon(QPointF center, QPointF radii, double start, double extent);

private:
    QPolygonF myPolygon;
};

#endif // ARC_H
