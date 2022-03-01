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
* File    : GUI_AddDec.c
* Purpose : Routines to display values in decimal form
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
#include "string.h"

/*********************************************************************
*
*       Public routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_AddDec
*/
void GUI_AddDec(I32 v, U8 Len, char **ps)
{
    GUI_AddDecShift(v, Len, 0, ps);
}

/*************************** End of file ****************************/
