include(../Main/config.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += \
		main.cc


LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs \
		-lopencv_calib3d
