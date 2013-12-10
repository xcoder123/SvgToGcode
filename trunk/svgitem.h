#ifndef SVGITEM_H
#define SVGITEM_H

#include <QGraphicsRectItem>
#include <QPainter>
#include "line.h"
#include <QDebug>
#include "circle.h"
#include "qbezier.h"
#include "polygon.h"
#include "arc.h"
#include "basicpolygon.h"

class SvgItem : public QGraphicsRectItem
{
public:
    enum { Type = UserType + 15 };
    explicit SvgItem(QGraphicsItem *parent = 0);

    int type() const
             { return Type;}


protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public slots:
    void setLineList(QList<Line*> list) { lineList = list; }
    void setCircleList(QList<Circle*> list) { circleList = list; }
    void setQBezierList(QList<QBezier*> list) { qBezierList = list; }
    void setPolygonList(QList<BasicPolygon*> list) { polyList = list; }
    void setArcList(QList<Arc*> list) { arcList = list; }

private:
    QList<Line*> lineList;
    QList<Circle*> circleList;
    QList<QBezier*> qBezierList;
    QList<BasicPolygon*> polyList;
    QList<Arc*> arcList;


};

#endif // SVGITEM_H
