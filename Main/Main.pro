TEMPLATE = app
CONFIG += console c++11
QT += network core

SOURCES += main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/release/ -lPiCommunicator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/debug/ -lPiCommunicator
else:unix: LIBS += -L$$OUT_PWD/../PiCommunicator/ -lPiCommunicator

INCLUDEPATH += $$PWD/../PiCommunicator $$PWD/../PiCommunicator/include
DEPENDPATH += $$PWD/../PiCommunicator
