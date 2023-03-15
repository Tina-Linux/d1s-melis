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
/* @(#) $Id: zutil.c,v 1.3 2006/04/29 07:31:16 wl Exp $ */

#include "zutil.h"

#ifndef STDC
extern void exit OF((int));
#endif


#ifndef HAVE_MEMCPY

void zmemcpy(dest, source, len)
Bytef *dest;
const Bytef *source;
uInt  len;
{
    if (len == 0)
    {
        return;
    }

    do
    {
        *dest++ = *source++; /* ??? to be unrolled */
    } while (--len != 0);
}

int zmemcmp(s1, s2, len)
const Bytef *s1;
const Bytef *s2;
uInt  len;
{
    uInt j;

    for (j = 0; j < len; j++)
    {
        if (s1[j] != s2[j])
        {
            return 2 * (s1[j] > s2[j]) - 1;
        }
    }

    return 0;
}

void zmemzero(dest, len)
Bytef *dest;
uInt  len;
{
    if (len == 0)
    {
        return;
    }

    do
    {
        *dest++ = 0;  /* ??? to be unrolled */
    } while (--len != 0);
}
#endif

#ifdef __TURBOC__
#if (defined( __BORLANDC__) || !defined(SMALL_MEDIUM)) && !defined(__32BIT__)
/* Small and medium model in Turbo C are for now limited to near allocation
 * with reduced MAX_WBITS and MAX_MEM_LEVEL
 */
#  define MY_ZCALLOC

/* Turbo C malloc() does not allow dynamic allocation of 64K bytes
 * and farmalloc(64K) returns a pointer with an offset of 8, so we
 * must fix the pointer. Warning: the pointer must be put back to its
 * original form in order to free it, use zcfree().
 */

#define MAX_PTR 10
/* 10*64K = 640K */

local int next_ptr = 0;

typedef struct ptr_table_s
{
    voidpf org_ptr;
    voidpf new_ptr;
} ptr_table;

local ptr_table table[MAX_PTR];
/* This table is used to remember the original form of pointers
 * to large buffers (64K). Such pointers are normalized with a zero offset.
 * Since MSDOS is not a preemptive multitasking OS, this table is not
 * protected from concurrent access. This hack doesn't work anyway on
 * a protected system like OS/2. Use Microsoft C instead.
 */

voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
    voidpf buf = opaque; /* just to make some compilers happy */
    ulg bsize = (ulg)items * size;

    /* If we allocate less than 65520 bytes, we assume that farmalloc
     * will return a usable pointer which doesn't have to be normalized.
     */
    if (bsize < 65520L)
    {
        buf = farmalloc(bsize);

        if (*(ush *)&buf != 0)
        {
            return buf;
        }
    }
    else
    {
        buf = farmalloc(bsize + 16L);
    }

    if (buf == NULL || next_ptr >= MAX_PTR)
    {
        return NULL;
    }

    table[next_ptr].org_ptr = buf;
    /* Normalize the pointer to seg:0 */
    *((ush *)&buf + 1) += ((ush)((uch *)buf - 0) + 15) >> 4;
    *(ush *)&buf = 0;
    table[next_ptr++].new_ptr = buf;
    return buf;
}

void  zcfree(voidpf opaque, voidpf ptr)
{
    int n;

    if (*(ush *)&ptr != 0)  /* object < 64K */
    {
        farfree(ptr);
        return;
    }

    /* Find the original pointer */
    for (n = 0; n < next_ptr; n++)
    {
        if (ptr != table[n].new_ptr)
        {
            continue;
        }

        farfree(table[n].org_ptr);

        while (++n < next_ptr)
        {
            table[n - 1] = table[n];
        }

        next_ptr--;
        return;
    }

    ptr = opaque; /* just to make some compilers happy */
    Assert(0, "zcfree: ptr not found");
}
#endif
#endif /* __TURBOC__ */


#if defined(M_I86) && !defined(__32BIT__)
/* Microsoft C in 16-bit mode */

#  define MY_ZCALLOC

#if (!defined(_MSC_VER) || (_MSC_VER <= 600))
#  define _halloc  halloc
#  define _hfree   hfree
#endif

voidpf zcalloc(voidpf opaque, unsigned items, unsigned size)
{
    if (opaque)
    {
        opaque = 0;    /* to make compiler happy */
    }

    return _halloc((long)items, size);
}

void  zcfree(voidpf opaque, voidpf ptr)
{
    if (opaque)
    {
        opaque = 0;    /* to make compiler happy */
    }

    _hfree(ptr);
}

#endif /* MSC */


#ifndef MY_ZCALLOC /* Any system without a special alloc function */

#ifndef STDC
extern voidp  ft_scalloc OF((uInt items, uInt size));
extern void   ft_sfree   OF((voidpf ptr));
#endif

voidpf zcalloc(opaque, items, size)
voidpf opaque;
unsigned items;
unsigned size;
{
    if (opaque)
    {
        items += size - size;    /* make compiler happy */
    }

    return (voidpf)ft_scalloc(items, size);
}

void  zcfree(opaque, ptr)
voidpf opaque;
voidpf ptr;
{
    ft_sfree(ptr);

    if (opaque)
    {
        return;    /* make compiler happy */
    }
}

#endif /* MY_ZCALLOC */
