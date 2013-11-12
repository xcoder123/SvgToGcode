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
    pen.setWidthF(0.15);
    painter->setPen(pen);

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
        QBrush brush = painter->brush();
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);

        painter->drawEllipse(circle->center(), circle->height(), circle->width());
    }

    //QGraphicsRectItem::paint(painter, option, widget);
}
