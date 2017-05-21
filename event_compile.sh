#! /bin/sh
#make && make install
export NDK_ROOT=/home/jeffrey/ideTools/android-ndk-r14b
export TOOLCHAINS_ROOT=$NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
export TOOLCHAINS_PREFIX=$TOOLCHAINS_ROOT/bin
#export TOOLCHAINS_INCLUDE=$TOOLCHAINS_ROOT/lib/gcc/arm-linux-androideabi/4.9.x/include-fixed
export TOOLCHAINS_INCLUDE=$TOOLCHAINS_ROOT/lib/gcc/arm-linux-androideabi/4.9.x/include
export PLATFORM_ROOT=$NDK_ROOT/platforms/android-24/arch-arm
export PLATFORM_INCLUDE=$PLATFORM_ROOT/usr/include
export PLATFORM_LIB=$PLATFORM_ROOT/usr/lib

SOURCE_PATH=`pwd`
echo "source path = $SOURCE_PATH"

if [ ! -f "crtbegin_so.o" ]; then  
    ln -s $PLATFORM_LIB/crtbegin_so.o ${pwd} 
fi 

if [ ! -f "crtend_so.o" ]; then  
    ln -s $PLATFORM_LIB/crtend_so.o ${pwd} 
fi 

./configure --prefix=$SOURCE_PATH/output --host=arm-linux-androideabi \
CC=$TOOLCHAINS_PREFIX/arm-linux-androideabi-gcc \
LD=$TOOLCHAINS_PREFIX/arm-linux-androideabi-ld \
CPPFLAGS="-I$PLATFORM_INCLUDE" \
CFLAGS="-nostdlib" \
LDFLAGS="-Wl,-rpath-link=$PLATFORM_LIB -L$PLATFORM_LIB" \
LIBS="-lc -lgcc -L$PLATFORM_LIB -L$TOOLCHAINS_INCLUDE"


