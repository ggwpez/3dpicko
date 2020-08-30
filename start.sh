#!/bin/sh -

# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

# Open the 3DpickO installation path
cd "$(realpath $0)"
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.:libs/

# Someone pls tell me how to 'do while' works
./main serverconfig.ini $*
while [ $? -eq 101 ]; do
	# 101 is the hardcoded restart exit code
	./main serverconfig.ini $*
done

exit 0
