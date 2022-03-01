/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_DispString.c
* Purpose : Implementation of GUI_DispString function.
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
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
void GUI_DispString(const char GUI_UNI_PTR *s)
{
    int xAdjust, yAdjust, xOrg;
    int FontSizeY;

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
        int xLineSize;
        int LineNumChars = GUI__GetLineNumChars(s, 0x7fff);

        if (LineNumChars == 0)
        {
            break;
        }

        xLineSize    = GUI__GetLineDistX(s, LineNumChars);

        /* Check if x-position needs to be changed due to h-alignment */
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
        GUI__DispLine(s, LineNumChars, &r);
        GUI_Context.DispPosY[GUI_Context.SelLayer] = r.y0;
        s += GUI_UC__NumChars2NumBytes(s, LineNumChars);

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

/*************************** End of file ****************************/
