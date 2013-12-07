#ifndef POLYGON_H
#define POLYGON_H

#include <QPolygonF>
#include <QtMath>
#include <QDebug>

class Polygon
{
public:
    explicit Polygon(QVector<QPointF> points, bool close = true);
    QPolygonF getPolygon() { return myPolygon; }
    void setTranslate(QPointF offset);

private:
    QPolygonF myPolygon;

};

#endif // POLYGON_H
