/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : orange
* File    : GUI_DispStringInRectWrap.c
* Purpose :  \n  auto
*
* By      : Derek
* Version : v1.0
* Date    : 2010-11-03 10:58:42
**********************************************************************************************************************
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
*       _GetNoWrap
*/
static __s32 UC_GetNoWrap(const __u16 GUI_UNI_PTR *s)
{
    return GUI__UC_GetLineNumChars(s, 0x7FFF);
}
/*********************************************************************
*
*       _GetCharWrap
*/
static __s32 UC_GetCharWrap(const __u16 GUI_UNI_PTR *s, __s32 xSize)
{
    __s32 xDist = 0, NumChars = 0;
    __u16 Char;
    while ((Char = *s++) != 0)
    {
        xDist += GUI_GetCharDistX(Char);
        if ((NumChars && (xDist > xSize)) || (Char == '\n') || (Char == '\r'))
        {
            break;
        }
        NumChars++;
    }
    return NumChars;
}
/*********************************************************************
*
*       _GetWordWrap
*/
static __s32 UC_GetWordWrap(const __u16 GUI_UNI_PTR *s, __s32 xSize)
{
    __s32 xDist = 0, NumChars = 0, WordWrap = 0;
    __u16 Char, PrevChar = 0;
    while (1)
    {
        Char = *s++;   /* Similar to:  *s++ */
        /* Let's first check if the line end is reached. In this case we are done. */
        if (_IsLineEnd(Char, s))
        {
            WordWrap = NumChars;
            break;
        }
        /* If current character is a space, we found a wrap position */
        if ((Char == ' ') && (Char != PrevChar))
        {
            WordWrap = NumChars;
        }
        PrevChar = Char;
        xDist += GUI_GetCharDistX(Char);
        if ((xDist <= xSize) || (NumChars == 0))
        {
            NumChars++;
        }
        else
        {
            break;
        }
    }
    if (!WordWrap)
    {
        WordWrap = NumChars;
    }
    return WordWrap;
}
/*********************************************************************
*
*       GUI__WrapGetNumCharsDisp
*
* Returns:
*  Number of characters to display in the line.
*  Trailing spaces and line end character are
*  not counted
*/
static __s32 GUI__UC_WrapGetNumCharsDisp(const __u16 GUI_UNI_PTR *pText, __s32 xSize, GUI_WRAPMODE WrapMode)
{
    __s32 r;
    switch (WrapMode)
    {
        case GUI_WRAPMODE_WORD:
            r = UC_GetWordWrap(pText, xSize);
            break;
        case GUI_WRAPMODE_CHAR:
            r = UC_GetCharWrap(pText, xSize);
            break;
        default:
            r = UC_GetNoWrap(pText);
    }
    return r;
}

/*********************************************************************
*
*       GUI__WrapGetNumCharsToNextLine
*/
static __s32 GUI__UC_WrapGetNumCharsToNextLine(const __u16 GUI_UNI_PTR *pText, __s32 xSize, GUI_WRAPMODE WrapMode)
{
    __s32 NumChars;
    __u16 Char;
    NumChars = GUI__UC_WrapGetNumCharsDisp(pText, xSize, WrapMode);
    pText   += NumChars;
    Char     = *pText++;
    if (Char == '\n')
    {
        NumChars++;
    }
    else if (Char == '\r')
    {
        NumChars++;
        Char = *pText++;

        if (Char == '\n')
        {
            NumChars++;
        }
    }
    else
    {
        if (WrapMode == GUI_WRAPMODE_WORD)
        {
            while (Char == ' ')
            {
                NumChars++;
                Char = *pText++;
            }
        }
    }
    return NumChars;
}
/*********************************************************************
*
*       GUI__WrapGetNumBytesToNextLine
*/
static __s32 GUI__UC_WrapGetNumBytesToNextLine(const __u16 GUI_UNI_PTR *pText, __s32 xSize, GUI_WRAPMODE WrapMode)
{
    __s32 NumChars;
    NumChars = GUI__UC_WrapGetNumCharsToNextLine(pText, xSize, WrapMode);
    //NumBytes = GUI_UC__NumChars2NumBytes(pText, NumChars);
    return NumChars;
}
/*********************************************************************
*
*       GUI_DispStringInRectWrap
*/
void GUI_UC_DispStringInRectWrap(const __u16 GUI_UNI_PTR *s, GUI_RECT *pRect, __s32 TextAlign, GUI_WRAPMODE WrapMode)
{
    const GUI_RECT *pOldClipRect;
    GUI_RECT Rect;
    __s32 xSize, ySizeFont, NumBytesToNextLine, NumCharsDisp, NumLines;
    const __u16 GUI_UNI_PTR *pText;

    if (!s)
    {
        return;
    }

    GUI_LOCK();
    Rect      = *pRect;
    xSize     = Rect.x1 - Rect.x0 + 1;
    ySizeFont = GUI_GetFontDistY();
    // Vertical alignment
    if ((TextAlign & GUI_TA_VERTICAL) != GUI_TA_TOP)
    {
        NumLines = 0;
        pText = s;
        // Count the number of lines
        do
        {
            NumBytesToNextLine = GUI__UC_WrapGetNumBytesToNextLine(pText, xSize, WrapMode);
            pText += NumBytesToNextLine;
            if (NumBytesToNextLine)
            {
                NumLines++;
            }
        } while (NumBytesToNextLine);
        // Do the vertical alignment
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
        __s32 xLineSize;
        __s32 count;
        GUI_RECT RectText;
        NumCharsDisp = GUI__UC_WrapGetNumCharsDisp(s, xSize, WrapMode);
        Rect.y1      = Rect.y0 + ySizeFont - 1;
        RectText     = Rect;
        // Horizontal alignment
        xLineSize = GUI__UC_GetLineDistX(s, NumCharsDisp);
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
        //GUI__DispLine(s, NumCharsDisp, &RectText);
        _DispLine(s, NumCharsDisp, &RectText);
        count = GUI__UC_WrapGetNumBytesToNextLine(s, xSize, WrapMode);
        s += count;
        if (count == 0)
        {
            break;
        }
        Rect.y0 += ySizeFont;
    } while (*s);

    GUI_SetClipRect(pOldClipRect);
    GUI_UNLOCK();
}

