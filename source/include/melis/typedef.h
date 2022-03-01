/*
 * =====================================================================================
 *
 *       Filename:  typedef.h
 *
 *    Description:  root types of melis sdk. origins from compiler stdxxx.h files.
 *
 *        Version:  Melis3.0
 *         Create:  2017-11-02 10:21:07
 *       Revision:  none
 *       Compiler:  gcc version 6.3.0 (crosstool-NG crosstool-ng-1.23.0)
 *
 *         Author:  caozilong@allwinnertech.com
 *   Organization:  BU1-PSW
 *  Last Modified:  2020-07-09 19:17:36
 *
 * =====================================================================================
 */

#ifndef __MELIS_TYPEDEF__
#define __MELIS_TYPEDEF__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <limits.h>

#define CHIP_PLATFORM
#define __used                __attribute__((__used__))
#define __deprecated          __attribute__((deprecated))

#define likely(x)             __builtin_expect((long)!!(x), 1L)
#define unlikely(x)           __builtin_expect((long)!!(x), 0L)

#define CODE_UNREACHABLE      __builtin_unreachable()
#define FUNC_NORETURN         __attribute__((__noreturn__))

#ifndef __packed
#define __packed              __attribute__((__packed__))
#endif
#ifndef __aligned
#define __aligned(x)          __attribute__((__aligned__(x)))
#endif
#define __may_alias           __attribute__((__may_alias__))
#ifndef __printf_like
#define __printf_like(f, a)   __attribute__((format (printf, f, a)))
#endif

/*
 *#if (!defined(unlikely))
 *#define unlikely(EXPRESSSION) !!(EXPRESSSION)
 *#endif
 */

#define ATTRIBUTE_UNUSED __attribute__ ((__unused__))
#define ARG_UNUSED(NAME) NAME ATTRIBUTE_UNUSED

#define RT_TIMER_THREAD_PRIO   8
/*
**********************************************************************************************************************
*                                              常量定义
**********************************************************************************************************************
*/

#ifndef EBASE_TRUE
#define EBASE_TRUE      0
#endif

#ifndef EBASE_FALSE
#define EBASE_FALSE     (-1)
#endif

#ifndef EBSP_TRUE
#define EBSP_TRUE       EBASE_TRUE
#endif

#ifndef EBSP_FALSE
#define EBSP_FALSE      EBASE_FALSE
#endif



#ifndef EPDK_OK
#define EPDK_OK         0                   /* 代表成功*/
#endif

#ifndef EPDK_FAIL
#define EPDK_FAIL       (-1)                /* 代表失败*/
#endif

#ifndef EPDK_TRUE
#define EPDK_TRUE       1
#endif

#ifndef EPDK_FALSE
#define EPDK_FALSE      0
#endif

#ifndef EPDK_DISABLED
#define EPDK_DISABLED   0
#endif

#ifndef EPDK_ENABLED
#define EPDK_ENABLED    1
#endif

#ifndef EPDK_NO
#define EPDK_NO         0
#endif

#ifndef EPDK_YES
#define EPDK_YES        1
#endif

#ifndef EPDK_OFF
#define EPDK_OFF        0
#endif

#ifndef EPDK_ON
#define EPDK_ON         1
#endif

#ifndef EPDK_CLR
#define EPDK_CLR        0
#endif

#ifndef EPDK_SET
#define EPDK_SET        1
#endif

#ifndef NULL
#define NULL            (void*)(0)
#endif

#ifndef DATA_TYPE_X___u64
#define DATA_TYPE_X___u64
typedef uint64_t    __u64;
#endif

#ifndef DATA_TYPE_X___u32
#define DATA_TYPE_X___u32
typedef uint32_t    __u32;
#endif

#ifndef DATA_TYPE_X___u16
#define DATA_TYPE_X___u16
typedef uint16_t    __u16;
#endif

#ifndef DATA_TYPE_X___u8
#define DATA_TYPE_X___u8
typedef uint8_t     __u8;
#endif

#ifndef DATA_TYPE_X___s64
#define DATA_TYPE_X___s64
typedef int64_t     __s64;
#endif

#ifndef DATA_TYPE_X___s32
#define DATA_TYPE_X___s32
typedef int32_t     __s32;
#endif

#ifndef DATA_TYPE_X___s16
#define DATA_TYPE_X___s16
typedef int16_t     __s16;
#endif

#ifndef DATA_TYPE_X___s8
#define DATA_TYPE_X___s8
typedef int8_t      __s8;
#endif

#ifndef DATA_TYPE_X_u64
#define DATA_TYPE_X_u64
typedef uint64_t    u64;
#endif

#ifndef DATA_TYPE_X_u32
#define DATA_TYPE_X_u32
typedef uint32_t    u32;
#endif

#ifndef DATA_TYPE_X_u16
#define DATA_TYPE_X_u16
typedef uint16_t    u16;
#endif

#ifndef DATA_TYPE_X_u8
#define DATA_TYPE_X_u8
typedef uint8_t     u8;
#endif

#ifndef DATA_TYPE_X_s64
#define DATA_TYPE_X_s64
typedef int64_t     s64;
#endif

#ifndef DATA_TYPE_X_s32
#define DATA_TYPE_X_s32
typedef int32_t     s32;
#endif

#ifndef DATA_TYPE_X_s16
#define DATA_TYPE_X_s16
typedef int16_t     s16;
#endif

#ifndef DATA_TYPE_X_s8
#define DATA_TYPE_X_s8
typedef int8_t      s8;
#endif


#ifndef DATA_TYPE_X___hdle
#define DATA_TYPE_X___hdle
typedef void       *__hdle;
#endif

#ifndef DATA_TYPE_X___bool
#define DATA_TYPE_X___bool
typedef signed char __bool;
#endif

#ifndef DATA_TYPE_X___size
#define DATA_TYPE_X___size
typedef unsigned int __size;
#endif

#ifndef DATA_TYPE_X___fp32
#define DATA_TYPE_X___fp32
typedef float        __fp32;
#endif

#ifndef DATA_TYPE_X___wchar_t
#define DATA_TYPE_X___wchar_t
typedef uint16_t   __wchar_t;
#endif

#ifndef DATA_TYPE_X_uint
#define DATA_TYPE_X_uint
typedef unsigned int uint;
#endif

#ifdef __cplusplus
}
#endif

#endif

