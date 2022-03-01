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
* File    : GUI_SetColor.c
* Purpose : Implementation of optional routines
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
*       GUI_SetBkColor
*/
void GUI_SetBkColor(GUI_COLOR color)
{
    GUI_LOCK();
    {
        LCD_SetBkColor(color);
    }
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_SetColor
*/
void GUI_SetColor(GUI_COLOR color)
{
    GUI_LOCK();
    {
        LCD_SetColor(color);
    }
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
