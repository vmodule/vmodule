
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <sys/resource.h>
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


static void sig_handler(const int sig) {
    MY_LOGD("Signal handled: %s.\n", 
		strsignal(sig));
    exit(0);
}

#ifndef HAVE_SIGIGNORE
int sigignore(int sig) {
    struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
    if (sigemptyset(&sa.sa_mask) == -1 || 
		sigaction(sig, &sa, 0) == -1) {
        return -1;
    }
    return 0;
}
#endif

int main(int argc, char **argv)
{
	int retval = 0;
	vmodue_init();
	int count = 0;
	while (count < 100){
		MY_LOGD("start %s", __FUNCTION__);
		count++;
	};	
    /* handle SIGINT and SIGTERM */
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    /*
     * ignore SIGPIPE signals; we can use errno == EPIPE if we
     * need that information
     */
    if (sigignore(SIGPIPE) == -1) {
    	MY_LOGE("failed to ignore SIGPIPE; sigaction");
        exit(1);
    }
    g_vmodule.Initialize();
	
    /* initialise clock event */
    g_vmodule.clock_handler(0, 0, 0);

    //grow our pool of file descriptors to the max!
    struct rlimit rl;
    
    // set it to the absolute maximum that the operating system allows - have to be superuser to do this
    rl.rlim_cur = RLIM_INFINITY;
    rl.rlim_max = RLIM_INFINITY;
 
    setrlimit (RLIMIT_NOFILE, &rl);

    getrlimit(RLIMIT_NOFILE,  &rl);
    MY_LOGD("current open file limit =%ld\n", rl.rlim_cur);
    MY_LOGD("current open file max =%ld\n", rl.rlim_max);	
    /* enter the event loop */
    if (event_base_loop(g_vmodule.getMainEventBase(),
    		0) != 0) {
        retval = 1;
    }

	MY_LOGD("start end main......");

	return 0;
}
