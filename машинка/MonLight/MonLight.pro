#-------------------------------------------------
#
# Project created by QtCreator 2012-12-28T02:15:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MonLight
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ardserial.cpp \
    v_joystick_adapter.cpp

HEADERS  += mainwindow.h \
    ardserial.h \
    v_joystick_adapter.h

FORMS    += mainwindow.ui

LIBS += -lsetupapi -luuid -ladvapi32 -lSDL -lSDLmain

include(C:\Qt\lib\src\qserialdeviceenumerator\qserialdeviceenumerator.pri)
include(C:\Qt\lib\src\qserialdevice\qserialdevice.pri)
