#!/bin/sh

set -e

if [ -z "$1" ]; then
    echo "Working directory needed as first argument"
    exit 1
fi

if [ ! -z "$(ls -A $1)" ]
then
   echo "Working directory should be empty"
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

if [ "$CPICKO_ENV" = "docker" ]; then
    echo "Skipping service setup in docker"
else
    cd ..
    sudo cp dpicko.service /etc/systemd/system/dpicko.service
    sudo sed -i "s#%install#${INSTALL}#g" /etc/systemd/system/dpicko.service
    sudo systemctl daemon-reload
    sudo systemctl enable dpicko
    sudo systemctl start dpicko
    echo "Service started"
fi

echo "Setup complete"
exit 0

