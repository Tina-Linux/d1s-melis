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
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawBitmap_565
*/
static void _DrawBitmap_565(int x0, int y0, int xsize, int ysize, const U8 GUI_UNI_PTR *pPixel, const LCD_LOGPALETTE GUI_UNI_PTR *pLogPal, int xMag, int yMag)
{
    const U16 *pData;
    int y, OldIndex, PrevIndex, Mode, SwapRB, Mag, BytesPerLine, UseDrawBitmap = 0;
    GUI_USE_PARA(pLogPal);
    OldIndex = LCD_GetColorIndex();
    PrevIndex = OldIndex;
    pData = (const U16 *)pPixel;
    Mode = LCD_GetFixedPaletteEx(GUI_Context.SelLayer);
    SwapRB = LCD_GetSwapRBEx(GUI_Context.SelLayer);
    Mag = (xMag | yMag);
    BytesPerLine = xsize * 2;
#if GUI_SUPPORT_MEMDEV == 0

    if ((Mode == 565) && (SwapRB == 0) && (Mag == 1))
    {
        UseDrawBitmap = 1;
    }

#else

    if (GUI_Context.pDeviceAPI->pMemDevAPI)
    {
        if (((GUI_Context.hDevData && (GUI_Context.pDeviceAPI->pMemDevAPI->pfIndex2Color == LCD_Index2Color_565)) ||
             ((GUI_Context.hDevData == 0) && (Mode == 565) && (SwapRB == 0)))  && (Mag == 1))
        {
            UseDrawBitmap = 1;
        }
    }

#endif

    if (UseDrawBitmap)
    {
        LCD_DrawBitmap(x0, y0, xsize, ysize, 1, 1, 16, BytesPerLine, pPixel, 0);
    }
    else
    {
        if (Mag == 1)
        {
            /* Perform clipping */
            int x1, y1, Diff;
            y1 = y0 + ysize - 1;
            x1 = x0 + xsize - 1;

            /* Clip Y */
            if (y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0)
            {
                Diff = GUI_Context.ClipRect[GUI_Context.SelLayer].y0 - y0;
                y0   = GUI_Context.ClipRect[GUI_Context.SelLayer].y0;
#if GUI_SUPPORT_LARGE_BITMAPS                       /* Required only for 16 bit CPUs if some bitmaps are >64kByte */
                pData += (U32)Diff * (U32)BytesPerLine / 2;
#else
                pData += (unsigned)Diff * (unsigned)BytesPerLine / 2;
#endif
                ysize -= Diff;
            }

            Diff = y1 - GUI_Context.ClipRect[GUI_Context.SelLayer].y1;

            if (Diff > 0)
            {
                ysize -= Diff;
            }

            if (ysize <= 0)
            {
                return;
            }

            /* Clip right side */
            Diff = x1 - GUI_Context.ClipRect[GUI_Context.SelLayer].x1;

            if (Diff > 0)
            {
                xsize -= Diff;
            }

            /* Clip left side */
            Diff = 0;

            if (x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0)
            {
                Diff = GUI_Context.ClipRect[GUI_Context.SelLayer].x0 - x0;
                xsize -= Diff;
                pData += Diff;
                x0 += Diff;
            }

            if (xsize <= 0)
            {
                return;
            }

            /* Simple, unmagnified output using LCD_L0_SetPixel() */
            for (y = 0; y < ysize; y++)
            {
                int x;
                const U16 *p = pData;

                for (x = 0; x < xsize; x++)
                {
                    U16 Index;
                    Index = *p++;

                    if (Index != PrevIndex)
                    {
                        LCD_SetColor(LCD_Index2Color_565(Index));
                        PrevIndex = Index;
                    }

                    LCDDEV_L0_SetPixelIndex(x + x0, y + y0, LCD_COLORINDEX);
                }

                pData += BytesPerLine / 2;
            }
        }
        else
        {
            /* Magnified output using LCD_FillRect() */
            int x, y;
            int yi;

            for (y = y0, yi = 0; yi < ysize; yi++, y += yMag, pPixel += BytesPerLine)
            {
                int yMax;
                yMax = y + yMag - 1;

                /* Draw if within clip area */
                if ((yMax >= GUI_Context.ClipRect[GUI_Context.SelLayer].y0) && (y <= GUI_Context.ClipRect[GUI_Context.SelLayer].y1))
                {
                    int xi;
                    const U16 GUI_UNI_PTR *p = pData;

                    for (x = x0, xi = 0; xi < xsize; xi++, x += xMag)
                    {
                        U16 Index;
                        Index = *p++;

                        if (Index != PrevIndex)
                        {
                            LCD_SetColor(LCD_Index2Color_565(Index));
                            PrevIndex = Index;
                        }

                        LCD_FillRect(x, y, x + xMag - 1, yMax);
                    }
                }

                pData += BytesPerLine / 2;
            }
        }

        GUI_SetColorIndex(OldIndex);
    }
}

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_BitmapMethods565
*/
const GUI_BITMAP_METHODS GUI_BitmapMethods565 =
{
    _DrawBitmap_565,
    LCD_Index2Color_565
};

/*************************** End of file ****************************/
