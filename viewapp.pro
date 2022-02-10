TEMPLATE = app
OBJECTS_DIR = tmp/
MOC_DIR = tmp/
UI_DIR = tmp/
RCC_DIR = tmp/

QT += core gui widgets
QT += network

TARGET = viewapp

CONFIG += debug

SOURCES += main.cpp \
    visu.cpp \
    win.cpp

HEADERS += \
    visu.h \
    win.h
