# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

TEMPLATE = subdirs

SUBDIRS += \
	Main \
	depend/QtWebApp \
	depend/quazip/quazip

Main.depends = depend/quazip/quazip depend/QtWebApp
