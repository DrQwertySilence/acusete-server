TEMPLATE = app
QT += core serialport websockets multimedia sql
QT -= gui
#CONFIG -= app_bundle
#CONFIG += c++11 console
CONFIG += c++11

SOURCES += \
main.cpp \
application.cpp \
data.cpp \
device.cpp \
timer.cpp \
configuration.cpp \
database.cpp

HEADERS += \
application.h \
data.h \
device.h \
timer.h \
configuration.h \
database.h
