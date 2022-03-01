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
* File    : LCD1.c
* Purpose : Color conversion routines for 1 bpp (b/w) mode
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_kernel_private.h"


/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       LCD_Color2Index_1
*
* Purpose
*   API table for this color conversion mode. Only used by memory
*   devices in this mode.
*/
const LCD_API_COLOR_CONV LCD_API_ColorConv_1 =
{
    LCD_Color2Index_1,
    LCD_Index2Color_1,
    LCD_GetIndexMask_1
};

#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_1
*/
unsigned LCD_Color2Index_1(LCD_COLOR Color)
{
    int r, g, b;
    r = Color      & 255;
    g = (Color >> 8) & 255;
    b = Color >> 16;
    return (r + g + b + 383) / (3 * 255);
}

/*********************************************************************
*
*       LCD_Index2Color_1
*/
LCD_COLOR LCD_Index2Color_1(int Index)
{
    return Index ? 0xFFFFFF : 0;
}

/*********************************************************************
*
*       LCD_GetIndexMask_1
*/
unsigned LCD_GetIndexMask_1(void)
{
    return 0x01;
}

/*************************** End of file ****************************/
