include(../Main/config.pri)

TARGET = GUIWebserver

TEMPLATE = lib
CONFIG   += staticlib

SOURCES += \
    src/requestmapper.cpp \
    src/ws_server.cc \
    src/types/job.cc \
    src/table.cc \
    src/database.cc \
    src/types/image.cc \
    src/types/profile.cc \
    src/api_input.cpp \
    src/api_controller.cc \
    src/api_output.cc \
    src/api_commands.cc \
    src/types/report.cc \
    src/types/history.cc \
    src/reporter.cc \
    src/types/well.cc

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
    include/api_controller.h \
    include/api_commands.h \
    include/types/history.h \
    include/types/report.h \
    include/reporter.h \
    include/types/well.h

OTHER_FILES += serverconfig.ini

INCLUDEPATH += $$ROOTPATH/ImageRecognition $$ROOTPATH/Main $$ROOTPATH/PiCommunicator/include $$ROOTPATH/PiCommunicator $$ROOTPATH/Gcode
DEPENDPATH += $$ROOTPATH/ImageRecognition $$ROOTPATH/Main $$ROOTPATH/PiCommunicator/include $$ROOTPATH/PiCommunicator $$ROOTPATH/Gcode

include(../QtWebApp/httpserver/httpserver.pri)
