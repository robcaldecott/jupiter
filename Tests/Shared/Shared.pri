QT       += testlib
QT       -= gui

CONFIG   += console
CONFIG   += testcase
CONFIG   -= app_bundle

TEMPLATE = app

CONFIG(debug, debug|release): OUTPUT = debug
else: OUTPUT = release

LIBS += -L../../../Shared/$$OUTPUT -lShared
PRE_TARGETDEPS += ../../../Shared/$$OUTPUT/libShared.a
INCLUDEPATH += ../../../Shared

# Run tests when the build has completed
QMAKE_POST_LINK = $$OUTPUT/$$TARGET
