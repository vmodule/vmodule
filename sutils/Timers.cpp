/*
 * Copyright (C) 2005 The Android Open Source Project
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

//
// Timer functions.
//
#include <sutils/Timers.h>

#include <limits.h>
#include <sys/time.h>
#include <time.h>

#if defined(TARGET_ANDROID)
nsecs_t systemTime(int clock)
{
	static const clockid_t clocks[] = {
		CLOCK_REALTIME,
		CLOCK_MONOTONIC,
		CLOCK_PROCESS_CPUTIME_ID,
		CLOCK_THREAD_CPUTIME_ID,
		CLOCK_BOOTTIME
	};
	struct timespec t;
	t.tv_sec = t.tv_nsec = 0;
	clock_gettime(clocks[clock], &t);
	return nsecs_t(t.tv_sec)*1000000000LL + t.tv_nsec;
}
#else
nsecs_t systemTime(int /*clock*/) {
	// Clock support varies widely across hosts. Mac OS doesn't support
	// posix clocks, older glibcs don't support CLOCK_BOOTTIME and Windows
	// is windows.
	struct timeval t;
	t.tv_sec = t.tv_usec = 0;
	gettimeofday(&t, NULL);
	return nsecs_t(t.tv_sec) * 1000000000LL + nsecs_t(t.tv_usec) * 1000LL;
}
#endif

int toMillisecondTimeoutDelay(nsecs_t referenceTime, nsecs_t timeoutTime) {
	int timeoutDelayMillis;
	if (timeoutTime > referenceTime) {
		uint64_t timeoutDelay = uint64_t(timeoutTime - referenceTime);
		if (timeoutDelay > uint64_t((INT_MAX - 1) * 1000000LL)) {
			timeoutDelayMillis = -1;
		} else {
			timeoutDelayMillis = (timeoutDelay + 999999LL) / 1000000LL;
		}
	} else {
		timeoutDelayMillis = 0;
	}
	return timeoutDelayMillis;
}

struct tm* systemTimeInfo() {
	struct tm *timeinfo = NULL;
	time_t rawtime;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	return timeinfo;
}

unsigned long systemMaketime(const unsigned int year0,
		const unsigned int mon0, const unsigned int day,
		const unsigned int hour, const unsigned int min,
		const unsigned int sec) {
	unsigned int mon = mon0, year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int) (mon -= 2)) {
		mon += 12; /* Puts Feb last since it has leap day */
		year -= 1;
	}
	return ((((unsigned long) (year / 4 - year / 100 + year / 400
			+ 367 * mon / 12 + day) + year * 365 - 719499) * 24 + hour /* now have hours */
	) * 60 + min /* now have minutes */
	) * 60 + sec; /* finally seconds */
}

std::string systemDateTimeToString() {
	struct tm *timeinfo;
	time_t rawtime;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char *buf = new char[30];
	strftime(buf, 30, "%Y%m%dT%H%M%S", timeinfo);
	std::string datetime(buf);
	delete buf;
	return datetime;
}

void GetCurrentLocalTime(int &hour, int &minute, int &second) {
	time_t curTime;
	struct tm localTime;
	if (time(&curTime) != -1 && localtime_r(&curTime, &localTime) != NULL) {
		hour = localTime.tm_hour;
		minute = localTime.tm_min;
		second = localTime.tm_sec;
	} else
		hour = minute = second = 0;
}

