#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sutils/Logger.h>
#include <sutils/Timers.h>
#include <sutils/FileUtils.h>
#include <tests/StrongPointerTest.h>
#include <tests/ThreadTest.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "VMODULE"

void vmodue_init() {
	char* pCurrDir = new char[260];
	vmodule::CFileUtils::getCurrentDirectory(260,pCurrDir);
	std::string pCurrDirString(pCurrDir);
#if defined(TARGET_WINDOWS)
	pCurrDirString.append("\\");
#else
	pCurrDirString.append("/");
#endif
	VLOGD(LOG_TAG,"pCurrDirString:%s\n", pCurrDirString.c_str());
	s_globalsLogger.Init(pCurrDirString);
	delete pCurrDir;

}

int main()
{
	vmodue_init();
	VLOGD(LOG_TAG,"start- %s\n", __FUNCTION__);
	VLOGD(LOG_TAG,"start- %s\n", __FUNCTION__);
	VLOGD(LOG_TAG,"start- %s\n", __FUNCTION__);
	VLOGD(LOG_TAG,"start- %s\n", __FUNCTION__);
	VLOGD(LOG_TAG,"start- %s\n", __FUNCTION__);
	VLOGD(LOG_TAG,"start- %s\n", __FUNCTION__);
	StrongPointerTest_main();
	//ThreadTest_main();
	while(true){
		sleep(1);
		VLOGD(LOG_TAG,"start %s\n", __FUNCTION__);
		//printf("start %s\n", __FUNCTION__);
	};
	return 0;
}
