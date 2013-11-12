#ifndef CIRCLE_H
#define CIRCLE_H

#include <QRectF>
#include <QSizeF>

class Circle : public QRectF
{

public:
    Circle ();
    Circle ( const QPointF & topLeft, const QSizeF & size, double thickness = 0.25 );
    Circle ( const QPointF & topLeft, const QPointF & bottomRight, double thickness = 0.25 );
    Circle ( qreal x, qreal y, qreal width, qreal height, double thickness = 0.25 );
    Circle ( const QRect & rectangle, double thickness = 0.25 );

    double getWidth() { return thickness; }

private:
    double thickness;


};

#endif // CIRCLE_H
