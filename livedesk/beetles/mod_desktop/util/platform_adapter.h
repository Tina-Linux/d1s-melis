/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
