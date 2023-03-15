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
*********************************************************************/
/*********************************************************************
*
*       GUI_UC_GetLineDistX
*
*  This routine is used to calculate the length of a line in pixels.
*/
__s32 GUI__UC_GetLineDistX(const __u16 GUI_UNI_PTR *s, __s32 MaxNumChars)
{
    __s32 Dist = 0;

    if (s)
    {
        __u16 Char;
        while (--MaxNumChars >= 0)
        {
            //Char  = GUI_UC__GetCharCodeInc(&s);
            Char  = *s++;
            Dist += GUI_GetCharDistX(Char);
        }
    }

    return Dist;
}
/*********************************************************************
*
*       GUI__UC_HandleEOLine
*
* Is called when processing strings which may consist of
* multiple lines after a line has been processed. It will
* a) Swall the line feed character (if it is there)
* b) Return 1 if end of string, otherwise 0
*/
static __s32 GUI__UC_HandleEOLine(const __u16 GUI_UNI_PTR **ps)
{
    const __u16 GUI_UNI_PTR *s = *ps;
    __u16 c = *s++;

    if (c == 0x0000)
    {
        return 1;
    }
    if (c == 0x000A)
    {
        *ps = s;
    }

    return 0;
}
/*********************************************************************
*
*       GUI__UC_GetLineNumChars
*/
__s32 GUI__UC_GetLineNumChars(const __u16 GUI_UNI_PTR *s, __s32 MaxNumChars)
{
    __s32 NumChars = 0;
    if (s)
    {
        for (; NumChars < MaxNumChars; NumChars++)
        {
            if ((*s == 0x0000) || (*s == 0x000A))
            {
                break;
            }
            s++;
        }
    }
    return NumChars;
}

/*********************************************************************
*
*       GUI__UC_DispStringInRect
*/
static void GUI__UC_DispStringInRect(const __u16 GUI_UNI_PTR *s, GUI_RECT *pRect, __s32 TextAlign, __s32 MaxNumChars)
{
    GUI_RECT r;
    GUI_RECT rLine;
    __s32 y = 0;
    const __u16 GUI_UNI_PTR *sOrg = s;
    __s32 FontYSize;
    __s32 xLine = 0;
    __s32 LineLen;
    __s32 NumCharsRem;           /* Number of remaining characters */

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
        __s32 NumLines;
        /* Count the number of lines */
        for (NumCharsRem = MaxNumChars, NumLines = 1; NumCharsRem ; NumLines++)
        {
            LineLen = GUI__UC_GetLineNumChars(s, NumCharsRem);
            if (LineLen == 0)
            {
                break;
            }
            NumCharsRem -= (LineLen + 1);    //LineLen
            //s += GUI_UC__NumChars2NumBytes(s, LineLen);
            s += (LineLen + 1);    //LineLen
            /*if (GUI__UC_HandleEOLine(&s))
            {
                  break;
            }*/
            if (NumCharsRem <= 0)
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
        __s32 xLineSize;
        LineLen = GUI__UC_GetLineNumChars(s, NumCharsRem);
        if ((LineLen == 0) && (NumCharsRem <= 0))
        {
            break;
        }

        NumCharsRem -= (LineLen + 1);        //LineLen
        xLineSize = GUI__UC_GetLineDistX(s, LineLen);
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
        //GUI__DispLine(s, LineLen, &rLine);
        _DispLine(s, LineLen, &rLine);
        s += (LineLen + 1);    //LineLen
        y += GUI_GetFontDistY();
        /*if (GUI__UC_HandleEOLine(&s))
        {
            break;
        }*/
        if (NumCharsRem <= 0)
        {
            break;
        }
    }
}

/*********************************************************************
*
*       GUI_UC_DispStringInRectMax
*/
#if (GUI_WINSUPPORT)
static void GUI_UC_DispStringInRectMax(const __u16 GUI_UNI_PTR *s, GUI_RECT *pRect, __s32 TextAlign, __s32 MaxLen)
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
        GUI__UC_DispStringInRect(s, pRect, TextAlign, MaxLen);
        WM_SetUserClipRect(pOldClipRect);
        GUI_UNLOCK();
    }
}

#else

static void GUI_UC_DispStringInRectMax(const __u16 GUI_UNI_PTR *s, GUI_RECT *pRect, __s32 TextAlign, __s32 MaxLen)
{
    GUI_RECT Rect_Old, r;
    if (s && pRect)
    {
        GUI_LOCK();
        Rect_Old = GUI_Context.ClipRect[GUI_Context.SelLayer];
        GUI__IntersectRects(&r, pRect, &Rect_Old);
        LCD_SetClipRectEx(&r);
        GUI__UC_DispStringInRect(s, pRect, TextAlign, MaxLen);
        LCD_SetClipRectEx(&Rect_Old);
        GUI_UNLOCK();
    }
}
#endif

/*********************************************************************
*
*       GUI_UC_DispStringInRect
*/
void GUI_UC_DispStringInRect(const __u16 GUI_UNI_PTR *s, GUI_RECT *pRect, __s32 TextAlign, __s32 MaxLen)
{
    GUI_UC_DispStringInRectMax(s, pRect, TextAlign, MaxLen);
}

/*************************** End of file ****************************/
