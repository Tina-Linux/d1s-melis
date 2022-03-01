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
* File    : LCD_DrawVLine.c
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
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

#define CLIP_X() \
    if (x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0) { x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0; } \
    if (x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1) { x1 = GUI_Context.ClipRect.x1; }

#define CLIP_Y() \
    if (y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0) { y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0; } \
    if (y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1) { y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1; }

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_DrawVLine
*/
void LCD_DrawVLine(int x, int y0,  int y1)
{
    /* Perform clipping and check if there is something to do */
    RETURN_IF_X_OUT();
    CLIP_Y();

    if (y1 < y0)
    {
        return;
    }

    /* Call driver to draw */
    LCDDEV_L0_DrawVLine(x, y0, y1);
}

/*************************** End of file ****************************/
