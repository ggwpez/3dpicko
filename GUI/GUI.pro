QT  += core
QT  -= gui
QT  += network

TARGET = GUIWebserver

CONFIG   += console

TEMPLATE = app

SOURCES += src/main.cpp \
	src/requestmapper.cpp \
    src/apicontroller.cc

HEADERS += \
	include/requestmapper.h \
	include/global.h \
    include/apicontroller.h

OTHER_FILES += etc/webapp1.ini \
	 docroot/files/hello.html

include(../QtWebApp/httpserver/httpserver.pri)
