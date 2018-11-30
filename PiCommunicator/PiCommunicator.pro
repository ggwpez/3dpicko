TEMPLATE = lib
QT -= gui
QT += network

CONFIG += c++11
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
	#main.cpp \
	src/pi_communicator.cc \
	src/octoprint.cc \
	src/command.cc \
	src/commands/arbitrary_command.cc \
	src/commands/responses/retrive_response.cc \
	src/commands/get_all_files.cc \
	src/commands/responses/file_information.cc

HEADERS += \
	include/pi_communicator.h \
	include/octoprint.h \
	include/apikey.h \
	include/command.h \
	include/commands/arbitrary_command.h \
	include/commands/get_all_files.h \
	include/commands/responses/retrive_response.h \
	include/commands/responses/response.h \
	include/commands/responses/file_information.h

INCLUDEPATH += include/

#QMAKE_CXXFLAGS += -Werror -Wall -Wextra -Wno-return

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
