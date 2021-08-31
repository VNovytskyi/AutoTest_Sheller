QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_base.cpp \
    ../../../Libs/sheller/Source/crc.c \
    ../../../Libs/sheller/Source/sheller.c

HEADERS += \
    ../../../Libs/sheller/Source/crc.h \
    ../../../Libs/sheller/Source/sheller.h
