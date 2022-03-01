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
* File    : GUI__GetFontSizeY.c
* Purpose : Implementation of optional routine
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetFontSizeY
*/
int GUI__GetFontSizeY(void)
{
    return GUI_Context.pAFont->YSize * GUI_Context.pAFont->YMag;
}

/*************************** End of file ****************************/
