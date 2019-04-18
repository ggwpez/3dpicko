include(../Main/config.pri)

TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GUI/release/ -lGUIWebserver
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GUI/debug/ -lGUIWebserver
else:unix: LIBS += -L$$OUT_PWD/../GUI/ -lGUIWebserver

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/release/libGUIWebserver.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/debug/libGUIWebserver.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/release/GUIWebserver.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GUI/debug/GUIWebserver.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GUI/libGUIWebserver.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Gcode/release/ -lGcode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Gcode/debug/ -lGcode
else:unix: LIBS += -L$$OUT_PWD/../Gcode/ -lGcode

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/release/libGcode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/debug/libGcode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/release/Gcode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/debug/Gcode.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Gcode/libGcode.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/release/ -lPiCommunicator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/debug/ -lPiCommunicator
else:unix: LIBS += -L$$OUT_PWD/../PiCommunicator/ -lPiCommunicator

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/release/libPiCommunicator.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/debug/libPiCommunicator.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/release/PiCommunicator.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/debug/PiCommunicator.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/libPiCommunicator.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ImageRecognition/release/ -lImageRecognition
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ImageRecognition/debug/ -lImageRecognition
else:unix: LIBS += -L$$OUT_PWD/../ImageRecognition/ -lImageRecognition

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/release/libImageRecognition.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/debug/libImageRecognition.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/release/ImageRecognition.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/debug/ImageRecognition.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ImageRecognition/libImageRecognition.a

SOURCES += \
	global.cc \
	marshalling.cc \
	exception.cc \
	setting.cc \
	resource_path.cc \
	updater.cc \
	version.cc \
	version_manager.cc \
	process.cc \
    main.cc

HEADERS += \
	include/json_convertable.h \
	include/global.h \
	include/exception.h \
	include/setting.h \
	include/resource_path.h \
	include/updater.h \
	include/version.h \
	include/version_manager.h \
	include/process.h \
    include/json_constructable.h \
    include/marshalling.h

unix {
	HEADERS += include/signal_daemon.h
	SOURCES += signal_daemon.cc
}

INCLUDEPATH += $$ROOTPATH/Main $$ROOTPATH/GUI $$ROOTPATH/ImageRecognition $$ROOTPATH/Gcode $$ROOTPATH/PiCommunicator $$ROOTPATH/PiCommunicator/include $$ROOTPATH/QtWebApp/httpserver/
DEPENDPATH += $$ROOTPATH/Main $$ROOTPATH/GUI $$ROOTPATH/ImageRecognition $$ROOTPATH/Gcode $$ROOTPATH/PiCommunicator $$ROOTPATH/PiCommunicator/include $$ROOTPATH/QtWebApp/httpserver/

INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs

# Updater defines
DEFINES += GIT_HASH="'\"$$system('git log --pretty=format:\'%H\' --max-count=1')\"'"
DEFINES += GIT_DATE="'\"$$system('git log --pretty=format:\'%H\' --pretty=format:\'%ad\' --date=rfc2822 --max-count=1')\"'"
#DEFINES += SOURCE_DIR="'\"$$ROOTPATH\"'"
#DEFINES += BUILD_DIR="'\"$$shadowed($$ROOTPATH)\"'"
message($$DEFINES)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../quazip/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../quazip/quazip/debug/ -lquazip
else:unix: LIBS += -L$$OUT_PWD/../quazip/quazip/ -lquazip
