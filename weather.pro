#-------------------------------------------------
#
# Project created by QtCreator 2016-08-26T15:58:11
#
#-------------------------------------------------

QT       += core gui
QT       += network script sql
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = weather
TEMPLATE = app


SOURCES += main.cpp\
        settingform.cpp \
        widget.cpp

HEADERS  += widget.h \
    settingform.h

FORMS    += widget.ui \
    settingform.ui

RESOURCES += \
    img.qrc

DISTFILES +=
