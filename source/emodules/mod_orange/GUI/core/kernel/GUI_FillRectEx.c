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
* File    : GUI_FillRectEx.c
* Purpose : Implementation of GUI_FillRectEx
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
*       GUI_FillRectEx
*/
void GUI_FillRectEx(const GUI_RECT *pRect)
{
	__wrn("GUI_FillRectEx  %d %d %d %d  add= %p",pRect->x0, pRect->y0, pRect->x1, pRect->y1,pRect);
    GUI_FillRect(pRect->x0, pRect->y0, pRect->x1, pRect->y1);
}

/*************************** End of file ****************************/
