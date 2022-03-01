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
* File    : LCDP233.c
* Purpose : Color conversion routines for 233 mode
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
*       LCD_FIXEDPALETTE == 233, 256 colors, BBGGGRRR
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_233
*/
unsigned LCD_Color2Index_233(LCD_COLOR Color)
{
    int r, g, b;
    r = Color & 255;
    g = (Color >> 8) & 255;
    b = Color >> 16;
    r = (r * 7 + 127) / 255;
    g = (g * 7 + 127) / 255;
    b = (b + 42) / 85;
    return r + (g << 3) + (b << 6);
}

/*********************************************************************
*
*       LCD_Index2Color_233
*/
LCD_COLOR LCD_Index2Color_233(int Index)
{
    int r, g, b;
    r = (Index & 7) * 255 / 7;
    g = ((Index >> 3) & 7) * 255 / 7;
    b = ((Index >> 6) & 3) * 85;
    return r + (g << 8) + (((U32)b) << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_233
*/
unsigned LCD_GetIndexMask_233(void)
{
    return 0xff;
}

/*************************** End of file ****************************/
