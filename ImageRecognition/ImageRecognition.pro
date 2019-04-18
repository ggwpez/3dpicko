include(../Main/config.pri)

TEMPLATE = lib
CONFIG   += staticlib

SOURCES += \
	src/colony.cc \
	src/algo_setting.cc \
	src/algorithm.cc \
	src/algorithm_job.cc \
	src/algorithms/helper.cc \
	src/algorithm_result.cc \
	src/algorithms/fluro1.cc \
	src/algorithms/normal1.cc \
	src/detection_result.cc \
	src/plate_result.cc \
	src/plates/plate.cc \
	src/algorithms/plate1.cc \
	src/matrix_result.cc \
	src/algorithms/plate2.cc \
	src/plates/round_plate.cc \
	src/plates/rect_plate.cc \
    src/algorithm_manager.cc

HEADERS += \
	include/colony.hpp \
	include/algo_setting.h \
	include/algorithm.h \
	include/algorithm_manager.h \
	include/algorithm_job.h \
	include/algorithms/helper.h \
	include/algorithm_result.h \
	include/algorithms/fluro1.h \
	include/algorithms/normal1.h \
	include/detection_result.h \
	include/plate_result.h \
	include/plates/plate.h \
	include/algorithms/plate1.h \
	include/matrix_result.h \
	include/algorithms/plate2.h \
    include/plates/rect_plate.h \
    include/plates/round_plate.h

INCLUDEPATH += $$ROOTPATH/Main
DEPENDPATH += $$ROOTPATH/Main
