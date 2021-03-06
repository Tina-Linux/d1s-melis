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
* File    : LCDP655.c
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

#define B_BITS 6
#define G_BITS 5
#define R_BITS 5

#define R_MASK ((1 << R_BITS) -1)
#define G_MASK ((1 << G_BITS) -1)
#define B_MASK ((1 << B_BITS) -1)

/*********************************************************************
*
*       Public code,
*
*       LCD_FIXEDPALETTE == 655, 65536 colors, BBBBBBGGGGGRRRRR
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Color2Index_655
*/
unsigned LCD_Color2Index_655(LCD_COLOR Color)
{
    int r, g, b;
    r = (Color >> (8  - R_BITS)) & R_MASK;
    g = (Color >> (16 - G_BITS)) & G_MASK;
    b = (Color >> (24 - B_BITS)) & B_MASK;
    return r + (g << R_BITS) + (b << (G_BITS + R_BITS));
}

/*********************************************************************
*
*       LCD_Index2Color_655
*/
LCD_COLOR LCD_Index2Color_655(int Index)
{
    unsigned int r, g, b;
    /* Seperate the color masks */
    r = Index                                  & R_MASK;
    g = (Index >> R_BITS)                      & G_MASK;
    b = ((unsigned)Index >> (R_BITS + G_BITS)) & B_MASK;
    /* Convert the color masks */
    r = r * 255 / R_MASK;
    g = g * 255 / G_MASK;
    b = b * 255 / B_MASK;
    return r + (g << 8) + (((U32)b) << 16);
}

/*********************************************************************
*
*       LCD_GetIndexMask_655
*/
unsigned LCD_GetIndexMask_655(void)
{
    return 0xffff;
}

/*************************** End of file ****************************/
