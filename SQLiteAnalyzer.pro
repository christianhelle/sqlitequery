#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T13:32:52
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "SQLite Query Analyzer"
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbanalyzer.cpp \
    highlighter.cpp \
    dbquery.cpp \
    database.cpp

HEADERS  += mainwindow.h \
    dbanalyzer.h \
    databaseinfo.h \
    highlighter.h \
    dbquery.h \
    database.h

FORMS    += mainwindow.ui
