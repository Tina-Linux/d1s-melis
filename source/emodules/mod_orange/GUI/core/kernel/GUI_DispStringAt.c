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
* File    : GUI_DispStringAt.c
* Purpose : Implementation of optional routines
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
*       GUI_DispStringAt
*/
void GUI_DispStringAt(const char GUI_UNI_PTR *s, int x, int y)
{
    GUI_LOCK();
    GUI_Context.DispPosX[GUI_Context.SelLayer] = x;
    GUI_Context.DispPosY[GUI_Context.SelLayer] = y;
    GUI_DispString(s);
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
