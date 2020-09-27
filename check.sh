#!/bin/sh

# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

code=0

# C++
if [ ! -z "$(find Main/src Main/include -regex '.*\.\(cc\|h\|hpp\|cpp\)' -exec bash -c 'clang-format -style=file {} | diff {} -' \;)" ]; then
    echo '--- C++  Format error'; code=1
else
    echo '+++ C++  Format good'
fi
# HTML
if [ ! -z "$(find . -name '*.html' -type f -exec xmllint --noout --html --format {} 2>&1 \;)" ]; then
    echo '--- HTML Format error'; code=1
else
    echo '+++ HTML Format good'
fi
# JS
if [ ! -z "$(find . -name '*.js' -not -path "*vendor*" -type f -exec bash -c 'js-beautify {} | diff {} -' \;)" ]; then
    echo '--- JS   Format error'; code=1
else
    echo '+++ JS   Format good'
fi

exit $code