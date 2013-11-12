#ifndef SVGITEM_H
#define SVGITEM_H

#include <QGraphicsRectItem>
#include <QPainter>
#include "line.h"
#include <QDebug>
#include "circle.h"

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

private:
    QList<Line*> lineList;
    QList<Circle*> circleList;

};

#endif // SVGITEM_H
