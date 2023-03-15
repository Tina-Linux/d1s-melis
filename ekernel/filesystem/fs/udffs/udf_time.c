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
#include "udf_decl.h"
#include "udf_fs.h"

#define EPOCH_YEAR 1970

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

#define MAX_YEAR_SECONDS    69
#define SPD         0x15180 /*3600*24 */
#define SPY(y, l, s)        (SPD * (365 * y + l) + s)

static clock_t year_seconds[MAX_YEAR_SECONDS] =
{
    /*1970*/ SPY(0,   0, 0), SPY(1,   0, 0), SPY(2,   0, 0), SPY(3,   1, 0),
    /*1974*/ SPY(4,   1, 0), SPY(5,   1, 0), SPY(6,   1, 0), SPY(7,   2, 0),
    /*1978*/ SPY(8,   2, 0), SPY(9,   2, 0), SPY(10,  2, 0), SPY(11,  3, 0),
    /*1982*/ SPY(12,  3, 0), SPY(13,  3, 0), SPY(14,  3, 0), SPY(15,  4, 0),
    /*1986*/ SPY(16,  4, 0), SPY(17,  4, 0), SPY(18,  4, 0), SPY(19,  5, 0),
    /*1990*/ SPY(20,  5, 0), SPY(21,  5, 0), SPY(22,  5, 0), SPY(23,  6, 0),
    /*1994*/ SPY(24,  6, 0), SPY(25,  6, 0), SPY(26,  6, 0), SPY(27,  7, 0),
    /*1998*/ SPY(28,  7, 0), SPY(29,  7, 0), SPY(30,  7, 0), SPY(31,  8, 0),
    /*2002*/ SPY(32,  8, 0), SPY(33,  8, 0), SPY(34,  8, 0), SPY(35,  9, 0),
    /*2006*/ SPY(36,  9, 0), SPY(37,  9, 0), SPY(38,  9, 0), SPY(39, 10, 0),
    /*2010*/ SPY(40, 10, 0), SPY(41, 10, 0), SPY(42, 10, 0), SPY(43, 11, 0),
    /*2014*/ SPY(44, 11, 0), SPY(45, 11, 0), SPY(46, 11, 0), SPY(47, 12, 0),
    /*2018*/ SPY(48, 12, 0), SPY(49, 12, 0), SPY(50, 12, 0), SPY(51, 13, 0),
    /*2022*/ SPY(52, 13, 0), SPY(53, 13, 0), SPY(54, 13, 0), SPY(55, 14, 0),
    /*2026*/ SPY(56, 14, 0), SPY(57, 14, 0), SPY(58, 14, 0), SPY(59, 15, 0),
    /*2030*/ SPY(60, 15, 0), SPY(61, 15, 0), SPY(62, 15, 0), SPY(63, 16, 0),
    /*2034*/ SPY(64, 16, 0), SPY(65, 16, 0), SPY(66, 16, 0), SPY(67, 17, 0),
    /*2038*/ SPY(68, 17, 0)
};

extern const struct timezone sys_tz;

#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)

struct timespec *
udf_disk_stamp_to_time(struct timespec *dest, struct timestamp src)
{
    int yday;
    u16 typeAndTimezone = le16_to_cpu(src.typeAndTimezone);
    u16 year = le16_to_cpu(src.year);
    uint8_t type = typeAndTimezone >> 12;
    int16_t offset;

    if (type == 1)
    {
        offset = typeAndTimezone << 4;
        /* sign extent offset */
        offset = (offset >> 4);
        if (offset == -2047) /* unspecified offset */
        {
            offset = 0;
        }
    }
    else
    {
        offset = 0;
    }

    if ((year < EPOCH_YEAR) ||
        (year >= EPOCH_YEAR + MAX_YEAR_SECONDS))
    {
        return NULL;
    }
    dest->tv_sec = year_seconds[year - EPOCH_YEAR];
    dest->tv_sec -= offset * 60;

    yday = ((__mon_yday[__isleap(year)][src.month - 1]) + src.day - 1);
    dest->tv_sec += (((yday * 24) + src.hour) * 60 + src.minute) * 60 + src.second;
    dest->tv_nsec = 1000 * (src.centiseconds * 10000 +
                            src.hundredsOfMicroseconds * 100 + src.microseconds);
    return dest;
}

struct timestamp *
udf_time_to_disk_stamp(struct timestamp *dest, struct timespec ts)
{
    long int days, rem, y;
    const unsigned short int *ip;
    int16_t offset;

    offset = -sys_tz.tz_minuteswest;

    if (!dest)
    {
        return NULL;
    }

    dest->typeAndTimezone = cpu_to_le16(0x1000 | (offset & 0x0FFF));

    ts.tv_sec += offset * 60;
    days = ts.tv_sec / SECS_PER_DAY;
    rem = ts.tv_sec % SECS_PER_DAY;
    dest->hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    dest->minute = rem / 60;
    dest->second = rem % 60;
    y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
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
    dest->year = cpu_to_le16(y);
    ip = __mon_yday[__isleap(y)];
    for (y = 11; days < (long int)ip[y]; --y)
    {
        continue;
    }
    days -= ip[y];
    dest->month = y + 1;
    dest->day = days + 1;

    dest->centiseconds = ts.tv_nsec / 10000000;
    dest->hundredsOfMicroseconds = (ts.tv_nsec / 1000 -
                                    dest->centiseconds * 10000) / 100;
    dest->microseconds = (ts.tv_nsec / 1000 - dest->centiseconds * 10000 -
                          dest->hundredsOfMicroseconds * 100);
    return dest;
}

/* EOF */