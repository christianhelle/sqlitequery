#-------------------------------------------------
#
# Project created by QtCreator 2015-05-10T13:32:52
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "SQLiteQueryAnalyzer"
TEMPLATE = app

CONFIG   += static

SOURCES += main.cpp\
        mainwindow.cpp \
    dbanalyzer.cpp \
    highlighter.cpp \
    dbquery.cpp \
    database.cpp \
    dbtree.cpp \
    recentfiles.cpp

HEADERS  += mainwindow.h \
    dbanalyzer.h \
    databaseinfo.h \
    highlighter.h \
    dbquery.h \
    database.h \
    dbtree.h \
    recentfiles.h

FORMS    += mainwindow.ui
