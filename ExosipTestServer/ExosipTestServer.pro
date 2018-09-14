QT += core
QT += network

TARGET = ExosipTestServer
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
    exosiptestserver.cpp \
    sipservice.cpp \
    handlesipevents.cpp

HEADERS += \
    sipservice.h \
    handlesipevents.h
