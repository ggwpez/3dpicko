#!/bin/sh

# SPDX-FileCopyrightText: 2018-2020 Oliver Tale-Yazdi <oliver@tasty.limo>
#
# SPDX-License-Identifier: GPL-3.0-only

set -e

if [ -z "$1" ]; then
    echo "Installation directory needed as first argument"
    exit 1
fi
if [ -d "$1" ] && [ ! -z "$(ls -A $1)" ]; then
   echo "$1 directory should be empty"
   exit 1
fi

INSTALL="$(realpath $1)"
mkdir -p build/
mkdir -p "$INSTALL"

cp -n server/database-default.json $INSTALL/database.json
cp -n server/serverconfig.ini $INSTALL
cp -n start.sh $INSTALL
chmod +x $INSTALL/start.sh
DOCINSTALL=server/docroot
mkdir -p $INSTALL/$DOCINSTALL
cp -Rn $DOCINSTALL $INSTALL/server
sed -i "s#%docroot#${DOCINSTALL}#g" $INSTALL/serverconfig.ini
sed -i "s#%branch#${2}#g" $INSTALL/serverconfig.ini
sed -i "s#%db#./#g" $INSTALL/serverconfig.ini
# configure it
echo "working_dir=\"$INSTALL\"" >> $INSTALL/serverconfig.ini
cd build/

qmake ../
make -j$(nproc) > /dev/null

cp -n Main/Main $INSTALL/main
cp -n depend/quazip/quazip/libquazip.so.1 $INSTALL/libquazip.so.1

if [ -z "$CPICKO_USER" ]; then
   CPICKO_USER=ubuntu
fi
if id -u "$CPICKO_USER" > /dev/null 2>&1; then
    echo "Using user $CPICKO_USER"
else
    printf "User $CPICKO_USER not found. Try to set it with:\nCPICKO_USER=alice ./setup.sh $INSTALL\n"
    exit 1
fi

if [ "$CPICKO_ENV" = "docker" ]; then
    echo "Skipping service setup in docker"
else
    cd ..
    sudo systemctl stop dpicko > /dev/null 2>&1 || true
    sudo cp dpicko.service /etc/systemd/system/dpicko.service
    sudo sed -i "s#%install#${INSTALL}#g" /etc/systemd/system/dpicko.service
    sudo sed -i "s#%user#${CPICKO_USER}#g" /etc/systemd/system/dpicko.service
    sudo systemctl daemon-reload
    sudo systemctl enable dpicko
    sudo systemctl start dpicko
    echo "Service started"
fi

echo "Setup complete"
exit 0
