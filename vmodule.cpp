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

// This is the constructor of a class that has been exported.
// see vmodule.h for the class definition
event_base *Vmodule::main_base = NULL;
event Vmodule::clockevent;	
Vmodule::Vmodule() {
	MY_LOGD("start %s",__FUNCTION__);
}

Vmodule::~Vmodule() {
	if(main_base) {
		free(main_base);
		main_base = NULL;
	}
}

event_base *Vmodule::getMainEventBase() {
	return main_base;
}

void Vmodule::Initialize() {
    /* initialize main thread libevent instance */
	if(NULL == main_base) {
		MY_LOGD("start %s",__FUNCTION__);
		main_base = event_init();
	}
	g_vmodule.process_started = 
		time(0) - ITEM_UPDATE_INTERVAL - 2;
}

void Vmodule::clock_handler(const int fd, 
	const short which, void *arg) {

    struct timeval t;
	t.tv_sec = 1;
	t.tv_usec = 0;
    static bool initialized = false;
#if defined(TARGET_POSIX) || defined(TARGET_ANDROID)
    static bool monotonic = false;
    static time_t monotonic_start;
#endif

    if (initialized) {
        /* only delete the event if it's actually there. */
        evtimer_del(&clockevent);
    } else {
        initialized = true;
        /* process_started is initialized to time() - 2. We initialize to 1 so
         * flush_all won't underflow during tests. */
#if defined(TARGET_POSIX) || defined(TARGET_ANDROID)
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
            monotonic = true;
            monotonic_start = ts.tv_sec - ITEM_UPDATE_INTERVAL - 2;
        }
#endif
    }

    evtimer_set(&clockevent, clock_handler, 0);
    event_base_set(main_base, &clockevent);
    evtimer_add(&clockevent, &t);

#if defined(TARGET_POSIX) || defined(TARGET_ANDROID)
    if (monotonic) {
        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1)
            return;
        g_vmodule.current_time = 
			(nsecs_t) (ts.tv_sec - monotonic_start);
        return;
    }
#endif
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        g_vmodule.current_time = (nsecs_t) (tv.tv_sec - 
			g_vmodule.process_started);
    }
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
