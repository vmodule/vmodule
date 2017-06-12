#include <sutils/ThreadImpl.h>
#include <sutils/ILogger.h>
#include <sutils/Thread.h>

#include <errno.h>
#include <stdio.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "ThreadImpl"

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
		ILogger* logger = vmodule::CThread::GetLogger();
		 if(NULL != logger)
		 logger->Log(VMODULE_LOG_ERROR,LOG_TAG,
		 "createRawThread failed (entry=%p, res=%d, %s)\n",
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

vthread_id_t GetCurrentThreadId() {
	return (vthread_id_t)pthread_self();
}

bool IsCurrentThread(const vthread_id_t tid)
{
	return pthread_equal(pthread_self(), (pthread_t)tid);
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

static unsigned int threadIntermediary(void* vDetails) {
	struct threadDetails* pDetails = (struct threadDetails*) vDetails;
	int result;
	result = (*(pDetails->func))(pDetails->arg);
	if(pDetails !=	NULL) {
		delete pDetails;
		pDetails = NULL;
	}
	printf("thread exiting\n");
	return (unsigned int) result;
}

/*
 * Create and run a new thread.
 */
static bool doCreateThread(vthread_func_t fn, void* arg, vthread_id_t *id) {
	HANDLE hThread;
	struct threadDetails* pDetails = new threadDetails; // must be on heap
	unsigned int thrdaddr;

	ILogger* logger = NULL;

	pDetails->func = fn;
	pDetails->arg = arg;

#if defined(HAVE__BEGINTHREADEX)
	hThread = (HANDLE) _beginthreadex(NULL, 0, threadIntermediary, pDetails, 0,
			&thrdaddr);
	if (hThread == 0)
#elif defined(HAVE_CREATETHREAD)
	hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) threadIntermediary,
			(void*) pDetails, 0, (DWORD*) &thrdaddr);
	if (hThread == NULL)
#endif
	{
		logger = vmodule::CThread::GetLogger();
		if (NULL != logger)
			logger->Log(VMODULE_LOG_ERROR, LOG_TAG,
					"WARNING: thread create failed\n");
		else
			printf("WARNING: thread create failed\n");
		return false;
	}

#if defined(HAVE_CREATETHREAD)
	/* close the management handle */
	CloseHandle(hThread);
#endif
	if (id != NULL) {
		*id = (vthread_id_t) thrdaddr;
	}
	logger = vmodule::CThread::GetLogger();
	if (NULL != logger)
		logger->Log(VMODULE_LOG_ERROR, LOG_TAG, "thread create Success\n");
	else {
		printf("thread create Success\n");
	}
	return true;
}

int createRawThread(vthread_func_t entryFunction, void *userData,
		const char* threadName, size_t threadStackSize,
		vthread_id_t *threadId) {
	return doCreateThread(entryFunction, userData, threadId);
}

bool IsCurrentThread(const vthread_id_t tid) {
	return (GetCurrentThreadId() == tid);
}

#endif
