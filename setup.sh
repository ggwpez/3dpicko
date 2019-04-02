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
cp $SOURCE/GUI/database.json $1
cp $SOURCE/GUI/serverconfig.ini $1
echo 'working_dir="$1"' >> $1/serverconfig.ini
cd $BUILDS
mkdir $2
cd $2

# TODO release
qmake $SOURCE
make -j8 > /dev/null

ln builds/$2/Main/Main $1/main