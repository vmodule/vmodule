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

// CreateFile defines
#if !defined(TARGET_WINDOWS)
#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000
#endif

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5

#define FILE_ATTRIBUTE_NORMAL 0x00000080
#define FILE_ATTRIBUTE_READONLY 0x00000001
#define FILE_ATTRIBUTE_HIDDEN 0x00000002
#define FILE_ATTRIBUTE_SYSTEM 0x00000004
#define FILE_ATTRIBUTE_DIRECTORY  0x00000010

#if !defined(TARGET_WINDOWS)
#define FILE_READ_DATA   ( 0x0001 )
#define FILE_WRITE_DATA  ( 0x0002 )
#define FILE_APPEND_DATA ( 0x0004 )

#define GENERIC_READ  FILE_READ_DATA
#define GENERIC_WRITE FILE_WRITE_DATA
#endif

#define FILE_SHARE_READ                  0x00000001
#define FILE_SHARE_WRITE                 0x00000002
#define FILE_SHARE_DELETE                0x00000004


#endif //__PLATFORM_DEFS_H__
