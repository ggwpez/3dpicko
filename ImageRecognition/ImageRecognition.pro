QT  += core network websockets
QT  -= gui

TEMPLATE = lib
CONFIG   += staticlib

SOURCES += \
	src/colonydetection.cpp \
	src/conversion.cpp \
	src/detectionsettings.cpp \
	src/dishdetection.cpp \
	src/imageediting.cpp \
	src/preprocessing.cpp \
    src/test.cpp

HEADERS += \
    include/colonydetection.h \
    include/conversion.h \
    include/detectionsettings.h \
    include/dishdetection.h \
    include/imageediting.h \
    include/preprocessing.h \
    include/test.h

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs

LIBS =
