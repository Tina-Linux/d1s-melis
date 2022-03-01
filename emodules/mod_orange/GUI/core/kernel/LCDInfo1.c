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
* File    : LCDInfo1.c
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

#if GUI_NUM_LAYERS > 1
int LCD_GetXSize_1(void)
{
    return Screen_Context.LCDXSize[GUI_Context.SelLayer];
}
int LCD_GetYSize_1(void)
{
    return Screen_Context.LCDYSize[GUI_Context.SelLayer];
}
int LCD_GetVXSize_1(void)
{
    return LCD_VXSIZE_1;
}
int LCD_GetVYSize_1(void)
{
    return LCD_VYSIZE_1;
}
int LCD_GetBitsPerPixel_1(void)
{
    return LCD_BITSPERPIXEL_1;
}
U32 LCD_GetNumColors_1(void)
{
    return LCD_NUM_COLORS_1;
}
int LCD_GetYMag_1(void)
{
    return LCD_YMAG_1;
}
int LCD_GetXMag_1(void)
{
    return LCD_XMAG_1;
}
int LCD_GetFixedPalette_1(void)
{
    return LCD_FIXEDPALETTE_1;
}
#else
int LCD_GetXSize_1(void)
{
    return 0;
}
int LCD_GetYSize_1(void)
{
    return 0;
}
int LCD_GetVXSize_1(void)
{
    return 0;
}
int LCD_GetVYSize_1(void)
{
    return 0;
}
int LCD_GetBitsPerPixel_1(void)
{
    return 0;
}
U32 LCD_GetNumColors_1(void)
{
    return 0;
}
int LCD_GetYMag_1(void)
{
    return 0;
}
int LCD_GetXMag_1(void)
{
    return 0;
}
int LCD_GetFixedPalette_1(void)
{
    return 0;
}
#endif

/*************************** End of file ****************************/
