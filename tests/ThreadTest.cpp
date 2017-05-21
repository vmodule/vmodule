//============================================================================
// Name        : vmodule.cpp
// Author      : jeffrey
// Version     :
// Copyright   : vmodule.org
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sutils/Logger.h>
#include <sutils/RefBase.h>
#include <sutils/Thread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ThreadTest"

#ifdef DEBUG_ENABLE
#define MY_LOGD(fmt, arg...)  VLOGD(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#else
#define MY_LOGD(fmt, arg...)
#define MY_LOGE(fmt, arg...)  VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#endif
using namespace std;
using namespace vmodule;
class ThreadTest: public virtual CThread {
public:
	ThreadTest();
	~ThreadTest();
	virtual bool threadLoop();
};

/** Public members */
ThreadTest::ThreadTest() {
	MY_LOGD("%s", __FUNCTION__);
}

ThreadTest::~ThreadTest() {
	MY_LOGD("%s: ", __FUNCTION__);
}

bool ThreadTest::threadLoop() {
	MY_LOGD("%s: ", __FUNCTION__);
	sleep(1);
	return true;
}

int main() {
	vmodule::Logger::Init("/home/jeffrey/workSpace/workApp/vmodule/log/");
	sp<ThreadTest> mThreadTest;
	std::string threadName("ThreadTest");
	mThreadTest = new ThreadTest();
	mThreadTest->run(threadName.c_str());
	int i = 0;
	while (true) {
		sleep(1);
		//mThreadTest->sleep(10);
		MY_LOGD("=====i = %d=======",i);
		i++;
		if (i == 5) {
			mThreadTest->requestExitAndWait();
		}
		if (i == 10)
			break;

	}
	mThreadTest.clear();
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
