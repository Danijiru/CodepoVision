#-------------------------------------------------
#
# Project created by QtCreator 2017-03-08T16:09:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CodepoVision
TEMPLATE = app


INCLUDEPATH += -I/usr/include/opencv
LIBS += -L/usr/lib/x86_64-linux-gnu/ -lopencv_core -lopencv_imgproc -lopencv_highgui

SOURCES += main.cpp\
        codepovision.cpp \
    webcamwindow.cpp

HEADERS  += codepovision.h \
    webcamwindow.h

FORMS    += codepovision.ui \
    webcamwindow.ui

RESOURCES += \
    projecticons.qrc
