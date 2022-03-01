/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : platform_adapter.h
* By        : Andy.zhang
* Func      : platform_adapter
* Version   : v1.0
* ============================================================================================================
* 2010-9-3 9:46:25  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#ifndef __PLATFORM_ADAPTER_H__
#define __PLATFORM_ADAPTER_H__

#include <log.h>
#include <elibs_string.h>
#include <kapi.h>
#include "dfs_posix.h"
#define inline              inline


#ifdef __cplusplus
#define __BEGIN_DECLS__     extern "C"{
#define __END_DECLS__       }
#else
#define __BEGIN_DECLS__
#define __END_DECLS__
#endif

/* fail print*/
#define return_if_fail(p)           if(!(p)) { __msg(" "#p" failed \n"); return;}
#define return_val_if_fail(p, val)  if(!(p)) {__log(" "#p" failed \n"); return (val);}

/* libc redefine  */
#if 0/*langaojie temp test*/
#define memcpy              eLIBs_memcpy
#define memset              eLIBs_memset
#define memmove             eLIBs_memmove
#define strcmp              eLIBs_strcmp
#define strncmp             eLIBs_strncmp
#define strcat              eLIBs_strcat
#define strncat             eLIBs_strncat
#define strcpy              eLIBs_strcpy
#define strncpy             eLIBs_strncpy
#define strlen              eLIBs_strlen
#define strstr(s1,s2)       eLIBs_strstr((char *)s1, s2)
#define strchr(s1,s2)       eLIBs_strchr((char *)s1, s2)
#define strtol              eLIBs_strtol
#define toupper             eLIBs_toupper
#endif
#define isalpha(c)          ((('a' <= (c)) && ((c) <= 'z')) || (('A' <= (c)) && ((c) <= 'Z')))
#define isprint(c)          (' ' <= (c) && (c) <= '~')
#define isspace(c)          (((((unsigned int)(c)) - '\t') < 5U) || ((c) == ' '))


#endif  /*__PLATFORM_ADAPTER_H__*/
