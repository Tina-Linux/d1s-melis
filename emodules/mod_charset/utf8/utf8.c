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
#include "mod_charset_i.h"

struct utf8_table
{
    int     cmask;
    int     cval;
    int     shift;
    long    lmask;
    long    lval;
};

static const struct utf8_table utf8_table[] =
{
    {0x80,  0x00,   0 * 6,    0x7F,           0,         /* 1 byte sequence */},
    {0xE0,  0xC0,   1 * 6,    0x7FF,          0x80,      /* 2 byte sequence */},
    {0xF0,  0xE0,   2 * 6,    0xFFFF,         0x800,     /* 3 byte sequence */},
    {0xF8,  0xF0,   3 * 6,    0x1FFFFF,       0x10000,   /* 4 byte sequence */},
    {0xFC,  0xF8,   4 * 6,    0x3FFFFFF,      0x200000,  /* 5 byte sequence */},
    {0xFE,  0xFC,   5 * 6,    0x7FFFFFFF,     0x4000000, /* 6 byte sequence */},
    {0,                            /* end of table    */}
};


static __s32 utf8_mbtowc(__u16 *p, const __u8 *s, __s32 n)
{
    long l;
    __s32 c0, c, nc;
    const struct utf8_table *t;
    nc = 0;
    c0 = *s;
    l = c0;

    for (t = utf8_table; t->cmask; t++)
    {
        nc++;

        if ((c0 & t->cmask) == t->cval)
        {
            l &= t->lmask;

            if (l < t->lval)
            {
                return -1;
            }

            *p = l;
            return nc;
        }

        if (n <= nc)
        {
            return -1;
        }

        s++;
        c = (*s ^ 0x80) & 0xFF;

        if (c & 0xC0)
        {
            return -1;
        }

        l = (l << 6) | c;
    }

    return -1;
}


static __s32 utf8_wctomb(__u8 *s, __u16 wc, __s32 maxlen)
{
    long l;
    __s32 c, nc;
    const struct utf8_table *t;

    if (!s)
    {
        return 0;
    }

    l = wc;
    nc = 0;

    for (t = utf8_table; t->cmask && maxlen; t++, maxlen--)
    {
        nc++;

        if (l <= t->lmask)
        {
            c = t->shift;
            *s = t->cval | (l >> c);

            while (c > 0)
            {
                c -= 6;
                s++;
                *s = 0x80 | ((l >> c) & 0x3F);
            }

            return nc;
        }
    }

    return -1;
}

__s32 UTF8_uni2char(const __u16 uni, __u8 *out, __u32 boundlen)
{
    __s32 n;

    if ((n = utf8_wctomb(out, uni, boundlen)) == -1)
    {
        *out = '?';
        return -1;
    }

    return n;
}

__s32 UTF8_char2uni(const __u8 *rawstring, __u32 boundlen, __u16 *uni)
{
    __s32 n;

    if ((n = utf8_mbtowc(uni, rawstring, boundlen)) == -1)
    {
        *uni = 0x003f;  /* ? */
        n = -1;
    }

    return n;
}


__u32 UTF8_get_charset2upper_table(void *buf, __u32 size)
{
    return 0;
}


__u32 UTF8_get_charset2lower_table(void *buf, __u32 size)
{
    return 0;
}
