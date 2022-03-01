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
* File    : GUI_SetLineStyle.C
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
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
*       GUI_SetLineStyle
*/
U8 GUI_SetLineStyle(U8 LineStyle)
{
    U8 r;
    GUI_LOCK();
    r = GUI_Context.LineStyle;
    GUI_Context.LineStyle = LineStyle;
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
