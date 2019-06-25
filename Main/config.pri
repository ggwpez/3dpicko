CONFIG += warn_on qt
CONFIG -= c++11
CONFIG += c++14
QT = network core websockets gui concurrent

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES -= QT_NO_INFO_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

ROOTPATH = $$PWD/../

DOCROOTPATH=$$ROOTPATH/GUI/docroot/
ETCPATH=$$DOCROOTPATH/etc/

LIBS =
QMAKE_CXXFLAGS += -Werror=return-type -Wshadow -Wreturn-local-addr


# List with all possible defines
# C3PICKO_API_KEY_CHECK_OFF, C3PICKO_NO_ZLIB, C3PICKO_NO_QUAZIP, C3PICKO_TEXT_COLORS_OFF
