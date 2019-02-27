include(../Main/config.pri)

TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
	src/pi_communicator.cc \
	src/octoprint.cc \
	src/command.cc \
	src/commands/arbitrary_command.cc \
	src/responses/retrive_response.cc \
	src/commands/get_all_files.cc \
	src/commands/upload_file.cc \
	src/responses/upload_response.cc \
	src/datamodel/abridged_file_info.cc \
	src/datamodel/reference.cc \
	src/responses/file_or_folder_info.cc \
	src/datamodel/file_type.cc \
	src/commands/delete_file.cc \
	src/datamodel/location.cc \
	src/commands/connection.cc \
	src/datamodel/connection_info.cc \
	src/print_job.cc \
	src/datamodel/progress_info.cc \
	src/datamodel/job_general_info.cc \
	src/responses/job_info.cc \
	src/response.cc \
	src/commands/file_operation.cc \
	src/commands/job_operation.cc \
    src/apikey.cc \
    src/octoconfig.cc

HEADERS += \
	include/pi_communicator.h \
	include/octoprint.h \
	include/apikey.h \
	include/command.h \
	include/commands/arbitrary_command.h \
	include/commands/get_all_files.h \
	include/responses/retrive_response.h \
	include/response.h \
	include/commands/upload_file.h \
	include/responses/upload_response.h \
	include/datamodel/abridged_file_info.h \
	include/datamodel/reference.h \
	include/datamodel/gcode_analysis.h \
	include/datamodel/file_type.h \
	include/responses/file_or_folder_info.h \
	include/command.inc.h \
	include/commands/delete_file.h \
	include/datamodel/location.h \
	include/commands/connection.h \
	include/responses/connection_info.h \
	include/commands/all.h \
	include/print_job.h \
	include/gcode.h \
	include/responses/job_info.h \
	include/datamodel/progress_info.h \
	include/datamodel/job_general_info.h \
	include/commands/file_operation.h \
	include/commands/job_operation.h \
    include/octoconfig.h
