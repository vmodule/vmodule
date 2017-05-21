/*
 * Thread.cpp
 *
 *  Created on: Jan 1, 2017
 *      Author: jeffrey
 */
#include <sutils/Thread.h>
#include <sutils/ThreadImpl.h>
#if defined(TARGET_ANDROID)
#include <unistd.h>
#else
#include <sys/syscall.h>
#endif

#if !defined(TARGET_WINDOWS)
# include <pthread.h>
# include <sched.h>
# include <sys/resource.h>
#else
# include <windows.h>
# include <stdint.h>
# include <process.h>
# define HAVE_CREATETHREAD  // Cygwin, vs. HAVE__BEGINTHREADEX for MinGW
#endif

#if defined(TARGET_POSIX)
#include <sys/prctl.h>
#endif

#include <sutils/Errors.h>
#include <sutils/Timers.h>
#include <sutils/ThreadLocal.h>
#include <sutils/ThreadImpl.h>
#include <sutils/Logger.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "CThread"

#ifdef DEBUG_ENABLE
#define MY_LOGD(fmt, arg...)  //VLOGD(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#define MY_LOGE(fmt, arg...)  //VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#else
#define MY_LOGD(fmt, arg...)
#define MY_LOGE(fmt, arg...)  //VLOGE(LOG_TAG,fmt, ##arg)//MY_LOGD(fmt, ##arg)X
#endif

namespace vmodule {

static ThreadLocal<CThread> currentThread;

CThread::CThread()
	: m_ThreadId(vthread_id_t(-1))
	, mStatus(NO_ERROR)
	, mExitPending(false)
	, mRunning(false)
{
	MY_LOGD("%s", __FUNCTION__);
}

CThread::~CThread() {
	MY_LOGD("%s", __FUNCTION__);
}

status_t CThread::readyToRun() {
	MY_LOGD("%s", __FUNCTION__);
	return NO_ERROR;
}

status_t CThread::run(const char* name, size_t stack) {

	Mutex::Autolock _l(mLock);
	if (name)
		m_ThreadName = name;
	else
		m_ThreadName = "00-CThread";

	if (mRunning) {
		// thread already started
		return INVALID_OPERATION;
	}

	// reset status and exitPending to their default value, so we can
	// try again after an error happened (either below, or in readyToRun())
	mStatus = NO_ERROR;
	mExitPending = false;
	m_ThreadId = vthread_id_t(-1);
	// hold a strong reference on ourself
	mHoldSelf = this;
	mRunning = true;

	bool res;
	res = createRawThread(_threadLoop, this, name, stack, &m_ThreadId);

	if (res == false) {
		mStatus = UNKNOWN_ERROR; // something happened!
		mRunning = false;
		m_ThreadId = vthread_id_t(-1);
		mHoldSelf.clear(); // "this" may have gone away after this.
		return UNKNOWN_ERROR;
	}
	return NO_ERROR;
}

int CThread::_threadLoop(void* user) {
	MY_LOGD("%s", __FUNCTION__);
	CThread* const self = static_cast<CThread*>(user);
	currentThread.set(self);
	sp<CThread> strong(self->mHoldSelf);
	wp<CThread> weak(strong);
	self->mHoldSelf.clear();
#if defined(TARGET_ANDROID)
	// this is very useful for debugging with gdb
	self->mTid = gettid();
#endif
	bool first = true;
	do {
		bool result;
		if (first) {
			first = false;
			self->mStatus = self->readyToRun();
			result = (self->mStatus == NO_ERROR);

			if (result && !self->exitPending())
				result = self->threadLoop();
		} else {
			result = self->threadLoop();
		}
		// establish a scope for mLock
		{
			Mutex::Autolock _l(self->mLock);
			if (result == false || self->mExitPending) {
				self->mExitPending = true;
				self->mRunning = false;
				// clear thread ID so that requestExitAndWait() does not exit if
				// called by a new thread using the same thread ID as this one.
				self->m_ThreadId = vthread_id_t(-1);
				// note that interested observers blocked in requestExitAndWait are
				// awoken by broadcast, but blocked on mLock until break exits scope
				self->mExitedCondition.broadcast();
				break;
			}
		}
		strong.clear();
		strong = weak.promote();
	} while (strong != 0);

	return 0;
}

void CThread::requestExit() {
    Mutex::Autolock _l(mLock);
    mExitPending = true;
}

status_t CThread::requestExitAndWait() {
    Mutex::Autolock _l(mLock);
    if (m_ThreadId == CurrentThreadId()) {
        printf(
        "Thread (this=%p): don't call waitForExit() from this "
        "Thread object's thread. It's a guaranteed deadlock!",
        this);

        return WOULD_BLOCK;
    }

    mExitPending = true;

    while (mRunning == true) {
        mExitedCondition.wait(mLock);
    }
    // This next line is probably not needed any more, but is being left for
    // historical reference. Note that each interested party will clear flag.
    mExitPending = false;

    return mStatus;
}

status_t CThread::join() {
    Mutex::Autolock _l(mLock);
    if (m_ThreadId == CurrentThreadId()) {
    	MY_LOGE(
        "Thread (this=%p): don't call join() from this "
        "Thread object's thread. It's a guaranteed deadlock!",
        this);

        return WOULD_BLOCK;
    }

    while (mRunning == true) {
        mExitedCondition.wait(mLock);
    }

    return mStatus;
}

bool CThread::isRunning() const {
	Mutex::Autolock _l(mLock);
	return mRunning;
}

#if defined(TARGET_ANDROID)
pid_t CThread::getTid() const
{
    // mTid is not defined until the child initializes it, and the caller may need it earlier
    Mutex::Autolock _l(mLock);
    pid_t tid;
    if (mRunning) {
        pthread_t pthread = (pthread_t)m_ThreadId;
        tid = pthread_gettid_np(pthread);
    } else {
    	MY_LOGE("Thread (this=%p): getTid() is undefined before run()", this);
        tid = -1;
    }
    return tid;
}
#endif


bool CThread::exitPending() const {
    Mutex::Autolock _l(mLock);
    return mExitPending;
}

vthread_id_t CThread::ThreadId() const {
	return m_ThreadId;
}

CThread* CThread::GetCurrentThread() {
	return currentThread.get();
}

#if !defined(TARGET_WINDOWS)
vthread_id_t CThread::CurrentThreadId()
{
	return (vthread_id_t)pthread_self();
}

bool CThread::IsCurrentThread(const vthread_id_t tid)
{
	return pthread_equal(pthread_self(), (pthread_t)tid);
}
#else
vthread_id_t CThread::CurrentThreadId()
{
	return GetCurrentThreadId();
}

bool CThread::IsCurrentThread(const vthread_id_t tid)
{
	return (GetCurrentThreadId() == tid);
}
#endif

}

