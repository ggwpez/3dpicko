include(../Main/config.pri)

QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_colonycollision.cc \
	tst_platedetection.cc \
	main.cc \
	tst_report.cc \
	minimumcutproblemsolver.cc

HEADERS += \
	tst_colonycollision.h \
	tst_platedetection.h \
	tst_report.h \
	minimumcutproblemsolver.h

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs
