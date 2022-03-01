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
* File    : LCD_ClipRectEx.c
* Purpose : Implementation of optional routines
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
*       LCD_SetClipRectEx
*
* Purpose:
*   This function is actually a driver function.
*   Since it is identical for all drivers with only one controller,
*   it is placed here.
*   For multi-controller systems, this routine is placed in the
*   distribution driver.
*/

void LCD_SetClipRectEx(const GUI_RECT *pRect)
{
    LCD_RECT r;
    LCDDEV_L0_GetRect(&r);
    GUI__IntersectRects(&GUI_Context.ClipRect[GUI_Context.SelLayer], pRect, &r);
}

/*************************** End of file ****************************/
