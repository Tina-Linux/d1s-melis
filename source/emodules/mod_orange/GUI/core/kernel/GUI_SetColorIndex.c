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
* File    : GUI_DrawBitmap.c
* Purpose : Implementation of GUI_DrawBitmap
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:51:50
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
*       GUI_SetBkColorIndex
*/
void GUI_SetBkColorIndex(int Index)
{
    GUI_LOCK();
    {
        GUI_Context.BkColor = GUI_INVALID_COLOR;
        LCD_SetBkColorIndex(Index);
    }
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_SetColorIndex
*/
void GUI_SetColorIndex(int Index)
{
    GUI_LOCK();
    {
        GUI_Context.Color = GUI_INVALID_COLOR;
        LCD_SetColorIndex(Index);
    }
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
