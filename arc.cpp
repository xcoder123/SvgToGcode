#include "arc.h"


/*!
 * \brief Arc::Arc
 * \param radii - radius of x and y axis of ellipse, semi-minor and semi-major axis
 * \param rotation - rotated relative to the current coordinate system.
 * \param largeArcFlag - large arc flag. If true the arc spanning less than or equal
 *                       to 180 degrees is chose, otherwise the spanning greate than 180
 *                       degrees is chosen
 * \param sweepFlag -   If true line joining center to arch sweeps though decreasing angles
 *                      otherwise it sweeps thorugh decreasing angles
 * \param endPos    -   absolute position of final points of the arc (read from SVG)
 * \param initPos   -   absoule initial position of the arc (cPos)
 */


Arc::Arc(QPointF radii, double rotation, int largeArcFlag, int sweepFlag, QPointF endPos, QPointF initPos)
{
    //See these pages for implemenatation:
    //https://java.net/projects/svgsalamander/sources/svn/content/trunk/svg-core/src/main/java/com/kitfox/svg/pathcmd/Arc.java
    //http://www.w3.org/TR/SVG/implnote.html#ArcImplementationNotes



    //Compute half distance between current and final point
    QPointF deltaPos = (initPos-endPos)/2.0;
    double angle = qDegreesToRadians( fmod( rotation, 360.0) );
    double cosAngle = qCos(angle);
    double sinAngle = qSin(angle);

    //Step 1 : Compute (x1, y1)
    QPointF primePos = QPointF(
                                 cosAngle * deltaPos.x() + sinAngle * deltaPos.y(),
                                 -sinAngle * deltaPos.x() + cosAngle * deltaPos.y()
                                );

    //Corrention of out-of-range-radii
    radii = QPointF( qAbs(radii.x()), qAbs(radii.y()) );
    double deltaRadii = (primePos.x() * primePos.x())/(radii.x()*radii.x())
            + (primePos.y()*primePos.y())/(radii.y() * radii.y());
    if(deltaRadii > 1)
    {
        radii = QPointF(
                    qSqrt(deltaRadii) * radii.x(),
                    qSqrt(deltaRadii) * radii.y()
                    );
    }

    //Step 2 : Compute (cx1, cy1)
    QPointF radiiSq = QPointF( radii.x() * radii.x() , radii.y() * radii.y()); //Squared radii
    int sign = largeArcFlag == sweepFlag ? -1 : 1;
    double sq = (radiiSq.x()*radiiSq.y() - radiiSq.x()*(primePos.y()*primePos.y()) - radiiSq.y()*(primePos.x()*primePos.x()))
            / ( radiiSq.x()*(primePos.y()*primePos.y()) + radiiSq.y()*(primePos.x()*primePos.x()) );
    sq = sq < 0 ? 0 : sq;
    double coef = sign * qSqrt(sq);
    QPointF cp1 = QPointF(
                coef*((radii.x()*primePos.y())/radii.y()),
                coef* (-( (radii.y()*primePos.x())/(radii.x()) ))
                        ); //center position prime

    //Step 3 : compute (cx, cy) from cx' and cy'
    QPointF sumPos = (initPos+endPos)/2.0;
    QPointF cp = QPointF(
                    cosAngle*cp1.x() - sinAngle*cp1.y() + sumPos.x(),
                    sinAngle*cp1.x() + cosAngle*cp1.y() + sumPos.y()
                );
    //Step 4 : compute theta1 and deltaTheta
    QPointF up = QPointF(
                    (primePos.x()-cp1.x()) / radii.x(),
                    (primePos.y()-cp1.y()) / radii.y()
                );

    QPointF vp = QPointF(
                    (-primePos.x()-cp1.x()) / radii.x(),
                    (-primePos.y()-cp1.y()) / radii.y()
                );

    double p, n;

    //Compute theta
    n = qSqrt( up.x()*up.x() + up.y()*up.y() );
    p = up.x(); //(1*up.x())+(0*u.y());
    sign = (up.y() < 0) ? -1 : 1;
    double theta = qRadiansToDegrees( sign* qAcos( p/n ) );

    //Compute deltaTheta
    n = qSqrt( (up.x()*up.x()+up.y()*up.y()) * ( vp.x()*vp.x()+vp.y()*vp.y() ) );
    p = up.x()*vp.x()+up.y()*vp.y();

    sign = (up.x() * vp.y() - up.y()*vp.x() < 0) ? -1 : 1;
    double deltaTheta = qRadiansToDegrees( sign*qAcos( p/n ) );

    if(!sweepFlag && deltaTheta > 0)
        deltaTheta -= 360.0;
    else if(sweepFlag && deltaTheta < 0)
        deltaTheta += 360.0;

    deltaTheta = fmod(deltaTheta, 360.0);
    theta = fmod(theta, 360.0);

    constructPolygon( cp , radii , qDegreesToRadians(-theta), qDegreesToRadians(-deltaTheta) );

    //Finally rotate:
    rotationTransform rot;
    rot.angle = rotation;
    rot.center = cp;
    setRotation( rot );

}

void Arc::constructPolygon(QPointF center, QPointF radii, double start, double extent)
{
    qDebug() << "Arc: " << center << radii <<  start << extent << qRadiansToDegrees(start) << qRadiansToDegrees(extent);
    double inc = (extent) / (ARC_POINTS-1);
    double theta = start;
    for(int i=0; i<=ARC_POINTS-1; i++)
    {

        myPolygon << QPointF(
                       center.x() + radii.x()*qCos(theta),
                       center.y() - radii.y()*qSin(theta)
                       );

        theta += inc;
    }

    //myPolygon << myPolygon[0];
}

