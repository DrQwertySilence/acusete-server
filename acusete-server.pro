TEMPLATE = app
QT += core serialport websockets multimedia
QT -= gui
CONFIG -= app_bundle
CONFIG += c++11 console

SOURCES += \
main.cpp \
application.cpp \
data.cpp \
device.cpp \
timer.cpp \
configuration.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
application.h \
data.h \
device.h \
timer.h \
configuration.h

