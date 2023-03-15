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
#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static struct
{
    int x, y;
    const U8 GUI_UNI_PTR *pPixel;
    const U8 GUI_UNI_PTR *pPixelStart;
} Cache;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawBitmap_RLE8
*/
static void _DrawBitmap_RLE8(int x0, int y0, int xsize, int ysize, const U8 GUI_UNI_PTR *pPixel, const LCD_LOGPALETTE GUI_UNI_PTR *pLogPal, int xMag, int yMag)
{
    LCD_PIXELINDEX aColorIndex[2];
    LCD_PIXELINDEX PixelIndex;
    int xi, y;
    int xL, yL;
    const U8 GUI_UNI_PTR *pPixelOrg = pPixel;
    char NoTrans = !(GUI_Context.DrawMode & LCD_DRAWMODE_TRANS);
    const LCD_PIXELINDEX *pTrans = NULL;
    char IsMagnified = ((yMag | xMag) != 1);
    aColorIndex[0] = LCD_ACOLORINDEX[0];
    aColorIndex[1] = LCD_ACOLORINDEX[1];

    /* Handle color translation */
    if ((pLogPal) && (pLogPal->pPalEntries))
    {
        if ((pTrans = LCD_GetpPalConvTable(pLogPal)) == NULL)
        {
            return;
        }
    }

    /* Check if we can limit the number of lines due to clipping) */
    if (yMag == 1)
    {
        if (ysize > GUI_Context.ClipRect[GUI_Context.SelLayer].y1 - y0 + 1)
        {
            ysize = GUI_Context.ClipRect[GUI_Context.SelLayer].y1 - y0 + 1;
        }
    }

    /* Init variables for looping */
    xi = 0;
    y = 0;

    /* Check if we can use the cache to save some unnecessary iterations */
    if (!IsMagnified)
    {
        int yDiff = GUI_Context.ClipRect[GUI_Context.SelLayer].y0 - y0;

        if ((Cache.pPixelStart == pPixel) && (yDiff > Cache.y))
        {
            /* Accept cache values */
            y = Cache.y;
            xi = Cache.x;
            pPixel = Cache.pPixel;
        }
    }

    /* Init values for caching */
    Cache.pPixel = Cache.pPixelStart = pPixelOrg;
    Cache.x = Cache.y = 0;

    /* Repeat until we have reached bottom */
    for (; y < ysize;)
    {
        U8 Cmd  = *pPixel++;
        U8 Data = *pPixel++;

        if (Cmd)
        {
            /* Save cache info */
            Cache.pPixel = pPixel - 2;
            Cache.x = xi;
            Cache.y = y;
            LCD_ACOLORINDEX[1] = pTrans ? *(pTrans + Data) : Data;

            while (Cmd)
            {
                int xi1 = xi + Cmd;

                if (xi1 >= xsize)
                {
                    xi1 = xsize;
                }

                Cmd -= (xi1 - xi);

                if (Data || NoTrans)    /* Skip transparent pixels */
                {
                    if (IsMagnified)
                    {
                        xL = xMag * xi + x0;
                        yL = yMag * y + y0;
                        LCD_FillRect(xL, yL, xL + xMag * (xi1 - xi) - 1, yL + yMag - 1);
                    }
                    else
                    {
                        LCD_DrawHLine(x0 + xi, y + y0, xi1 + x0 - 1);
                    }
                }

                xi = xi1;

                if (xi1 == xsize)
                {
                    y++;
                    xi = 0;
                }
            }
        }
        else
        {
            do
            {
                U8 Index = *pPixel++;

                if (Index || NoTrans)    /* Skip transparent pixels */
                {
                    int x = x0 + xi;
                    PixelIndex = pTrans ? *(pTrans + Index) : Index;

                    if (IsMagnified)
                    {
                        LCD_SetColorIndex(PixelIndex);
                        xL = xMag * xi + x0;
                        yL = yMag * y + y0;
                        LCD_FillRect(xL, yL, xL + xMag - 1, yL + yMag - 1);
                    }
                    else
                    {
#if 1 /* High speed variant */

                        if (y + y0 >= GUI_Context.ClipRect[GUI_Context.SelLayer].y0)
                            if (x >= GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
                                if (x <= GUI_Context.ClipRect[GUI_Context.SelLayer].x1)
                                {
                                    LCDDEV_L0_SetPixelIndex(x, y + y0, PixelIndex);
                                }

#else
                        LCD_SetPixelIndex(x, y + y0, PixelIndex);
#endif
                    }
                }

                if (++xi >= xsize)
                {
                    xi = 0;
                    y++;

                    if (y >= ysize)
                    {
                        break;
                    }
                }
            } while (--Data);
        }
    }

    LCD_ACOLORINDEX[0] = aColorIndex[0];
    LCD_ACOLORINDEX[1] = aColorIndex[1];
}

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BitmapMethodsRLE8
*/
const GUI_BITMAP_METHODS GUI_BitmapMethodsRLE8 =
{
    _DrawBitmap_RLE8,
    NULL
};

/*************************** End of file ****************************/
