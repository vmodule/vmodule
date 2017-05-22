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
#include <sutils/Timers.h>
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
	MY_LOGD("ThreadTest,%s: ", __FUNCTION__);
	//Sleep(1000);
	return true;
}


class ThreadTest2: public virtual CThread {
public:
	ThreadTest2();
	~ThreadTest2();
	virtual bool threadLoop();
};

/** Public members */
ThreadTest2::ThreadTest2() {
	MY_LOGD("%s", __FUNCTION__);
}

ThreadTest2::~ThreadTest2() {
	MY_LOGD("%s: ", __FUNCTION__);

}

bool ThreadTest2::threadLoop() {
	MY_LOGD("ThreadTest2,%s: ", __FUNCTION__);
	return true;
}


int ThreadTest_main() {
	//vmodule::Logger::Init("/home/jeffrey/workSpace/workApp/vmodule/log/");
	sp<ThreadTest> mThreadTest1;
	sp<ThreadTest2> mThreadTest2;
#if 0
	std::string threadName1("ThreadTest1");
	mThreadTest1 = new ThreadTest();
	mThreadTest1->run(threadName1.c_str());
#endif
	std::string threadName2("ThreadTest2");
	mThreadTest2 = new ThreadTest2();
	mThreadTest2->run(threadName2.c_str());
	while (true) {
		//Sleep(1000);
		mThreadTest2->Sleep(1);
		MY_LOGD("ThreadTest_main,%s: \n", __FUNCTION__);
	}
	mThreadTest1.clear();
	mThreadTest2.clear();
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
