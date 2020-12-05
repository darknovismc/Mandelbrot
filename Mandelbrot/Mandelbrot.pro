#-------------------------------------------------
#
# Project created by QtCreator 2020-09-18T11:32:07
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Mandelbrot
TEMPLATE = app

LIBS += -lOpengl32
LIBS += -lglu32

SOURCES += main.cpp\
        mainwindow.cpp \
    canvasarea.cpp \
    glwidget.cpp

HEADERS  += mainwindow.h \
    canvasarea.h \
    glwidget.h \
    spandata.h

FORMS    += \
    mainwindow.ui

DISTFILES +=
