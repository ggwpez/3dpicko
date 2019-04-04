#!/bin/sh

export LD_LIBRARY_PATH=.

while : ; do
	./main serverconfig.ini $*
	# 101 is the hardcoded restart exit code
	[[ $? -eq 101 ]] || break
done

exit 0