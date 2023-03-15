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
#include <stdlib.h>
#include <string.h>
#include "gui_core_kernel_private.h"


#ifndef ABS
#define ABS(x)                          ( (x) >0 ? (x) : -(x) )
#endif

#define spcl_size_align( x, y )         ( ( (x) + (y) - 1 ) & ~( (y) - 1 ) )

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetStep
*/
static int _GetStep(int *pYSize, int *pY)
{
    if (*pYSize > 0)
    {
        *pY = *pYSize - 1;
        return -1;
    }
    else if (*pYSize < 0)
    {
        *pYSize = -*pYSize;
        *pY = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*********************************************************************
*
*       _DrawBitmap_32bpp_Alpha   // mcl  08.11.27
*/
static int _DrawBitmap_32bpp_Alpha(const U8 *pData, int x0, int y0, int XSize, int YSize)
{
    int  y, BytesPerLine, Step;
    int LCDXSize = 0;
    U32 *pDest = NULL;

    if ((Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE90)
        || (Screen_Context.LCDDirection[GUI_Context.SelLayer] == GUI_SCNDIR_ROTATE270))
    {
        LCDXSize = Screen_Context.LCDYSize[GUI_Context.SelLayer];
    }
    else
    {
        LCDXSize = Screen_Context.LCDXSize[GUI_Context.SelLayer];
    }

    pDest = (U32 *)((U32 *)orange.fbaddr[GUI_Context.SelLayer] + y0 * LCDXSize + x0);
    Step  = _GetStep(&YSize, &y);

    if (!Step)
    {
        return 1;
    }

    if (Step == -1)
    {
        pDest = (U32 *)(pDest + (YSize - 1) * LCDXSize);
    }

    BytesPerLine = ((32 * XSize + 31) >> 5) << 2;

    for (; (y < YSize) && (y >= 0); y += Step)
    {
        memcpy(pDest, pData, (XSize << 2));
        pData += BytesPerLine;
        pDest = (U32 *)(pDest + (Step) * LCDXSize);
    }

    return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BMP_GetXSize
*/
int GUI_ARGB_GetXSize(const void *pBMP)
{
    const U8 *pSrc = (const U8 *)pBMP;

    if (!pBMP)
    {
        return 0;
    }

    pSrc += 18;                   /* skip rest of BITMAPFILEHEADER */
    return GUI__Read32(&pSrc);
}

/*********************************************************************
*
*       GUI_BMP_GetYSize
*/
int GUI_ARGB_GetYSize(const void *pBMP)
{
    const U8 *pSrc = (const U8 *)pBMP;

    if (!pBMP)
    {
        return 0;
    }

    pSrc += 22;
    return labs((int)GUI__Read32(&pSrc));
}

/*********************************************************************
*
*       GUI_BMP_Draw
*/
int GUI_ARGB_Draw(const void *pBMP, int x0, int y0)
{
    if (Screen_Context.LCDDirection[GUI_Context.SelLayer])
    {
        GUI_BMP_Draw(pBMP, x0, y0);
    }
    else
    {
        int         Ret = 0;
        I32         Width;
        I32         Height;
        U16         BitCount;
        U16         Type;
        U32         ClrUsed;
        U32         Compression;
        int         NumColors;
        U32         alphaEn;
        const       U8 *pSrc = (const U8 *)pBMP;
        U8          *pSrcTemp = NULL;
        GUI_POINT   srcPoint;
        GUI_POINT   dstPoint;
        Type        = GUI__Read16(&pSrc); /* get type from BITMAPFILEHEADER */
        pSrc        += 4;
        alphaEn     = GUI__Read32(&pSrc);
        pSrc        += 4;                   /* skip rest of BITMAPFILEHEADER */
        /* get values from BITMAPINFOHEADER */
        pSrc        += 4;
        Width       = GUI__Read32(&pSrc);
        Height      = GUI__Read32(&pSrc);
        pSrc        += 2;
        BitCount    = GUI__Read16(&pSrc);
        Compression = GUI__Read32(&pSrc);
        pSrc        += 12;
        ClrUsed     = GUI__Read32(&pSrc);
        pSrc        += 4;

        /* calculate number of colors */
        switch (BitCount)
        {
            case 0:
                return 1; /* biBitCount = 0 (JPEG format) not supported. Please convert image ! */

            case 1:
                NumColors = 2;
                break;

            case 4:
                NumColors = 16;
                break;

            case 8:
                NumColors = 256;
                break;

            case 24:
                NumColors = 0;
                break;

            case 32:
                NumColors = 0;
                break;

            default:
                return 1; /* biBitCount should be 1, 4, 8 or 24 */
        }

        if (NumColors && ClrUsed)
        {
            NumColors = ClrUsed;
        }

        /* check validity of bmp */
        if ((NumColors > LCD_MAX_LOG_COLORS) ||
            (Type != 0x4d42)                 || /* 'BM' */
            (Compression)                    || /* only uncompressed bitmaps */
            (Width  > 1920)                  ||
            (ABS(Height) > 1080))
        {
            return 1;
        }

        /* start output */
        GUI_LOCK();

        /* Show bitmap */
        switch (BitCount)
        {
            case 1:
            case 4:
            case 8:
            case 24:
            case 32:
                Ret = _DrawBitmap_32bpp_Alpha(pSrc, x0, y0, Width, Height);    // mcl add
                break;
        }

        GUI_UNLOCK();
        return Ret;
    }

    return 0;
}
