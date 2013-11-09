#ifndef GCODEVIEWER_H
#define GCODEVIEWER_H

#include <QDialog>

namespace Ui {
class GCodeViewer;
}

class GCodeViewer : public QDialog
{
    Q_OBJECT

public:
    explicit GCodeViewer(QWidget *parent = 0);
    ~GCodeViewer();

    void setGCode(QStringList list);

private:
    Ui::GCodeViewer *ui;

    QStringList gcode;
};

#endif // GCODEVIEWER_H
