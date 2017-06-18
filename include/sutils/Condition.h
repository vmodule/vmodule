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

#ifndef VMODULE_SUTILS_CONDITION_H
#define VMODULE_SUTILS_CONDITION_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#if !defined(TARGET_WINDOWS)
# include <pthread.h>
#endif

#include <sutils/Errors.h>
#include <sutils/Mutex.h>
#include <sutils/Timers.h>

// ---------------------------------------------------------------------------
namespace vmodule {
// ---------------------------------------------------------------------------

/*
 * Condition variable class.  The implementation is system-dependent.
 *
 * Condition variables are paired up with mutexes.  Lock the mutex,
 * call wait(), then either re-wait() if things aren't quite what you want,
 * or unlock the mutex and continue.  All threads calling wait() must
 * use the same mutex for a given Condition.
 */
class Condition {
public:
	enum {
		PRIVATE = 0, SHARED = 1
	};

	enum WakeUpType {
		WAKE_UP_ONE = 0, WAKE_UP_ALL = 1
	};

	Condition();
	Condition(int type);
	~Condition();
	// Wait on the condition variable.  Lock the mutex before calling.
	status_t wait(Mutex& mutex);
	// same with relative timeout
	status_t waitRelative(Mutex& mutex, nsecs_t reltime);
	// Signal the condition variable, allowing exactly one thread to continue.
	void signal();
	// Signal the condition variable, allowing one or all threads to continue.
	void signal(WakeUpType type) {
		if (type == WAKE_UP_ONE) {
			signal();
		} else {
			broadcast();
		}
	}
	// Signal the condition variable, allowing all threads to continue.
	void broadcast();

private:
#if !defined(TARGET_WINDOWS)
	pthread_cond_t mCond;
#else
	void* mState;
#endif
};
}
// namespace vmodule
// ---------------------------------------------------------------------------

#endif // VMODULE_CONDITON_H
