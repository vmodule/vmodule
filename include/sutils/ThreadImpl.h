#ifndef VMODULE_THREADIMPL_H_
#define VMODULE_THREADIMPL_H_

#include <stdint.h>
#include <sys/types.h>

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

#include <sutils/ThreadDefs.h>
#include <sutils/PlatformDefs.h>


#ifdef __cplusplus
extern "C" {
#endif

extern int createRawThread(vthread_func_t entryFunction, void *userData,
		const char* threadName, size_t threadStackSize,
		vthread_id_t *threadId);

bool IsCurrentThread(const vthread_id_t tid);

#if !defined(TARGET_WINDOWS)
vthread_id_t GetCurrentThreadId();
#endif

#ifdef __cplusplus
} // extern "C"
#endif
#endif

