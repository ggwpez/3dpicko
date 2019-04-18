include(../Main/config.pri)

TARGET = GUIWebserver

TEMPLATE = lib
CONFIG   += staticlib

SOURCES += \
	src/ws_server.cc \
	src/types/job.cc \
	src/table.cc \
	src/database.cc \
	src/types/image.cc \
	src/types/profile.cc \
	src/api_controller.cc \
	src/api_output.cc \
	src/api_commands.cc \
	src/types/report.cc \
	src/types/history.cc \
	src/reporter.cc \
	src/types/well.cc \
	src/types/profile_type.cc \
    src/autosaver.cc \
    src/requestmapper.cc \
    src/api_input.cc

HEADERS += \
	include/requestmapper.h \
	include/table.hpp \
	include/database.hpp \
	include/api_output.h \
	include/api_input.h \
	include/api_controller.h \
	include/api_commands.h \
	include/types/history.h \
	include/types/report.h \
	include/reporter.h \
	include/types/well.h \
	include/types/profile_type.h \
    include/autosaver.h \
    include/types/image.h \
    include/types/job.h \
    include/types/profile.h \
    include/ws_server.h

OTHER_FILES += serverconfig.ini

INCLUDEPATH += $$ROOTPATH/ImageRecognition $$ROOTPATH/Main $$ROOTPATH/PiCommunicator/include $$ROOTPATH/PiCommunicator $$ROOTPATH/Gcode
DEPENDPATH += $$ROOTPATH/ImageRecognition $$ROOTPATH/Main $$ROOTPATH/PiCommunicator/include $$ROOTPATH/PiCommunicator $$ROOTPATH/Gcode

include(../QtWebApp/httpserver/httpserver.pri)
