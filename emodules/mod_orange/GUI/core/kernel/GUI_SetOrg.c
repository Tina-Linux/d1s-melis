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
* File    : GUI_SetOrg.C
* Purpose : Defines the GUI_SetOrg function
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
*       GUI_SetOrg
*/
void GUI_SetOrg(int x, int y)
{
    GUI_LOCK();
    LCD_L0_SetOrg(x, y);
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
