#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen_SVG, SIGNAL(triggered()), this, SLOT(openSvg()));
    connect(ui->actionGenerate_GCode, SIGNAL(triggered()), this, SLOT(generateGCode()));

    QDoubleSpinBox *penWidth = new QDoubleSpinBox(this);
    penWidth->setMinimum(0.1);
    penWidth->setValue(0.5);
    penWidth->setSingleStep( 0.1 );
    penWidth->setToolTip( tr("Pen width") );
    penWidth->setSuffix( " mm" );

    ui->mainToolBar->addWidget( penWidth );

    ui->graphicsView->setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
    scene = new QGraphicsScene(0, 0, 1200,1200, this);
    ui->graphicsView->setScene( scene );

    svgItem = new SvgItem();
    svgItem->setRect(0,0,200,200);
    scene->addItem( svgItem );


    elevation = 2.2;
    qDebug() << tr("Application locked and loaded...");   




}

void MainWindow::openSvg()
{
    /*QVector<QPointF> points;
    points << QPointF(10,10) << QPointF(20,40) << QPointF(30,10) << QPointF(50,20) << QPointF(40,80) << QPointF(70,20) << QPointF(10,40);
    qBezierList.append( new QBezier(points , 32 ) );
    svgItem->setQBezierList( qBezierList );*/

    QPointF cp = QPointF(20,20);
    Circle * c = new Circle(cp, 48, 20.0, 20.0);
    circleList.append(c);
    svgItem->setCircleList(circleList);
    Arc *arc = new Arc(QPointF(150,150), 0, 1, 0, QPointF(300,50), QPointF(150,200));
    arcList.push_back( arc );
    svgItem->setArcList( arcList );

    //lineList.append( new Line(48,10, 48,5) );

    //svgItem->setLineList(lineList);
    //svgItem->setRect(0,0,80, 90);
    qDebug() << "HW " << svgItem->rect();

    ui->graphicsView->fitInView(svgItem, Qt::KeepAspectRatio);

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open SVG"), "", tr("SVG File (*.svg)"));

    if(fileName.isEmpty())
        return;

    qDebug() << "Opening SVG: " << fileName;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Svg2GCode"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    read(&file);
}

void MainWindow::read(QIODevice *device)
{
    xml.setDevice( device );
    if (xml.readNextStartElement())
    {
        if(xml.name() == "svg")
        {
            readSVG();
        }
        else
        {
            QMessageBox::warning(this, tr("Svg2GCode"),
                                 tr("Not a valid SVG file"));
        }
    }
}

