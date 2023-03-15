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
*       static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _Min
*/
static int _Min(int v0, int v1)
{
    if (v0 <= v1)
    {
        return v0;
    }

    return v1;
}

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_Draw
*
*  This routine uses a banding memory device to draw the
*  given area flicker free. It not only draws, but also
*  automatically calculates the size of, creates, moves
*  and then destroys the memory device.
*/
int GUI_MEMDEV_Draw(GUI_RECT *pRect, GUI_CALLBACK_VOID_P *pfDraw, void *pData, int NumLines, int Flags)
{
    int x0, y0, x1, y1, xsize, ysize;
    GUI_MEMDEV_Handle hMD;

    if (pRect)
    {
        x0 = (pRect->x0 < 0) ? 0 : pRect->x0;
        y0 = (pRect->y0 < 0) ? 0 : pRect->y0;
#if GUI_NUM_LAYERS == 1
        x1 = _Min(pRect->x1, LCD_GET_XSIZE() - 1);
        y1 = _Min(pRect->y1, LCD_GET_YSIZE() - 1);
#else
        x1 = _Min(pRect->x1, LCD_GetXSizeEx(GUI_Context.SelLayer) - 1);
        y1 = _Min(pRect->y1, LCD_GetYSizeEx(GUI_Context.SelLayer) - 1);
#endif
        xsize = x1 - x0 + 1;
        ysize = y1 - y0 + 1;
    }
    else
    {
        x0 = 0;
        y0 = 0;
#if GUI_NUM_LAYERS == 1
        xsize = LCD_GET_XSIZE();
        ysize = LCD_GET_YSIZE();
#else
        xsize = LCD_GetXSizeEx(GUI_Context.SelLayer);
        ysize = LCD_GetYSizeEx(GUI_Context.SelLayer);
#endif
    }

    if (NumLines == 0)
    {
        NumLines = -ysize;   /* Request <ysize> lines ... Less is o.k. */
    }

    if ((xsize <= 0) || (ysize <= 0))
    {
        return 0;    /* Nothing to do ... */
    }

    /* Create memory device */
    hMD = GUI_MEMDEV_CreateEx(x0, y0, xsize, NumLines, Flags);

    if (!hMD)
    {
        GUI_DEBUG_ERROROUT("GUI_MEMDEV_Draw() Not enough memory ...");            /* Not enough memory ! */
        pfDraw(pData);
        return 1;
    }

    NumLines = GUI_MEMDEV_GetYSize(hMD);
    GUI_MEMDEV_Select(hMD);
    /* Start drawing ... */
    {
        int i;

        for (i = 0; i < ysize; i += NumLines)
        {
            int RemLines = ysize - i;

            if (RemLines < NumLines)
            {
                GUI_MEMDEV_ReduceYSize(hMD, RemLines);
            }

            if (i)
            {
                GUI_MEMDEV_SetOrg(hMD, x0, y0 + i);
                GUI_MEMDEV_Clear(hMD);
            }

            pfDraw(pData);
            GUI_MEMDEV_CopyToLCD(hMD);
        }
    }
    GUI_MEMDEV_Delete(hMD);
    GUI_MEMDEV_Select(0);
    return 0;             /* Success ! */
}

#else

void GUIDEV_Banding(void) {} /* avoid empty object files */

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
