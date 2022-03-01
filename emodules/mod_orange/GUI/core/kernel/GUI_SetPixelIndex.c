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
* File    : LCD_SetPixelIndex.C
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define RETURN_IF_Y_OUT() \
    if (y < GUI_Context.ClipRect[GUI_Context.SelLayer].y0) return;             \
    if (y > GUI_Context.ClipRect[GUI_Context.SelLayer].y1) return;

#define RETURN_IF_X_OUT() \
    if (x < GUI_Context.ClipRect[GUI_Context.SelLayer].x0) return;             \
    if (x > GUI_Context.ClipRect[GUI_Context.SelLayer].x1) return;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_SetPixelIndex
*
* Purpose:
*   Writes 1 pixel into the display.
*/
void LCD_SetPixelIndex(int x, int y, int ColorIndex)
{
    RETURN_IF_X_OUT();
    RETURN_IF_Y_OUT();
    LCDDEV_L0_SetPixelIndex(x, y, ColorIndex);
}



/*************************** End of file ****************************/
