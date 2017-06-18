#ifndef VMODULE_SUTILS_SYS_TYPES_H
#define VMODULE_SUTILS_SYS_TYPES_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif
#if defined(TARGET_WINDOWS)
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned long long u_int64_t;
typedef unsigned long id_t;
#endif
#ifdef __cplusplus
}
#endif

#endif // VMODULE_SYS_TYPES_H
