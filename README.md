# vmodule
vmodule streaming

#how to CMake_Cross_Compiling?
http://www.vtk.org/Wiki/CMake_Cross_Compiling#Setting_up_the_system_and_toolchain
https://cmake.org/cmake/help/v3.8/manual/cmake-toolchains.7.html


#How to compile vmodule to Linux
1).download the vmodule source the make dir build
    $mkdir build
    $cd build 
    $cmake ../ -DCMAKE_SYSTEM_NAME=Linux -DCMAKE_INSTALL_PREFIX=../output
    $make
    $make install

#how to compile vmodule to android
#if the ndk path:/home/jeffrey/ideTools/android-ndk-r14b
    $cmake ../ \
     -DCMAKE_SYSTEM_NAME=Android \
     -DCMAKE_SYSTEM_VERSION=24 \
     -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a \
     -DCMAKE_ANDROID_NDK=/home/jeffrey/ideTools/android-ndk-r14b \
     -DCMAKE_ANDROID_STL_TYPE=gnustl_static \
     -DCMAKE_INSTALL_PREFIX=../output

#windows virtual studuio config
# please create enpty project
1.
C/C++->Preprocessor:
WIN32
_DEBUG
_CRT_SECURE_NO_WARNINGS
DEBUG_ENABLE
TARGET_WINDOWS
VMODULE_EXECUTABLE

2.
C/C++->Advanced->Compile As:
	Compile as C++ Code
	
3.	
C/C++->Preprocessor->Preprocessor Header:
	Not Using Precompiled Header
4.
C/C++->Code Generation->Runtime Library:
	Thread Debug DLL (MDd)
