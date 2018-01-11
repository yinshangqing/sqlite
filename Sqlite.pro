#-------------------------------------------------
#
# Project created by QtCreator 2018-01-04T09:15:59
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sqlite
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    database_sqlite.cpp \
    sqlite.cpp

HEADERS  += widget.h \
    database_sqlite.h \
    sqlite.h

FORMS    += widget.ui
