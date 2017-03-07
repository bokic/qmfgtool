QT       += core gui widgets xml

TARGET = qmfgtool
TEMPLATE = app

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

DESTDIR = $$PWD/bin

unix:  INCLUDEPATH += /usr/include/libusb-1.0
win32: INCLUDEPATH += windows

SOURCES += main.cpp maindialog.cpp qmfgtooluploader.cpp qmfgtoolsettings.cpp qmfgtooldevice.cpp
HEADERS +=          maindialog.h   qmfgtooluploader.h   qmfgtoolsettings.h   qmfgtooldevice.h
FORMS   +=          maindialog.ui

SOURCES += qmfgtoolcommand.cpp qmfgtoolcommandfind.cpp qmfgtoolcommandboot.cpp qmfgtoolcommandinit.cpp qmfgtoolcommandload.cpp qmfgtoolcommandjump.cpp qmfgtoolcommandpush.cpp
HEADERS += qmfgtoolcommand.h   qmfgtoolcommandfind.h   qmfgtoolcommandboot.h   qmfgtoolcommandinit.h   qmfgtoolcommandload.h   qmfgtoolcommandjump.h   qmfgtoolcommandpush.h

HEADERS += qmfgutils.h

win32: SOURCES += qmfgtooldevice_win.cpp
unix:  SOURCES += qmfgtooldevice_unix.cpp
macx:  SOURCES += qmfgtooldevice_mac.cpp

win32: SOURCES += windows/hid.c
win32: HEADERS += windows/hidapi.h

unix:  LIBS += -lusb-1.0
win32: LIBS += -lhid -lsetupapi
