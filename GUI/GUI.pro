QT  += core network websockets
QT  -= gui

TARGET = GUIWebserver

TEMPLATE = lib
CONFIG   += staticlib

SOURCES += src/main.cpp \
	src/requestmapper.cpp \
	src/apicontroller.cpp \
	src/ws_server.cc \
	src/types/job.cc \
	src/table.cc \
	src/database.cc \
	src/types/image.cc

HEADERS += \
	include/requestmapper.h \
	include/apicontroller.h \
	include/ws_server.hpp \
	include/types/job.hpp \
	include/table.hpp \
	include/database.hpp \
	include/types/image.hpp

INCLUDEPATH += $$PWD/../Main

LIBS =
DEFINES += QT_NO_INFO_OUTPUT
OTHER_FILES += etc/serverconfig.ini

INCLUDEPATH += $$PWD/../ImageRecognition

include(../QtWebApp/httpserver/httpserver.pri)
