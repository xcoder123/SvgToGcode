#ifndef LINE_H
#define LINE_H

#include <QLineF>
#include <QDebug>

class Line : public QLineF
{

public:
    Line ( const QPointF & p1, const QPointF & p2, double width = 0.25 );
    Line ( qreal x1, qreal y1, qreal x2, qreal y2, double width = 0.25 );
    Line ( const QLine & line, double width = 0.25 );
    Line();


public:
    double getWidth() { return width; }

private:
    double width;



};

#endif // LINE_H
