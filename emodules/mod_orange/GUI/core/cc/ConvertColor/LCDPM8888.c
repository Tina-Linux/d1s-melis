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
* File    : LCDM888.c
* Purpose : Color conversion routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_cc_private.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define B_BITS 8
#define G_BITS 8
#define R_BITS 8
#define A_BITS 8

#define R_MASK ((1 << R_BITS) -1)
#define G_MASK ((1 << G_BITS) -1)
#define B_MASK ((1 << B_BITS) -1)
#define A_MASK ((1 << A_BITS) -1)
/*********************************************************************
*
*       Public code,
*
*       LCD_FIXEDPALETTE == 888, 16777216 colors, RRRRRRRRGGGGGGGGBBBBBBBB
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_M888
*/
unsigned LCD_Color2Index_M8888(LCD_COLOR Color)
{
    int a, r, g, b;
    r = (Color >> (8  - R_BITS)) & R_MASK;
    g = (Color >> (16 - G_BITS)) & G_MASK;
    b = (Color >> (24 - B_BITS)) & B_MASK;
    a = (Color >> (32 - A_BITS)) & A_MASK;
    return b + (g << B_BITS) + (r << (G_BITS + B_BITS)) + (a << (G_BITS + B_BITS + R_BITS));
}


/*********************************************************************
*
*       LCD_Index2Color_M888
*/
LCD_COLOR LCD_Index2Color_M8888(int Index)
{
    unsigned int a, r, g, b;
    /* Seperate the color masks */
    b = Index                                            & B_MASK;
    g = (Index >> B_BITS)                                & G_MASK;
    r = ((unsigned)Index >> (B_BITS + G_BITS))           & R_MASK;
    a = ((unsigned)Index >> (B_BITS + G_BITS + R_BITS))  & A_MASK;
    /* Convert the color masks */
    r = r * 255 / R_MASK;
    g = g * 255 / G_MASK;
    b = b * 255 / B_MASK;
    a = a * 255 / A_MASK;
    return r + (g << 8) + (((U32)b) << 16) + (((U32)a) << 24);
}
/*********************************************************************
*
*       LCD_GetIndexMask_M565
*/
unsigned LCD_GetIndexMask_M8888(void)
{
    return 0xffffffff;
}

/*************************** End of file ****************************/
