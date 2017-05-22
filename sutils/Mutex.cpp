/*
 * Mutex.cpp
 *
 *  Created on: May 19, 2017
 *      Author: jeffrey
 */
#include <assert.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(TARGET_WINDOWS)
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#include <unistd.h>
#else
#include <windows.h>
#include <stdint.h>
#include <process.h>
#define HAVE_CREATETHREAD  // Cygwin, vs. HAVE__BEGINTHREADEX for MinGW
#endif

#if defined (TARGET_POSIX) || defined(__POSIX__)
#include <sys/prctl.h>
#endif

#include <sutils/Mutex.h>

namespace vmodule {

#if !defined(TARGET_WINDOWS)
Mutex::Mutex() {
	pthread_mutex_init(&mMutex, NULL);
}

Mutex::Mutex(__attribute__((unused)) const char* name) {
	pthread_mutex_init(&mMutex, NULL);
}

Mutex::Mutex(int type, __attribute__((unused)) const char* name) {
	if (type == SHARED) {
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_mutex_init(&mMutex, &attr);
		pthread_mutexattr_destroy(&attr);
	} else {
		pthread_mutex_init(&mMutex, NULL);
	}
}

Mutex::~Mutex() {
	pthread_mutex_destroy(&mMutex);
}

status_t Mutex::lock() {
	return -pthread_mutex_lock(&mMutex);
}

void Mutex::unlock() {
	pthread_mutex_unlock(&mMutex);
}

status_t Mutex::tryLock() {
	return -pthread_mutex_trylock(&mMutex);
}

#if defined(TARGET_ANDROID)
status_t Mutex::timedLock(nsecs_t timeoutNs) {
	timeoutNs += systemTime(SYSTEM_TIME_REALTIME);
	const struct timespec ts = {
		/* .tv_sec = */static_cast<time_t>(timeoutNs / 1000000000),
		/* .tv_nsec = */static_cast<long>(timeoutNs % 1000000000),
	};
	return -pthread_mutex_timedlock(&mMutex, &ts);
}
#endif

#else

Mutex::Mutex()
{
	HANDLE hMutex;

	assert(sizeof(hMutex) == sizeof(mState));

	hMutex = CreateMutex(NULL, FALSE, NULL);
	mState = (void*) hMutex;
}

Mutex::Mutex(const char* name)
{
	// XXX: name not used for now
	HANDLE hMutex;

	assert(sizeof(hMutex) == sizeof(mState));

	hMutex = CreateMutex(NULL, FALSE, NULL);
	mState = (void*) hMutex;
}

Mutex::Mutex(int type, const char* name)
{
	// XXX: type and name not used for now
	HANDLE hMutex;

	assert(sizeof(hMutex) == sizeof(mState));

	hMutex = CreateMutex(NULL, FALSE, NULL);
	mState = (void*) hMutex;
}

Mutex::~Mutex()
{
	CloseHandle((HANDLE) mState);
}

status_t Mutex::lock()
{
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject((HANDLE) mState, INFINITE);
	return dwWaitResult != WAIT_OBJECT_0 ? -1 : NO_ERROR;
}

void Mutex::unlock()
{
	if (!ReleaseMutex((HANDLE) mState))
		printf("%s,WARNING: bad result from unlocking mutex\n", __FUNCTION__);
}

status_t Mutex::tryLock()
{
	DWORD dwWaitResult;

	dwWaitResult = WaitForSingleObject((HANDLE) mState, 0);
	if (dwWaitResult != WAIT_OBJECT_0
			&& dwWaitResult != WAIT_TIMEOUT)
			printf("%s,WARNING: bad result from try-locking mutex\n", __FUNCTION__);
	return (dwWaitResult == WAIT_OBJECT_0) ? 0 : -1;
}

#endif // !defined(TARGET_WINDOWS)
} /* namespace vmodule */
