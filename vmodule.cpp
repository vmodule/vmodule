// vmodule.cpp : Defines the exported functions for the DLL application.
//

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

