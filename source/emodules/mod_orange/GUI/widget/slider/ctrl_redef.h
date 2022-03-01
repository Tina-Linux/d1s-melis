/*
************************************************************************************************************************
*                                                     ctrl_redef
*
*                             Copyright(C), 2006-2009, SoftWinners Microelectronic Co., Ltd.
*                                                  All Rights Reserved
*
* File Name : ctrl_redef.h
*
* Author : Gary.Wang
*
* Version : 1.1.0
*
* Date : 2009.08.07
*
* Description :
*
* Others : None at present.
*
*
* History :
*
*  <Author>        <time>       <version>      <description>
*
* Gary.Wang      2009.08.07       1.1.0        build the file
*
************************************************************************************************************************
*/
#ifndef  __ctrl_redef_h
#define  __ctrl_redef_h


#define g_malloc( size )      esMEMS_Malloc( 0, size )
#define g_free( p )           esMEMS_Mfree( 0, p )

#define g_memcpy              eLIBs_memcpy
#define g_memset              eLIBs_memset
#define g_memmove             eLIBs_memmove

#define g_strcmp              eLIBs_strcmp
#define g_strncmp             eLIBs_strncmp
#define g_strcat              eLIBs_strcat
#define g_strncat             eLIBs_strncat
#define g_strcpy              eLIBs_strcpy
#define g_strncpy             eLIBs_strncpy
#define g_strlen              eLIBs_strlen
#define g_toupper             eLIBs_toupper
#define g_strtol              eLIBs_strtol


#undef ABS
#define ABS( x )                        ( (x) >0 ? (x) : -(x) )

#undef  spcl_size_align
#define spcl_size_align( x, y )         ( ( (x) + (y) - 1 ) & ~( (y) - 1 ) )




#endif     //  ifndef __ctrl_redef_h

/* end of ctrl_redef.h  */
