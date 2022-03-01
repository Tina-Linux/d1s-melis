/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : fstime.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-15
* Descript: vfs timer support, code is extracted from linux.
* Update  : date                auther      ver     notes
*           2011-3-15 15:24:02  Sunny       1.0     Create this file.
*********************************************************************************************************
*/
#include "fs.h"
#include "fstime.h"
#include <kapi.h>
#include <sys_time.h>

/* current time zone
 * system time zone initialize as beijing of china
 * beijing : 120бу E, faster (8 * 60) minute then Greenwich
 */
const struct timezone sys_tz       =  { -480, 0};

/* current time spec */
static struct timespec xtime =  {0, 0};

#ifndef EPOCH_YEAR
#define EPOCH_YEAR 1970
#endif

#ifndef __isleap
/* Nonzero if YEAR is a leap year (every 4 years,
   except every 100th isn't, and every 400th is).  */
#define __isleap(year)  \
    ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
#endif

/* How many days come before each month (0-12).  */
static const unsigned short int __mon_yday[2][13] =
{
    /* Normal years.  */
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    /* Leap years.  */
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

#define MAX_YEAR_SECONDS        69
#define SPD                     0x15180 /*3600*24 */
#define SPY(y,l,s)              (SPD * (365*y+l)+s)
static  clock_t year_seconds[MAX_YEAR_SECONDS] =
{
    /*1970*/ SPY(0, 0, 0), SPY(1, 0, 0), SPY(2, 0, 0), SPY(3, 1, 0),
    /*1974*/ SPY(4, 1, 0), SPY(5, 1, 0), SPY(6, 1, 0), SPY(7, 2, 0),
    /*1978*/ SPY(8, 2, 0), SPY(9, 2, 0), SPY(10, 2, 0), SPY(11, 3, 0),
    /*1982*/ SPY(12, 3, 0), SPY(13, 3, 0), SPY(14, 3, 0), SPY(15, 4, 0),
    /*1986*/ SPY(16, 4, 0), SPY(17, 4, 0), SPY(18, 4, 0), SPY(19, 5, 0),
    /*1990*/ SPY(20, 5, 0), SPY(21, 5, 0), SPY(22, 5, 0), SPY(23, 6, 0),
    /*1994*/ SPY(24, 6, 0), SPY(25, 6, 0), SPY(26, 6, 0), SPY(27, 7, 0),
    /*1998*/ SPY(28, 7, 0), SPY(29, 7, 0), SPY(30, 7, 0), SPY(31, 8, 0),
    /*2002*/ SPY(32, 8, 0), SPY(33, 8, 0), SPY(34, 8, 0), SPY(35, 9, 0),
    /*2006*/ SPY(36, 9, 0), SPY(37, 9, 0), SPY(38, 9, 0), SPY(39, 10, 0),
    /*2010*/ SPY(40, 10, 0), SPY(41, 10, 0), SPY(42, 10, 0), SPY(43, 11, 0),
    /*2014*/ SPY(44, 11, 0), SPY(45, 11, 0), SPY(46, 11, 0), SPY(47, 12, 0),
    /*2018*/ SPY(48, 12, 0), SPY(49, 12, 0), SPY(50, 12, 0), SPY(51, 13, 0),
    /*2022*/ SPY(52, 13, 0), SPY(53, 13, 0), SPY(54, 13, 0), SPY(55, 14, 0),
    /*2026*/ SPY(56, 14, 0), SPY(57, 14, 0), SPY(58, 14, 0), SPY(59, 15, 0),
    /*2030*/ SPY(60, 15, 0), SPY(61, 15, 0), SPY(62, 15, 0), SPY(63, 16, 0),
    /*2034*/ SPY(64, 16, 0), SPY(65, 16, 0), SPY(66, 16, 0), SPY(67, 17, 0),
    /*2038*/ SPY(68, 17, 0)
};
#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)

int localtime_to_xtime(clock_t *dest, long *dest_usec,
                       __awos_time_t *time, __awos_date_t *date)
{
    int    yday;

    /* melis not support time zone, the time is locale time,
     * so no need to adjust time offset,
     * By Sunny at 2010-9-10 9:18:27.
     */
    if ((date->year < EPOCH_YEAR) ||
        (date->year >= EPOCH_YEAR + MAX_YEAR_SECONDS))
    {
        *dest = 0;
        *dest_usec = 0;
        return -1;
    }

    *dest = year_seconds[date->year - EPOCH_YEAR];
    yday = ((__mon_yday[__isleap(date->year)][date->month - 1]) + date->day - 1);
    *dest += (((yday * 24) + time->hour) * 60 + time->minute) * 60 + time->second;

    /* adjust time zone offset from Greenwich time */
    *dest += (sys_tz.tz_minuteswest * 60);

    //  if (use_daylight) {
    //
    //      /* daylight faster 1 hour then Greenwich time */
    //      *dest -= (1 * 60 * 60);
    //  }

    /* melis the min time precision is second */
    *dest_usec = 0;

    return 0;
}

void update_xtime(void)
{
    __awos_time_t    time;
    __awos_date_t    date;
    unsigned long usec, sec;
#if 1
    /* get system date and time */
    esTIME_GetDate(&date);
    esTIME_GetTime(&time);

    /* convert melis local time stamp to timeval */
    localtime_to_xtime(&sec, &usec, &time, &date);

    xtime.tv_sec  = sec;
    xtime.tv_nsec = usec;
#else
    struct timeval
    {
        long    tv_sec;     /* seconds */
        long    tv_usec;    /* and microseconds */
    };
    struct timeval val;
    int gettimeofday(struct timeval *tp, void *ignore);
    gettimeofday(&val, NULL);
    xtime.tv_sec = val.tv_sec;
    xtime.tv_nsec = val.tv_usec * 1000;
#endif
}

/**
 * current_seconds - Get current seconds
 *
 * Return the current seconds
 */
unsigned long current_seconds(void)
{
    /* update xtime now */
    update_xtime();

    return xtime.tv_sec;
}

/**
 * current_timespec - Get current time spec
 *
 * Return the current timespec
 */
struct timespec current_timespec(void)
{
    struct timespec now;

    /* update xtime now */
    update_xtime();

    /* melis not provide current kernel time */
    now = xtime;
    return now;
}

/**
 * timespec_trunc - Truncate timespec to a granularity
 * @t: Timespec
 * @gran: Granularity in ns.
 *
 * Truncate a timespec to a granularity. gran must be smaller than a second.
 * Always rounds down.
 *
 * This function should be only used for timestamps returned by
 * current_timespec() or CURRENT_TIME, not with do_gettimeofday() because
 * it doesn't handle the better resolution of the later.
 */
struct timespec timespec_trunc(struct timespec t, unsigned gran)
{
    /*
     * Division is pretty slow so avoid it for common cases.
     * Currently current_timespec() never returns better than
     * jiffies resolution. Exploit that. */
    //  if (gran <= jiffies_to_usecs(1) * 1000) {
    //      /* nothing */
    //  } else
    if (gran == 1000000000)
    {
        t.tv_nsec = 0;
    }
    else
    {
        t.tv_nsec -= t.tv_nsec % gran;
    }
    return t;
}

/**
 * current_fs_time - Return FS time
 * @sb: Superblock.
 *
 * Return the current time truncated to the time granularity supported by
 * the fs.
 */
struct timespec current_fs_time(struct super_block *sb)
{
    struct timespec now = current_timespec();
    return timespec_trunc(now, sb->s_time_gran);
}

struct ktimestamp *fs_timespec_to_kstamp(struct ktimestamp *dest,
        struct timespec ts)
{
    long int days, rem, y;
    const unsigned short int *ip;
    __s16 offset;

    /* get time zone offset from Greenwich time */
    offset = -sys_tz.tz_minuteswest;

    if (!dest)
    {
        return NULL;
    }

    ts.tv_sec += offset * 60;
    days = ts.tv_sec / SECS_PER_DAY;
    rem = ts.tv_sec % SECS_PER_DAY;
    dest->hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    dest->minute = rem / 60;
    dest->second = rem % 60;
    y = 1970;

#define DIV(a,b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (__isleap(y) ? 366 : 365))
    {
        long int yg = y + days / 365 - (days % 365 < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -= ((yg - y) * 365
                 + LEAPS_THRU_END_OF(yg - 1)
                 - LEAPS_THRU_END_OF(y - 1));
        y = yg;
    }
    dest->year = y;
    ip = __mon_yday[__isleap(y)];
    for (y = 11; days < (long int)ip[y]; --y)
    {
        continue;
    }
    days -= ip[y];
    dest->month = y + 1;
    dest->day = days + 1;
    dest->milliseconds = ts.tv_nsec / 1000000;
    return dest;
}
