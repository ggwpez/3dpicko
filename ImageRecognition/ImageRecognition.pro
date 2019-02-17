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
    src/algo_setting.cc \
    src/algorithm.cc \
    src/algo1_test.cc \
    src/algorithm_manager.cpp \
    src/algorithm_job.cc

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
    include/algo_setting.h \
    include/algorithm.h \
    include/algo1_test.h \
    include/algorithm_manager.h \
    include/algorithm_job.h

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs
