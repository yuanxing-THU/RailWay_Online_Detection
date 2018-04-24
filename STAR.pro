#-------------------------------------------------
#
# Project created by QtCreator 2016-04-08T16:29:00
#
#-------------------------------------------------

QT       += core gui

TARGET = STAR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gpsDisplay.cpp \
    GPS_info.cpp \
    accDisplay.cpp \
    qcustomplot.cpp \
    myWavelet.cpp \
    storeDisplay.cpp \
    FFT.cpp

HEADERS  += mainwindow.h \
    gpsDisplay.h \
    GPS_info.h \
    accDisplay.h \
    qcustomplot.h \
    myWavelet.h \
    storeDisplay.h \
    FFT.h

FORMS    += mainwindow.ui
CONFIG += serialport
CONFIG += qt
