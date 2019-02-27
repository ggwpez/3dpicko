TEMPLATE = subdirs

SUBDIRS += \
	Main \
	PiCommunicator \
	Gcode \
	GUI \
	ImageRecognition \
	TestGcode

Main.depends = PiCommunicator GUI Gcode ImageRecognition
TestGcode.depends = Gcode
