//============================================================================
// Name        : StrongPointerTest.cpp
// Author      : jeffrey
// Version     :
// Copyright   : vmodule.org
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sutils/Logger.h>
#include <sutils/RefBase.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "StrongPointerTest"

#ifdef DEBUG_ENABLE
#define MY_LOGD(fmt, arg...)  VLOGD(LOG_TAG,fmt, ##arg)
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)
#else
#define MY_LOGD(fmt, arg...)
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)
#endif
using namespace std;
using namespace vmodule;

class StrongPointerTest: virtual public RefBase {
public:
	StrongPointerTest();
    virtual ~StrongPointerTest();
    virtual void onFirstRef();
};

StrongPointerTest::StrongPointerTest() {
	MY_LOGD("enter %s",__func__);
}

StrongPointerTest::~StrongPointerTest() {
	MY_LOGD("enter %s",__func__);
}

void StrongPointerTest::onFirstRef() {
	MY_LOGD("enter %s",__func__);
}

int main() {
	vmodule::Logger::Init("/home/jeffrey/workSpace/workApp/vmodule/log/");
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
    strong = weak.promote();
    MY_LOGD("========================");
	MY_LOGD("test1 getStrongCount = %d",test1->getStrongCount());
	MY_LOGD("strong getStrongCount = %d",strong->getStrongCount());
	return 0;
}
