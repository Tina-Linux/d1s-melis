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
/*                                                                         */
/*  ftmisc.h                                                               */
/*                                                                         */
/*    Miscellaneous macros for stand-alone rasterizer (specification       */
/*    only).                                                               */
/*                                                                         */
/*  Copyright 2005 by                                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used        */
/*  modified and distributed under the terms of the FreeType project       */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/
/***************************************************/
/*                                                 */
/* This file is *not* portable!  You have to adapt */
/* its definitions to your platform.               */
/*                                                 */
/***************************************************/

#ifndef __FTMISC_H__
#define __FTMISC_H__

#include <string.h> /* memset */

#define FT_BEGIN_HEADER
#define FT_END_HEADER

#define FT_LOCAL_DEF( x )   static x

/* from include/freetype2/fttypes.h */

typedef unsigned char  FT_Byte;
typedef signed int     FT_Int;
typedef unsigned int   FT_UInt;
typedef signed long    FT_Long;
typedef unsigned long  FT_ULong;
typedef signed long    FT_F26Dot6;
typedef int            FT_Error;

#define FT_MAKE_TAG( _x1, _x2, _x3, _x4 ) \
    ( ( (FT_ULong)_x1 << 24 ) |     \
      ( (FT_ULong)_x2 << 16 ) |     \
      ( (FT_ULong)_x3 <<  8 ) |     \
      (FT_ULong)_x4         )


/* from src/ftcalc.c */

#include <inttypes.h>

typedef int64_t  FT_Int64;

static FT_Long FT_MulDiv(FT_Long  a,
                         FT_Long  b,
                         FT_Long  c)
{
    FT_Int   s;
    FT_Long  d;
    s = 1;

    if (a < 0)
    {
        a = -a;
        s = -1;
    }

    if (b < 0)
    {
        b = -b;
        s = -s;
    }

    if (c < 0)
    {
        c = -c;
        s = -s;
    }

    d = (FT_Long)(c > 0 ? ((FT_Int64)a * b + (c >> 1)) / c
                  : 0x7FFFFFFFL);
    return (s > 0) ? d : -d;
}

#endif /* __FTMISC_H__ */


/* END */
