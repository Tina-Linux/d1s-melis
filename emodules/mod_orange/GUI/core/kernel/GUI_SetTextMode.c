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
* File    : GUI_SetTextMode.C
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:56:10
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
*       GUI_SetTextMode
*/
int GUI_SetTextMode(int Mode)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.TextMode;
    GUI_Context.TextMode = Mode;
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
