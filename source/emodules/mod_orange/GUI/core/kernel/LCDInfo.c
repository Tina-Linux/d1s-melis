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
* File    : LCDInfo.c
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
/*********************************************************************
*
*       LCD_GetDevCap
*/
I32 LCD_GetDevCap(int Index)
{
    switch (Index)
    {
        case LCD_DEVCAP_NUMCOLORS:
            return LCD_NUM_COLORS;

        case LCD_DEVCAP_XSIZE:
            return LCD_GetXSize();

        case LCD_DEVCAP_YSIZE:
            return LCD_GetYSize();

        case LCD_DEVCAP_VXSIZE:
            return LCD_VXSIZE;

        case LCD_DEVCAP_VYSIZE:
            return LCD_VYSIZE;

        case LCD_DEVCAP_CONTROLLER:
            return LCD_CONTROLLER;

        case LCD_DEVCAP_BITSPERPIXEL:
            return LCD_BITSPERPIXEL;
    }

    if ((Index >= LCD_DEVCAP_COLOR) && (Index < LCD_DEVCAP_COLOR + LCD_NUM_COLORS))
    {
        return LCD_Index2Color(Index - LCD_DEVCAP_COLOR);
    }

    return 0;
}

/*************************** End of file ****************************/