/*********************************************************************
*
*       _GetCharWrap
*/
static __s32 _GetMultipLine(const __u16 GUI_UNI_PTR *s, __s32 xSize)
{
    __s32 xDist = 0, num_chars = 0;
    __u16 Char;
    while ((Char = *s++) != 0)
    {
        xDist += GUI_GetCharDistX(Char);
        if (Char != '\n')
        {
            if (num_chars && (xDist > xSize))
            {
                //temp_xDist = xDist - GUI_GetCharDistX(Char);
                return num_chars;
            }
            num_chars++;
        }
        else
        {
            return 0;
        }
    }
}

/*********************************************************************
*
*       _GetCharWrap
*/
static __s32 _GetNumCharsPara(const __u16 GUI_UNI_PTR *s)
{
    __u16 Char;
    __s32 NumChars = 0;
    while ((Char = *s++) != 0)
    {
        if (Char == '\n')
        {
            break;
        }
        NumChars++;
    }
    return NumChars;
}
/*********************************************************************
*
*       _GetCharWrap
*/
static __s32 _GetDispLine(__s32 NumChars, __s32 tag_disp_multip_line)
{
    __s32 disp_line;

    if ((NumChars % tag_disp_multip_line) == 0)
    {
        disp_line = NumChars / tag_disp_multip_line;
    }
    else
    {
        disp_line = (NumChars / tag_disp_multip_line) + 1;
    }

    return disp_line;
}

/*********************************************************************
*
*       _GetCharWrap
*/
static __s32 _GetNumCharsDisp(const __u16 GUI_UNI_PTR *s, __s32 xSize)
{
    __s32 xDist = 0, NumChars = 0;
    __u16 Char;
    while ((Char = *s--) != 0)
    {
        xDist += GUI_GetCharDistX(Char);
        if ((NumChars && (xDist > xSize)) || (Char == '\n'))
        {
            break;
        }
        NumChars++;
    }
    return NumChars;
}

