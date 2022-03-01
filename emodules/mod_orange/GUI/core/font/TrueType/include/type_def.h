/*
************************************************************************************************************************
*                                                         eGON
*                                         the Embedded GO-ON Bootloader System
*
*                             Copyright(C), 2006-2008, Allwinner Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : type_def.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2007.11.26
*
* Description : This file defines some basic data types and general-purpose macros.
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang       2007.11.26      1.1.0        build the file
*
************************************************************************************************************************
*/

#ifndef  __type_def_h
#define  __type_def_h



#undef int8
#undef int16
#undef int32
#undef uint8
#undef uint16
#undef uint32

typedef signed char           int8;
typedef signed short          int16;
typedef signed int            int32;
typedef unsigned char         uint8;
typedef unsigned short        uint16;
typedef unsigned int          uint32;




#undef s8
#undef s16
#undef s32
#undef u8
#undef u16
#undef u32
/*
typedef signed char           s8;
typedef signed short          s16;
typedef signed int            s32;
typedef unsigned char         u8;
typedef unsigned short        u16;
typedef unsigned int          u32;
*/



#undef __s8
#undef __s16
#undef __s32
#undef __u8
#undef __u16
#undef __u32
/*
typedef signed char           __s8;
typedef signed short          __s16;
typedef signed int            __s32;
typedef unsigned char         __u8;
typedef unsigned short        __u16;
typedef unsigned int          __u32;

*/


#define SZ_512       0x00000200U
#define SZ_1K        0x00000400U
#define SZ_2K        0x00000800U
#define SZ_4K        0x00001000U
#define SZ_8K        0x00002000U
#define SZ_16K       0x00004000U
#define SZ_32K       0x00008000U
#define SZ_64K       0x00010000U
#define SZ_128K      0x00020000U
#define SZ_256K      0x00040000U
#define SZ_512K      0x00080000U
#define SZ_1M        0x00100000U
#define SZ_2M        0x00200000U
#define SZ_4M        0x00400000U
#define SZ_8M        0x00800000U
#define SZ_16M       0x01000000U
#define SZ_32M       0x02000000U
#define SZ_64M       0x04000000U
#define SZ_128M      0x08000000U
#define SZ_256M      0x10000000U
#define SZ_512M      0x20000000U
#define SZ_1G        0x40000000U
#define SZ_2G        0x80000000U

#define SZ_4G        0x0100000000ULL
#define SZ_8G        0x0200000000ULL
#define SZ_16G       0x0400000000ULL
#define SZ_32G       0x0800000000ULL
#define SZ_64G       0x1000000000ULL





#define NULL                  0


#define RSVD_VALUE            0


#define COMPILE_TIME_ASSERT(expr)       typedef char assert_type[(expr) ? 1 : -1]


#define set_bit( x, y )      ( (x) |=  ( 1U << (y) ) )
#define clear_bit( x, y )    ( (x) &= ~( 1U << (y) ) )
#define reverse_bit( x, y )  ( (x) ^=  ( 1U << (y) ) )
#define test_bit( x, y )     ( (x)  &  ( 1U << (y) ) )


#define min( x, y )          ( (x) < (y) ? (x) : (y) )
#define max( x, y )          ( (x) > (y) ? (x) : (y) )





#endif     //  ifndef __type_def_h


/* end of type_def.h  */
