#!/bin/sh

export LD_LIBRARY_PATH=.
./main serverconfig.ini $*

exit 0