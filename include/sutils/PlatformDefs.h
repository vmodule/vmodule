#ifndef __PLATFORM_DEFS_H__
#define __PLATFORM_DEFS_H__


#include <stdint.h>

#ifndef PRId64
#ifdef TARGET_WINDOWS
#define PRId64 "I64d"
#else
#if __WORDSIZE == 64
#define PRId64 "ld"
#else
#define PRId64 "lld"
#endif
#endif
#endif

#ifndef PRIu64
#ifdef TARGET_WINDOWS
#define PRIu64 "I64u"
#else
#if __WORDSIZE == 64
#define PRIu64 "lu"
#else
#define PRIu64 "llu"
#endif
#endif
#endif

#ifndef PRIx64
#ifdef TARGET_WINDOWS
#define PRIx64 "I64x"
#else
#if __WORDSIZE == 64
#define PRIx64 "lx"
#else
#define PRIx64 "llx"
#endif
#endif
#endif

#ifndef PRIdS
#define PRIdS "zd"
#endif

#ifndef PRIuS
#define PRIuS "zu"
#endif


#endif //__PLATFORM_DEFS_H__
