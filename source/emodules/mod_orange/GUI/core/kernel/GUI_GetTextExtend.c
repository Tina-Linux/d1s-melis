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
* File    : GUI_GetTextExtend.c
* Purpose : Implementation of GUI_GetTextExtend
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
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
*       GUI_GetTextExtend
*/
void GUI_GetTextExtend(GUI_RECT *pRect, const char GUI_UNI_PTR *s, int MaxNumChars)
{
    int xMax      = 0;
    int NumLines  = 0;
    int LineSizeX = 0;
    U16 Char;
    pRect->x0 = GUI_Context.DispPosX[GUI_Context.SelLayer];
    pRect->y0 = GUI_Context.DispPosY[GUI_Context.SelLayer];

    while (MaxNumChars--)
    {
        Char = GUI_UC__GetCharCodeInc(&s);

        if ((Char == '\n') || (Char == 0))
        {
            if (LineSizeX > xMax)
            {
                xMax = LineSizeX;
            }

            LineSizeX = 0;
            NumLines++;

            if (!Char)
            {
                break;
            }
        }
        else
        {
            LineSizeX += GUI_GetCharDistX(Char);
        }
    }

    if (LineSizeX > xMax)
    {
        xMax = LineSizeX;
    }

    if (!NumLines)
    {
        NumLines = 1;
    }

    pRect->x1 = pRect->x0 + xMax - 1;
    pRect->y1 = pRect->y0 + GUI__GetFontSizeY() * NumLines - 1;
}

/*************************** End of file ****************************/
