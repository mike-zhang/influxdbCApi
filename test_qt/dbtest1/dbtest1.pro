#-------------------------------------------------
#
# Project created by QtCreator 2017-05-23T16:11:27
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = dbtest1
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp
INCLUDEPATH += D:/thrdLibs/curl-7.51.0/include
INCLUDEPATH += D:/thrdLibs/influxdbCApi/src

LIBS += -LD:/thrdLibs/influxdbCApi/src -linfluxdb

LIBS += -LD:/thrdLibs/curl-7.51.0/lib -DCURL_STATIC -lcurl -lwldap32 -lws2_32
LIBS += -LD:/thrdLibs/zlib-1.2.8 -lz


