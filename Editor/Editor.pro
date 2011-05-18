#-------------------------------------------------
#
# Project created by QtCreator 2010-08-26T21:37:03
#
#-------------------------------------------------

QT       += core gui network

TARGET = JEditor
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    GraphicsView.cpp \
    ItemWidget.cpp \
    FormWidget.cpp \
    ColorComboBox.cpp \
    SelectedProperties.cpp \
    SelectPictureDialog.cpp \
    TextDialog.cpp

HEADERS  += MainWindow.h \
    GraphicsView.h \
    ItemWidget.h \
    FormWidget.h \
    ColorComboBox.h \
    SelectedProperties.h \
    SelectPictureDialog.h \
    TextDialog.h

FORMS    += MainWindow.ui \
    ItemWidget.ui \
    FormWidget.ui \
    SelectPictureDialog.ui \
    TextDialog.ui

RESOURCES += \
    Editor.qrc

win32:RC_FILE += Editor.rc

OTHER_FILES += Editor.manifest \
    Editor.iss \
    readme.txt \
    license.txt

DEFINES += WIN32
DEFINES += VER_MAJOR=0
DEFINES += VER_MINOR=0

INCLUDEPATH += ../JupiterLib
INCLUDEPATH += ../Shared

CONFIG(debug, debug|release) {
OUTPUT = debug
} else {
OUTPUT = release
}

LIBS += -L../JupiterLib/$$OUTPUT -lJupiterLib
LIBS += -L../Shared/$$OUTPUT -lShared

PRE_TARGETDEPS += ../JupiterLib/$$OUTPUT/libJupiterLib.a
PRE_TARGETDEPS += ../Shared/$$OUTPUT/libShared.a
