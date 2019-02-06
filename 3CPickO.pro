TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Main \
	Gcode \
	GUI \
	ImageRecognition \
	TestGcode

Main.depends = PiCommunicator GUI Gcode ImageRecognition
