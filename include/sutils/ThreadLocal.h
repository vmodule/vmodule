#pragma once
#ifndef TARGET_WINDOWS
#include <pthread.h>
#else
#include <windows.h>
#endif
#include <errno.h>
#include <sutils/ThreadDefs.h>
#include <sutils/Exception.h>
namespace vmodule {
#ifndef TARGET_WINDOWS
/**
 * A thin wrapper around pthreads thread specific storage
 * functionality.
 */
template<typename T>
class ThreadLocal {
	pthread_key_t key;
public:
	inline ThreadLocal() :
			key(0) {
		pthread_key_create(&key, NULL);
	}

	inline ~ThreadLocal() {
		pthread_key_delete(key);
	}

	inline void set(T* val) {
		pthread_setspecific(key, (void*) val);
	}

	inline T* get() {
		return (T*) pthread_getspecific(key);
	}
};
#else
/**
 * A thin wrapper around windows thread specific storage
 * functionality.
 */
template <typename T>
class ThreadLocal
{
	DWORD key;
public:
	inline ThreadLocal()
	{
		if ((key = TlsAlloc()) == TLS_OUT_OF_INDEXES)
		throw UncheckedException("Ran out of Windows TLS Indexes. Windows Error Code %d",(int)errno);
	}

	inline ~ThreadLocal()
	{
		if (!TlsFree(key))
		throw UncheckedException("Failed to free Tls %d, Windows Error Code %d",(int)key, (int)errno);
	}

	inline void set(T* val)
	{
		if (!TlsSetValue(key,(LPVOID)val))
		throw UncheckedException("Failed to set Tls %d, Windows Error Code %d",(int)key, (int)errno);
	}

	inline T* get() {return (T*)TlsGetValue(key);}
};
#endif
}
