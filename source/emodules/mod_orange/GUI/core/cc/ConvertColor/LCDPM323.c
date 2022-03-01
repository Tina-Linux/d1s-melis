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
* File    : LCDPM323.c
* Purpose : Color conversion routines for -323 mode
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Public code,
*
*       LCD_FIXEDPALETTE == -323, 256 colors, RRRGGBBB
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_M323
*/
unsigned LCD_Color2Index_M323(LCD_COLOR Color)
{
    unsigned r, g, b;
    r = Color & 255;
    g = (Color >> 8) & 255;
    b = Color >> 16;
    r = (r * 7 + 127) / 255;
    g = (g + 42) / 85;
    b = (b * 7 + 127) / 255;
    return b + (g << 3) + (r << 5);
}

/*********************************************************************
*
*       LCD_Index2Color_M323
*/
LCD_COLOR LCD_Index2Color_M323(int Index)
{
    unsigned r, g, b;
    b = (Index & 7) * 255 / 7;
    g = ((Index >> 3) & 3) * 85;
    r = ((Index >> 5) & 7) * 255 / 7;
    return r + (g << 8) + (((U32)b) << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_M323
*/
unsigned LCD_GetIndexMask_M323(void)
{
    return 0xff;
}

/*************************** End of file ****************************/
