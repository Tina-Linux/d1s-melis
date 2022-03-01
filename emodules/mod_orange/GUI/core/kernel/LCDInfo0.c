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
* File    : LCDInfo0.c
* Purpose : Routines returning info at runtime
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

int LCD_GetXSize(void)
{
    return Screen_Context.LCDXSize[GUI_Context.SelLayer];
}
int LCD_GetYSize(void)
{
    return Screen_Context.LCDYSize[GUI_Context.SelLayer];
}
int LCD_GetVXSize(void)
{
    return Screen_Context.LCDXSize[GUI_Context.SelLayer];
}
int LCD_GetVYSize(void)
{
    return Screen_Context.LCDYSize[GUI_Context.SelLayer];
}
int LCD_GetBitsPerPixel(void)
{
    return LCD_BITSPERPIXEL_0;
}
U32 LCD_GetNumColors(void)
{
    return LCD_NUM_COLORS;
}
int LCD_GetYMag(void)
{
    return LCD_YMAG;
}
int LCD_GetXMag(void)
{
    return LCD_XMAG;
}
int LCD_GetFixedPalette(void)
{
    return LCD_FIXEDPALETTE;
}

/*************************** End of file ****************************/
