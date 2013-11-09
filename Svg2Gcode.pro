#-------------------------------------------------
#
# Project created by QtCreator 2013-09-15T14:10:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Svg2Gcode
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    svgitem.cpp \
    gcodeviewer.cpp

HEADERS  += mainwindow.h \
    svgitem.h \
    gcodeviewer.h

FORMS    += mainwindow.ui \
    gcodeviewer.ui

RESOURCES += \
    res.qrc
