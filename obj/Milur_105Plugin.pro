#-------------------------------------------------
#
# Project created by QtCreator 2015-12-16T07:58:52
#
#-------------------------------------------------

QT -= gui
QT += core serialport

TARGET = Milur_105Plugin
TEMPLATE = lib

DEFINES += MILUR_105PLUGIN_LIBRARY

SOURCES += \
    ../src/milur_105plugin.cpp \
    ../src/connectserial.cpp \
    ../src/crc.cpp

HEADERS += \
    ../src/milur_105plugin.h \
    ../src/connectserial.h \
    ../src/crc.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG += c++11
