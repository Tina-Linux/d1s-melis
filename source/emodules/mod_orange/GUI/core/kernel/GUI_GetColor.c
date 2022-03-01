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
* File    : GUI_GetColor.c
* Purpose : Implementation of different GUI_GetColor routines
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
*       GUI_GetBkColorIndex
*/
int GUI_GetBkColorIndex(void)
{
    int r;
    GUI_LOCK();
    r = LCD_GetBkColorIndex();
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetBkColor
*/
GUI_COLOR GUI_GetBkColor(void)
{
    GUI_COLOR r;
    GUI_LOCK();
    r = LCD_Index2Color(LCD_GetBkColorIndex());
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetColorIndex
*/
int GUI_GetColorIndex(void)
{
    int r;
    GUI_LOCK();
    r = LCD_GetColorIndex();
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetColor
*/
GUI_COLOR GUI_GetColor(void)
{
    GUI_COLOR r;
    GUI_LOCK();
    r = LCD_Index2Color(LCD_GetColorIndex());
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
