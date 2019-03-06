include(../Main/config.pri)

#TEMPLATE = app
TEMPLATE = lib
CONFIG   += staticlib

SOURCES += \
	src/colony.cc \
	src/algo_setting.cc \
	src/algorithm.cc \
	src/algorithm_manager.cpp \
	src/algorithm_job.cc \
    src/algorithms/helper.cc \
    src/algorithm_result.cc \
    src/algorithms/fluro1.cc \
    src/algorithms/normal1.cc

	#src/main.cpp

HEADERS += \
	include/colony.hpp \
	include/algo_setting.h \
	include/algorithm.h \
	include/algorithm_manager.h \
	include/algorithm_job.h \
	include/algorithms/helper.h \
    include/algorithm_result.h \
    include/algorithms/fluro1.h \
    include/algorithms/normal1.h

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs
