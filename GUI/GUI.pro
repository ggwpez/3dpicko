!versionAtLeast(QT_VERSION, 5.3):error("Use at least Qt version 5.3")

QT  += core
QT  -= gui
QT  += network websockets

TARGET = GUIWebserver

CONFIG   += console

TEMPLATE = app

SOURCES += src/main.cpp \
	src/requestmapper.cpp \
	src/apicontroller.cpp \
	src/ws_server.cc

HEADERS += \
	include/requestmapper.h \
	include/global.h \
	include/apicontroller.h \
	include/ws_server.hpp

OTHER_FILES += etc/serverconfig.ini \

include(../QtWebApp/httpserver/httpserver.pri)
