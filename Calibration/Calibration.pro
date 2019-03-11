TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
		main.cc

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs \
		-lopencv_videoio \
-lopencv_calib3d
