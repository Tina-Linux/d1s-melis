/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                File system module
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fsys_timer.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-3-8
* Descript: timer for file system debug.
* Update  : date                auther      ver     notes
*           2011-3-8 14:42:59   Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#ifndef     __FSYS_TIMER_H__
#define     __FSYS_TIMER_H__

#include "buffer_head.h"
#include "list.h"
#include "page_pool.h"
#include "fsys_debug.h"

struct timer_cfg_s
{
    int on;
};

typedef enum ttype_e
{
    diodevicetime = 0,
    bufdevicetime,
    memcpytime,
    getblktime,
    fatcachetime,
    scanfattime,
    oneaccesstime,
    bmaptime,
    hashtime,
    shrinkcachetime,
    partiotime,

    invalidtime = 0x80,
    alltime = 0xff,
} ttype_t;

typedef struct item_s
{
    ttype_t      type;
    __u32        value;
    __u32        start;
    const char  *info;
} titem_t;

typedef struct timers_s
{
    int             count;
    titem_t         item[12];
} timers_t;

#if     FSYS_DEBUG_ON

extern void     debug_timerclr(ttype_t target);
extern void     debug_timerstart(ttype_t target);
extern __u32    debug_timerend(ttype_t target);
extern void     debug_timerprint(char *fmt, ...);
extern void     debug_timershow(ttype_t target);
extern __bool   fs_timer_parse_option(const char *option);

#else

#define debug_timerclr(target)
#define debug_timerstart(target)
#define debug_timerend(target)
#define debug_timerprint(fmt, ...)
#define debug_timershow(target)
#define fs_timer_parse_option(option)

#endif  /* FSYS_DEBUG_ON */


#endif  /* __FSYS_TIMER_H__ */