void MainWindow::readSVG()
{
    lineList.clear();
    circleList.clear();
    qBezierList.clear();
    polyList.clear();
    arcList.clear();


    QStringList commandsUSED;

    float width = 0;
    float height = 0;

    QPointF offset = QPointF(0,0);

    while (!xml.atEnd() && !xml.hasError())
    {

        if (xml.isStartElement())
        {
            QString name = xml.name().toString();

            qDebug() << "Cname " << name;
            if (name == "line")
            {
                //qDebug() << "element name: '" << name;
                QXmlStreamAttributes attrib = xml.attributes();
                QString style = attrib.value("style").toString();

                double lineWidth = style.mid( style.indexOf("stroke-width:")+13, style.indexOf(";")-13  ).toDouble();

                Line *line = new Line(qAbs(attrib.value("x1").toDouble()), qAbs(attrib.value("y1").toDouble()),
                                      qAbs(attrib.value("x2").toDouble()),qAbs(attrib.value("y2").toDouble()),
                                      lineWidth);



                qDebug() << "line: " << line->p1() << line->p2();
                lineList.push_back(line);


                updateBoundary( width, height,
                                (float)attrib.value("x1").toFloat(),
                                (float)attrib.value("x2").toFloat(),
                                (float)attrib.value("y1").toFloat(),
                                (float)attrib.value("y2").toFloat());

            }
            else if(name == "polygon")
            {
                QXmlStreamAttributes attrib = xml.attributes();
                QString pointsStr = attrib.value("points").toString();
                pointsStr.replace(",", " ");
                QVector<double> pointLst = parseSVGNumbers( pointsStr );
                if(pointLst.size() % 2 == 0) //Is the number of cooridnates even?
                {
                    Transform trans;
                    QPointF translate(0,0);
                    if(attrib.hasAttribute("transform"))
                    {
                        trans = Transform(attrib.value("transform").toString());
                        translate = trans.getTranslate();
                    }

                    QVector<QPointF> points;
                    for(int i = 0; i<pointLst.size(); i+=2)
                    {
                        points.push_back( QPointF(pointLst[i], pointLst[i+1]) );
                    }

                    Polygon* poly = new Polygon(points);
                    poly->setTranslate( translate );                    
                    if(trans.getMatrix().isSet == true)
                        poly->setMatrix( trans.getMatrix() );
                    if(trans.getRotate().angle != 0)
                    {
                        poly->setRotation( trans.getRotate() );
                    }
                    polyList.push_back( poly );

                    qDebug() << "Polygon:" << pointsStr << points;
                }
                else
                {
                    qDebug() << "Error, odd number of coordinates for polygon: " << pointLst;
                }
            }
            else if(name == "polyline")
            {
                QXmlStreamAttributes attrib = xml.attributes();
                QString pointsStr = attrib.value("points").toString();
                pointsStr.replace(",", " ");
                QVector<double> pointLst = parseSVGNumbers( pointsStr );
                if(pointLst.size() % 2 == 0) //Is the number of cooridnates even?
                {
                    Transform trans;
                    QPointF translate(0,0);
                    if(attrib.hasAttribute("transform"))
                    {
                        trans = Transform(attrib.value("transform").toString());
                        translate = trans.getTranslate();
                    }


                    QVector<QPointF> points;
                    for(int i = 0; i<pointLst.size(); i+=2)
                    {
                        points.push_back( QPointF(pointLst[i], pointLst[i+1]) );
                    }

                    Polygon* poly = new Polygon(points, false);
                    poly->setTranslate( translate );
                    if(trans.getRotate().angle != 0)
                    {
                        poly->setRotation( trans.getRotate() );
                    }
                    polyList.push_back( poly );

                    //qDebug() << "Polygon:" << pointsStr << points;
                }
                else
                {
                    qDebug() << "Error, odd number of coordinates for polygon: " << pointLst;
                }
            }
            else if(name == "ellipse")
            {
                //qDebug() << "element name: '" << name;
                QXmlStreamAttributes attrib = xml.attributes();
                float rx = attrib.value("rx").toFloat();
                float ry = attrib.value("rx").toFloat();
                /*Circle *circle = new Circle(attrib.value("cx").toFloat()-diameter/2, attrib.value("cy").toFloat()-diameter/2,
                            diameter,diameter);*/
                Circle *ellipse = new Circle(
                            QPointF(qAbs(attrib.value("cx").toDouble()), qAbs(attrib.value("cy").toDouble())),
                            24,
                            rx,
                            ry
                            );

                QRectF* cRect = new QRectF(attrib.value("cx").toFloat()-rx/2, attrib.value("cy").toFloat()-ry/2,
                                                     rx,ry);
                //qDebug() << "line: " << line;
                //qDebug() << offset;
                circleList.push_back(ellipse);
            }
            else if(name == "path")
            {
                //See this: http://www.w3.org/TR/SVG/paths.html#PathDataEllipticalArcCommands
                QXmlStreamAttributes attrib = xml.attributes();
                QString d = attrib.value("d").toString();
                qDebug( )  << "path: " << d << d.length();
                QStringList cmds;
                int i = 0;
                while(i < d.length())
                {

                    if(d[i] >= QChar('A')) //Captial A in ASCII
                    {
                        QString temp;
                        do
                        {
                            if(d[i] != ',')
                                temp.push_back( d[i] );
                            else
                                temp.push_back( ' ' );
                            i++;
                        } while(i < d.length() && d[i] < QChar('A'));
                        i--;
                        cmds << temp;
                    }
                    i++;
                }

                qDebug() << "CMDS" << cmds;

                Transform transformations = getTransforms( attrib );

                QPointF cPos = QPointF(0,0); //CURRENT POSITION
                QPointF zPos = QPointF(0,0); //Position of CLOSE PATH, used for Z command
                QPointF sPos = QPointF(0,0); //Position for S/s command, the second point of previous curve.
                foreach(QString cmd, cmds)
                {
                    qDebug() << cmd[0];
                    commandsUSED.push_back( cmd.mid(0,1) );
                    if(cmd[0] == 'M') //Absolute MOVE
                    {
                        // If a moveto is followed by multiple pairs of coordinates,
                        //the subsequent pairs are treated as implicit lineto commands.
                        qDebug() << "M" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=2)
                        {
                            if(i == 0) //If it's only first move, don't draw
                            {
                                cPos = QPointF(num[i], num[i+1]);
                                zPos = QPointF(num[i], num[i+1]);
                            }
                            else //Multiple moves = lineto command
                            {
                                QVector<QPointF> points;
                                points << cPos << QPointF(num[i], num[i+1]);
                                Polygon *poly = new Polygon(points, false);
                                cPos =  QPointF(num[i], num[i+1]);
                                //Apperantly, only the first point counts as new Z value.
                                //Couldn't find that in documentation, though. Strange.
                                //zPos = cPos;

                                    poly->applyTransformations( transformations );

                                polyList.push_back( poly );
                            }
                        }
                    }
                    if(cmd[0] == 'm') //Relative move
                    {
                        // If a moveto is followed by multiple pairs of coordinates,
                        //the subsequent pairs are treated as implicit lineto commands.
                        qDebug() << "m" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=2)
                        {
                            if(i == 0) //If it's only first move, don't draw
                            {
                                cPos = cPos + QPointF(num[i], num[i+1]);
                                zPos = cPos;
                            }
                            else //Multiple moves = lineto command
                            {
                                QVector<QPointF> points;
                                points << cPos <<cPos + QPointF(num[i], num[i+1]);
                                Polygon *poly = new Polygon(points, false);
                                cPos =  cPos + QPointF(num[i], num[i+1]);
                                //zPos = cPos;

                                    poly->applyTransformations( transformations );
                                polyList.push_back( poly );
                            }
                        }
                    }
                    if(cmd[0] == 'H') //Absolute horizontal line
                    {
                        qDebug() << "H" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=1)
                        {
                            QVector<QPointF> points;
                            points << cPos <<  QPointF(num[i], cPos.y());
                            Polygon *poly = new Polygon(points, false);
                            cPos =  QPointF(num[i], cPos.y());

                                poly->applyTransformations( transformations );
                            polyList.push_back( poly );
                        }
                    }
                    if(cmd[0] == 'h') //relative horizontal line
                    {
                        qDebug() << "h" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=1)
                        {
                            QVector<QPointF> points;
                            points << cPos <<cPos + QPointF(num[i],0);
                            Polygon *poly = new Polygon(points, false);
                            cPos =  cPos + QPointF(num[i], 0);

                                poly->applyTransformations( transformations );
                            polyList.push_back( poly );
                        }
                    }
                    if(cmd[0] == 'V') //Absolute vertical line
                    {
                        qDebug() << "V" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=1)
                        {
                            QVector<QPointF> points;
                            points << cPos <<  QPointF(cPos.x(), num[i]);
                            Polygon *poly = new Polygon(points, false);
                            cPos =  QPointF(cPos.x() , num[i]);

                                poly->applyTransformations( transformations );
                            polyList.push_back( poly );
                        }
                    }
                    if(cmd[0] == 'v') //relative vertical line
                    {
                        qDebug() << "v" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=1)
                        {
                            QVector<QPointF> points;
                            points << cPos <<cPos + QPointF(0 , num[i]);
                            Polygon *poly = new Polygon(points, false);
                            cPos =  cPos + QPointF(0, num[i]);

                                poly->applyTransformations( transformations );
                            polyList.push_back( poly );
                        }
                    }
                    if(cmd[0] == 'A')
                    {
                        qDebug() << "A" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=7)
                        {
                            Arc * arc = new Arc(QPointF(num[i], num[i+1]), num[i+2], num[i+3], num[i+4],
                                    QPointF(num[i+5], num[i+6]), cPos);
                            cPos =  QPointF(num[i+5], num[i+6]);


                                arc->applyTransformations( transformations );

                            //arcList.push_back( arc );
                                polyList.push_back( arc );
                        }
                    }
                    if(cmd[0] == 'a')
                    {
                        qDebug() << "a" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=7)
                        {
                            Arc * arc = new Arc(QPointF(num[i], num[i+1]), num[i+2], num[i+3], num[i+4],
                                    cPos+QPointF(num[i+5], num[i+6]), cPos);
                            cPos =  cPos + QPointF(num[i+5], num[i+6]);


                                arc->applyTransformations( transformations );

                            //arcList.push_back( arc );
                                polyList.push_back( arc );
                        }
                    }
                    if(cmd[0] == 'L') //Line to Absolute
                    {
                        qDebug() << "L" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=2)
                        {
                            QVector<QPointF> points;
                            points << cPos <<  QPointF(num[i], num[i+1]);
                            Polygon *poly = new Polygon(points, false);
                            cPos =  QPointF(num[i], num[i+1]);


                                poly->applyTransformations( transformations );

                            polyList.push_back( poly );
                        }
                    }
                    if(cmd[0] == 'l') // line to relative
                    {
                        qDebug() << "l" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=2)
                        {
                            QVector<QPointF> points;
                            points << cPos <<cPos + QPointF(num[i], num[i+1]);
                            Polygon *poly = new Polygon(points, false);
                            cPos =  cPos + QPointF(num[i], num[i+1]);


                                poly->applyTransformations( transformations );

                            polyList.push_back( poly );
                        }
                    }
                    if(cmd[0] == 'C') //Absolute cubic Bezier curve
                    {
                        //I think bezier curve might be auto filled, basically automaticaly Z cmd is initiated
                        qDebug() << "C" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=6)
                        {
                            /*cPos = QPointF(num[i], num[i+1]);
                            zPos = QPointF(num[i], num[i+1]);*/
                            QVector<QPointF> points;
                            points << cPos << QPointF(num[i], num[i+1]) << QPointF(num[i+2], num[i+3]) << QPointF(num[i+4], num[i+5]);
                            QBezier *bezier = new QBezier(points, 32);
                            cPos =  QPointF(num[i+4], num[i+5]);
                            sPos = QPointF( num[i+2], num[i+3] );


                                bezier->applyTransformations( transformations );

                            //qBezierList.push_back( bezier );
                                polyList.push_back( bezier );


                        }
                    }
                    if(cmd[0] == 'c') //Relative cubic Bezier curve
                    {
                        //I think bezier curve might be auto filled, basically automaticaly Z cmd is initiated
                        qDebug() << "c" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=6)
                        {
                            qDebug() << cPos;
                            /*cPos = QPointF(num[i], num[i+1]);
                            zPos = QPointF(num[i], num[i+1]);*/
                            QVector<QPointF> points;
                            points << cPos
                                   << cPos + QPointF(num[i], num[i+1])
                                   << cPos + QPointF(num[i+2], num[i+3])
                                   << cPos + QPointF(num[i+4], num[i+5]);

                            QBezier *bezier = new QBezier(points, 32);


                                bezier->applyTransformations( transformations );

                            //qBezierList.push_back( bezier );
                            polyList.push_back( bezier );

                            cPos = cPos + QPointF(num[i+4], num[i+5]);
                            sPos = cPos + QPointF( num[i+2], num[i+3] );

                        }
                    }
                    if(cmd[0] == 'S') //Absolute shorthand cubic Bezier curve
                    {
                        //I think bezier curve might be auto filled, basically automaticaly Z cmd is initiated
                        qDebug() << "S" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=4)
                        {
                            qDebug() << cPos;
                            /*cPos = QPointF(num[i], num[i+1]);
                            zPos = QPointF(num[i], num[i+1]);*/
                            QVector<QPointF> points;
                            points << cPos
                                   << 2*cPos - sPos
                                   << QPointF(num[i], num[i+1])
                                   << QPointF(num[i+2], num[i+3]);

                            QBezier *bezier = new QBezier(points, 32);



                                bezier->applyTransformations( transformations );

                            //qBezierList.push_back( bezier );
                                polyList.push_back( bezier );

                            sPos = QPointF(num[i], num[i+1]);
                            cPos = QPointF( num[i+2], num[i+3] );

                        }
                    }
                    if(cmd[0] == 's') //relative shorthand cubic Bezier curve
                    {
                        //I think bezier curve might be auto filled, basically automaticaly Z cmd is initiated
                        qDebug() << "s" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=4)
                        {
                            qDebug() << cPos;
                            /*cPos = QPointF(num[i], num[i+1]);
                            zPos = QPointF(num[i], num[i+1]);*/
                            QVector<QPointF> points;
                            points << cPos
                                   << 2*cPos - sPos
                                   << cPos + QPointF(num[i], num[i+1])
                                   << cPos + QPointF(num[i+2], num[i+3]);

                            QBezier *bezier = new QBezier(points, 32);


                                bezier->applyTransformations( transformations );

                            //qBezierList.push_back( bezier );
                                polyList.push_back( bezier );

                            sPos = cPos + QPointF(num[i], num[i+1]);
                            cPos = cPos + QPointF( num[i+2], num[i+3] );

                        }
                    }
                    if(cmd[0] == 'z' || cmd[0] == 'Z')
                    {
                        qDebug() << "z";
                        QVector<QPointF> points;
                        points << cPos <<zPos;
                        Polygon *poly = new Polygon(points, false);

                        //Line * line = new Polygon(cPos, zPos); //Needs width at some point
                        cPos = zPos;


                            poly->applyTransformations( transformations );

                        polyList.push_back( poly );
                        //this->lineList.push_back( line );
                    }
                }

            }
            else if (name == "circle")
            {
                //qDebug() << "element name: '" << name;
                QXmlStreamAttributes attrib = xml.attributes();
                float radius = attrib.value("r").toFloat();
                /*Circle *circle = new Circle(attrib.value("cx").toFloat()-diameter/2, attrib.value("cy").toFloat()-diameter/2,
                            diameter,diameter);*/
                Circle *circle = new Circle(
                            QPointF(qAbs(attrib.value("cx").toDouble()), qAbs(attrib.value("cy").toDouble())),
                            24,
                            radius,
                            radius
                            );

                QRectF* cRect = new QRectF(attrib.value("cx").toFloat()-radius/2, attrib.value("cy").toFloat()-radius/2,
                                                     radius,radius);
                //qDebug() << "line: " << line;
                //qDebug() << offset;
                //circleList.push_back(circle);
                polyList.push_back( circle );

                updateBoundary( width, height,
                                cRect->x(),
                                cRect->x()+cRect->width(),
                                cRect->y(),
                                cRect->y()+cRect->height()
                            );
            }
        }

        xml.readNext();
    }

    if (xml.hasError())
    {
        qDebug() << "XML error: " << xml.errorString() << endl;
    }
    else if (xml.atEnd())
    {
        qDebug() << "Reached end, done" << endl;


        height  *= -1;

        /*foreach(Line* line, lineList)
        {
            line->setPoints( QPointF(line->p1().x(), height+line->p1().y()),
                             QPointF(line->p2().x(), height+line->p2().y()));
        }*/

        /*foreach(Circle* circle, circleList)
        {
            circle->setRect( circle->x(), height+circle->y(), circle->width(), circle->height() );
        }*/



        svgItem->setLineList(lineList);
        svgItem->setCircleList(circleList);
        svgItem->setQBezierList( qBezierList );
        svgItem->setPolygonList( polyList );
        svgItem->setArcList( arcList );
        //svgItem->setRect(0,0,width, height);
        qDebug() << "HW " << width << height << svgItem->rect();

        qDebug() << "commands used: " << commandsUSED.toSet();
        ui->graphicsView->fitInView(svgItem, Qt::KeepAspectRatio);
        //ui->graphicsView->fitInView(QRectF(0,0,1200.0,1200.0), Qt::KeepAspectRatio);
    }
}

