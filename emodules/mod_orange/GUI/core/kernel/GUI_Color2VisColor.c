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
* File    : GUI_Color2VisColor.c
* Purpose : Implementation of GUI_Color2VisColor
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
*       GUI_Color2VisColor
*/
GUI_COLOR GUI_Color2VisColor(GUI_COLOR color)
{
    I16 Index;
    GUI_COLOR r;
    GUI_LOCK();
    Index = LCD_Color2Index(color);
    r = LCD_Index2Color(Index);
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_ColorIsAvailable
*/
char GUI_ColorIsAvailable(GUI_COLOR color)
{
    return (GUI_Color2VisColor(color) == color) ? 1 : 0;
}

/*********************************************************************
*
*       GUI_CalcVisColorError
*/
U32 GUI_CalcVisColorError(GUI_COLOR color)
{
    return GUI_CalcColorDist(color, GUI_Color2VisColor(color));
}

/*************************** End of file ****************************/
