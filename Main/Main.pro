# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

include(config.pri)

TEMPLATE = app

SOURCES += \
	src/ImageRecognition/algorithm_job.cc \
	src/ImageRecognition/algorithms/fluro1.cc \
	src/ImageRecognition/algorithms/helper.cc \
	src/ImageRecognition/algorithm_manager.cc \
	src/ImageRecognition/plates/round_plate.cc \
	src/ImageRecognition/plates/rect_plate.cc \
	src/ImageRecognition/plates/plate.cc \
	src/ImageRecognition/algo_setting.cc \
	src/ImageRecognition/matrix_result.cc \
	src/ImageRecognition/algorithm_result.cc \
	src/ImageRecognition/detection_result.cc \
	src/ImageRecognition/colony.cc \
	src/ImageRecognition/algorithm.cc \
	src/ImageRecognition/plate_result.cc \
	src/PiCommunicator/apikey.cc \
	src/PiCommunicator/octoconfig.cc \
	src/PiCommunicator/command.cc \
	src/PiCommunicator/print_job.cc \
	src/PiCommunicator/datamodel/reference.cc \
	src/PiCommunicator/datamodel/connection_info.cc \
	src/PiCommunicator/datamodel/abridged_file_info.cc \
	src/PiCommunicator/datamodel/job_general_info.cc \
	src/PiCommunicator/datamodel/progress_info.cc \
	src/PiCommunicator/datamodel/location.cc \
	src/PiCommunicator/datamodel/file_type.cc \
	src/PiCommunicator/responses/retrive_response.cc \
	src/PiCommunicator/responses/file_or_folder_info.cc \
	src/PiCommunicator/responses/upload_response.cc \
	src/PiCommunicator/responses/job_info.cc \
	src/PiCommunicator/pi_communicator.cc \
	src/PiCommunicator/response.cc \
	src/PiCommunicator/commands/connection.cc \
	src/PiCommunicator/commands/file_operation.cc \
	src/PiCommunicator/commands/delete_file.cc \
	src/PiCommunicator/commands/upload_file.cc \
	src/PiCommunicator/commands/job_operation.cc \
	src/PiCommunicator/commands/get_all_files.cc \
	src/PiCommunicator/commands/arbitrary_command.cc \
	src/PiCommunicator/octoprint.cc \
	src/Gcode/gcodegenerator.cc \
	src/Gcode/platesocketprofile.cc \
	src/Gcode/plateprofile.cc \
	src/Gcode/printerprofile.cc \
	src/Gcode/gcodeinstruction.cc \
	src/Gcode/point.cc \
	src/GUI/types/well.cc \
	src/GUI/types/report.cc \
	src/GUI/types/history.cc \
	src/GUI/types/profile_type.cc \
	src/GUI/types/image.cc \
	src/GUI/types/job.cc \
	src/GUI/types/profile.cc \
	src/GUI/autosaver.cc \
	src/GUI/api_input.cc \
	src/GUI/ws_server.cc \
	src/GUI/table.cc \
	src/GUI/api_controller.cc \
	src/GUI/requestmapper.cc \
	src/GUI/api_commands.cc \
	src/GUI/api_output.cc \
	src/GUI/reporter.cc \
	src/GUI/database.cc \
	src/Main/resource_path.cc \
	src/Main/main.cc \
	src/Main/exception.cc \
	src/Main/updater.cc \
	src/Main/version.cc \
	src/Main/global.cc \
	src/Main/setting.cc \
	src/Main/process.cc \
	src/Main/version_manager.cc \
	src/Main/marshalling.cc \
	src/ImageRecognition/algorithms/plate_rect.cc \
	src/ImageRecognition/algorithms/plate_round.cc \
	src/ImageRecognition/algorithms/colonies1.cc \
	src/Main/dtor_callback.cc \
	src/Main/json_convertable.cc \
	src/Gcode/platetype.cc \
	src/Main/logger.cc

