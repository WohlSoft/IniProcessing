TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

QT  += core

#QMAKE_CXXFLAGS += -O3

DESTDIR = $$PWD/bin

#DEFINES += INIDEBUG

SOURCES += \
    iniproc/ini_processing.cpp \
    ini_demo.cpp \
    fileMapper/file_mapper.cpp

HEADERS += \
    iniproc/ini_processing.h \
    fileMapper/file_mapper.h
