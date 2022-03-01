/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fstime.h
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: vfs timer support, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:24:02  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#ifndef __FS_TIME_H__
#define __FS_TIME_H__

#include "fstypedef.h"      /* just for type : clock_t */

/* Parameters used to convert the timespec values: */

struct timezone
{
    int tz_minuteswest;   /* minutes west of Greenwich */
    int tz_dsttime;       /* type of dst correction */
};

unsigned long   current_seconds(void);   /* get current seconds */
struct timespec current_timespec(void);  /* get current timespec */

#define CURRENT_TIME        (current_timespec())
#define CURRENT_TIME_SEC    ((struct timespec) { current_seconds(), 0 })

extern struct timespec timespec_trunc(struct timespec t, unsigned gran);

#endif  /* __FS_TIME_H__ */
