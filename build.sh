#!/bin/sh

if [ "x$1" != "x" ];then
	export BUILD_ARCH=$1
else
	if uname -a | grep arm ; then
		export BUILD_ARCH=arm
	elif uname -a | grep x86_64 ; then
		export BUILD_ARCH=x64
	elif uname -a | grep x86 ; then
		export BUILD_ARCH=ia32
	else
		echo "architecture is unknown!"
		exit;
	fi
fi
#export BUILD_ARCH=arm
#FPUHINT="-O3 -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -mabi=aapcs-linux -fforce-addr -fomit-frame-pointer -fstrength-reduce -ftree-vectorize -mvectorize-with-neon-quad"
FPUHINT="$2"

O_CFLAGS="$CFLAGS"
O_CXXFLAGS="$CXXFLAGS"
export CFLAGS="$CFLAGS $FPUHINT"
export CXXFLAGS="$CFLAGS $FPUHINT"
CURRENTDIR=`pwd`

cd julius
make clean
chmod +x ./configure
./configure  --disable-plugin --enable-setup=fast  --enable-alsa
make
cd ..

cd openjtalk/hts_engine_API
make clean
chmod +x ./configure
./configure
make
cd ../..

cd openjtalk/open_jtalk
make clean
chmod +x ./configure
./configure --with-charset=UTF-8 --with-hts-engine-header-path=${CURRENTDIR}/openjtalk/hts_engine_API/include --with-hts-engine-library-path=${CURRENTDIR}/openjtalk/hts_engine_API/lib
make
cd ../..

cd v8
CXXFLAGS="$FPUHINT -Wno-strict-overflow -Wno-unused-local-typedefs -Wno-aggressive-loop-optimizations"
chmod +x build/gyp/gyp
make clean
make $BUILD_ARCH
CXXFLAGS="$CFLAGS $FPUHINT"
cd ..

cd pcre
make clean
chmod +x ./configure
./configure
make
cd ..

cd babel
make clean
make
cd ..

cd liboauthcpp/src
make clean
make
cd ../..

cd Personal-HomeKit-HAP
make clean
make phklib
cd ..

cd naichichi2
make clean
make 
#make debug
cd ..

export CFLAGS="$O_CFLAGS"
export CXXFLAGS="$O_CXXFLAGS"
