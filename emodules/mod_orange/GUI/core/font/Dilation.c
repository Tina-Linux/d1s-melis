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
#include "gui_font_private.h"

static  U8 FRAME_A = 0xcf;
static  U32 FRAME_A_32BPP = 0x0;


U8 GUI_SetFrameColor8bpp32(U8 frameColor)
{
    U8 oldFrameColor;
    oldFrameColor = FRAME_A;
    FRAME_A = frameColor;
    return oldFrameColor;
}

U32 GUI_SetFrameColor32bpp(U32 frameColor)
{
    U32 oldFrameColor;
    oldFrameColor = FRAME_A_32BPP;
    FRAME_A_32BPP = frameColor;
    return oldFrameColor;
}




void shadow_Dilation(unsigned char *in, unsigned char *out, unsigned int xsize, unsigned int ysize)
{
    unsigned char   *src;
    unsigned char   *dest;
    unsigned int    num;
    unsigned int    i, j;
    unsigned char   *ptemp;
    xsize += 4;
    ysize += 4;
    ptemp = (unsigned char *)esMEMS_Malloc(0, (xsize) * (ysize));
    orange_memset(out, 0x0, xsize * ysize);

    for (i = 2; i < ysize - 2; i++)
    {
        eLIBs_memcpy(out + i * xsize + 2, in + (i - 2) * (xsize - 4), xsize - 4);
    }

    eLIBs_memcpy(ptemp, out, xsize * ysize);

    for (i = 1; i < ysize - 3 + 1 + 1; i++) // 行(除去边缘几行)
    {
        for (j = 1; j < xsize - 3 + 1 + 1; j++) // 列(除去边缘几列)
        {
            dest = out + xsize * (i) + j;
            src = ptemp + xsize * (i) + (j);
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = num;
                continue;
            }

            src = src - xsize - 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + xsize;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src - 2;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + xsize ;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
                continue;
            }

            src = src + 1;
            num = *(unsigned char *)src;

            if (num != 0)
            {
                *dest = FRAME_A;
            }
        }
    }

    //eLIBs_memcpy(in, out, xsize*ysize);
    esMEMS_Mfree(0, ptemp);
}
