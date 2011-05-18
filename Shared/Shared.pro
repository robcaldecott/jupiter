#-------------------------------------------------
#
# Project created by QtCreator 2010-08-27T21:09:57
#
#-------------------------------------------------

QT       += network

TARGET = Shared
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    File.cpp \
    Printer.cpp \
    MarginsF.cpp \
    Font.cpp \
    SimpleMapi.cpp \
    DownloadFile.cpp \
    StringList.cpp \
    TempFiles.cpp

HEADERS += \
    File.h \
    Printer.h \
    MarginsF.h \
    Font.h \
    SimpleMapi.h \
    DownloadFile.h \
    StringList.h \
    TempFiles.h

win32:LIBS += -lmapi32
