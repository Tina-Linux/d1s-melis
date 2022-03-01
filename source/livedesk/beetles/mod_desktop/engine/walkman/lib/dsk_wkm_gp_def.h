/*
************************************************************************************************************************
*                                                        dsk_walkman
*
*                             Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : dsk_wkm_gp_def.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.09.29
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
* Gary.Wang       2009.09.29      1.1.0        build the file
*
************************************************************************************************************************
*/

#ifndef  __dsk_wkm_gp_def_h
#define  __dsk_wkm_gp_def_h



#ifndef SZ_1K
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
#endif  // ifndef SZ_1K




#undef  NULL
#define NULL                  0


#undef  RSVD_VALUE
#define RSVD_VALUE            0


#undef  COMPILE_TIME_ASSERT
#define COMPILE_TIME_ASSERT(expr)       typedef char assert_type[(expr) ? 1 : -1]


#undef  set_bit
#define set_bit( x, y )                 ( (x) |=  ( 1U << (y) ) )

#undef  clear_bit
#define clear_bit( x, y )               ( (x) &= ~( 1U << (y) ) )

#undef  reverse_bit
#define reverse_bit( x, y )             ( (x) ^=  ( 1U << (y) ) )

#undef  test_bit
#define test_bit( x, y )                ( (x)  &  ( 1U << (y) ) )


#undef  min
#define min( x, y )                     ( (x) < (y) ? (x) : (y) )

#undef  max
#define max( x, y )                     ( (x) > (y) ? (x) : (y) )



#undef  spcl_size_align
#define spcl_size_align( x, y )         ( ( (x) + (y) - 1 ) & ~( (y) - 1 ) )


#undef  _4_char_to_int32
#define _4_char_to_int32(a,b,c,d)       ( (a)<<24 | (b)<<16 | (c)<<8 | (d) )


#undef  MEMBER_OFFSET
#define MEMBER_OFFSET( type, mem )      ( (unsigned int) (&( ( (type *)0 )->mem ) ) )


#define ABS( x )                        ( (x) >0 ? (x) : -(x) )


#define WREG( x )                       (*(volatile unsigned int *)(x))



#endif     //  ifndef __dsk_wkm_gp_def_h


/* end of dsk_wkm_gp_def.h  */
