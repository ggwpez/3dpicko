include(../Main/config.pri)

TARGET = GUIWebserver

TEMPLATE = lib
CONFIG   += staticlib

SOURCES += src/main.cpp \
	src/requestmapper.cpp \
	src/ws_server.cc \
	src/types/job.cc \
	src/table.cc \
	src/database.cc \
	src/types/image.cc \
	src/types/profile.cc \
	src/api_input.cpp \
	src/api_controller.cc \
	src/api_output.cc

HEADERS += \
	include/requestmapper.h \
	include/ws_server.hpp \
	include/types/job.hpp \
	include/table.hpp \
	include/database.hpp \
	include/types/image.hpp \
	include/types/profile.hpp \
	include/api_output.h \
	include/api_input.h \
	include/api_controller.h

DEFINES += QT_NO_INFO_OUTPUT
OTHER_FILES += etc/serverconfig.ini

include(../QtWebApp/httpserver/httpserver.pri)
