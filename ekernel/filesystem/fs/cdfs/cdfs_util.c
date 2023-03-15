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
#include "cdfs.h"

/*
 * We have to convert from a MM/DD/YY format to the Unix ctime format.
 * We have to take into account leap years and all of that good stuff.
 * Unfortunately, the kernel does not have the information on hand to
 * take into account daylight savings time, but it shouldn't matter.
 * The time stored should be localtime (with or without DST in effect),
 * and the timezone offset should hold the offset required to get back
 * to GMT.  Thus  we should always be correct.
 */

int iso_date(char *p, int flag)
{
    int year, month, day, hour, minute, second, tz;
    int crtime, days, i;

    year = p[0] - 70;
    month = p[1];
    day = p[2];
    hour = p[3];
    minute = p[4];
    second = p[5];
    if (flag == 0)
    {
        tz = p[6];    /* High sierra has no time zone */
    }
    else
    {
        tz = 0;
    }

    if (year < 0)
    {
        crtime = 0;
    }
    else
    {
        int monlen[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        days = year * 365;
        if (year > 2)
        {
            days += (year + 1) / 4;
        }
        for (i = 1; i < month; i++)
        {
            days += monlen[i - 1];
        }
        if (((year + 2) % 4) == 0 && month > 2)
        {
            days++;
        }
        days += day - 1;
        crtime = ((((days * 24) + hour) * 60 + minute) * 60)
                 + second;

        /* sign extend */
        if (tz & 0x80)
        {
            tz |= (-1 << 8);
        }

        /*
         * The timezone offset is unreliable on some disks,
         * so we make a sanity check.  In no case is it ever
         * more than 13 hours from GMT, which is 52*15min.
         * The time is always stored in localtime with the
         * timezone offset being what get added to GMT to
         * get to localtime.  Thus we need to subtract the offset
         * to get to true GMT, which is what we store the time
         * as internally.  On the local system, the user may set
         * their timezone any way they wish, of course, so GMT
         * gets converted back to localtime on the receiving
         * system.
         *
         * NOTE: mkisofs in versions prior to mkisofs-1.10 had
         * the sign wrong on the timezone offset.  This has now
         * been corrected there too, but if you are getting screwy
         * results this may be the explanation.  If enough people
         * complain, a user configuration option could be added
         * to add the timezone offset in with the wrong sign
         * for 'compatibility' with older discs, but I cannot see how
         * it will matter that much.
         *
         * Thanks to kuhlmav@elec.canterbury.ac.nz (Volker Kuhlmann)
         * for pointing out the sign error.
         */
        if (-52 <= tz && tz <= 52)
        {
            crtime -= tz * 15 * 60;
        }
    }
    return crtime;
}