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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__DispStringInRect
*/
void GUI__DispStringInRect(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, int MaxNumChars)
{
    GUI_RECT r;
    GUI_RECT rLine;
    int y = 0;
    const char GUI_UNI_PTR *sOrg = s;
    int FontYSize;
    int xLine = 0;
    int LineLen;
    int NumCharsRem;           /* Number of remaining characters */
    FontYSize = GUI_GetFontSizeY();

    if (pRect)
    {
        r = *pRect;
    }
    else
    {
        GUI_GetClientRect(&r);
    }

    /* handle vertical alignment */
    if ((TextAlign & GUI_TA_VERTICAL) == GUI_TA_TOP)
    {
        y = r.y0;
    }
    else
    {
        int NumLines;

        /* Count the number of lines */
        for (NumCharsRem = MaxNumChars, NumLines = 1; NumCharsRem ; NumLines++)
        {
            LineLen = GUI__GetLineNumChars(s, NumCharsRem);

            if (LineLen == 0)
            {
                break;
            }

            NumCharsRem -= LineLen;
            s += GUI_UC__NumChars2NumBytes(s, LineLen);

            if (GUI__HandleEOLine(&s))
            {
                break;
            }
        }

        /* Do the vertical alignment */
        switch (TextAlign & GUI_TA_VERTICAL)
        {
            case GUI_TA_BASELINE:
            case GUI_TA_BOTTOM:
                y = r.y1 - NumLines * FontYSize + 1;
                break;

            case GUI_TA_VCENTER:
                y = r.y0 + (r.y1 - r.y0 + 1 - NumLines * FontYSize) / 2;
                break;
        }
    }

    /* Output string */
    for (NumCharsRem = MaxNumChars, s = sOrg; NumCharsRem;)
    {
        int xLineSize;
        LineLen = GUI__GetLineNumChars(s, NumCharsRem);

        if (LineLen == 0)
        {
            break;
        }

        NumCharsRem -= LineLen;
        xLineSize = GUI__GetLineDistX(s, LineLen);

        switch (TextAlign & GUI_TA_HORIZONTAL)
        {
            case GUI_TA_HCENTER:
                xLine = r.x0 + (r.x1 - r.x0 - xLineSize) / 2;
                break;

            case GUI_TA_LEFT:
                xLine = r.x0;
                break;

            case GUI_TA_RIGHT:
                xLine = r.x1 - xLineSize + 1;
        }

        rLine.x0 = GUI_Context.DispPosX[GUI_Context.SelLayer] = xLine;
        rLine.x1 = rLine.x0 + xLineSize - 1;
        rLine.y0 = GUI_Context.DispPosY[GUI_Context.SelLayer] = y;
        rLine.y1 = y + FontYSize - 1;
        GUI__DispLine(s, LineLen, &rLine);
        s += GUI_UC__NumChars2NumBytes(s, LineLen);
        y += GUI_GetFontDistY();

        if (GUI__HandleEOLine(&s))
        {
            break;
        }
    }
}

/*********************************************************************
*
*       GUI_DispStringInRectMax
*/
#if (GUI_WINSUPPORT)
void GUI_DispStringInRectMax(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, int MaxLen)
{
    if (s)
    {
        const GUI_RECT *pOldClipRect = NULL;
        GUI_RECT r;
        GUI_LOCK();

        if (pRect)
        {
            pOldClipRect = WM_SetUserClipRect(pRect);

            if (pOldClipRect)
            {
                GUI__IntersectRects(&r, pRect, pOldClipRect);
                WM_SetUserClipRect(&r);
            }
        }

        GUI__DispStringInRect(s, pRect, TextAlign, MaxLen);
        WM_SetUserClipRect(pOldClipRect);
        GUI_UNLOCK();
    }
}

#else

void GUI_DispStringInRectMax(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign, int MaxLen)
{
    GUI_RECT Rect_Old, r;

    if (s && pRect)
    {
        GUI_LOCK();
        Rect_Old = GUI_Context.ClipRect[GUI_Context.SelLayer];
        GUI__IntersectRects(&r, pRect, &Rect_Old);
        LCD_SetClipRectEx(&r);
        GUI__DispStringInRect(s, pRect, TextAlign, MaxLen);
        LCD_SetClipRectEx(&Rect_Old);
        GUI_UNLOCK();
    }
}
#endif

/*********************************************************************
*
*       GUI_DispStringInRect
*/
void GUI_DispStringInRect(const char GUI_UNI_PTR *s, GUI_RECT *pRect, int TextAlign)
{
    GUI_DispStringInRectMax(s, pRect, TextAlign, 0x7fff);
}

/*************************** End of file ****************************/
