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
    ../gui/mainwindow.cpp \
    ../database/dbanalyzer.cpp \
    ../gui/highlighter.cpp \
    ../database/dbquery.cpp \
    ../database/database.cpp \
    ../database/dbtree.cpp \
    ../settings/recentfiles.cpp \
    ../settings/settings.cpp \
    ../database/dbexport.cpp \
    ../database/dbexportschema.cpp \
    ../database/dbexportdata.cpp \
    ../threading/cancellation.cpp \
    ../gui/prompts.cpp \
    ../cli/export.cpp \
    ../cli/script.cpp

HEADERS += \
    ../gui/mainwindow.h \
    ../database/dbanalyzer.h \
    ../database/databaseinfo.h \
    ../gui/highlighter.h \
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
    ../gui/prompts.h \
    ../cli/export.h \
    ../cli/script.h

FORMS    += ../gui/mainwindow.ui
