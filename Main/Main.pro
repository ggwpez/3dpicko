!versionAtLeast(QT_VERSION, 5.3):error("Use at least Qt version 5.3")

TEMPLATE = app
CONFIG += console c++11
QT = network core websockets gui

SOURCES += main.cpp \
	global.cc

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


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Gcode/release/ -lGcode
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Gcode/debug/ -lGcode
else:unix: LIBS += -L$$OUT_PWD/../Gcode/ -lGcode

INCLUDEPATH += $$PWD/../Gcode
DEPENDPATH += $$PWD/../Gcode

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/release/libGcode.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/debug/libGcode.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/release/Gcode.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Gcode/debug/Gcode.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Gcode/libGcode.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/release/ -lPiCommunicator
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../PiCommunicator/debug/ -lPiCommunicator
else:unix: LIBS += -L$$OUT_PWD/../PiCommunicator/ -lPiCommunicator

INCLUDEPATH += $$PWD/../PiCommunicator $$PWD/../PiCommunicator/include
DEPENDPATH += $$PWD/../PiCommunicator

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/release/libPiCommunicator.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/debug/libPiCommunicator.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/release/PiCommunicator.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/debug/PiCommunicator.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../PiCommunicator/libPiCommunicator.a

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


INCLUDEPATH += ../QtWebApp/httpserver/
DEPENDPATH += ../QtWebApp/httpserver/

HEADERS += \
	include/json_constructable.hpp \
	include/json_convertable.h \
	include/global.h

LIBS += -lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs
