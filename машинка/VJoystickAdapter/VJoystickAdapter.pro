#-------------------------------------------------
#
# Project created by QtCreator 2011-03-19T13:15:06
#
#-------------------------------------------------

QT       += core gui

TARGET = VJoystickAdapter
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    v_joystick_adapter.cpp

LIBS        += -lSDL
LIBS        += -lSDLmain

HEADERS  += mainwindow.h \
    v_joystick_adapter.h

FORMS    += mainwindow.ui
