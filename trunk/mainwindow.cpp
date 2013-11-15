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
    Circle * c = new Circle(cp, 48, 20.0);
    circleList.append(c);
    svgItem->setCircleList(circleList);

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
            else if(name == "path")
            {
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

                QPointF cPos = QPointF(0,0);
                QPointF zPos = QPointF(0,0);
                foreach(QString cmd, cmds)
                {
                    if(cmd[0] == 'M')
                    {
                        qDebug() << "M" << parseSVGNumbers(cmd);
                        QVector<double> num = parseSVGNumbers(cmd);
                        for(int i=0; i<num.size(); i+=2)
                        {
                            cPos = QPointF(num[i], num[i+1]);
                            zPos = QPointF(num[i], num[i+1]);
                        }
                    }
                    if(cmd[0] == 'C')
                    {
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
                            qBezierList.push_back( bezier );

                            //qDebug() << "bez";
                        }
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
                            radius
                            );

                QRectF* cRect = new QRectF(attrib.value("cx").toFloat()-radius/2, attrib.value("cy").toFloat()-radius/2,
                                                     radius,radius);
                //qDebug() << "line: " << line;
                //qDebug() << offset;
                circleList.push_back(circle);

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
        //svgItem->setRect(0,0,width, height);
        qDebug() << "HW " << width << height << svgItem->rect();

        ui->graphicsView->fitInView(svgItem, Qt::KeepAspectRatio);
        //ui->graphicsView->fitInView(QRectF(0,0,1200.0,1200.0), Qt::KeepAspectRatio);
    }
}

QVector<double> MainWindow::parseSVGNumbers(QString cmd)
{
    cmd = cmd.remove(0,1).trimmed();
    QStringList numStr = cmd.split(" ");
    QVector<double> num;
    foreach(QString str, numStr)
        num.push_back( qAbs(str.toDouble()) );

    return num;
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

    foreach(QBezier * bezier , qBezierList)
    {
        QPolygonF polygon = bezier->getPolygon();
        gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[0].x(),0, 'f', 3).arg(polygon[0].y(),0, 'f', 3);
        gcode << "G1 Z0.350 F7800.000";
        for(int i=1; i<polygon.size(); i++)
        {
            gcode << QString("G1 X%1 Y%2 F1800.000").arg(polygon[i].x(),0, 'f', 3).arg(polygon[i].y(),0, 'f', 3);
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
