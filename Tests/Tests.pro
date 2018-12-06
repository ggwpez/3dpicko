QT       += testlib core network
CONFIG   += qt console warn_on depend_includepath testcase

QT       -= gui

CONFIG   -= app_bundle

TEMPLATE = app

SOURCES +=  tst_picommands.cc

DEFINES += SRCDIR=\\\"$$PWD/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/release/ -lPiCommunicator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/debug/ -lPiCommunicator
else:unix: LIBS += -L$$OUT_PWD/../PiCommunicator/ -lPiCommunicator

INCLUDEPATH += $$PWD/../PiCommunicator $$PWD/../PiCommunicator/include/
DEPENDPATH += $$PWD/../PiCommunicator
QMAKE_RPATHDIR += $$PWD/../PiCommunicator
