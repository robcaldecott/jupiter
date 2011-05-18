#-------------------------------------------------
#
# Project created by QtCreator 2010-06-16T21:51:54
#
#-------------------------------------------------

QT       += testlib

QT       += gui

TARGET = ReporterTests
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE = app
CONFIG += qxt
QXT = network

SOURCES += \
    main.cpp \
    ReportTests.cpp \
    PrinterTests.cpp \
    TextTests.cpp \
    StringListTests.cpp

HEADERS += \
    ../../Shared/AutoTest.h \
    ReportTests.h \
    PrinterTests.h \
    TextTests.h \
    StringListTests.h

INCLUDEPATH += ../../Shared
INCLUDEPATH += ../ReporterLib

CONFIG(debug, debug|release) {
LIBS += -L../ReporterLib/debug
PRE_TARGETDEPS += ../ReporterLib/debug/libReporterLib.a
} else {
LIBS += -L../ReporterLib/release
PRE_TARGETDEPS += ../ReporterLib/release/libReporterLib.a
}
LIBS += -lReporterLib

#QMAKE_POST_LINK += ./debug/ReporterTests.exe
