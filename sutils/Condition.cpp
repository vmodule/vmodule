/*
 * Condition.cpp
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
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/resource.h>
#else
#include <windows.h>
#include <stdint.h>
#include <process.h>
#define HAVE_CREATETHREAD  // Cygwin, vs. HAVE__BEGINTHREADEX for MinGW
#endif

#include <sutils/Condition.h>

namespace vmodule {

#if !defined(TARGET_WINDOWS)
Condition::Condition() {
	pthread_cond_init(&mCond, NULL);
}

Condition::Condition(int type) {
	if (type == SHARED) {
		pthread_condattr_t attr;
		pthread_condattr_init(&attr);
		pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
		pthread_cond_init(&mCond, &attr);
		pthread_condattr_destroy(&attr);
	} else {
		pthread_cond_init(&mCond, NULL);
	}
}

Condition::~Condition() {
	pthread_cond_destroy(&mCond);
}

status_t Condition::wait(Mutex& mutex) {
	return -pthread_cond_wait(&mCond, &mutex.mMutex);
}
/*
 *reltime = ns
 *
 * */

status_t Condition::waitRelative(Mutex& mutex, nsecs_t reltime) {
#if defined(HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE)
	struct timespec ts;
	ts.tv_sec = reltime/1000000000;
	ts.tv_nsec = reltime%1000000000;
	return -pthread_cond_timedwait_relative_np(&mCond, &mutex.mMutex, &ts);
#else // HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE
	struct timespec ts;
#if defined (TARGET_POSIX) || defined(__POSIX__)
	clock_gettime(CLOCK_REALTIME, &ts);
#else // __APPLE__
	// we don't support the clocks here.
	struct timeval t;
	gettimeofday(&t, NULL);
	ts.tv_sec = t.tv_sec;
	ts.tv_nsec= t.tv_usec*1000;
#endif
	ts.tv_sec += reltime / 1000000000;
	ts.tv_nsec += reltime % 1000000000;
	if (ts.tv_nsec >= 1000000000) {
		ts.tv_nsec -= 1000000000;
		ts.tv_sec += 1;
	}
	return -pthread_cond_timedwait(&mCond, &mutex.mMutex, &ts);
#endif // HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE
}

void Condition::signal() {
	/*
	 * POSIX says pthread_cond_signal wakes up "one or more" waiting threads.
	 * However bionic follows the glibc guarantee which wakes up "exactly one"
	 * waiting thread.
	 *
	 * man 3 pthread_cond_signal
	 *   pthread_cond_signal restarts one of the threads that are waiting on
	 *   the condition variable cond. If no threads are waiting on cond,
	 *   nothing happens. If several threads are waiting on cond, exactly one
	 *   is restarted, but it is not specified which.
	 */
	pthread_cond_signal(&mCond);
}

void Condition::broadcast() {
	pthread_cond_broadcast(&mCond);
}

#else

/*
 * Windows doesn't have a condition variable solution.  It's possible
 * to create one, but it's easy to get it wrong.  For a discussion, and
 * the origin of this implementation, see:
 *
 *  http://www.cs.wustl.edu/~schmidt/win32-cv-1.html
 *
 * The implementation shown on the page does NOT follow POSIX semantics.
 * As an optimization they require acquiring the external mutex before
 * calling signal() and broadcast(), whereas POSIX only requires grabbing
 * it before calling wait().  The implementation here has been un-optimized
 * to have the correct behavior.
 */
typedef struct WinCondition {
	// Number of waiting threads.
	int waitersCount;

	// Serialize access to waitersCount.
	CRITICAL_SECTION waitersCountLock;

	// Semaphore used to queue up threads waiting for the condition to
	// become signaled.
	HANDLE sema;

	// An auto-reset event used by the broadcast/signal thread to wait
	// for all the waiting thread(s) to wake up and be released from
	// the semaphore.
	HANDLE waitersDone;

	// This mutex wouldn't be necessary if we required that the caller
	// lock the external mutex before calling signal() and broadcast().
	// I'm trying to mimic pthread semantics though.
	HANDLE internalMutex;

	// Keeps track of whether we were broadcasting or signaling.  This
	// allows us to optimize the code if we're just signaling.
	bool wasBroadcast;

	status_t wait(WinCondition* condState, HANDLE hMutex, nsecs_t* abstime)
	{
		// Increment the wait count, avoiding race conditions.
		EnterCriticalSection(&condState->waitersCountLock);
		condState->waitersCount++;
		//printf("+++ wait: incr waitersCount to %d (tid=%ld)\n",
		//    condState->waitersCount, getThreadId());
		LeaveCriticalSection(&condState->waitersCountLock);

		DWORD timeout = INFINITE;
		if (abstime) {
			nsecs_t reltime = *abstime - systemTime();
			if (reltime < 0)
			reltime = 0;
			timeout = reltime/1000000;
		}

		// Atomically release the external mutex and wait on the semaphore.
		DWORD res =
		SignalObjectAndWait(hMutex, condState->sema, timeout, FALSE);

		//printf("+++ wait: awake (tid=%ld)\n", getThreadId());

		// Reacquire lock to avoid race conditions.
		EnterCriticalSection(&condState->waitersCountLock);

		// No longer waiting.
		condState->waitersCount--;

		// Check to see if we're the last waiter after a broadcast.
		bool lastWaiter = (condState->wasBroadcast && condState->waitersCount == 0);

		//printf("+++ wait: lastWaiter=%d (wasBc=%d wc=%d)\n",
		//    lastWaiter, condState->wasBroadcast, condState->waitersCount);

		LeaveCriticalSection(&condState->waitersCountLock);

		// If we're the last waiter thread during this particular broadcast
		// then signal broadcast() that we're all awake.  It'll drop the
		// internal mutex.
		if (lastWaiter) {
			// Atomically signal the "waitersDone" event and wait until we
			// can acquire the internal mutex.  We want to do this in one step
			// because it ensures that everybody is in the mutex FIFO before
			// any thread has a chance to run.  Without it, another thread
			// could wake up, do work, and hop back in ahead of us.
			SignalObjectAndWait(condState->waitersDone, condState->internalMutex,
					INFINITE, FALSE);
		} else {
			// Grab the internal mutex.
			WaitForSingleObject(condState->internalMutex, INFINITE);
		}

		// Release the internal and grab the external.
		ReleaseMutex(condState->internalMutex);
		WaitForSingleObject(hMutex, INFINITE);

		return res == WAIT_OBJECT_0 ? NO_ERROR : -1;
	}
}WinCondition;