/*********************************************************************
*
*       GUI_DispStringInRectWrap
*/
void GUI_UC_DispStringInRectWrapEX(const __u16 GUI_UNI_PTR *s, GUI_RECT *pRect, __s32 TextAlign, GUI_WRAPMODE WrapMode, __u16 *baselevel)
{
    const GUI_RECT *pOldClipRect;
    GUI_RECT Rect;
    __s32 xSize, ySizeFont, NumBytesToNextLine, NumCharsDisp, NumLines;
    const __u16 GUI_UNI_PTR *pText;
    __epdk_charset_enm_e charset;
    //__u16 * temp_string;

    if (!s)
    {
        return;
    }
    charset = GUI_Context.charset_enum;

    if ((EPDK_CHARSET_ENM_UTF8 == charset) || (EPDK_CHARSET_ENM_UTF16BE == charset) || (EPDK_CHARSET_ENM_UTF16LE == charset)
        || (EPDK_CHARSET_ENM_ISO_8859_6 == charset) || (EPDK_CHARSET_ENM_CP1255 == charset) || (EPDK_CHARSET_ENM_CP1256 == charset) || (EPDK_CHARSET_ENM_ISO_8859_8 == charset))
    {
        __s32 tag_disp_multip_line;
        __s32 xLineSize, count, disp_line, i, num_chars, re_num_chars, disp_num_chars = 0;
        GUI_RECT RectText;
        __u16 para_baselevel;

        GUI_LOCK();
        Rect      = *pRect;
        xSize     = Rect.x1 - Rect.x0 + 1;
        ySizeFont = GUI_GetFontDistY();
        /* Vertical alignment */
        if ((TextAlign & GUI_TA_VERTICAL) != GUI_TA_TOP)
        {
            NumLines = 0;
            pText = s;
            /* Count the number of lines */
            do
            {
                NumBytesToNextLine = GUI__UC_WrapGetNumBytesToNextLine(pText, xSize, WrapMode);
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
            //__s32 xLineSize;
            //__s32 count, disp_line,i;
            //GUI_RECT RectText;

            tag_disp_multip_line = _GetMultipLine(s, xSize);
            if (0 == tag_disp_multip_line)
            {
                NumCharsDisp = GUI__UC_WrapGetNumCharsDisp(s, xSize, WrapMode);
                Rect.y1      = Rect.y0 + ySizeFont - 1;
                RectText     = Rect;
                // Horizontal alignment
                xLineSize = GUI__UC_GetLineDistX(s, NumCharsDisp);
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
                //GUI__DispLine(s, NumCharsDisp, &RectText);
                _DispLine(s, NumCharsDisp, &RectText);
                count = GUI__UC_WrapGetNumBytesToNextLine(s, xSize, WrapMode);
                s += count;
                if (count == 0)
                {
                    break;
                }
                Rect.y0 += ySizeFont;
                baselevel++;
            }
            else
            {
                para_baselevel = *baselevel++;
                if (0 == para_baselevel)
                {
                    num_chars = _GetNumCharsPara(s);
                    disp_line = _GetDispLine(num_chars, tag_disp_multip_line);
                    //temp_string = s;

                    //s += (num_chars - tag_disp_multip_line);
                    //s += num_chars;

                    for (i = 0; i < disp_line - 1; i++)
                    {
                        //s -= tag_disp_multip_line;
                        NumCharsDisp = GUI__UC_WrapGetNumCharsDisp(s, xSize, WrapMode);
                        Rect.y1      = Rect.y0 + ySizeFont - 1;
                        RectText     = Rect;
                        /* Horizontal alignment */
                        xLineSize = GUI__UC_GetLineDistX(s, NumCharsDisp);  //NumCharsDisp
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
                        //GUI__DispLine(s, NumCharsDisp, &RectText);
                        _DispLine(s, NumCharsDisp, &RectText);

                        s += NumCharsDisp;
                        disp_num_chars += NumCharsDisp;

                        Rect.y0 += ySizeFont;
                    }
                    /*每行中的剩余字符*/
                    {
                        re_num_chars = num_chars - disp_num_chars;
                        disp_num_chars = 0;
                        //s -= re_num_chars;

                        Rect.y1      = Rect.y0 + ySizeFont - 1;
                        RectText     = Rect;
                        /* Horizontal alignment */
                        xLineSize = GUI__UC_GetLineDistX(s, re_num_chars);  //NumCharsDisp
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
                        //GUI__DispLine(s, NumCharsDisp, &RectText);
                        _DispLine(s, re_num_chars, &RectText);

                        //s += (re_num_chars + 1);
                        s += re_num_chars;
                        if (*s == 0)
                        {
                            break;
                        }
                        else
                        {
                            s++;
                        }
                        Rect.y0 += ySizeFont;
                    }
                }
                else if (1 == para_baselevel)
                {
                    num_chars = _GetNumCharsPara(s);
                    disp_line = _GetDispLine(num_chars, tag_disp_multip_line);
                    //temp_string = s;

                    //s += (num_chars - tag_disp_multip_line);
                    s += (num_chars - 1);

                    for (i = 0; i < disp_line - 1; i++)
                    {
                        //NumCharsDisp = GUI__UC_WrapGetNumCharsDisp(s, xSize, WrapMode);
                        NumCharsDisp = _GetNumCharsDisp(s, xSize);
                        s -= (NumCharsDisp - 1);
                        Rect.y1      = Rect.y0 + ySizeFont - 1;
                        RectText     = Rect;
                        /* Horizontal alignment */
                        xLineSize = GUI__UC_GetLineDistX(s, NumCharsDisp);  //NumCharsDisp
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
                        //GUI__DispLine(s, NumCharsDisp, &RectText);
                        _DispLine(s, NumCharsDisp, &RectText);

                        s--;
                        disp_num_chars += NumCharsDisp;

                        Rect.y0 += ySizeFont;
                    }
                    /*每行中的剩余字符*/
                    {
                        re_num_chars = num_chars - disp_num_chars;
                        disp_num_chars = 0;
                        s -= (re_num_chars - 1);

                        Rect.y1      = Rect.y0 + ySizeFont - 1;
                        RectText     = Rect;
                        /* Horizontal alignment */
                        xLineSize = GUI__UC_GetLineDistX(s, re_num_chars);  //NumCharsDisp
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
                        //GUI__DispLine(s, NumCharsDisp, &RectText);
                        _DispLine(s, re_num_chars, &RectText);

                        Rect.y0 += ySizeFont;
                    }

                    //s += (num_chars + 1);
                    s += num_chars;
                    if (*s == 0)
                    {
                        break;
                    }
                    else
                    {
                        s++;
                    }

                }
                else
                {
                    __wrn("baselevel is invalid!");
                }
            }

        } while (*s);

        GUI_SetClipRect(pOldClipRect);
        GUI_UNLOCK();
    }
    else
    {
        GUI_UC_DispStringInRectWrap(s, pRect, TextAlign, WrapMode);
    }
}

/*************************** End of file ****************************/

