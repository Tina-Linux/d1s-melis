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
* File    : LCD_UpdateColorIndices.c
* Purpose : Implementation of LCD_UpdateColorIndices
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
*       LCD_UpdateColorIndices
*/
void LCD_UpdateColorIndices(void)
{
    LCD_SetColorIndex(LCD_Color2Index(GUI_Context.Color));
    LCD_SetBkColorIndex(LCD_Color2Index(GUI_Context.BkColor));
}

/*************************** End of file ****************************/
