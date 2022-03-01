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
* File    : GUI_Color2Index.c
* Purpose : Core routines
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
*       GUI_Color2Index
*/
int GUI_Color2Index(GUI_COLOR Color)
{
    int Index;
    GUI_LOCK();
    Index = LCD_Color2Index(Color);
    GUI_UNLOCK();
    return Index;
}

/*************************** End of file ****************************/