/*
 * Constructor.  Set up the WinCondition stuff.
 */
Condition::Condition()
{
	WinCondition* condState = new WinCondition;

	condState->waitersCount = 0;
	condState->wasBroadcast = false;
	// semaphore: no security, initial value of 0
	condState->sema = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
	InitializeCriticalSection(&condState->waitersCountLock);
	// auto-reset event, not signaled initially
	condState->waitersDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	// used so we don't have to lock external mutex on signal/broadcast
	condState->internalMutex = CreateMutex(NULL, FALSE, NULL);

	mState = condState;
}

/*
 * Destructor.  Free Windows resources as well as our allocated storage.
 */
Condition::~Condition()
{
	WinCondition* condState = (WinCondition*) mState;
	if (condState != NULL) {
		CloseHandle(condState->sema);
		CloseHandle(condState->waitersDone);
		delete condState;
	}
}

status_t Condition::wait(Mutex& mutex)
{
	WinCondition* condState = (WinCondition*) mState;
	HANDLE hMutex = (HANDLE) mutex.mState;

	return ((WinCondition*)mState)->wait(condState, hMutex, NULL);
}

status_t Condition::waitRelative(Mutex& mutex, nsecs_t reltime)
{
	WinCondition* condState = (WinCondition*) mState;
	HANDLE hMutex = (HANDLE) mutex.mState;
	nsecs_t absTime = systemTime()+reltime;

	return ((WinCondition*)mState)->wait(condState, hMutex, &absTime);
}

/*
 * Signal the condition variable, allowing one thread to continue.
 */
void Condition::signal()
{
	WinCondition* condState = (WinCondition*) mState;

	// Lock the internal mutex.  This ensures that we don't clash with
	// broadcast().
	WaitForSingleObject(condState->internalMutex, INFINITE);

	EnterCriticalSection(&condState->waitersCountLock);
	bool haveWaiters = (condState->waitersCount > 0);
	LeaveCriticalSection(&condState->waitersCountLock);

	// If no waiters, then this is a no-op.  Otherwise, knock the semaphore
	// down a notch.
	if (haveWaiters)
	ReleaseSemaphore(condState->sema, 1, 0);

	// Release internal mutex.
	ReleaseMutex(condState->internalMutex);
}

/*
 * Signal the condition variable, allowing all threads to continue.
 *
 * First we have to wake up all threads waiting on the semaphore, then
 * we wait until all of the threads have actually been woken before
 * releasing the internal mutex.  This ensures that all threads are woken.
 */
void Condition::broadcast()
{
	WinCondition* condState = (WinCondition*) mState;

	// Lock the internal mutex.  This keeps the guys we're waking up
	// from getting too far.
	WaitForSingleObject(condState->internalMutex, INFINITE);

	EnterCriticalSection(&condState->waitersCountLock);
	bool haveWaiters = false;

	if (condState->waitersCount > 0) {
		haveWaiters = true;
		condState->wasBroadcast = true;
	}

	if (haveWaiters) {
		// Wake up all the waiters.
		ReleaseSemaphore(condState->sema, condState->waitersCount, 0);

		LeaveCriticalSection(&condState->waitersCountLock);

		// Wait for all awakened threads to acquire the counting semaphore.
		// The last guy who was waiting sets this.
		WaitForSingleObject(condState->waitersDone, INFINITE);

		// Reset wasBroadcast.  (No crit section needed because nobody
		// else can wake up to poke at it.)
		condState->wasBroadcast = 0;
	} else {
		// nothing to do
		LeaveCriticalSection(&condState->waitersCountLock);
	}

	// Release internal mutex.
	ReleaseMutex(condState->internalMutex);
}
#endif // !defined(TARGET_WINDOWS)
} /* namespace vmodule */
