/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VMODULE_MUTEX_H
#define VMODULE_MUTEX_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#if !defined(TARGET_WINDOWS)
# include <pthread.h>
#endif

#include <sutils/Errors.h>
#include <sutils/Timers.h>

// ---------------------------------------------------------------------------
namespace vmodule {
// ---------------------------------------------------------------------------

class Condition;

/*
 * NOTE: This class is for code that builds on Win32.  Its usage is
 * deprecated for code which doesn't build for Win32.  New code which
 * doesn't build for Win32 should use std::mutex and std::lock_guard instead.
 *
 * Simple mutex class.  The implementation is system-dependent.
 *
 * The mutex must be unlocked by the thread that locked it.  They are not
 * recursive, i.e. the same thread can't lock it multiple times.
 */
class Mutex {
public:
	enum {
		PRIVATE = 0, SHARED = 1
	};

	Mutex();

	Mutex(const char* name);

	Mutex(int type, const char* name = NULL);

	~Mutex();

	// lock or unlock the mutex
	status_t lock();

	void unlock();

	// lock if possible; returns 0 on success, error otherwise
	status_t tryLock();

#if defined(TARGET_ANDROID)
	// Lock the mutex, but don't wait longer than timeoutNs (relative time).
	// Returns 0 on success, TIMED_OUT for failure due to timeout expiration.
	//
	// OSX doesn't have pthread_mutex_timedlock() or equivalent. To keep
	// capabilities consistent across host OSes, this method is only available
	// when building Android binaries.
	//
	// FIXME?: pthread_mutex_timedlock is based on CLOCK_REALTIME,
	// which is subject to NTP adjustments, and includes time during suspend,
	// so a timeout may occur even though no processes could run.
	// Not holding a partial wakelock may lead to a system suspend.
	status_t timedLock(nsecs_t timeoutNs);
#endif

	// Manages the mutex automatically. It'll be locked when Autolock is
	// constructed and released when Autolock goes out of scope.
	class Autolock {
	public:
		inline Autolock(Mutex& mutex) :
				mLock(mutex) {
			mLock.lock();
		}

		inline Autolock(Mutex* mutex) :
				mLock(*mutex) {
			mLock.lock();
		}

		inline ~Autolock() {
			mLock.unlock();
		}

	private:
		Mutex& mLock;
	};

private:
	friend class Condition;

	// A mutex cannot be copied
	Mutex(const Mutex&);
	Mutex& operator =(const Mutex&);

#if !defined(TARGET_WINDOWS)
	pthread_mutex_t mMutex;
#else
	void _init();
	void* mState;
#endif
};

/*
 * Automatic mutex.  Declare one of these at the top of a function.
 * When the function returns, it will go out of scope, and release the
 * mutex.
 */

typedef Mutex::Autolock AutoMutex;

// ---------------------------------------------------------------------------
}
// namespace vmodule
// ---------------------------------------------------------------------------

#endif // VMODULE_MUTEX_H
