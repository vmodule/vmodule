// vmodule.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sutils/Logger.h>
#include <sutils/FileUtils.h>
#include <vmodule/vmodule.h>
#if defined(TARGET_WINDOWS)
#include <windows.h>
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "VMODULE"

#define MY_LOGD(format,...) 	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format,...) 	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
// This is an example of an exported variable
VMODULE_API int nvmodule=0;

// This is an example of an exported function.
VMODULE_API int fnvmodule(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see vmodule.h for the class definition
Cvmodule::Cvmodule()
{
	return;
}

VMODULE_API void vmodue_init() {
	char* pCurrDir = new char[260];
	GetCurrentDirectory(260, pCurrDir);
	std::string pCurrDirString(pCurrDir);
#if defined(TARGET_WINDOWS)
	pCurrDirString.append("\\");
#else
	pCurrDirString.append("/");
#endif
	vmodule::Logger::Init(pCurrDirString);
	if (pCurrDir != NULL) {
		delete pCurrDir;
		pCurrDir = NULL;
	}
}

#ifdef VMODULE_EXECUTABLE
int main()
{
	vmodue_init();
	int count = 0;
	while (count < 100){
		MY_LOGD("start %s", __FUNCTION__);
		count++;
	};
	#if defined(TARGET_WINDOWS)
	system("pause");
	#else
	while(true);
	#endif
	return 0;
}
#endif