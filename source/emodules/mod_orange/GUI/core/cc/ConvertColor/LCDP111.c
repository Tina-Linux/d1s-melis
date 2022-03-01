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
* File    : LCD111.c
* Purpose : Color conversion routines for 111 color mode
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Public code, LCD_FIXEDPALETTE == 111, 8 basic colors
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_111
*/
unsigned LCD_Color2Index_111(LCD_COLOR Color)
{
    int r, g, b;
    r = (Color >> (0 + 7))  & 1;
    g = (Color >> (8 + 7))  & 1;
    b = (Color >> (16 + 7)) & 1;
    return r + (g << 1) + (b << 2);
}

/*********************************************************************
*
*       LCD_Index2Color_111
*/
LCD_COLOR LCD_Index2Color_111(int Index)
{
    U16 r, g, b;
    r = (((Index >> 0) & 1) * 0xff);
    g = (((Index >> 1) & 1) * 0xff);
    b = (Index >> 2)   * 0xff;
    return r | (g << 8) | ((U32)b << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_111
*/
unsigned LCD_GetIndexMask_111(void)
{
    return 0x0007;
}

/*********************************************************************
*
*       Public code, LCD_FIXEDPALETTE == 111, 8 basic colors, SWAP_RB
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_M111
*/
unsigned LCD_Color2Index_M111(LCD_COLOR Color)
{
    int r, g, b;
    r = (Color >> (0 + 7))  & 1;
    g = (Color >> (8 + 7))  & 1;
    b = (Color >> (16 + 7)) & 1;
    return b + (g << 1) + (r << 2);
}

/*********************************************************************
*
*       LCD_Index2Color_M111
*/
LCD_COLOR LCD_Index2Color_M111(int Index)
{
    U16 r, g, b;
    r = (((Index >> 0) & 1) * 0xff);
    g = (((Index >> 1) & 1) * 0xff);
    b = (Index >> 2)   * 0xff;
    return b | (g << 8) | ((U32)r << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_M111
*/
unsigned LCD_GetIndexMask_M111(void)
{
    return 0x0007;
}

/*************************** End of file ****************************/
