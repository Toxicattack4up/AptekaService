QT += testlib
QT -= gui
CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app
TARGET = PasswordUtilTest

SOURCES += \
    PasswordUtilTest.cpp \
    ../PasswordUtil.cpp

HEADERS += \
    ../PasswordUtil.h

INCLUDEPATH += ..
