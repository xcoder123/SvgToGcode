#include "transform.h"

Transform::Transform()
{
    translate = QPointF(0,0);
    rotation.angle = 0;
    matrix.isSet = false;

}

Transform::Transform(QString transform)
{
    translate = QPointF(0,0);
    rotation.angle = 0;
    matrix.isSet = false;

    QStringList cmds = transform.split(")");

    foreach(QString cmd, cmds)
    {
        cmd = cmd.simplified();

        if(cmd.contains("translate"))
        {
            QString translateStr = cmd.mid( cmd.indexOf("(")+1, cmd.indexOf(")")-1 );
            translateStr.replace(",", " ");
            QStringList numStr = translateStr.split(" ");
            translate = QPointF( qAbs(numStr[0].toDouble()), qAbs(numStr[1].toDouble()));
        }
        else if(cmd.contains("rotate"))
        {
            QString rotateStr = cmd.mid( cmd.indexOf("(")+1, cmd.indexOf(")")-1 );
            rotateStr = rotateStr.simplified();
            QStringList numStr = rotateStr.split( "," );
            rotation.angle = numStr.at(0).toDouble();
            if(numStr.size() > 1)
            {
                rotation.center = QPointF(numStr[1].toDouble() , numStr[2].toDouble());
            }
        }
        else if(cmd.contains("matrix"))
        {
            QString matrixStr = cmd.mid( cmd.indexOf("(")+1, cmd.indexOf(")")-1 );
            matrixStr = matrixStr.simplified();
            QStringList numStr = matrixStr.split( "," );
            matrix.m11 = numStr[0].toDouble();
            matrix.m21 = numStr[1].toDouble();
            matrix.m12 = numStr[2].toDouble();
            matrix.m22 = numStr[3].toDouble();
            matrix.m13 = numStr[4].toDouble();
            matrix.m23 = numStr[5].toDouble();
            matrix.isSet = true;

            qDebug() << "matrix: " <<  matrix.m11 <<  matrix.m21 <<  matrix.m12 << matrix.m22
                            <<  matrix.m13 <<  matrix.m23;
        }
    }

}
