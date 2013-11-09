#ifndef SVGITEM_H
#define SVGITEM_H

#include <QGraphicsRectItem>
#include <QPainter>

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
    void setLineList(QList<QLineF*> list) { lineList = list; }
    void setCircleList(QList<QRectF*> list) { circleList = list; }

private:
    QList<QLineF*> lineList;
    QList<QRectF*> circleList;

};

#endif // SVGITEM_H
