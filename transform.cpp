#include "transform.h"

Transform::Transform(QString transform)
{
    //Parse the translate
    QString translateStr = transform.mid( transform.indexOf("transform")+11, transform.indexOf(")")-11 );
    translateStr.replace(",", " ");
    QStringList numStr = translateStr.split(" ");
    translate = QPointF( qAbs(numStr[0].toDouble()), qAbs(numStr[1].toDouble()));

}
