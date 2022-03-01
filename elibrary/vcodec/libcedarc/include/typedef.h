
/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : typedef.h
* Description :
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/


#ifndef TYPEDEF_H
#define TYPEDEF_H
#include <stdint.h>

#ifndef DATA_TYPE_X_u8
#define DATA_TYPE_X_u8
typedef uint8_t u8;
#endif

#ifndef DATA_TYPE_X_u16
#define DATA_TYPE_X_u16
typedef uint16_t u16;
#endif

#ifndef DATA_TYPE_X_u32
#define DATA_TYPE_X_u32
typedef uint32_t u32;
#endif

#ifndef DATA_TYPE_X_u64
#define DATA_TYPE_X_u64
#ifdef COMPILER_ARMCC
typedef unsigned __int64 u64;
#else
typedef uint64_t u64;
#endif
#endif

#ifndef DATA_TYPE_X_s8
#define DATA_TYPE_X_s8
typedef int8_t s8;
#endif

#ifndef DATA_TYPE_X_s16
#define DATA_TYPE_X_s16
typedef int16_t s16;
#endif

#ifndef DATA_TYPE_X_s32
#define DATA_TYPE_X_s32
typedef int32_t s32;
#endif

#ifndef DATA_TYPE_X_s64
#define DATA_TYPE_X_s64
#ifdef COMPILER_ARMCC
typedef signed __int64 s64;
#else
typedef int64_t s64;
#endif
#endif

#ifndef DATA_TYPE_X___u8
#define DATA_TYPE_X___u8
typedef uint8_t     __u8;
#endif

#ifndef DATA_TYPE_X___u16
#define DATA_TYPE_X___u16
typedef uint16_t    __u16;
#endif

#ifndef DATA_TYPE_X___u32
#define DATA_TYPE_X___u32
typedef uint32_t    __u32;
#endif

#ifndef DATA_TYPE_X___u64
#define DATA_TYPE_X___u64
typedef uint64_t    __u64;
#endif

#ifndef DATA_TYPE_X___hdle
#define DATA_TYPE_X___hdle
typedef void       *__hdle;
#endif

typedef uintptr_t size_addr;

#endif

