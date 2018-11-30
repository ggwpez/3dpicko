TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS += \
	PiCommunicator \
	PiCommunicatorTest

PiCommunicatorTest.depends = PiCommunicator
