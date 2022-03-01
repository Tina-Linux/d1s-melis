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
* File    : GUI_MoveRect.c
* Purpose : Implementation of GUI_MoveRect
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
*       GUI_MoveRect
*/
void GUI_MoveRect(GUI_RECT *pRect, int dx, int dy)
{
    if (pRect)
    {
        pRect->x0 += dx;
        pRect->x1 += dx;
        pRect->y0 += dy;
        pRect->y1 += dy;
    }
}

/*************************** End of file ****************************/