HEADERS += \
	include/ImageRecognition/algorithms/fluro1.h \
	include/ImageRecognition/algorithms/helper.h \
	include/ImageRecognition/plates/plate.h \
	include/ImageRecognition/plates/round_plate.h \
	include/ImageRecognition/plates/rect_plate.h \
	include/ImageRecognition/plate_result.h \
	include/ImageRecognition/detection_result.h \
	include/ImageRecognition/algorithm.h \
	include/ImageRecognition/algorithm_manager.h \
	include/ImageRecognition/colony.h \
	include/ImageRecognition/algorithm_job.h \
	include/ImageRecognition/algo_setting.h \
	include/ImageRecognition/matrix_result.h \
	include/ImageRecognition/algorithm_result.h \
	include/PiCommunicator/pi_communicator.h \
	include/PiCommunicator/command.h \
	include/PiCommunicator/command.inc.h \
	include/PiCommunicator/print_job.h \
	include/PiCommunicator/response.h \
	include/PiCommunicator/datamodel/abridged_file_info.h \
	include/PiCommunicator/datamodel/gcode_analysis.h \
	include/PiCommunicator/datamodel/job_general_info.h \
	include/PiCommunicator/datamodel/location.h \
	include/PiCommunicator/datamodel/progress_info.h \
	include/PiCommunicator/datamodel/file_type.h \
	include/PiCommunicator/datamodel/reference.h \
	include/PiCommunicator/gcode.h \
	include/PiCommunicator/responses/upload_response.h \
	include/PiCommunicator/responses/job_info.h \
	include/PiCommunicator/responses/file_or_folder_info.h \
	include/PiCommunicator/responses/connection_info.h \
	include/PiCommunicator/responses/retrive_response.h \
	include/PiCommunicator/octoprint.h \
	include/PiCommunicator/apikey.h \
	include/PiCommunicator/commands/get_all_files.h \
	include/PiCommunicator/commands/file_operation.h \
	include/PiCommunicator/commands/all.h \
	include/PiCommunicator/commands/connection.h \
	include/PiCommunicator/commands/arbitrary_command.h \
	include/PiCommunicator/commands/delete_file.h \
	include/PiCommunicator/commands/job_operation.h \
	include/PiCommunicator/commands/upload_file.h \
	include/PiCommunicator/octoconfig.h \
	include/Gcode/gcodegenerator.h \
	include/Gcode/plateprofile.h \
	include/Gcode/printerprofile.h \
	include/Gcode/gcodeinstruction.h \
	include/Gcode/platesocketprofile.h \
	include/Gcode/point.h \
	include/GUI/requestmapper.h \
	include/GUI/types/job.h \
	include/GUI/types/image.h \
	include/GUI/types/report.h \
	include/GUI/types/profile.h \
	include/GUI/types/profile_type.h \
	include/GUI/types/history.h \
	include/GUI/types/well.h \
	include/GUI/api_controller.h \
	include/GUI/autosaver.h \
	include/GUI/reporter.h \
	include/GUI/api_output.h \
	include/GUI/api_commands.h \
	include/GUI/table.h \
	include/GUI/ws_server.h \
	include/GUI/api_input.h \
	include/GUI/database.h \
	include/GUI/history.h \
	include/Main/json_constructable.h \
	include/Main/marshalling.h \
	include/Main/process.h \
	include/Main/global.h \
	include/Main/version.h \
	include/Main/version_manager.h \
	include/Main/setting.h \
	include/Main/updater.h \
	include/Main/exception.h \
	include/Main/resource_path.h \
	include/Main/json_convertable.h \
	include/ImageRecognition/algorithms/plate_rect.h \
	include/ImageRecognition/algorithms/plate_round.h \
	include/ImageRecognition/algorithms/colonies1.h \
	include/Main/dtor_callback.h \
	include/Gcode/platetype.h \
	include/Main/logger.h

unix {
	HEADERS += include/Main/signal_daemon.h
	SOURCES += src/Main/signal_daemon.cc
}

DISTFILES += ../server/serverconfig.ini

INCLUDEPATH += /usr/local/include/include/opencv
INCLUDEPATH += include/ depend/
DEPENDPATH += include/ depend/

LIBS += -L/usr/local/lib/ \
		-lopencv_core \
		-lopencv_imgproc \
		-lopencv_highgui \
		-lopencv_imgcodecs \
		-lopencv_calib3d

# Updater defines
DEFINES += GIT_HASH="'\"$$system('command -v git && git log --pretty=format:\'%H\' --max-count=1')\"'"
DEFINES += GIT_DATE="'\"$$system('command -v git && git log --pretty=format:\'%H\' --pretty=format:\'%ad\' --date=rfc2822 --max-count=1')\"'"
DEFINES += BUILD_DATE="'\"$$system('date --iso-8601=seconds')\"'"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

OTHER_FILES += serverconfig.ini
include(../depend/QtWebApp/httpserver/httpserver.pri)

INCLUDEPATH += $$PWD/../depend/QtWebApp/httpserver
DEPENDPATH += $$PWD/../depend/QtWebApp/httpserver

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../depend/quazip/quazip/release/ -lquazip
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../depend/quazip/quazip/debug/ -lquazip
else:unix: LIBS += -L$$OUT_PWD/../depend/quazip/quazip/ -lquazip

INCLUDEPATH += $$PWD/../depend/quazip/quazip
DEPENDPATH += $$PWD/../depend/quazip/quazip
