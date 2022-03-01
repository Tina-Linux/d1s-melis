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
* File    : GUI_SetTextAlign.C
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
*       GUI_SetTextAlign
*/
int GUI_SetTextAlign(int Align)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.TextAlign;
    GUI_Context.TextAlign = Align;
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
