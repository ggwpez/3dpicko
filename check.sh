# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

#/bin/sh

clang-format --version
[ -z "$(find Main/src Main/include -regex '.*\.\(cc\|h\|hpp\|cpp\)' -exec clang-format -style=file {} \;)" ] || (echo '--- C++ Format error' && false)
[ -z '$(find . -name "*.html" -type f -exec xmllint --noout --html --format "{}" \;)' ] || (echo '--- HTML Format error' && false)
