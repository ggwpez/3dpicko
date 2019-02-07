TEMPLATE = lib
CONFIG += c++11 staticlib


SOURCES += \
   # source/main.cpp \
	source/gcodeinstruction.cpp \
	source/gcodegenerator.cpp \
	source/point.cpp \
	source/platesocketprofile.cpp \
	source/printerprofile.cpp \
	source/plateprofile.cpp

HEADERS += \
	include/gcodeinstruction.h \
	include/gcodegenerator.h \
	include/point.h \
	include/printerprofile.h \
	include/platesocketprofile.h \
	include/plateprofile.h

INCLUDEPATH += ../Main
LIBS =
