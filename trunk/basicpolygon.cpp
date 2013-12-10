#include "basicpolygon.h"

void BasicPolygon::applyTransformations(Transform trans)
{
    this->setTranslate( trans.getTranslate() );
    if(trans.getMatrix().isSet == true)
        this->setMatrix( trans.getMatrix() );
    if(trans.getRotate().angle != 0)
    {
        this->setRotation( trans.getRotate() );
    }
}

void BasicPolygon::setTranslate(QPointF offset)
{
    myPolygon.translate( offset );
}

void BasicPolygon::setRotation(rotationTransform transform)
{
    QTransform t;
    t.translate( transform.center.x(), transform.center.y() );
    t.rotate( transform.angle );
    t.translate( -transform.center.x(), -transform.center.y() );
    myPolygon = t.map(myPolygon);
}

void BasicPolygon::setMatrix(matrix3x3_struct matrix)
{
    //qDebug() << "BEFORE Matrix transformed polygon: " << myPolygon;
    QPolygonF tempPoly;
    foreach(QPointF p, myPolygon)
    {
        //See this website: https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/transform
        tempPoly << QPointF( matrix.m11*p.x() + matrix.m12*p.y()+matrix.m13,
                             matrix.m21*p.x()+matrix.m22*p.y()+matrix.m23 );
    }

    //qDebug() << "Matrix transformed polygon: " << tempPoly;
    myPolygon = tempPoly;
}
