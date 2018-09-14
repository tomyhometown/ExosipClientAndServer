QT += core
QT += network

TARGET = ExosipTestClient
CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app

LIBS +=  -L$$PWD/../lib    \
             -losipparser2  \
             -losip2    \
             -leXosip2

INCLUDEPATH += $$PWD/../include/exosip

SOURCES += \
    exosiptestclient.cpp \
    sipservice.cpp \
    gbsip.cpp \
    handlesipevents.cpp

HEADERS += \
    sipservice.h \
    gbsip.h \
    handlesipevents.h
