include(../Main/config.pri)

#TEMPLATE = app
TEMPLATE = lib
CONFIG   += staticlib

SOURCES += \
	src/conversion.cpp \
	src/detectionsettings.cpp \
	src/dishdetection.cpp \
	src/imageediting.cpp \
	src/preprocessing.cpp \
	src/test.cpp \
	src/colony.cc \
	src/colony_type.cc \
	src/colonydetector.cpp

	#src/main.cpp

HEADERS += \
	include/conversion.h \
	include/detectionsettings.h \
	include/dishdetection.h \
	include/imageediting.h \
	include/preprocessing.h \
	include/test.h \
	include/main.h \
	include/colony.hpp \
	include/colony_type.h \
	include/colonydetector.h

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs
