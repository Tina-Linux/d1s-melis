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
* File    : GUI_DrawRectEx.c
* Purpose : Implementation of GUI_DrawRectEx
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 11:42:00
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
*       GUI_DrawRectEx
*/
void GUI_DrawRectEx(const GUI_RECT *pRect)
{
    GUI_DrawRect(pRect->x0, pRect->y0, pRect->x1, pRect->y1);
}

/*************************** End of file ****************************/
