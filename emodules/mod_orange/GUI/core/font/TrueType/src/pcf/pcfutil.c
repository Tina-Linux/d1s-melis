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
/* $XFree86: xc/lib/font/util/utilbitmap.c,v 1.3 1999/08/22 08:58:58 dawes Exp $ */

/*
 * Author:  Keith Packard, MIT X Consortium
 */

/* Modified for use with FreeType */


#include <ft2build.h>
#include "pcfutil.h"


/*
 *  Invert bit order within each BYTE of an array.
 */

FT_LOCAL_DEF(void)
BitOrderInvert(unsigned char  *buf,
               int             nbytes)
{
    for (; --nbytes >= 0; buf++)
    {
        unsigned int  val = *buf;
        val = ((val >> 1) & 0x55) | ((val << 1) & 0xAA);
        val = ((val >> 2) & 0x33) | ((val << 2) & 0xCC);
        val = ((val >> 4) & 0x0F) | ((val << 4) & 0xF0);
        *buf = (unsigned char)val;
    }
}


/*
 *  Invert byte order within each 16-bits of an array.
 */

FT_LOCAL_DEF(void)
TwoByteSwap(unsigned char  *buf,
            int             nbytes)
{
    unsigned char  c;

    for (; nbytes >= 2; nbytes -= 2, buf += 2)
    {
        c      = buf[0];
        buf[0] = buf[1];
        buf[1] = c;
    }
}

/*
 *  Invert byte order within each 32-bits of an array.
 */

FT_LOCAL_DEF(void)
FourByteSwap(unsigned char  *buf,
             int             nbytes)
{
    unsigned char  c;

    for (; nbytes >= 4; nbytes -= 4, buf += 4)
    {
        c      = buf[0];
        buf[0] = buf[3];
        buf[3] = c;
        c      = buf[1];
        buf[1] = buf[2];
        buf[2] = c;
    }
}


/* END */
