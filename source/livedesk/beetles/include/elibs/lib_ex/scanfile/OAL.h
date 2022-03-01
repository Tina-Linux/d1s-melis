/*
*********************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Development Kit
*                                       ebook information manager libary
*
*                                    (c) Copyright 2010-2012, Sunny China
*                                              All Rights Reserved
*
* File    : OAL.h
* By      : Sunny
* Version : V1.00
* Date    : 2010-4-22 14:18:46
*********************************************************************************************************
*/

#ifndef     __OAL_H__
#define     __OAL_H__

//#include  <stdarg.h>
//#include  "epdk.h"
#include       <emodules/mod_defs.h>

#define         rat_atoi(str)       ((int)eLIBs_strtol(str, NULL, 10))

#if      0
extern int  MCnt;
#define  rat_malloc(size)       (MCnt++, \
                                 __inf("MALLOC: [%s][%d], time:%d\n", __FILE__, __LINE__, MCnt),\
                                 eLIBs_malloc(size))

#define  rat_realloc(p, size)   (MCnt++, \
                                 __inf("REALLOC:[%s][%d], time:%d\n", __FILE__, __LINE__, MCnt),\
                                 eLIBs_realloc(p, size))

#define  rat_free(p)            (MCnt--, \
                                 __inf("FREE:[%s][%d], time:%d\n", __FILE__, __LINE__, MCnt),\
                                 eLIBs_free(p))
#else
#define  rat_malloc(size)       eLIBs_malloc(size)
#define  rat_realloc(p, size)   eLIBs_realloc(p, size)
#define  rat_free(p)            eLIBs_free(p)
#endif  /* DEBUG */


#define  RAT_ALIGN(val, align)      (((val) + ((align) - 1)) & ~((align) - 1))

extern void     rat_sprintf(char *buffer, const char *str, ...);
extern double   rat_atof(const char *string);
extern void     rat_dtoa(double value, char *string);

#endif      /* __OAL_H__ */
