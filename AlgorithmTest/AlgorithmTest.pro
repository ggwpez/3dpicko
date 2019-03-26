include(../Main/config.pri)

QT += testlib

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_colonycollision.cc \
	tst_platedetection.cc \
	main.cc \
	minimumcutproblemsolver.cpp \
	tst_report.cc

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ImageRecognition/release/ -lImageRecognition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ImageRecognition/debug/ -lImageRecognition
else:unix: LIBS += -L$$OUT_PWD/../ImageRecognition/ -lImageRecognition

INCLUDEPATH += $$PWD/../ImageRecognition
DEPENDPATH += $$PWD/../ImageRecognition

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/release/libImageRecognition.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/debug/libImageRecognition.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/release/ImageRecognition.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/debug/ImageRecognition.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/libImageRecognition.a

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs \
		-lopencv_calib3d

HEADERS += \
	tst_colonycollision.h \
	tst_platedetection.h \
	minimumcutproblemsolver.hpp \
	tst_report.h


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GUI/release/ -lGUIWebserver
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GUI/debug/ -lGUIWebserver
else:unix: LIBS += -L$$OUT_PWD/../GUI/ -lGUIWebserver

INCLUDEPATH += $$PWD/../GUI
DEPENDPATH += $$PWD/../GUI

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/release/libGUIWebserver.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/debug/libGUIWebserver.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/release/GUIWebserver.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/debug/GUIWebserver.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GUI/libGUIWebserver.a