QVector<double> MainWindow::parseSVGNumbers(QString cmd)
{
    cmd.replace(QRegExp("[a-zA-Z]"), "");
    cmd.replace("-", " -");
    //qDebug( ) << "COMMAND " << cmd;
    cmd = cmd.trimmed();
    cmd = cmd.simplified();

    QStringList numStr = cmd.split(" ");
    QVector<double> num;
    foreach(QString str, numStr)
        //num.push_back( qAbs(str.toDouble()) ); // <-- honestly that was stupido.
        num.push_back( str.toDouble() );

    return num;
}

Transform MainWindow::getTransforms(QXmlStreamAttributes &attrib)
{
    Transform trans;
    if(attrib.hasAttribute("transform"))
    {
        trans = Transform(attrib.value("transform").toString());
    }

    return trans;
}

void MainWindow::generateGCode()
{
    QStringList gcode;

    gcode << "G21 ; set units to millimeters";
    //gcode << "G28 ;home all axes";
    gcode << "G92 X0 Y0 Z10 ;set home position to 10mm above platform";
    gcode << "G0 Z5";
    gcode << "G90 ; use absolute coordinates";
    gcode << "G1 F1800.000";
    gcode << QString("G1 Z%1 F7800.000 ;lift up %1 mm").arg(elevation,0,'f',3);

    //Do calculation here
    float dx = svgItem->rect().width()/2.0;
    float dy = svgItem->rect().height()/2.0;

    qDebug() << "Lines: " << lineList.size();
    qDebug() << "Bezier curver: " << qBezierList.size();

    foreach(QLineF *line, lineList)
    {
        gcode << QString("G1 X%1 Y%2 F1800.000").arg(line->x1(),0, 'f', 3).arg(line->y1(),0, 'f', 3);
        gcode << "G1 Z0.350 F7800.000";
        gcode << QString("G1 X%1 Y%2 F1800.000").arg(line->x2(),0, 'f', 3).arg(line->y2(),0, 'f', 3);
        gcode << QString("G1 Z%1 F1800.000 ;lift up %1 mm").arg(elevation,0,'f',3);
    }

//    foreach(QBezier * bezier , qBezierList)
//    {
//        QPolygonF polygon = bezier->getPolygon();
//        gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[0].x(),0, 'f', 3).arg(polygon[0].y(),0, 'f', 3);
//        gcode << "G1 Z0.350 F7800.000";
//        for(int i=1; i<polygon.size(); i++)
//        {
//            gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[i].x(),0, 'f', 3).arg(polygon[i].y(),0, 'f', 3);
//        }

//        gcode << QString("G1 Z%1 F7800.000 ;lift up %1 mm").arg(elevation,0,'f',3);
//    }

    QList< QList<BasicPolygon*> > optimizedPoly;

    QList<BasicPolygon*> tempPoly = polyList;
    int polyCount = 0;
    while(tempPoly.size())
    {
        QList<BasicPolygon*> tmpPolyLst;

        tmpPolyLst = searchPolygons( tempPoly.first(), tempPoly );

        optimizedPoly.append(tmpPolyLst  );

        foreach(BasicPolygon* poly, tmpPolyLst)
        {
            tempPoly.removeOne( poly );
            polyCount++;
        }


    }
    qDebug() << "Poly count: " << polyCount << polyList.size() << optimizedPoly.size();


    /*foreach(BasicPolygon * poly , polyList)
    {
        QPolygonF polygon = poly->getPolygon();
        gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[0].x(),0, 'f', 3).arg(polygon[0].y(),0, 'f', 3);
        gcode << "G1 Z0.350 F7800.000";
        for(int i=1; i<polygon.size(); i++)
        {
            gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[i].x(),0, 'f', 3).arg(polygon[i].y(),0, 'f', 3);
        }

        gcode << QString("G1 Z%1 F7800.000 ;lift up %1 mm").arg(elevation,0,'f',3);
    }*/

    foreach(QList<BasicPolygon*> list, optimizedPoly)
    {
        gcode << QString("G1 X%1 Y%2 F1800.000").arg(list.first()->getPolygon().at(0).x(),0, 'f', 3).arg(list.first()->getPolygon().at(0).y(),0, 'f', 3);
        gcode << "G1 Z0.350 F7800.000";
        foreach(BasicPolygon * poly , list)
        {
            QPolygonF polygon = poly->getPolygon();

            for(int i=1; i<polygon.size(); i++)
            {
                gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[i].x(),0, 'f', 3).arg(polygon[i].y(),0, 'f', 3);
            }

        }
        gcode << QString("G1 Z%1 F7800.000 ;lift up %1 mm").arg(elevation,0,'f',3);
    }



    foreach(Circle * circle , circleList)
    {
        QPolygonF polygon = circle->getPolygon();
        gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[0].x(),0, 'f', 3).arg(polygon[0].y(),0, 'f', 3);
        gcode << "G1 Z0.350 F7800.000";
        for(int i=1; i<polygon.size(); i++)
        {
            gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[i].x(),0, 'f', 3).arg(polygon[i].y(),0, 'f', 3);
        }

        gcode << QString("G1 Z%1 F7800.000 ;lift up %1 mm").arg(elevation,0,'f',3);
    }

    gcode << "G1 Z10 F7800.000 ;Pull up z axis by 10mm";
    gcode << "M84 ;Disable motors";

    GCodeViewer *gviewer = new GCodeViewer(this);
    gviewer->setGCode( gcode );
    gviewer->show();
}

QList<BasicPolygon*> MainWindow::searchPolygons(BasicPolygon *previous, QList<BasicPolygon*> fullLst)
{
    QList<BasicPolygon*> lst;
    lst.append(previous);
    fullLst.removeOne( previous );
    foreach(BasicPolygon* poly, fullLst)
    {
        if(previous->getPolygon().last() == poly->getPolygon().first())
        {
            lst.append( searchPolygons(poly, fullLst) );
            break;
        }
    }
    return lst;
}

void MainWindow::updateBoundary(float &w, float &h, float x1, float x2, float y1, float y2)
{
    if(w < x1)
        w = x1;
    if(w < x2)
        w =x2;
    if(h > y1)
        h = y1;
    if(h > y2)
        h = y2;
}

MainWindow::~MainWindow()
{
    delete ui;
}
