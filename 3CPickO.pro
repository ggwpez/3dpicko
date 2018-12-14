TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Tests \
	Main \
	Gcode \
	GUI \
    TestGcode

Tests.depends = PiCommunicator
Main.depends = PiCommunicator
