#include "qbezier.h"

//http://www.codeproject.com/Articles/25237/Bezier-Curves-Made-Simple
//Check the general case. Notice I missed binominial coefficient when coding

QBezier::QBezier(QVector<QPointF> points, int steps)
{
    int n = points.size()-1;
    for(double t=0; t<1; t+=1.0/steps)
    {
        QPointF newP;

        for(int i=0; i<=n; i++)
        {
            newP.setX( newP.x() + points[i].x()*qPow(1.0-t,n-i)*qPow(t,i)*binomialCoefficient(n, i) );
            newP.setY( newP.y() + points[i].y()*qPow(1.0-t,n-i)*qPow(t,i)*binomialCoefficient(n, i) );
        }
        /*QPointF p0 = points.at(0);
        QPointF p1 = points.at(1);
        QPointF p2 = points.at(2);
        newP.setX( qPow((1.0-t),2.0)*p0.x()+2*t*(1.0-t)*p1.x()+qPow(t,2.0)*p2.x() );
        newP.setY( qPow((1.0-t),2.0)*p0.y()+2*t*(1.0-t)*p1.y()+qPow(t,2.0)*p2.y() );*/

        //this->setPoint(point++, newP);
        myPolygon.push_back( newP );

    }

    //qDebug() << "Bezier: " << myPolygon << n;
}

int QBezier::binomialCoefficient(int n, int i)
{
    return (factorial(n) / ( factorial(i)*factorial(n-i) ) );
}

int QBezier::factorial(int n)
{
    if(n == 0)
        return 1;
    return n*factorial(n-1);
}
