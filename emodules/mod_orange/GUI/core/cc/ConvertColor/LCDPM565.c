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
* File    : LCDM565.c
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

#define B_BITS 5
#define G_BITS 6
#define R_BITS 5

#define R_MASK ((1 << R_BITS) -1)
#define G_MASK ((1 << G_BITS) -1)
#define B_MASK ((1 << B_BITS) -1)

/*********************************************************************
*
*       Public code,
*
*       LCD_FIXEDPALETTE == 565, 65536 colors, RRRRRGGGGGGBBBBB
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_M565
*/
unsigned LCD_Color2Index_M565(LCD_COLOR Color)
{
    int r, g, b;
    r = (Color >> (8  - R_BITS)) & R_MASK;
    g = (Color >> (16 - G_BITS)) & G_MASK;
    b = (Color >> (24 - B_BITS)) & B_MASK;
    return b + (g << B_BITS) + (r << (G_BITS + B_BITS));
}

/*********************************************************************
*
*       LCD_GetIndexMask_M565
*/
unsigned LCD_GetIndexMask_M565(void)
{
    return 0xffff;
}

/*************************** End of file ****************************/
