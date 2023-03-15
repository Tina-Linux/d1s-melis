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
#include "zutil.h"
#include "infblock.h"
#include "inftrees.h"
#include "infcodes.h"
#include "infutil.h"


/* And'ing with mask[n] masks the lower n bits */
local const uInt inflate_mask[17] =
{
    0x0000,
    0x0001, 0x0003, 0x0007, 0x000f, 0x001f, 0x003f, 0x007f, 0x00ff,
    0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};


/* copy as much as possible from the sliding window to the output area */
local int inflate_flush( /* s, z, r) */
    inflate_blocks_statef *s,
    z_streamp z,
    int r)
{
    uInt n;
    Bytef *p;
    Bytef *q;
    /* local copies of source and destination pointers */
    p = z->next_out;
    q = s->read;
    /* compute number of bytes to copy as far as end of window */
    n = (uInt)((q <= s->write ? s->write : s->end) - q);

    if (n > z->avail_out)
    {
        n = z->avail_out;
    }

    if (n && r == Z_BUF_ERROR)
    {
        r = Z_OK;
    }

    /* update counters */
    z->avail_out -= n;
    z->total_out += n;

    /* update check information */
    if (s->checkfn != Z_NULL)
    {
        z->adler = s->check = (*s->checkfn)(s->check, q, n);
    }

    /* copy as far as end of window */
    zmemcpy(p, q, n);
    p += n;
    q += n;

    /* see if more to copy at beginning of window */
    if (q == s->end)
    {
        /* wrap pointers */
        q = s->window;

        if (s->write == s->end)
        {
            s->write = s->window;
        }

        /* compute bytes to copy */
        n = (uInt)(s->write - q);

        if (n > z->avail_out)
        {
            n = z->avail_out;
        }

        if (n && r == Z_BUF_ERROR)
        {
            r = Z_OK;
        }

        /* update counters */
        z->avail_out -= n;
        z->total_out += n;

        /* update check information */
        if (s->checkfn != Z_NULL)
        {
            z->adler = s->check = (*s->checkfn)(s->check, q, n);
        }

        /* copy */
        zmemcpy(p, q, n);
        p += n;
        q += n;
    }

    /* update pointers */
    z->next_out = p;
    s->read = q;
    /* done */
    return r;
}
