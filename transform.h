#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QtMath>
#include <QDebug>
#include <QPointF>
#include <QStringList>

class Transform
{
public:
    Transform(QString transform);

    QPointF getTranslate() { return translate; }
    double getRotate() {return rotation; }

private:
    QPointF translate;
    double rotation;
};

#endif // TRANSFORM_H
