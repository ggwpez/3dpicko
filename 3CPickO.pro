TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Tests \
	Main

Tests.depends = PiCommunicator
Main.depends = PiCommunicator
