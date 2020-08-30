#!/bin/sh

# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

code=0

# C++
if [ ! -z "$(find Main/src Main/include -regex '.*\.\(cc\|h\|hpp\|cpp\)' -exec bash -c 'clang-format -style=file {} | diff {} -' \;)" ]; then
    echo '\e[31m--- C++  Format error\e[0m'; code=1
else
    echo '\e[32m+++ C++  Format good\e[0m'
fi
# HTML
if [ ! -z "$(find . -name '*.html' -type f -exec xmllint --noout --html --format {} 2>&1 \;)" ]; then
    echo '\e[31m--- HTML Format error\e[0m'; code=1
else
    echo '\e[32m+++ HTML Format good\e[0m'
fi
# JS
if [ ! -z "$(find . -name '*.js' -not -path "*vendor*" -type f -exec bash -c 'js-beautify {} | diff {} -' \;)" ]; then
    echo '\e[31m--- HTML Format error\e[0m'; code=1
else
    echo '\e[32m+++ HTML Format good\e[0m'
fi

exit $code