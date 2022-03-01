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
* File    : GUI_SetLBorder.C
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SetLBorder
*
* Purpose:
*   Sets the left border (for carriage return).
*/
int GUI_SetLBorder(int x)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.LBorder;
    GUI_Context.LBorder = x;
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
