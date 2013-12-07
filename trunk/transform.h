#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QtMath>
#include <QDebug>
#include <QPointF>
#include <QStringList>

struct rotationTransform
{
    double angle;
    QPointF center;
};

struct matrix3x3_struct
{
    double m11;
    double m12;
    double m13;
    double m21;
    double m22;
    double m23;
    bool isSet;
};


class Transform
{
public:
    Transform();
    Transform(QString transform);

    QPointF getTranslate() { return translate; }
    rotationTransform getRotate() {return rotation; }
    matrix3x3_struct getMatrix() { return matrix; }


private:
    QPointF translate;
    rotationTransform rotation;
    matrix3x3_struct matrix;

};

#endif // TRANSFORM_H

