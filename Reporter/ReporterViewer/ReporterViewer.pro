#-------------------------------------------------
#
# Project created by QtCreator 2010-06-16T21:23:06
#
#-------------------------------------------------

QT       += core gui network

TARGET = JView
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    PrintDialog.cpp \
    PrintDefault.cpp \
    SimpleMapi.cpp \
    CommandLine.cpp

HEADERS  += MainWindow.h \
    PrintDialog.h \
    PrintDefault.h \
    SimpleMapi.h \
    CommandLine.h

FORMS    += MainWindow.ui

include(../version.pri)

INCLUDEPATH += ../ReporterLib

CONFIG(debug, debug|release) {
OUTPUT = debug
} else {
OUTPUT = release
}

win32:LIBS += -lmapi32
LIBS += -L../ReporterLib/$$OUTPUT -lReporterLib
PRE_TARGETDEPS += ../ReporterLib/$$OUTPUT/libReporterLib.a

RESOURCES += \
    ReporterViewer.qrc

win32:RC_FILE += ReporterViewer.rc

OTHER_FILES += \
    ReporterViewer.manifest \
    ReporterViewer.iss \
    readme.txt \
    license.txt
