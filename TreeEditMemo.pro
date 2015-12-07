#-------------------------------------------------
#
# Project created by QtCreator 2014-11-30T16:49:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TreeEditMemo
TEMPLATE = app
CONFIG += debug

SOURCES += main.cpp\
        mainwindow.cpp \
    treeitem.cpp \
    treemodel.cpp \
    combo-delegate.cpp \
    about_dialog.cpp

HEADERS  += mainwindow.h \
    treeitem.h \
    treemodel.h \
    combo-delegate.h \
    about_dialog.h

FORMS    += mainwindow.ui \
    about_dialog.ui

OTHER_FILES +=

exists(../ModelTest/modeltest.pri){
	DEFINES += MODEL_TEST
	include(../ModelTest/modeltest.pri)
}

RESOURCES += \
    TreeEditMemo.qrc

TRANSLATIONS += translations/TreeEditMemo_ru_RU.ts
