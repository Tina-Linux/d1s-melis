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
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispStringInRectWrap
*/
__s32 GUI_DispStringInRectWrap(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, GUI_WRAPMODE WrapMode)
{
    const GUI_RECT *pOldClipRect;
    GUI_RECT Rect;
    int xSize, ySizeFont, NumBytesToNextLine, NumCharsDisp;
    __s32 ret_cnt = 0;

    if (!s)
    {
        return ret_cnt;
    }

    GUI_LOCK();
    Rect      = *pRect;
    xSize     = Rect.x1 - Rect.x0 + 1;
    ySizeFont = GUI_GetFontDistY();

    /* Vertical alignment */
    if ((TextAlign & GUI_TA_VERTICAL) != GUI_TA_TOP)
    {
        int NumLines = 0;
        const char GUI_UNI_PTR *pText;
        pText = s;

        /* Count the number of lines */
        do
        {
            NumBytesToNextLine = GUI__WrapGetNumBytesToNextLine(pText, xSize, WrapMode);
            pText += NumBytesToNextLine;

            if (NumBytesToNextLine)
            {
                NumLines++;
            }
        } while (NumBytesToNextLine);

        /* Do the vertical alignment */
        switch (TextAlign & GUI_TA_VERTICAL)
        {
            case GUI_TA_BASELINE:
            case GUI_TA_BOTTOM:
                Rect.y0 = Rect.y1 - NumLines * ySizeFont + 1;
                break;

            case GUI_TA_VCENTER:
                Rect.y0 = Rect.y0 + (Rect.y1 - Rect.y0 + 1 - NumLines * ySizeFont) / 2;
                break;
        }
    }

    pOldClipRect = GUI_SetClipRect(pRect);

    do
    {
        int         xLineSize;
        int         count;
        GUI_RECT    RectText;
        NumCharsDisp = GUI__WrapGetNumCharsDisp(s, xSize, WrapMode);
        Rect.y1      = Rect.y0 + ySizeFont - 1;
        RectText     = Rect;
        /* Horizontal alignment */
        xLineSize   = GUI__GetLineDistX(s, NumCharsDisp);

        switch (TextAlign & GUI_TA_HORIZONTAL)
        {
            case GUI_TA_HCENTER:
                RectText.x0 = pRect->x0 + (pRect->x1 - pRect->x0 - xLineSize) / 2;
                break;

            case GUI_TA_LEFT:
                RectText.x0 = pRect->x0;
                break;

            case GUI_TA_RIGHT:
                RectText.x0 = pRect->x1 - xLineSize + 1;
                break;
        }

        GUI__DispLine(s, NumCharsDisp, &RectText);
        count = GUI__WrapGetNumBytesToNextLine(s, xSize, WrapMode);
        s += count;
        ret_cnt++;

        if (count == 0)
        {
            break;
        }

        Rect.y0 += ySizeFont;
    } while (*s);

    GUI_SetClipRect(pOldClipRect);
    GUI_UNLOCK();
    return ret_cnt;
}

/*************************** End of file ****************************/
