/*
*********************************************************************************************************
*                                                    MELIS
*                                    the Easy Portable/Player Develop Kits
*                                                  File System
*
*                                    (c) Copyright 2011-2014, Sunny China
*                                             All Rights Reserved
*
* File    : exfat_utils.c
* By      : Sunny
* Version : v1.0
* Date    : 2011-1-16
* Descript: exfat file system.
* Update  : date                auther      ver     notes
*           2011-3-16 14:28:54  Sunny       1.0     Create this file.
*********************************************************************************************************
*/

#include <stdarg.h>
#include "exfatfs.h"
#include "errno.h"
#include "endians.h"
#include "fsys_libs.h"
#include "nls.h"
#include "fsys_debug.h"

extern const struct timezone sys_tz;

#define C_32(s, p)  do {                        \
        C(s, (p)[ 0]); C(s, (p)[ 1]); C(s, (p)[ 2]); C(s, (p)[ 3]); \
        C(s, (p)[ 4]); C(s, (p)[ 5]); C(s, (p)[ 6]); C(s, (p)[ 7]); \
        C(s, (p)[ 8]); C(s, (p)[ 9]); C(s, (p)[10]); C(s, (p)[11]); \
        C(s, (p)[12]); C(s, (p)[13]); C(s, (p)[14]); C(s, (p)[15]); \
        C(s, (p)[16]); C(s, (p)[17]); C(s, (p)[18]); C(s, (p)[19]); \
        C(s, (p)[20]); C(s, (p)[21]); C(s, (p)[22]); C(s, (p)[23]); \
        C(s, (p)[24]); C(s, (p)[25]); C(s, (p)[26]); C(s, (p)[27]); \
        C(s, (p)[28]); C(s, (p)[29]); C(s, (p)[30]); C(s, (p)[31]); \
    } while (0)

u32 exfat_checksum32(u32 sum, const void *__buf, int size)
{
#define C(s, v)     do { s = __exfat_checksum32(s, v); } while (0)
    const u8 *buf = __buf;
    int i;

    for (i = 0; (i + 511) < size; i += 512)
    {
        C_32(sum, &buf[i + 32 *  0]);
        C_32(sum, &buf[i + 32 *  1]);
        C_32(sum, &buf[i + 32 *  2]);
        C_32(sum, &buf[i + 32 *  3]);
        C_32(sum, &buf[i + 32 *  4]);
        C_32(sum, &buf[i + 32 *  5]);
        C_32(sum, &buf[i + 32 *  6]);
        C_32(sum, &buf[i + 32 *  7]);
        C_32(sum, &buf[i + 32 *  8]);
        C_32(sum, &buf[i + 32 *  9]);
        C_32(sum, &buf[i + 32 * 10]);
        C_32(sum, &buf[i + 32 * 11]);
        C_32(sum, &buf[i + 32 * 12]);
        C_32(sum, &buf[i + 32 * 13]);
        C_32(sum, &buf[i + 32 * 14]);
        C_32(sum, &buf[i + 32 * 15]);
    }
    for (; (i + 31) < size; i += 32)
    {
        C_32(sum, &buf[i]);
    }
    for (; i < size; i++)
    {
        C(sum, buf[i]);
    }

    return sum;
#undef C
}

u16 exfat_checksum16(u16 sum, const void *__buf, int size)
{
#define C(s, v)     do { s = __exfat_checksum16(s, v); } while (0)
    const u8 *buf = __buf;
    int i;

    for (i = 0; (i + 31) < size; i += 32)
    {
        C_32(sum, &buf[i]);
    }
    for (; i < size; i++)
    {
        C(sum, buf[i]);
    }

    return sum;
#undef C
}

int exfat_conv_c2u(struct nls_table *nls, const unsigned char **in, unsigned int *in_left,
                   __wchar_t **uni, unsigned int *uni_left)
{
    int len;
    while (*in_left)
    {
        if (!*uni_left)
        {
            return -ENAMETOOLONG;
        }

        len = nls->char2uni(*in, *in_left, *uni);
        if (len < 0)
        {
            return len;
        }

        *in += len;
        *in_left -= len;
        (*uni)++;
        (*uni_left)--;
    }
    return 0;
}

/*
 * The epoch of exFAT timestamp is 1980.
 *     :  bits :     value
 * date:  0 -  4: day   (1 -  31)
 * date:  5 -  8: month (1 -  12)
 * date:  9 - 15: year  (0 - 127) from 1980
 * time:  0 -  4: sec   (0 -  29) 2sec counts
 * time:  5 - 10: min   (0 -  59)
 * time: 11 - 15: hour  (0 -  23)
 */
#define SECS_PER_MIN    60
#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)
#define UNIX_SECS_1980  315532800L
#if BITS_PER_LONG == 64
#define UNIX_SECS_2108  4354819200L
#endif
/* days between 1.1.70 and 1.1.80 (2 leap days) */
#define DAYS_DELTA  (365 * 10 + 2)
/* 120 (2100 - 1980) isn't leap year */
#define YEAR_2100   120
#define IS_LEAP_YEAR(y) (!((y) & 3) && (y) != YEAR_2100)

