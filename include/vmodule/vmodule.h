/*
 * vmodule.h
 *
 *  Created on: May 21, 2017
 *      Author: jeffrey
 */

#ifndef VMODULE_H_
#define VMODULE_H_

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

/*extern*/ VMODULE_API void vmodue_init();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* VMODULE_H_ */



