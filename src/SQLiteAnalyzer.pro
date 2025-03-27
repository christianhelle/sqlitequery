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
    database/dbanalyzer.cpp \
    highlighter.cpp \
    database/dbquery.cpp \
    database/database.cpp \
    database/dbtree.cpp \
    recentfiles.cpp \
    settings.cpp \
    database/dbexport.cpp \
    threading/cancellation.cpp

HEADERS  += mainwindow.h \
    database/dbanalyzer.h \
    database/databaseinfo.h \
    highlighter.h \
    database/dbquery.h \
    database/database.h \
    database/dbtree.h \
    recentfiles.h \
    settings.h \
    database/dbexport.h \
    threading/cancellation.h \
    mainthread.h

FORMS    += mainwindow.ui
