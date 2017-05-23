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

#if !defined(TARGET_WINDOWS)
typedef unsigned int  DWORD;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;
typedef char        CHAR;
typedef wchar_t     WCHAR;
typedef int         BOOL;
typedef int         INT;
typedef unsigned int  UINT;
// typedef int INT32;              // unused; must match Xmd.h but why bother
typedef long long     INT64;
typedef unsigned long long    UINT64;
typedef long        LONG;
typedef long long     LONGLONG;
#if defined(TARGET_DARWIN_OSX)
typedef UInt32          ULONG;
#else
typedef unsigned long   ULONG;
#endif
typedef size_t        SIZE_T;
typedef void*         PVOID;
typedef void*         LPVOID;

typedef void*       HWND;
typedef BYTE*       LPBYTE;
typedef DWORD*        LPDWORD;
typedef CONST CHAR*   LPCSTR;
typedef CONST WCHAR*    LPCWSTR;
typedef CHAR*     LPTSTR;
typedef WCHAR         *PWSTR,      *LPWSTR,    *NWPSTR;
typedef CHAR            *PSTR,       *LPSTR,     *NPSTR;
typedef LONG        *PLONG, *LPLONG;
#ifdef UNICODE
typedef LPCWSTR       LPCTSTR;
#else
typedef LPCSTR      LPCTSTR;
#endif
typedef unsigned __int64 ULONGLONG;
typedef unsigned long   ULONG_PTR;
typedef ULONG_PTR     DWORD_PTR;
typedef intptr_t (*FARPROC)(void);

#define MAXWORD   0xffff

typedef DWORD LCID;
typedef WORD* LPWORD;
typedef BOOL* LPBOOL;
typedef CHAR* LPCHAR;
typedef CHAR* PCHAR;
typedef const void* LPCVOID;
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

#if !defined(TARGET_WINDOWS)
#define EV_BREAK 64
#define EV_CTS 8
#define EV_DSR 16
#define EV_ERR 128
#define EV_EVENT1 2048
#define EV_EVENT2 4096
#define EV_PERR 512
#define EV_RING 256
#define EV_RLSD 32
#define EV_RX80FULL 1024
#define EV_RXCHAR 1
#define EV_RXFLAG 2
#define EV_TXEMPTY 4
#endif

/**
 * for file handle...
 */
#if !defined(TARGET_WINDOWS)
typedef int HANDLE;
typedef FILE FILEWRAP;
#else
typedef void FILEWRAP;
#endif

/**
 * for uart..
 */
#if !defined(TARGET_WINDOWS)
#define CBR_110		B110
#define CBR_300		B300
#define CBR_600		B600
#define CBR_1200	B1200
#define CBR_2400	B2400
#define CBR_4800	B4800
#define CBR_9600	B9600
#define CBR_14400	B14400
#define CBR_19200	B19200
#define CBR_38400	B38400
#define CBR_56000	B56000
#define CBR_57600	B57600
#define CBR_115200	B115200
#define CBR_128000	B128000
#define CBR_256000	B256000
#endif

#if !defined(TARGET_WINDOWS)
#define INVALID_HANDLE_VALUE     -1
#endif
#endif //__PLATFORM_DEFS_H__
