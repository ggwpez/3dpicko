message(Using Qt version $$QT_VERSION)

TEMPLATE = subdirs

SUBDIRS += \
	Main \
	PiCommunicator \
	Gcode \
	GUI \
	ImageRecognition \
	TestGcode \
	AlgorithmTest \
	quazip

Main.depends = PiCommunicator GUI Gcode ImageRecognition quazip
TestGcode.depends = Gcode
AlgorithmTest.depends = ImageRecognition GUI
