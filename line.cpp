#include "line.h"

Line::Line()
{
}

Line::Line(const QLine &line, double width)
    :QLineF(line)
{
    this->width = width;
}

Line::Line(const QPointF &p1, const QPointF &p2, double width)
    :QLineF(p1,p2)
{
    this->width = width;
}

Line::Line(qreal x1, qreal y1, qreal x2, qreal y2, double width)
    :QLineF(x1,y1,x2,y2)
{
    this->width = width;
}
