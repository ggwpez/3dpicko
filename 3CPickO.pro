TEMPLATE = subdirs

SUBDIRS += \
	Main \
	PiCommunicator \
	Gcode \
	GUI \
	ImageRecognition \
	TestGcode \
    Calibration

Main.depends = PiCommunicator GUI Gcode ImageRecognition
TestGcode.depends = Gcode
