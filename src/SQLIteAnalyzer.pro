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

HEADERS += cli/export.h \
           cli/script.h \
           database/databaseinfo.h \
           database/dbanalyzer.h \
           database/dbexport.h \
           database/dbexportdata.h \
           database/dbexportschema.h \
           database/dbtree.h \
           database/idatabase.h \
           database/inmemorydatabase.h \
           database/progress.h \
           database/queryexecutor.h \
           database/queryresult.h \
           database/sqlitedatabase.h \
           gui/exportorchestrator.h \
           gui/highlighter.h \
           gui/mainwindow.h \
           gui/prompts.h \
           gui/queryexecutionpresenter.h \
           gui/queryresultpresenter.h \
           gui/sessionmanager.h \
           settings/recentfiles.h \
           settings/settings.h \
           threading/cancellation.h \
           threading/mainthread.h
FORMS += gui/mainwindow.ui
SOURCES += main.cpp \
           cli/export.cpp \
           cli/script.cpp \
           database/dbanalyzer.cpp \
           database/dbexport.cpp \
           database/dbexportdata.cpp \
           database/dbexportschema.cpp \
           database/dbtree.cpp \
           database/inmemorydatabase.cpp \
           database/queryexecutor.cpp \
           database/sqlitedatabase.cpp \
           gui/exportorchestrator.cpp \
           gui/highlighter.cpp \
           gui/mainwindow.cpp \
           gui/prompts.cpp \
           gui/queryexecutionpresenter.cpp \
           gui/queryresultpresenter.cpp \
           gui/sessionmanager.cpp \
           settings/recentfiles.cpp \
           settings/settings.cpp \
           threading/cancellation.cpp
