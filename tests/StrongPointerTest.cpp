//============================================================================
// Name        : StrongPointerTest.cpp
// Author      : jeffrey
// Version     :
// Copyright   : vmodule.org
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sutils/Logger.h>
#include <sutils/RefBase.h>
#include <sutils/FileUtils.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "StrongPointerTest"

#ifdef DEBUG_ENABLE
#define MY_LOGD(format, ...)	vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format, ...)	vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
#else
#define MY_LOGD(format, ...)
#define MY_LOGE(format, ...)  vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
#endif

using namespace std;
using namespace vmodule;

class StrongPointerTest: virtual public RefBase {
public:
	StrongPointerTest();
    virtual ~StrongPointerTest();
    virtual void onFirstRef();
	virtual void onLastStrongRef(const void* id);
	virtual void onLastWeakRef(const void* id);
};

StrongPointerTest::StrongPointerTest() {
	MY_LOGD("enter %s",__FUNCTION__);
}


StrongPointerTest::~StrongPointerTest() {
	MY_LOGD("enter %s", __FUNCTION__);
}

void StrongPointerTest::onFirstRef() {
	MY_LOGD("enter %s", __FUNCTION__);
}

void StrongPointerTest::onLastStrongRef(const void* id) {
	MY_LOGD("enter %s", __FUNCTION__);
}

void StrongPointerTest::onLastWeakRef(const void* id) {
	MY_LOGD("enter %s", __FUNCTION__);
}

int main(int argc, char **argv) {
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
	sp<StrongPointerTest> test1 = new StrongPointerTest();
	MY_LOGD("test1 getStrongCount = %d",test1->getStrongCount());
    sp<StrongPointerTest> strong(test1);
	MY_LOGD("test1 getStrongCount = %d",test1->getStrongCount());
	MY_LOGD("strong getStrongCount = %d",strong->getStrongCount());
	MY_LOGD("========================");
    wp<StrongPointerTest> weak(strong);
	MY_LOGD("strong getWeakCount = %d",strong->getWeakRefs()->getWeakCount());
	MY_LOGD("test1 getStrongCount = %d",test1->getStrongCount());
	MY_LOGD("strong getStrongCount = %d",strong->getStrongCount());
    strong.clear();
    MY_LOGD("========================");
	MY_LOGD("test1 getStrongCount = %d",test1->getStrongCount());
	MY_LOGD("========================");
	test1.clear();//it will enter onLastStrongRef then enter ~StrongPointerTest
	while(true);
	return 0;
}
