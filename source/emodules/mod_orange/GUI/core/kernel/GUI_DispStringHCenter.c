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
* File    : GUI_DispStringHCenter.c
* Purpose : Implementation of optional routine
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
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
*       GUI_DispStringHCenterAt
*/
void GUI_DispStringHCenterAt(const char GUI_UNI_PTR *s, int x, int y)
{
    int Align;
    Align = GUI_SetTextAlign((GUI_Context.TextAlign & ~GUI_TA_LEFT) | GUI_TA_CENTER);
    GUI_DispStringAt(s, x, y);
    GUI_SetTextAlign(Align);
}

/*************************** End of file ****************************/
