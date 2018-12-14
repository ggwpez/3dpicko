TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Tests \
	Main \
	Gcode \
	GUI

Tests.depends = PiCommunicator
Main.depends = PiCommunicator
