QT  += core
QT  -= gui
QT  += network

TARGET = GUIWebserver

CONFIG   += console

TEMPLATE = app

SOURCES += src/main.cpp \
	src/helloworldcontroller.cpp \
	src/listdatacontroller.cpp \
	src/requestmapper.cpp \
	src/cookietestcontroller.cpp \
    src/apicontroller.cc

HEADERS += \
	include/helloworldcontroller.h \
	include/listdatacontroller.h \
	include/requestmapper.h \
	include/cookietestcontroller.h \
	include/global.h \
    include/apicontroller.h

OTHER_FILES += etc/webapp1.ini \
	 docroot/files/hello.html

include(../QtWebApp/httpserver/httpserver.pri)
