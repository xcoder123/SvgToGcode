#ifndef ARC_H
#define ARC_H

#include <QPolygonF>
#include <QtMath>
#include <QDebug>
#include "transform.h"
#include <QTransform>

#include "basicpolygon.h"

#define ARC_POINTS 64

class Arc : public BasicPolygon
{
public:
    explicit Arc(QPointF radii, double rotation,
                 int largeArcFlag, int sweepFlag,
                 QPointF endPos, QPointF initPos);

private slots:
    void constructPolygon(QPointF center, QPointF radii, double start, double extent);


};

#endif // ARC_H
