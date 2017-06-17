
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sutils/Logger.h>
#include <sutils/FileUtils.h>
#include <vmodule/vmodule.h>

#if defined(TARGET_WINDOWS)
#include <windows.h>
#if defined(WIN32)
#include "stdafx.h"
#endif
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "VMODULE_MAIN"

#define MY_LOGD(format,...) 	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format,...) 	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)

int main(int argc, char **argv)
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
