/***************************************************************************/
/*                                                                         */
/*                                                                         */
/*    ANSI-specific library and header configuration file (specification   */
/*    only).                                                               */
/*                                                                         */
/*  Copyright 2002, 2003, 2004, 2005, 2006, 2007 by                        */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


/*************************************************************************/
/*                                                                       */
/* This file is used to group all #includes to the ANSI C library that   */
/* FreeType normally requires.  It also defines macros to rename the     */
/* standard functions within the FreeType source code.                   */
/*                                                                       */
/* Load a file which defines __FTSTDLIB_H__ before this one to override  */
/* it.                                                                   */
/*                                                                       */
/*************************************************************************/


#ifndef __FTSTDLIB_H__
#define __FTSTDLIB_H__

#include "head_plus/FontEngine_cfg.h"
#include <elibs_stdio.h>
#include <elibs_stdlib.h>
#include <typedef.h>

long int atol(char *s);
#define labs(x)                          ( (x) >0 ? (x) : -(x) )

/*---------------------------------------- EPDK_WIN32 ----------------------------------------*/
#if defined( EPDK_WIN32 )

#include <stddef.h>

#define ft_ptrdiff_t  ptrdiff_t


/**********************************************************************/
/*                                                                    */
/*                           integer limits                           */
/*                                                                    */
/* UINT_MAX and ULONG_MAX are used to automatically compute the size  */
/* of `int' and `long' in bytes at compile-time.  So far, this works  */
/* for all platforms the library has been tested on.                  */
/*                                                                    */
/* Note that on the extremely rare platforms that do not provide      */
/* integer types that are _exactly_ 16 and 32 bits wide (e.g. some    */
/* old Crays where `int' is 36 bits), we do not make any guarantee    */
/* about the correct behaviour of FT2 with all fonts.                 */
/*                                                                    */
/* In these case, `ftconfig.h' will refuse to compile anyway with a   */
/* message like `couldn't find 32-bit type' or something similar.     */
/*                                                                    */
/* IMPORTANT NOTE: We do not define aliases for heap management and   */
/*                 i/o routines (i.e. malloc/free/fopen/fread/...)    */
/*                 since these functions should all be encapsulated   */
/*                 by platform-specific implementations of            */
/*                 `ftsystem.c'.                                      */
/*                                                                    */
/**********************************************************************/


#include <limits.h>

#define FT_CHAR_BIT   CHAR_BIT
#define FT_INT_MAX    INT_MAX
#define FT_UINT_MAX   UINT_MAX
#define FT_ULONG_MAX  ULONG_MAX


/**********************************************************************/
/*                                                                    */
/*                 character and string processing                    */
/*                                                                    */
/**********************************************************************/


#include <string.h>

#define ft_memchr   memchr
#define ft_memcmp   memcmp
#define ft_memcpy   memcpy
#define ft_memmove  memmove
#define ft_memset   memset
#define ft_strcat   strcat
#define ft_strcmp   strcmp
#define ft_strcpy   strcpy
#define ft_strlen   strlen
#define ft_strncmp  strncmp
#define ft_strncpy  strncpy
#define ft_strrchr  strrchr
#define ft_strstr   strstr


/**********************************************************************/
/*                                                                    */
/*                           file handling                            */
/*                                                                    */
/**********************************************************************/

#include <module/mod_slib.h>


#define FT_FILE     FILE
#define ft_fclose   fclose
#define ft_fopen    fopen
#define ft_fread    fread
#define ft_fwrite   fwrite
#define ft_fseek    fseek
#define ft_ftell    ftell
#define ft_sprintf  eLIBs_sprintf


/**********************************************************************/
/*                                                                    */
/*                             sorting                                */
/*                                                                    */
/**********************************************************************/



#define ft_qsort  qsort

#define ft_exit   exit    /* only used to exit from unhandled exceptions */


/**********************************************************************/
/*                                                                    */
/*                        memory allocation                           */
/*                                                                    */
/**********************************************************************/


#define ft_scalloc   calloc
#define ft_sfree     free
#define ft_smalloc   malloc
#define ft_srealloc  realloc


/**********************************************************************/
/*                                                                    */
/*                          miscellaneous                             */
/*                                                                    */
/**********************************************************************/


#define ft_atol   eLIBs_strtol
#define ft_labs   labs


/**********************************************************************/
/*                                                                    */
/*                         execution control                          */
/*                                                                    */
/**********************************************************************/


#include <setjmp.h>

#define ft_jmp_buf     jmp_buf  /* note: this cannot be a typedef since */
/*       jmp_buf is defined as a macro  */
/*       on certain platforms           */

#define ft_longjmp     longjmp
#define ft_setjmp( b ) setjmp( *(jmp_buf*) &(b) )    /* same thing here */


/* the following is only used for debugging purposes, i.e., if */
/* FT_DEBUG_LEVEL_ERROR or FT_DEBUG_LEVEL_TRACE are defined    */

#include <stdarg.h>


