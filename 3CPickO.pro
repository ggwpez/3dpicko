TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Main \
	Gcode \
	GUI \
	ImageRecognition

Main.depends = PiCommunicator GUI Gcode ImageRecognition
