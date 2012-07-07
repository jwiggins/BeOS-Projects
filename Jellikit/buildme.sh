#!/bin/sh

DIRNAME=$(dirname $0)
MACHINE=$(uname -m)
if [ $MACHINE == "BePC" ]
then
	CPU=x86
else
	CPU=ppc
fi

cd $DIRNAME/Addon\ sources/bool/
make
mv obj.$CPU/BoolAddon ../../add-ons/BoolAddon
cd ../integer/
make
mv obj.$CPU/IntegerAddon ../../add-ons/IntegerAddon
cd ../rect/
make
mv obj.$CPU/BRectAddon ../../add-ons/BRectAddon
cd ../string/
make
mv obj.$CPU/StringAddon ../../add-ons/StringAddon
cd ../../
make
echo "All done!"