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
* File    : GUI_GetStringDistX.c
* Purpose : Implementation of optional routine
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
#include <string.h>

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetStringDistX
*/
int GUI_GetStringDistX(const char GUI_UNI_PTR *s)
{
    return GUI__GetLineDistX(s, GUI__strlen(s));
}

/*************************** End of file ****************************/
