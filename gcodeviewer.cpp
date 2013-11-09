#include "gcodeviewer.h"
#include "ui_gcodeviewer.h"

GCodeViewer::GCodeViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GCodeViewer)
{
    ui->setupUi(this);
}

void GCodeViewer::setGCode(QStringList list)
{
    gcode = list;

    ui->gcodeEdit->clear();

    foreach(QString cmd, gcode)
    {
        ui->gcodeEdit->append(cmd);
    }
}

GCodeViewer::~GCodeViewer()
{
    delete ui;
}
