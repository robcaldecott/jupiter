#-------------------------------------------------
#
# Project created by QtCreator 2010-07-12T21:13:08
#
#-------------------------------------------------

QT       += network
QT       -= gui
TARGET = LicensingLib
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    UserLicense.cpp \
    Rc4.cpp \
    NodeLockedLicense.cpp \
    MacAddresses.cpp \
    Hex.cpp

HEADERS +=\
    UserLicense.h \
    Rc4.h \
    NodeLockedLicense.h \
    MacAddresses.h \
    Hex.h
