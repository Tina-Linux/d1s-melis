/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Apps Sub-System
*
*                                   (c) Copyright 2007-2009, john.fu <john.fu@allwinnerme.com>
*                                             All Rights Reserved
*
* Moudle  : Charset
* File    : utf-8.c
*
* By      : john.fu
* Version : v1.0
* Date    : 2009-8-25
**********************************************************************************************************************
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

