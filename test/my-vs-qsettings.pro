TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

QT  += core

DEFINES += INI_PROCESSING_ALLOW_QT_TYPES

release:{
    QMAKE_CXXFLAGS += -Ofast
}
#QMAKE_CFLAGS += -march=native
#QMAKE_CXXFLAGS += -march=native
#QMAKE_CXXFLAGS += -static-libgcc -static-libstdc++

DESTDIR = $$PWD/bin

#DEFINES += INIDEBUG

SOURCES += \
    ../src/ini_processing.cpp \
    ini_demo.cpp

HEADERS += \
    ../src/ini_processing.h \
    ../src/ini_processing_variant.h
