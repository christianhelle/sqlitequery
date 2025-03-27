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
VERSION = 1.0.0.0
RC_ICONS = resources/icon.ico
ICON = resources/icon.icns

SOURCES += main.cpp\
        mainwindow.cpp \
    dbanalyzer.cpp \
    highlighter.cpp \
    dbquery.cpp \
    database.cpp \
    dbtree.cpp \
    recentfiles.cpp \
    settings.cpp \
    dbexport.cpp \
    threading/cancellation.cpp

HEADERS  += mainwindow.h \
    dbanalyzer.h \
    databaseinfo.h \
    highlighter.h \
    dbquery.h \
    database.h \
    dbtree.h \
    recentfiles.h \
    settings.h \
    dbexport.h \
    threading/cancellation.h \
    mainthread.h

FORMS    += mainwindow.ui
