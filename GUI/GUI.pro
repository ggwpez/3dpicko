QT  += core
QT  -= gui
QT  += network

TARGET = GUIWebserver

CONFIG   += console

TEMPLATE = app

SOURCES += src/main.cpp \
	src/requestmapper.cpp \
    src/apicontroller.cpp

HEADERS += \
	include/requestmapper.h \
	include/global.h \
    include/apicontroller.h

OTHER_FILES += etc/serverconfig.ini \
	 
include(../QtWebApp/httpserver/httpserver.pri)
