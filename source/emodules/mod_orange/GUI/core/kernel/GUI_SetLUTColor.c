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
* File    : GUI_SetLUTColor.C
* Purpose : Implementation of GUI_SetLUTColor
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
*       GUI_SetLUTColor
*/
void GUI_SetLUTColor(U8 Pos, LCD_COLOR Color)
{
#if LCD_PHYSCOLORS_IN_RAM
    GUI_LOCK();
    LCD_PhysColors[Pos] = Color;
    LCD_SetLUTEntry(Pos, Color);
    GUI_UNLOCK();
#else
    GUI_USE_PARA(Pos);
    GUI_USE_PARA(Color);
#endif
}

/*************************** End of file ****************************/
