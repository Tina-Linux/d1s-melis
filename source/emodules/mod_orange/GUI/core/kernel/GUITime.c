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
* File    : GUITime.c
* Purpose : Time related routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
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
*       GUI_GetTime
*/
int GUI_GetTime(void)
{
    return GUI_X_GetTime();
}

/*********************************************************************
*
*       GUI_Delay
*/


void GUI_Delay(int Period)
{
    GUI_Exec();
    GUI_X_Delay(Period);  /*without os*/
}


/*************************** End of file ****************************/
