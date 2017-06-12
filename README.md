# vmodule
vmodule streaming

#for windows virtual studuio config
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
