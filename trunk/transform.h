#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QtMath>
#include <QDebug>
#include <QPointF>
#include <QStringList>
#include "abstracttransform.h"




class Transform
{
public:
    Transform();

    QList<AbstractTransform*> getTransforms(QString transform);

    /*QPointF getTranslate() { return translate; }
    rotationTransform getRotate() {return rotation; }
    matrix3x3_struct getMatrix() { return matrix; }*/


private:
    /*QPointF translate;
    rotationTransform rotation;
    matrix3x3_struct matrix;*/
    QList<AbstractTransform*> trList;

};

#endif // TRANSFORM_H

