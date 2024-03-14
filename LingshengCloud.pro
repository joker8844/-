#-------------------------------------------------
#
# Project created by QtCreator 2020-10-23T12:00:16
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LingshengCloud
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    logindialog.cpp \
    titlewidget.cpp \
    common.cpp \
    des.c \
    logininfoinstance.cpp \
    buttongroup.cpp \
    myfilewidget.cpp \
    mymenu.cpp \
    networkdata.cpp \
    filepropertyinfodialog.cpp \
    uploadtask.cpp \
    filedataprogress.cpp \
    transformwidget.cpp \
    uploadlayout.cpp \
    downloadlayout.cpp \
    downloadtask.cpp \
    sharewidget.cpp \
    downloadwidget.cpp

HEADERS += \
        mainwindow.h \
    logindialog.h \
    titlewidget.h \
    common.h \
    des.h \
    logininfoinstance.h \
    buttongroup.h \
    myfilewidget.h \
    mymenu.h \
    networkdata.h \
    filepropertyinfodialog.h \
    uploadtask.h \
    filedataprogress.h \
    transformwidget.h \
    uploadlayout.h \
    downloadlayout.h \
    downloadtask.h \
    fileinfo.h \
    sharewidget.h \
    downloadwidget.h

FORMS += \
        mainwindow.ui \
    logindialog.ui \
    titlewidget.ui \
    buttongroup.ui \
    myfilewidget.ui \
    filepropertyinfodialog.ui \
    filedataprogress.ui \
    transformwidget.ui \
    sharewidget.ui \
    downloadwidget.ui

RESOURCES += \
    images.qrc
