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
* File    : LCD2.c
* Purpose : Color conversion routines for 4 bpp gray LCDs
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Public data
*
**********************************************************************
*/

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       LCD_Color2Index_2
*
* Purpose
*   API table for this color conversion mode. Only used by memory
*   devices in this mode.
*/
const LCD_API_COLOR_CONV LCD_API_ColorConv_2 =
{
    LCD_Color2Index_2,
    LCD_Index2Color_2,
    LCD_GetIndexMask_2
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
*       LCD_Color2Index_2
*/
unsigned LCD_Color2Index_2(LCD_COLOR Color)
{
    int r, g, b;
    r = Color      & 255;
    g = (Color >> 8) & 255;
    b = Color >> 16;
    return (r + g + b + 127) / 255;
}

/*********************************************************************
*
*       LCD_Index2Color_2
*/
LCD_COLOR LCD_Index2Color_2(int Index)
{
    return ((U32)Index) * 0x555555;
}

/*********************************************************************
*
*       LCD_GetIndexMask_2
*/
unsigned LCD_GetIndexMask_2(void)
{
    return 0x03;
}

/*************************** End of file ****************************/
