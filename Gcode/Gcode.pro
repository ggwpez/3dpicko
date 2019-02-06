TEMPLATE = lib
CONFIG += c++11 staticlib
QT += core

SOURCES += \
	source/main.cpp \
	source/gcodeinstruction.cpp \
	source/gcodegenerator.cpp \
	source/point.cpp \
	source/platesocketprofile.cpp \
	source/printerprofile.cpp \
	source/masterandgoalplateprofile.cpp

HEADERS += \
	include/gcodeinstruction.h \
	include/gcodegenerator.h \
	include/point.h \
	include/printerprofile.h \
	include/platesocketprofile.h \
	include/masterandgoalplateprofile.h

INCLUDEPATH += $$PWD/../Main
LIBS =
