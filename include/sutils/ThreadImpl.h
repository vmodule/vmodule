#ifndef VMODULE_THREADIMPL_H_
#define VMODULE_THREADIMPL_H_

#include <stdint.h>
#include <sys/types.h>

#if !defined(TARGET_WINDOWS)
# include <pthread.h>
#endif

#include <sutils/ThreadDefs.h>
#ifdef __cplusplus
extern "C" {
#endif

extern int createRawThread(vthread_func_t entryFunction, void *userData,
		const char* threadName, size_t threadStackSize,
		vthread_id_t *threadId);

#ifdef __cplusplus
} // extern "C"
#endif
#endif

