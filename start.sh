#!/bin/sh

export LD_LIBRARY_PATH=.

# Someone pls tell me how to 'do while' works
./main serverconfig.ini $*
while [ $? -eq 101 ]; do
	# 101 is the hardcoded restart exit code
	./main serverconfig.ini $*
done

exit 0