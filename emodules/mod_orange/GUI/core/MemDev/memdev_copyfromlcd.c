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

#define LCD_LIMIT(Var, Op, Limit) if (Var Op Limit) Var = Limit

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_CopyFromLCD
*/
void GUI_MEMDEV_CopyFromLCD(GUI_MEMDEV_Handle hMem)
{
    /* Make sure memory handle is valid */
    if (!hMem)
    {
        hMem = GUI_Context.hDevData;
    }

    if (!hMem)
    {
        return;
    }

    {
        GUI_MEMDEV *pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */
        LCD_RECT r;
        int y;
        int XMax;
        GUI_USAGE *pUsage = 0;
        GUI_MEMDEV_Handle hMemOld = GUI_Context.hDevData;
        GUI_MEMDEV_Select(hMem);

        if (pDevData->hUsage)
        {
            pUsage = GUI_USAGE_H2P(pDevData->hUsage);
        }

        /* Get bounding rectangle */
        r.y0  = pDevData->y0;
        r.x0  = pDevData->x0;
        r.x1  = pDevData->x0 + pDevData->XSize - 1;
        r.y1  = pDevData->y0 + pDevData->YSize - 1;
        /* Make sure bounds are within LCD area so we can call driver directly */
        LCD_LIMIT(r.x0, <, 0);
        LCD_LIMIT(r.y0, <, 0);
        LCD_LIMIT(r.x1, >, LCD_GET_XSIZE() - 1);
        LCD_LIMIT(r.y1, >, LCD_GET_YSIZE() - 1);
        XMax = r.x1;

        for (y = r.y0; y <= r.y1; y++)
        {
            int x = r.x0;
            LCD_PIXELINDEX *pData = (LCD_PIXELINDEX *)GUI_MEMDEV__XY2PTR(x, y);

            if (pUsage)
            {
                GUI_USAGE_AddHLine(pUsage, x, y, r.x1 - r.x0 + 1);
            }

            for (; x <= XMax; x++)
            {
                *pData++ = LCD_L0_GetPixelIndex(x, y);
            }
        }

        GUI_MEMDEV_Select(hMemOld);
    }
}

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
