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
* File    : GUI_SetLUTEntry.C
* Purpose : Implementation of GUI_SetLUTEntry
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
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
*       GUI_SetLUTEntry
*/
void GUI_SetLUTEntry(U8 Pos, LCD_COLOR Color)
{
    GUI_LOCK();
    LCD_SetLUTEntry(Pos, Color);
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
