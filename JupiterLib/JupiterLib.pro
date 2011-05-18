#-------------------------------------------------
#
# Project created by QtCreator 2010-08-26T22:48:30
#
#-------------------------------------------------

QT       += network gui

TARGET = JupiterLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    Form.cpp \
    Page.cpp \
    GraphicsPaperItem.cpp \
    GraphicsPageItem.cpp \
    GraphicsGridItem.cpp \
    GraphicsPrintDataItem.cpp \
    GraphicsItem.cpp \
    Watermark.cpp

HEADERS += \
    Form.h \
    Page.h \
    GraphicsPaperItem.h \
    GraphicsItemTypes.h \
    GraphicsPageItem.h \
    GraphicsGridItem.h \
    GraphicsPrintDataItem.h \
    GraphicsItem.h \
    FormPrivate.h \
    Watermark.h

INCLUDEPATH += ../Shared
