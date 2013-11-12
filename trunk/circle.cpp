#include "circle.h"

Circle::Circle()
    :QRectF()
{
}

Circle::Circle(const QPointF &topLeft, const QPointF &bottomRight, double thickness)
    :QRectF(topLeft, bottomRight)
{
    this->thickness = thickness;
}

Circle::Circle(const QPointF &topLeft, const QSizeF &size, double thickness)
    :QRectF(topLeft, size)
{
    this->thickness = thickness;
}

Circle::Circle(const QRect &rectangle, double thickness)
    :QRectF(rectangle)
{
    this->thickness = thickness;
}

Circle::Circle(qreal x, qreal y, qreal width, qreal height, double thickness)
    :QRectF(x,y,width,height)
{
    this->thickness = thickness;
}
