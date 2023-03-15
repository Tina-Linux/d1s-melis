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
#include <stddef.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GL_DrawBitmap
*
* Purpose:
*  Translates the external bitmap format into an internal
*  format. This turned out to be necessary as the internal
*  format is easier to create and more flexible for routines
*  that draw text-bitmaps.
*/
void GL_DrawBitmap(const GUI_BITMAP GUI_UNI_PTR *pBitmap, int x0, int y0)
{
    GUI_DRAWMODE PrevDraw;
    const GUI_LOGPALETTE GUI_UNI_PTR *pPal;
    pPal = pBitmap->pPal;
    PrevDraw = GUI_SetDrawMode(0);  /* No Get... at this point */
    GUI_SetDrawMode((pPal && pPal->HasTrans) ? (PrevDraw | GUI_DRAWMODE_TRANS) : PrevDraw & (~GUI_DRAWMODE_TRANS));

    if (pBitmap->pMethods)
    {
#if GUI_COMPILER_SUPPORTS_FP      /* Do not support this on VERY simple chips and compilers */
        pBitmap->pMethods->pfDraw(x0, y0, pBitmap->XSize, pBitmap->YSize, (U8 const *)pBitmap->pData, pBitmap->pPal, 1, 1);
#endif
    }
    else
    {
        const LCD_PIXELINDEX *pTrans;
        pTrans = LCD_GetpPalConvTable(pBitmap->pPal);

        if (!pTrans)
        {
            pTrans = (pBitmap->BitsPerPixel != 1) ? NULL : &LCD_BKCOLORINDEX;
        }

        LCD_DrawBitmap(x0, y0
                       , pBitmap->XSize, pBitmap->YSize
                       , 1, 1
                       , pBitmap->BitsPerPixel
                       , pBitmap->BytesPerLine
                       , pBitmap->pData
                       , pTrans);
    }

    GUI_SetDrawMode(PrevDraw);
}

/*********************************************************************
*
*       GUI_DrawBitmap
*/
void GUI_DrawBitmap(const GUI_BITMAP GUI_UNI_PTR *pBitmap, int x0, int y0)
{
#if (GUI_WINSUPPORT)
    GUI_RECT r;
#endif
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    r.x1 = (r.x0 = x0) + pBitmap->XSize - 1;
    r.y1 = (r.y0 = y0) + pBitmap->YSize - 1;
    WM_ITERATE_START(&r)
    {
#endif
        GL_DrawBitmap(pBitmap, x0, y0);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