/* Linear day numbers of the respective 1sts in non-leap years. */
static clock_t days_in_year[] =
{
    /* Jan  Feb  Mar  Apr  May  Jun  Jul  Aug  Sep  Oct  Nov  Dec */
    0,   0,  31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334, 0, 0, 0,
};

/* Convert a exFAT time/date pair to a UNIX date (seconds since 1 1 70). */
void exfat_time_exfat2unix(struct timespec *ts, __le16 __time, __le16 __date,
                           u8 time_cs)
{
    u16 time = le16_to_cpu(__time), date = le16_to_cpu(__date);
    clock_t second, day, leap_day, month, year;

    year  = date >> 9;
    month = max(1, (date >> 5) & 0xf);
    day   = max(1, date & 0x1f) - 1;

    leap_day = (year + 3) / 4;
    if (year > YEAR_2100)       /* 2100 isn't leap year */
    {
        leap_day--;
    }
    if (IS_LEAP_YEAR(year) && month > 2)
    {
        leap_day++;
    }

    second = (time & 0x1f) << 1;
    second += ((time >> 5) & 0x3f) * SECS_PER_MIN;
    second += (time >> 11) * SECS_PER_HOUR;
    second += (year * 365 + leap_day
               + days_in_year[month] + day
               + DAYS_DELTA) * SECS_PER_DAY;

    second += sys_tz.tz_minuteswest * SECS_PER_MIN;

    if (time_cs)
    {
        ts->tv_sec = second + (time_cs / 100);
        ts->tv_nsec = (time_cs % 100) * 10000000;
    }
    else
    {
        ts->tv_sec = second;
        ts->tv_nsec = 0;
    }
}

int exfat_conv_u2c(struct nls_table *nls,
                   const __le16 **uni, unsigned int *uni_left,
                   unsigned char **out, unsigned int *out_left)
{
    __wchar_t wc;
    int len;

    while (*uni_left)
    {
        wc = le16_to_cpu(**uni);
        if (!wc)
        {
            return -EINVAL;
        }

        len = nls->uni2char(wc, *out, *out_left);
        if (len < 0)
        {
            return len;
        }

        (*uni)++;
        (*uni_left)--;
        *out += len;
        *out_left -= len;
    }

    return 0;
}

#if defined CONFIG_FSYS_EXFAT_RW

/* Convert linear UNIX date to a exFAT time/date pair. */
void exfat_time_unix2exfat(struct timespec *ts, __le16 *time, __le16 *date,
                           u8 *time_cs)
{
    clock_t second = ts->tv_sec;
    clock_t day, leap_day, month, year;

    second -= sys_tz.tz_minuteswest * SECS_PER_MIN;

    /* Jan 1 GMT 00:00:00 1980. But what about another time zone? */
    if (second < UNIX_SECS_1980)
    {
        *time = 0;
        *date = cpu_to_le16((0 << 9) | (1 << 5) | 1);
        if (time_cs)
        {
            *time_cs = 0;
        }
        return;
    }
#if BITS_PER_LONG == 64
    if (second >= UNIX_SECS_2108)
    {
        *time = cpu_to_le16((23 << 11) | (59 << 5) | 29);
        *date = cpu_to_le16((127 << 9) | (12 << 5) | 31);
        if (time_cs)
        {
            *time_cs = 199;
        }
        return;
    }
#endif

    day = second / SECS_PER_DAY - DAYS_DELTA;
    year = day / 365;
    leap_day = (year + 3) / 4;
    if (year > YEAR_2100)       /* 2100 isn't leap year */
    {
        leap_day--;
    }
    if (year * 365 + leap_day > day)
    {
        year--;
    }
    leap_day = (year + 3) / 4;
    if (year > YEAR_2100)       /* 2100 isn't leap year */
    {
        leap_day--;
    }
    day -= year * 365 + leap_day;

    if (IS_LEAP_YEAR(year) && day == days_in_year[3])
    {
        month = 2;
    }
    else
    {
        if (IS_LEAP_YEAR(year) && day > days_in_year[3])
        {
            day--;
        }
        for (month = 1; month < 12; month++)
        {
            if (days_in_year[month + 1] > day)
            {
                break;
            }
        }
    }
    day -= days_in_year[month];

    *time = cpu_to_le16(((second / SECS_PER_HOUR) % 24) << 11
                        | ((second / SECS_PER_MIN) % 60) << 5
                        | (second % SECS_PER_MIN) >> 1);
    *date = cpu_to_le16((year << 9) | (month << 5) | (day + 1));
    if (time_cs)
    {
        *time_cs = (ts->tv_sec & 1) * 100 + ts->tv_nsec / 10000000;
    }
}
#endif  /* FSYS_EXFAT_RW */

