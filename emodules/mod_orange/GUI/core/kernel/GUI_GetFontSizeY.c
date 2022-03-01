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
* File    : GUI_GetFontSizeY.c
* Purpose : Implementation of optional routine
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
*       GUI_GetFontSizeY
*/
int GUI_GetFontSizeY(void)
{
    int r;
    GUI_LOCK();
    r = GUI__GetFontSizeY();
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
