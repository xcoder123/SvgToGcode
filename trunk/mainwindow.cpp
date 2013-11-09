#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen_SVG, SIGNAL(triggered()), this, SLOT(openSvg()));
    connect(ui->actionGenerate_GCode, SIGNAL(triggered()), this, SLOT(generateGCode()));

    scene = new QGraphicsScene(0, 0, 800,600, this);
    ui->graphicsView->setScene( scene );

    svgItem = new SvgItem();


    scene->addItem( svgItem );

}

void MainWindow::openSvg()
{
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

    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            QString name = xml.name().toString();
            if (name == "line")
            {
                qDebug() << "element name: '" << name;
                QXmlStreamAttributes attrib = xml.attributes();
                QLineF *line = new QLineF(attrib.value("x1").toFloat(), attrib.value("y1").toFloat(),
                            attrib.value("x2").toFloat(),attrib.value("y2").toFloat());

                //qDebug() << "line: " << line;
                lineList.push_back(line);


                updateBoundary( width, height,
                                (float)attrib.value("x1").toFloat(),
                                (float)attrib.value("x2").toFloat(),
                                (float)attrib.value("y1").toFloat(),
                                (float)attrib.value("y2").toFloat());

            }
            else if (name == "circle")
            {
                qDebug() << "element name: '" << name;
                QXmlStreamAttributes attrib = xml.attributes();
                float diameter = attrib.value("r").toFloat();
                QRectF *circle = new QRectF(attrib.value("cx").toFloat()-diameter/2, attrib.value("cy").toFloat()-diameter/2,
                            diameter,diameter);

                //qDebug() << "line: " << line;
                circleList.push_back(circle);

                updateBoundary( width, height,
                                circle->x(),
                                circle->x()+circle->width(),
                                circle->y(),
                                circle->y()+circle->height()
                            );
            }
        }
    }

    if (xml.hasError())
    {
        qDebug() << "XML error: " << xml.errorString() << endl;
    }
    else if (xml.atEnd())
    {
        qDebug() << "Reached end, done" << endl;


        height  *= -1;

        foreach(QLineF* line, lineList)
        {
            line->setPoints( QPointF(line->p1().x(), height+line->p1().y()),
                             QPointF(line->p2().x(), height+line->p2().y()));
        }

        foreach(QRectF* circle, circleList)
        {
            circle->setRect( circle->x(), height+circle->y(), circle->width(), circle->height() );
        }



        svgItem->setLineList(lineList);
        svgItem->setCircleList(circleList);
        svgItem->setRect(0,0,width, height);
        qDebug() << "HW " << width << height << svgItem->rect();

        ui->graphicsView->fitInView(svgItem, Qt::KeepAspectRatio);
    }
}

void MainWindow::generateGCode()
{
    QStringList gcode;

    gcode << "G21 ; set units to millimeters";
    gcode << "G28 ;home all axes";
    gcode << "G0 Z5";
    gcode << "G90 ; use absolute coordinates";
    gcode << "G1 F1800.000";
    gcode << "G1 Z1 F7800.000 ;lift up 1 mm";

    //Do calculation here
    float dx = svgItem->rect().width()/2.0;
    float dy = svgItem->rect().height()/2.0;

    qDebug() << "Lines: " << lineList.size();

    foreach(QLineF *line, lineList)
    {
        gcode << QString("G1 X%1 Y%2 F540.000").arg(line->x1(),0, 'f', 3).arg(line->y1(),0, 'f', 3);
        gcode << "G1 Z0.350 F7800.000";
        gcode << QString("G1 X%1 Y%2 F540.000").arg(line->x2(),0, 'f', 3).arg(line->y2(),0, 'f', 3);
        gcode << "G1 Z1 F7800.000 ;lift up 1 mm";
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
