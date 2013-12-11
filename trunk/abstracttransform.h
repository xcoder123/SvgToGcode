#ifndef ABSTRACTTRANSFORM_H
#define ABSTRACTTRANSFORM_H

#include <QtMath>
#include <QDebug>
#include <QPointF>
#include <QStringList>

class AbstractTransform
{
public:
    virtual int type() = 0;
    enum TransformTypes { TRANS_ROTATE, TRANS_MATRIX, TRANS_TRANSLATE };
};

class RotationTransform : public AbstractTransform
{
public:
    int type() { return 0; }

    RotationTransform(double a, QPointF c) { angle = a; center = c; }

    double angle;
    QPointF center;
};

class Matrix3x3 : public AbstractTransform
{
public:
    int type() { return 1; }

    Matrix3x3() {}

    double m11;
    double m12;
    double m13;
    double m21;
    double m22;
    double m23;
    bool isSet;
};

class Translate : public AbstractTransform
{
public:
    int type() { return 2; }

    Translate(QPointF t) { translate = t; }

    QPointF translate;
};

#endif // ABSTRACTTRANSFORM_H
