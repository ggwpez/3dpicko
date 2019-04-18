TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
   # source/main.cpp \
    source/gcodegenerator.cc \
    source/gcodeinstruction.cc \
    source/plateprofile.cc \
    source/platesocketprofile.cc \
    source/point.cc \
    source/printerprofile.cc

HEADERS += \
	include/gcodeinstruction.h \
	include/gcodegenerator.h \
	include/point.h \
	include/printerprofile.h \
	include/platesocketprofile.h \
	include/plateprofile.h

LIBS =
