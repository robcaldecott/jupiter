#-------------------------------------------------
#
# Project created by QtCreator 2010-06-16T21:41:34
#
#-------------------------------------------------

TARGET = ReporterLib
TEMPLATE = lib
QT += network
CONFIG += qxt
QXT = network
CONFIG += staticlib

SOURCES += \
    Report.cpp \
    Printer.cpp \
    Font.cpp \
    File.cpp \
    Text.cpp \
    MarginsF.cpp \
    DownloadFile.cpp \
    EmailSettings.cpp \
    SendSettings.cpp \
    StringList.cpp \
    Smtp.cpp \
    Picture.cpp \
    DownloadPictures.cpp \
    ReportView.cpp \
    TempFiles.cpp

HEADERS +=\
    Report.h \
    Printer.h \
    Font.h \
    File.h \
    Text.h \
    Measurements.h \
    MarginsF.h \
    DownloadFile.h \
    EmailSettings.h \
    SendSettings.h \
    StringList.h \
    Smtp.h \
    Picture.h \
    DownloadPictures.h \
    ReportView.h \
    TempFiles.h
