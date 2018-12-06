TEMPLATE = subdirs

SUBDIRS += \
	PiCommunicator \
	Tests

Tests.depends = PiCommunicator
