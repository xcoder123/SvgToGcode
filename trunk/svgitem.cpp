#include "svgitem.h"

SvgItem::SvgItem(QGraphicsItem *parent) :
    QGraphicsRectItem(parent)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
}

void SvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint( QPainter::Antialiasing );
    QPen pen(Qt::black, 0.15, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setWidthF(0.5);
    painter->setPen(pen);


//    foreach(QBezier *qbezier, qBezierList)
//    {
//        //qDebug() << qbezier->getPolygon();
//        painter->drawPolyline(qbezier->getPolygon());
//    }

    foreach(BasicPolygon *poly, polyList)
    {
        //qDebug() << qbezier->getPolygon();
        painter->drawPolyline(poly->getPolygon());
    }

//    foreach(Arc *arc, arcList)
//    {
//        //qDebug() << qbezier->getPolygon();
//        painter->drawPolyline(arc->getPolygon());
//    }

//    foreach(BasicPolygon* poly, polygons)
//    {
//        painter->drawPolyline( poly->getPolygon() );
//    }

    foreach(Line* line, lineList)
    {
        pen.setWidthF( line->getWidth() );
        painter->setPen(pen);
        painter->drawLine(line->p1(), line->p2());
    }



    pen.setWidthF(0.25);
    painter->setPen(pen);
    foreach(Circle* circle, circleList)
    {
        /*QBrush brush = painter->brush();
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);

        painter->drawEllipse(circle->center(), circle->height(), circle->width());*/
        painter->drawPolyline(circle->getPolygon());
    }


    //QGraphicsRectItem::paint(painter, option, widget);
}
