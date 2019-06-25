message(Using Qt version $$QT_VERSION)

TEMPLATE = subdirs

SUBDIRS += \
    Main \
    depend/QtWebApp \
    depend/quazip/quazip

Main.depends = depend/quazip/quazip depend/QtWebApp
