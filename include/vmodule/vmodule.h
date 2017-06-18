/*
 * vmodule.h
 *
 *  Created on: May 21, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_H_
#define VMODULE_H_
#include <event.h>
#include <sutils/SingletonHelper.h>
#include <sutils/Timers.h>
#ifdef __cplusplus
extern "C" {
#endif

#if defined(TARGET_WINDOWS) && defined(WIN32)
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VMODULE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VMODULE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VMODULE_EXPORTS
#define VMODULE_API __declspec(dllexport)
#else
#define VMODULE_API __declspec(dllimport)
#endif
#else
#define VMODULE_API
#endif

#define ITEM_UPDATE_INTERVAL 60
// This class is exported from the vmodule.dll
class VMODULE_API Vmodule {
public:
	Vmodule();
	~Vmodule();
	// TODO: add your methods here.
	event_base *getMainEventBase();
	static void Initialize();
	static void clock_handler(const int fd, 
		const short which, void *arg);
	time_t getProcessStarted(){return process_started;}
protected:
	static event_base *main_base;
	static event clockevent;		
	volatile nsecs_t current_time;
	time_t process_started;
	
};

/*extern*/ VMODULE_API void vmodue_init();

VMODULE_GLOBAL_REF(Vmodule, g_vmodule);
#define g_vmodule VMODULE_GLOBAL_USE(Vmodule)

#ifdef __cplusplus
} // extern "C"
#endif
#endif /* VMODULE_H_ */

