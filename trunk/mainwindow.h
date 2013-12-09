#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QGraphicsScene>
#include "svgitem.h"
#include "gcodeviewer.h"
#include <QTimer>
#include <QDoubleSpinBox>
#include "line.h"
#include "circle.h"
#include "qbezier.h"
#include "polygon.h"
#include "transform.h"
#include <QString>
#include "arc.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //enum Cmds { none, Move, move, Circle, circle };

private slots:
    void openSvg();
    void read(QIODevice *device);
    void readSVG();
    void updateBoundary(float &w, float&h, float x1, float x2, float y1, float y2);
    void generateGCode();
    QVector<double> parseSVGNumbers(QString cmd);

private:
    Ui::MainWindow *ui;
    QXmlStreamReader xml;
    QGraphicsScene * scene;
    SvgItem * svgItem;
    QList<Line*> lineList;
    QList<Circle*> circleList;
    QList<QBezier*> qBezierList;
    QList<Polygon*> polyList;
    QList<Arc*> arcList;
    double elevation;
};


#endif // MAINWINDOW_H
