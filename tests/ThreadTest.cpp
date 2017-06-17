//============================================================================
// Name        : vmodule.cpp
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
#include <sutils/Thread.h>
#include <sutils/Timers.h>
#include <vmodule/vmodule.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ThreadTest"

#ifdef DEBUG_ENABLE
#define MY_LOGD(format, ...) vmodule::Logger::Log(VMODULE_LOG_DEBUG,LOG_TAG,format,##__VA_ARGS__)
#define MY_LOGE(format, ...)  vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
#else
#define MY_LOGD(format, ...)
#define MY_LOGE(format, ...)  vmodule::Logger::Log(VMODULE_LOG_ERROR,LOG_TAG,format,##__VA_ARGS__)
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

int main(int argc, char **argv) {
	vmodue_init();
	sp<ThreadTest> mThreadTest1;
	sp<ThreadTest2> mThreadTest2;
	std::string threadName1("ThreadTest1");
	mThreadTest1 = new ThreadTest();
	mThreadTest1->run(threadName1.c_str());
	std::string threadName2("ThreadTest2");
	mThreadTest2 = new ThreadTest2();
	mThreadTest2->run(threadName2.c_str());
	while (true) {
		//Sleep(1000);
		mThreadTest2->Sleep(1000);
		MY_LOGD("ThreadTest_main,%s: \n", __FUNCTION__);
	}
	mThreadTest1.clear();
	mThreadTest2.clear();
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
