#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T13:32:52
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SQLiteAnalyzer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbanalyzer.cpp \
    highlighter.cpp

HEADERS  += mainwindow.h \
    dbanalyzer.h \
    databaseinfo.h \
    highlighter.h

FORMS    += mainwindow.ui
