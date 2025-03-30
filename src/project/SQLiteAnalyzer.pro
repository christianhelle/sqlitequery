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
RC_ICONS = ../resources/icon.ico
ICON = ../resources/icon.icns

SOURCES += \
    ../main.cpp\
    ../ui/mainwindow.cpp \
    ../database/dbanalyzer.cpp \
    ../ui/highlighter.cpp \
    ../database/dbquery.cpp \
    ../database/database.cpp \
    ../database/dbtree.cpp \
    ../settings/recentfiles.cpp \
    ../settings/settings.cpp \
    ../database/dbexport.cpp \
    ../database/dbexportschema.cpp \
    ../database/dbexportdata.cpp \
    ../threading/cancellation.cpp \
    ../ui/prompts.cpp \

HEADERS += \
    ../ui/mainwindow.h \
    ../database/dbanalyzer.h \
    ../database/databaseinfo.h \
    ../ui/highlighter.h \
    ../database/dbquery.h \
    ../database/database.h \
    ../database/dbtree.h \
    ../settings/recentfiles.h \
    ../settings/settings.h \
    ../database/dbexport.h \
    ../database/dbexportschema.h \
    ../database/dbexportdata.h \
    ../threading/cancellation.h \
    ../threading/mainthread.h \
    ../database/progress.h \
    ../ui/prompts.h

FORMS    += ../ui/mainwindow.ui
