
#include <sutils/ThreadImpl.h>
#include <sutils/Logger.h>
#include <errno.h>
#include <stdio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ThreadImpl"

#ifdef DEBUG_ENABLE
#define MY_LOGD(fmt, arg...)  //VLOGD(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#define MY_LOGE(fmt, arg...)  //VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#else
#define MY_LOGD(fmt, arg...)
#define MY_LOGE(fmt, arg...)  //VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#endif

typedef void* (*v_pthread_entry)(void*);
#if !defined(TARGET_WINDOWS)
int createRawThread(vthread_func_t entryFunction, void *userData,
		const char* threadName, size_t threadStackSize,
		vthread_id_t *threadId) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (threadStackSize) {
		pthread_attr_setstacksize(&attr, threadStackSize);
	}
	errno = 0;
	pthread_t thread;
	int result = pthread_create(&thread, &attr, (v_pthread_entry) entryFunction,
			userData);

	pthread_attr_destroy(&attr);
	if (result != 0) {
		MY_LOGE("createRawThread failed (entry=%p, res=%d, %s)\n",
				entryFunction, result, strerror(errno));
		return 0;
	}
	// Note that *threadID is directly available to the parent only, as it is
	// assigned after the child starts.  Use memory barrier / lock if the child
	// or other threads also need access.
	if (threadId != NULL) {
		*threadId = (vthread_id_t) thread; // XXX: this is not portable
	}
	return 1;
}
#else
/*
 * Trampoline to make us __stdcall-compliant.
 *
 * We're expected to delete "vDetails" when we're done.
 */
struct threadDetails {
    int (*func)(void*);
    void* arg;
};
static __stdcall unsigned int threadIntermediary(void* vDetails)
{
    struct threadDetails* pDetails = (struct threadDetails*) vDetails;
    int result;

    result = (*(pDetails->func))(pDetails->arg);

    delete pDetails;

    MY_LOGD("thread exiting\n");
    return (unsigned int) result;
}

int createRawThread(vthread_func_t entryFunction, void *userData,
		const char* threadName, size_t threadStackSize,
		vthread_id_t *threadId) {
    HANDLE hThread;
    struct threadDetails* pDetails = new threadDetails; // must be on heap
    unsigned int thrdaddr;

    pDetails->func = fn;
    pDetails->arg = arg;

#if defined(HAVE__BEGINTHREADEX)
    hThread = (HANDLE) _beginthreadex(NULL, 0, threadIntermediary, pDetails, 0,
                    &thrdaddr);
    if (hThread == 0)
#elif defined(HAVE_CREATETHREAD)
    hThread = CreateThread(NULL, 0,
                    (LPTHREAD_START_ROUTINE) threadIntermediary,
                    (void*) pDetails, 0, (DWORD*) &thrdaddr);
    if (hThread == NULL)
#endif
    {
        MY_LOGD("WARNING: thread create failed\n");
        return 0;
    }
#if defined(HAVE_CREATETHREAD)
    /* close the management handle */
    CloseHandle(hThread);
#endif
    if (id != NULL) {
      	*id = (vthread_id_t)thrdaddr;
    }
    return 1;
}
#endif
