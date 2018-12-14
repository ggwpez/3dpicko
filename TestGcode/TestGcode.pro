QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \ 
    tst_gcodeinstruction.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Gcode/release/ -lGcode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Gcode/debug/ -lGcode
else:unix:!macx: LIBS += -L$$OUT_PWD/../Gcode/ -lGcode

INCLUDEPATH += $$PWD/../Gcode
DEPENDPATH += $$PWD/../Gcode
