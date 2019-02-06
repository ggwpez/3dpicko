TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Tests \
	Main \
	Gcode \
	GUI \
	ImageRecognition \
	TestGcode

Tests.depends = PiCommunicator
Main.depends = PiCommunicator GUI Gcode ImageRecognition
