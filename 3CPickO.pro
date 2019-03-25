message(Using Qt version $$QT_VERSION)

TEMPLATE = subdirs

SUBDIRS += \
	Main \
	PiCommunicator \
	Gcode \
	GUI \
	ImageRecognition \
	TestGcode \
	Calibration \
	AlgorithmTest

Main.depends = PiCommunicator GUI Gcode ImageRecognition
TestGcode.depends = Gcode
AlgorithmTest.depends = ImageRecognition
