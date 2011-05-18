#-------------------------------------------------
#
# Project created by QtCreator 2010-08-14T15:35:51
#
#-------------------------------------------------

QT       += core network gui
CONFIG += qxt
QXT = network

TARGET = JReporter
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    Print.cpp \
    Pdf.cpp \
    Email.cpp
	
include(../version.pri)

INCLUDEPATH += ../ReporterLib
INCLUDEPATH += ../../Licensing/LicensingLib

CONFIG(debug, debug|release) {
OUTPUT = debug
} else {
OUTPUT = release
}

LIBS += -L../ReporterLib/$$OUTPUT -lReporterLib
LIBS += -L../../Licensing/LicensingLib/$$OUTPUT -lLicensingLib

PRE_TARGETDEPS += ../ReporterLib/$$OUTPUT/libReporterLib.a
PRE_TARGETDEPS += ../../Licensing/LicensingLib/$$OUTPUT/libLicensingLib.a

HEADERS += \
    Print.h \
    Pdf.h \
    Email.h

win32:RC_FILE += ReporterApp.rc

OTHER_FILES += \
    ReporterApp.iss \
    readme.txt \
    license.txt
