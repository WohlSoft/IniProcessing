TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += qt

QT  += core

DEFINES += INI_PROCESSING_ALLOW_QT_TYPES
QMAKE_CXXFLAGS_RELEASE += -Ofast
QMAKE_CFLAGS_RELEASE += -march=native
QMAKE_CXXFLAGS_RELEASE += -march=native
QMAKE_CXXFLAGS_RELEASE += -static-libgcc -static-libstdc++

DESTDIR = $$PWD/bin

#DEFINES += INIDEBUG
#DEFINE += CASE_SENSITIVE_KEYS

include(iniprocessing.pri)

SOURCES += \
    my-vs-qsettings-benchmark.cpp

