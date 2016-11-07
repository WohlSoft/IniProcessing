TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

QT  += core

release:{
QMAKE_CXXFLAGS += -Ofast
}
#QMAKE_CFLAGS += -march=native
#QMAKE_CXXFLAGS += -march=native
#QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++

DESTDIR = $$PWD/bin

#DEFINES += INIDEBUG

SOURCES += \
    iniproc/ini_processing.cpp \
    ini_demo.cpp \
    fileMapper/file_mapper.cpp

HEADERS += \
    iniproc/ini_processing.h \
    fileMapper/file_mapper.h
