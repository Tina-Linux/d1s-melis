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
*       GUI_DispString
*/
static void GUI_UC_DispString(const __u16 GUI_UNI_PTR *s)
{
    __s32 xAdjust, yAdjust, xOrg;
    __s32 FontSizeY;
    if (!s)
    {
        return;
    }
    GUI_LOCK();
    FontSizeY = GUI_GetFontDistY();
    xOrg = GUI_Context.DispPosX[GUI_Context.SelLayer];
    /* Adjust vertical position */
    yAdjust = GUI_GetYAdjust();
    GUI_Context.DispPosY[GUI_Context.SelLayer] -= yAdjust;
    for (; *s; s++)
    {
        GUI_RECT r;
        __s32 xLineSize;
        __s32 LineNumChars = GUI__UC_GetLineNumChars(s, 0x7fff);
        if (LineNumChars == 0)
        {
            break;
        }
        xLineSize    = GUI__UC_GetLineDistX(s, LineNumChars);
        /*   Check if x-position needs to be changed due to h-alignment */
        switch (GUI_Context.TextAlign & GUI_TA_HORIZONTAL)
        {
            case GUI_TA_CENTER:
                xAdjust = xLineSize / 2;
                break;
            case GUI_TA_RIGHT:
                xAdjust = xLineSize;
                break;
            default:
                xAdjust = 0;
        }
        r.x0 = GUI_Context.DispPosX[GUI_Context.SelLayer] -= xAdjust;
        r.x1 = r.x0 + xLineSize - 1;
        r.y0 = GUI_Context.DispPosY[GUI_Context.SelLayer];
        r.y1 = r.y0 + FontSizeY - 1;
        _DispLine(s, LineNumChars, &r);
        GUI_Context.DispPosY[GUI_Context.SelLayer] = r.y0;
        s += LineNumChars;
        if ((*s == '\n') || (*s == '\r'))
        {
            switch (GUI_Context.TextAlign & GUI_TA_HORIZONTAL)
            {
                case GUI_TA_CENTER:
                case GUI_TA_RIGHT:
                    GUI_Context.DispPosX[GUI_Context.SelLayer] = xOrg;
                    break;
                default:
                    GUI_Context.DispPosX[GUI_Context.SelLayer] = GUI_Context.LBorder;
                    break;
            }
            if ((*s == '\n') || (*s == '\r'))
            {
                GUI_Context.DispPosY[GUI_Context.SelLayer] += FontSizeY;
            }
        }
        else
        {
            GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x0 + xLineSize;
        }
        if (*s == 0)    /* end of string (last line) reached ? */
        {
            break;
        }
    }
    GUI_Context.DispPosY[GUI_Context.SelLayer] += yAdjust;
    GUI_Context.TextAlign &= ~GUI_TA_HORIZONTAL;
    GUI_UNLOCK();
}
/*********************************************************************
*
*       GUI_DispStringAt
*/
void GUI_UC_DispStringAt(const __u16 GUI_UNI_PTR *s, __s32 x, __s32 y)
{
    GUI_LOCK();
    GUI_Context.DispPosX[GUI_Context.SelLayer] = x;
    GUI_Context.DispPosY[GUI_Context.SelLayer] = y;
    GUI_UC_DispString(s);
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
