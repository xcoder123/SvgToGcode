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
    QPen pen(Qt::black);
    pen.setWidthF(0.25);
    painter->setPen(pen);

    foreach(QLineF* line, lineList)
        painter->drawLine(line->p1(), line->p2());
    foreach(QRectF* circle, circleList)
        painter->drawEllipse(circle->center(), circle->height(), circle->width());

    //QGraphicsRectItem::paint(painter, option, widget);
}
