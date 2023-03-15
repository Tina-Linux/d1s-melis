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
#include "gui_core_memdev_private.h"

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       defines
*
**********************************************************************
*/

#define SETPIXEL(x,y,colorindex) _SetPixel(x,y,colorindex);
#define BKCOLORINDEX GUI_Context.pDevData->aColorIndex[0]
#define COLORINDEX   GUI_Context.pDevData->aColorIndex[1]

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_CopyToLCDAA
*/
void GUI_MEMDEV_CopyToLCDAA(GUI_MEMDEV_Handle hMem)
{
    GUI_LOCK();

    /* Make sure memory handle is valid */
    if (hMem)
    {
        GUI_MEMDEV_Handle hMemPrev;
#if GUI_NUM_LAYERS > 1
        int PrevLayer;
#endif
        int x, y;
        GUI_MEMDEV *pDev = (GUI_MEMDEV *)GUI_ALLOC_h2p(hMem); /* Convert to pointer */
        LCD_PIXELINDEX *pData0 = (LCD_PIXELINDEX *)(pDev + 1);
        int LineOff = pDev->XSize;
        int x0 = pDev->x0;
        int y0 = pDev->y0;
        int XMax = pDev->XSize / 2;
        int YMax = pDev->YSize / 2;
        hMemPrev = GUI_Context.hDevData;
#if GUI_NUM_LAYERS > 1
        PrevLayer = GUI_SelectLayer(pDev->LayerIndex);   /* Should not we switch back to the orig. layer when done ? */
#else
        GUI_SelectLCD();  /* Activate LCD */
#endif

        for (y = 0; y < YMax; y++)
        {
            LCD_PIXELINDEX *pData = pData0;

            for (x = 0; x < XMax; x++)
            {
                LCD_PIXELINDEX PixelIndex;
                int ColorSep[3];
                U32 Color = LCD_Index2Color(*pData);
                ColorSep[0] = Color & 255;
                ColorSep[1] = (Color >> 8)  & 255;
                ColorSep[2] = (Color >> 16) & 255;
                Color = LCD_Index2Color(*(pData + 1));
                ColorSep[0] += Color & 255;
                ColorSep[1] += (Color >> 8)  & 255;
                ColorSep[2] += (Color >> 16) & 255;
                Color = LCD_Index2Color(*(pData + LineOff));
                ColorSep[0] += Color & 255;
                ColorSep[1] += (Color >> 8)  & 255;
                ColorSep[2] += (Color >> 16) & 255;
                Color = LCD_Index2Color(*(pData + LineOff + 1));
                ColorSep[0] += Color & 255;
                ColorSep[1] += (Color >> 8)  & 255;
                ColorSep[2] += (Color >> 16) & 255;
                Color  = (ColorSep[0] + 2) >> 2;
                Color |= ((ColorSep[1] + 2) >> 2) << 8;
                Color |= ((U32)((ColorSep[2] + 2) >> 2)) << 16;
                PixelIndex = LCD_Color2Index(Color);
                LCD_SetPixelIndex(x + x0, y + y0, PixelIndex);
                pData += 2;
            }

            pData0 += 2 * LineOff;
        }

#if GUI_NUM_LAYERS > 1
        GUI_SelectLayer(PrevLayer);
#endif
        /* Reactivate previously used device */
        GUI_MEMDEV_Select(hMemPrev);
    }

    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_MEMDEV_CopyFromLCDAA
*/
void GUI_MEMDEV_CopyFromLCDAA(GUI_MEMDEV_Handle hMem)
{
    /* Make sure memory handle is valid */
    if (!hMem)
    {
        hMem = GUI_Context.hDevData;
    }

    if (hMem)
    {
        int x, y;
        GUI_MEMDEV *pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */
        GUI_USAGE *pUsage = 0;
        int x0 = pDevData->x0;
        int y0 = pDevData->y0;
        int XMax = pDevData->XSize / 2;
        int YMax = pDevData->YSize / 2;
        LCD_PIXELINDEX *pData = (LCD_PIXELINDEX *)(pDevData + 1);
        int LineOff = pDevData->BytesPerLine;

        if (pDevData->hUsage)
        {
            pUsage = GUI_USAGE_H2P(pDevData->hUsage);
        }

        for (y = 0; y < YMax; y++)
        {
            if (pUsage)
            {
                GUI_USAGE_AddHLine(pUsage, 0, y * 2, pDevData->XSize);
                GUI_USAGE_AddHLine(pUsage, 0, y * 2 + 1, pDevData->XSize);
            }

            for (x = 0; x < XMax; x++)
            {
                *pData = *(pData + 1) = *(pData + LineOff) = *(pData + LineOff + 1) = LCD_GetPixelIndex(x + x0, y + y0);
                pData += 2;
            }

            pData += LineOff + (pDevData->XSize & 1);
        }
    }
}

#else

void GUIDEV_AA(void) {} /* avoid empty object files */

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