//  /**********************************************************************/
//  /*                                                                    */
//  /*                          message printf                            */
//  /*                                                                    */
//  /**********************************************************************/
//#ifndef NDEBUG
//  #define msg               printf
//#else
//  #define msg(x)
//#endif



#define __PACKED



/*----------------------------------------- EPDK_AC320 ----------------------------------------*/
#elif defined( EPDK_AC320 )



#include <stddef.h>

#define ft_ptrdiff_t  ptrdiff_t


/**********************************************************************/
/*                                                                    */
/*                           integer limits                           */
/*                                                                    */
/* UINT_MAX and ULONG_MAX are used to automatically compute the size  */
/* of `int' and `long' in bytes at compile-time.  So far, this works  */
/* for all platforms the library has been tested on.                  */
/*                                                                    */
/* Note that on the extremely rare platforms that do not provide      */
/* integer types that are _exactly_ 16 and 32 bits wide (e.g. some    */
/* old Crays where `int' is 36 bits), we do not make any guarantee    */
/* about the correct behaviour of FT2 with all fonts.                 */
/*                                                                    */
/* In these case, `ftconfig.h' will refuse to compile anyway with a   */
/* message like `couldn't find 32-bit type' or something similar.     */
/*                                                                    */
/* IMPORTANT NOTE: We do not define aliases for heap management and   */
/*                 i/o routines (i.e. malloc/free/fopen/fread/...)    */
/*                 since these functions should all be encapsulated   */
/*                 by platform-specific implementations of            */
/*                 `ftsystem.c'.                                      */
/*                                                                    */
/**********************************************************************/


#include <limits.h>

#define FT_CHAR_BIT   CHAR_BIT
#define FT_INT_MAX    INT_MAX
#define FT_UINT_MAX   UINT_MAX
#define FT_ULONG_MAX  ULONG_MAX


/**********************************************************************/
/*                                                                    */
/*                 character and string processing                    */
/*                                                                    */
/**********************************************************************/


#include <string.h>
#include <kapi.h>

#define ft_memchr   memchr
#define ft_memcmp   memcmp
#define ft_memcpy   memcpy
#define ft_memmove  memmove
#define ft_memset   memset
#define ft_strcat   strcat
#define ft_strcmp   strcmp
#define ft_strcpy   strcpy
#define ft_strlen   strlen
#define ft_strncmp  strncmp
#define ft_strncpy  strncpy
#define ft_strrchr  strrchr
#define ft_strstr   strstr


/**********************************************************************/
/*                                                                    */
/*                           file handling                            */
/*                                                                    */
/**********************************************************************/



#define size_t      unsigned int

#define FT_FILE     ES_FILE

#define ft_fclose   eLIBs_fclose
#define ft_fopen    eLIBs_fopen
#define ft_fread    eLIBs_fread
#define ft_fwrite   eLIBs_fwrite
#define ft_fseek    eLIBs_fseek
#define ft_ftell    eLIBs_ftell


#define ft_sprintf  eLIBs_sprintf



/**********************************************************************/
/*                                                                    */
/*                             sorting                                */
/*                                                                    */
/**********************************************************************/


void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
#define ft_qsort  qsort

//#define ft_exit   exit    /* only used to exit from unhandled exceptions */


/**********************************************************************/
/*                                                                    */
/*                        memory allocation                           */
/*                                                                    */
/**********************************************************************/


#define ft_smalloc( size )    esMEMS_Malloc( 0, size )
#define ft_sfree( p )         esMEMS_Mfree( 0, p )
#define ft_scalloc( x, y )    ft_smalloc( x*y )
extern void *ft_srealloc(void *ptr, size_t size);


/**********************************************************************/
/*                                                                    */
/*                          miscellaneous                             */
/*                                                                    */
/**********************************************************************/


#define ft_atol   atol
#define ft_labs   labs


/**********************************************************************/
/*                                                                    */
/*                         execution control                          */
/*                                                                    */
/**********************************************************************/


#include <setjmp.h>

#define ft_jmp_buf     jmp_buf  /* note: this cannot be a typedef since */
/*       jmp_buf is defined as a macro  */
/*       on certain platforms           */

#define ft_longjmp     longjmp
#define ft_setjmp( b ) setjmp( *(jmp_buf*) &(b) )    /* same thing here */


/* the following is only used for debugging purposes, i.e., if */
/* FT_DEBUG_LEVEL_ERROR or FT_DEBUG_LEVEL_TRACE are defined    */

#include <stdarg.h>


//  /**********************************************************************/
//  /*                                                                    */
//  /*                          message printf                            */
//  /*                                                                    */
//  /**********************************************************************/
//#ifndef NDEBUG
//  #define msg(...)          __inf(__VA_ARGS__)
//#else
//  #define msg(...)
//#endif




#define __PACKED              __packed



/*----------------------------------------- platform end -----------------------------------------*/
#else
#error PLATFORM must be selected.
#endif




#endif /* __FTSTDLIB_H__ */


/* END */
