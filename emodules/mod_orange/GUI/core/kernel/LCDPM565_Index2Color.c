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
* File    : LCDPM565_Index2Color.c
* Purpose : Color conversion routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_kernel_private.h"

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
*       LCD_Index2Color_M565
*/
LCD_COLOR LCD_Index2Color_M565(int Index)
{
    unsigned int r, g, b;
    /* Seperate the color masks */
    // __msg("here!\n");
    b = Index                                  & B_MASK;
    g = (Index >> B_BITS)                      & G_MASK;
    r = ((unsigned)Index >> (B_BITS + G_BITS)) & R_MASK;
    /* Convert the color masks */
    r = r * 255 / R_MASK;
    g = g * 255 / G_MASK;
    b = b * 255 / B_MASK;
    return r + (g << 8) + (((U32)b) << 16);
}

/*************************** End of file ****************************/
