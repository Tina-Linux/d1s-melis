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
* File    : GUIIndex2Color.c
* Purpose : Converts a color index to a RGB-value
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
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
*       GUI_Index2Color
*/
GUI_COLOR GUI_Index2Color(int Index)
{
    GUI_COLOR Color;
    GUI_LOCK();
    Color = LCD_Index2Color(Index);
    GUI_UNLOCK();
    return Color;
}

/*************************** End of file ****************************/
