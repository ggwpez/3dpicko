#!/bin/sh

set -e

if [ -z "$1" ]; then
    echo "Working directory needed as first argument"
    exit 1
fi

if [ -z "$2" ]; then
    echo "Commit ahsh needed as second argument";
    exit 1
fi

if [ ! -z "$(ls -A $1)" ]
then
   echo "Working directory should be empty"
   exit 1
fi

ROOT=$(realpath $1)
SOURCE=$ROOT/source
BUILDS=$ROOT/builds

mkdir -p $SOURCE
mkdir -p $BUILDS

git clone -n https://gitlab.com/ggwpez/3cpicko.git $SOURCE
cd $SOURCE && git checkout $2
git submodule init
git submodule update
cp -n $SOURCE/server/database.json $ROOT
cp -n $SOURCE/server/serverconfig.ini $ROOT
cp start.sh $ROOT
chmod +x $ROOT/start.sh
DOCROOT=source/server/docroot
sed -i "s#%docroot#${DOCROOT}#g" $ROOT/serverconfig.ini
sed -i "s#%branch#${2}#g" $ROOT/serverconfig.ini
sed -i "s#%db#./#g" $ROOT/serverconfig.ini
# configure it
echo "working_dir=\"$ROOT\"" >> $ROOT/serverconfig.ini
cd $BUILDS
mkdir $2
cd $2

qmake $SOURCE
make -j$(nproc) > /dev/null
# GCC 8 likes to segfault, just try it twice
make -j$(nproc) > /dev/null

cd $ROOT
ln -s builds/$2/Main/Main $ROOT/main
ln -s builds/$2/depend/quazip/quazip/libquazip.so.1 $ROOT/libquazip.so.1

if [ "$CPICKO_ENV" = "docker" ]; then
    echo "Skipping service setup in docker"
else
    sudo cp $SOURCE/dpicko.service /etc/systemd/system/dpicko.service
    sudo sed -i "s#%install#${ROOT}#g" /etc/systemd/system/dpicko.service
    sudo systemctl daemon-reload
    sudo systemctl enable dpicko
    sudo systemctl start dpicko
    echo "Service started"
fi

echo "Setup complete"
exit 0

