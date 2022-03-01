/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Miracle.M China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : LCDP323.c
* Purpose : Color conversion routines for 323 mode
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
*       LCD_FIXEDPALETTE == 323, 256 colors, BBBGGRRR
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_323
*/
unsigned LCD_Color2Index_323(LCD_COLOR Color)
{
    int r, g, b;
    r = Color & 255;
    g = (Color >> 8) & 255;
    b = Color >> 16;
    r = (r * 7 + 127) / 255;
    g = (g + 42) / 85;
    b = (b * 7 + 127) / 255;
    return r + (g << 3) + (b << 5);
}

/*********************************************************************
*
*       LCD_Index2Color_323
*/
LCD_COLOR LCD_Index2Color_323(int Index)
{
    int r, g, b;
    r = (Index & 7) * 255 / 7;
    g = ((Index >> 3) & 3) * 85;
    b = ((Index >> 5) & 7) * 255 / 7;
    return r + (g << 8) + (((U32)b) << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_323
*/
unsigned LCD_GetIndexMask_323(void)
{
    return 0xff;
}

/*************************** End of file ****************************/
