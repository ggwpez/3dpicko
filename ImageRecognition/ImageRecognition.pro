include(../Main/config.pri)

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
    src/algorithms/normal1.cc \
    src/detection_result.cc \
    src/plate_result.cc \
    src/plate.cc \
    src/algorithms/plate1.cc \
    src/matrix_result.cc

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
    include/plate.h \
    include/algorithms/plate1.h \
    include/matrix_result.h

INCLUDEPATH += $$ROOTPATH/Main
DEPENDPATH += $$ROOTPATH/Main
