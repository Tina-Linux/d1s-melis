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
* File    : LCD_GetPixelColor.c
* Purpose : Get Pixel routines
*           Note: These routines are in a module of their own
*           because they are mostly not required to link
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
*       LCD_GetPixelColor
*/
LCD_COLOR LCD_GetPixelColor(int x, int y)
{
    return LCD_Index2Color(LCD_GetPixelIndex(x, y));
}

/*************************** End of file ****************************/
