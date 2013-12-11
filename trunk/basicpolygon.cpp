#include "basicpolygon.h"

void BasicPolygon::applyTransformations(QList<AbstractTransform*> trans)
{
    if(trans.size() == 0)
        return;


    for(int i=trans.size()-1; i>=0; i--)
    {
        AbstractTransform* t = trans.at(i);
        if(t->type() == AbstractTransform::TRANS_ROTATE)
        {
            RotationTransform *rot = dynamic_cast<RotationTransform*>(t);
            setRotation(rot);
        }
        else if(t->type() == AbstractTransform::TRANS_MATRIX)
        {
            Matrix3x3* m = dynamic_cast<Matrix3x3*>(t);
            setMatrix(m);
        }
        else if(t->type() == AbstractTransform::TRANS_TRANSLATE)
        {
            Translate* translate = dynamic_cast<Translate*>(t);
            setTranslate(translate);
        }
    }
}

void BasicPolygon::applyTransformations(QList<QList<AbstractTransform *> > trans)
{
    for(int i=trans.size()-1; i>=0; i--)
    {
        applyTransformations( trans.at(i) );
    }
}

void BasicPolygon::setTranslate(Translate* offset)
{
    myPolygon.translate( offset->translate );
}

void BasicPolygon::setRotation(RotationTransform* transform)
{
    QTransform t;
    t.translate( transform->center.x(), transform->center.y() );
    t.rotate( transform->angle );
    t.translate( -transform->center.x(), -transform->center.y() );
    myPolygon = t.map(myPolygon);
}

void BasicPolygon::setMatrix(Matrix3x3* matrix)
{
    //qDebug() << "BEFORE Matrix transformed polygon: " << myPolygon;
    QPolygonF tempPoly;
    foreach(QPointF p, myPolygon)
    {
        //See this website: https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/transform
        tempPoly << QPointF( matrix->m11*p.x() + matrix->m12*p.y()+matrix->m13,
                             matrix->m21*p.x()+matrix->m22*p.y()+matrix->m23 );
    }

    //qDebug() << "Matrix transformed polygon: " << tempPoly;
    myPolygon = tempPoly;
}
