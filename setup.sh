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


SOURCE=$1/source
BUILDS=$1/builds

mkdir -p $SOURCE
mkdir -p $BUILDS

git clone -n git@gitlab.com:ggwpez/3cpicko.git $SOURCE
cd $SOURCE && git checkout $2
git submodule init
git submodule update
cp -n $SOURCE/server/database.json $1
cp -n $SOURCE/server/serverconfig.ini $1
cp $SOURCE/start.sh $1
chmod +x $1/start.sh
DOCROOT=source/server/docroot
sed -i "s#%docroot#${DOCROOT}#g" $1/serverconfig.ini
sed -i "s#%branch#${2}#g" $1/serverconfig.ini
# configure it
echo "working_dir=\"$1\"" >> $1/serverconfig.ini
cd $BUILDS
mkdir $2
cd $2

# TODO release
qmake $SOURCE
make -j7 > /dev/null
# GCC 8 likes to segfault, just try it twice
make -j7 > /dev/null

cd $1
ln -s builds/$2/Main/Main $1/main
ln -s builds/$2/depend/quazip/quazip/libquazip.so.1 $1/libquazip.so.1

echo "Setup complete, start with ./start.sh"
exit 0
