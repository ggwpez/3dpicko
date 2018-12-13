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
	src/pi_communicator.cc \
	src/octoprint.cc \
	src/command.cc \
	src/commands/arbitrary_command.cc \
	src/responses/retrive_response.cc \
	src/commands/get_all_files.cc \
	src/commands/upload_file.cc \
	src/responses/upload_response.cc \
	src/responses/abridged_file_info.cc \
	src/datamodel/reference.cc \
	src/responses/file_or_folder_info.cc \
	src/datamodel/file_type.cc \
	src/commands/delete_file.cc \
	src/datamodel/location.cc \
	src/commands/connection.cc \
	src/datamodel/connection_type.cc \
	src/datamodel/connection_info.cc \
    src/print_job.cc

HEADERS += \
	include/pi_communicator.h \
	include/octoprint.h \
	include/apikey.h \
	include/command.h \
	include/commands/arbitrary_command.h \
	include/commands/get_all_files.h \
	include/responses/retrive_response.h \
	include/responses/response.h \
	include/commands/upload_file.h \
	include/responses/upload_response.h \
	include/responses/abridged_file_info.h \
	include/datamodel/reference.h \
	include/datamodel/gcode_analysis.h \
	include/datamodel/file_type.h \
	include/responses/file_or_folder_info.h \
	include/command.inc.h \
	include/commands/delete_file.h \
	include/datamodel/location.h \
	include/commands/connection.h \
	include/datamodel/connection_type.h \
	include/responses/connection_info.h \
    include/commands/all.h \
    include/print_job.h \
    include/gcode.h

INCLUDEPATH += include/

QMAKE_CXXFLAGS += -Werror

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
