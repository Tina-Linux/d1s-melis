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
* File    : LCD_GetColorIndex.c
* Purpose : Implementation of different LCD_GetColorIndex routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
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
*       LCD_GetBkColorIndex
*/
int LCD_GetBkColorIndex(void)
{
    return LCD_BKCOLORINDEX;
}

/*********************************************************************
*
*       LCD_GetColorIndex
*/
int LCD_GetColorIndex(void)
{
    return LCD_COLORINDEX;
}

/*************************** End of file ****************************/
