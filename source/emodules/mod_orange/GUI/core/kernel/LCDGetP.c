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
* File    : LCDGetP.c
* Purpose : Get Pixel routines
*           Note: These routines are in a module of their own
*           because they are mostly not required to link
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
*       LCD_GetPixelIndex
*
* NOTE:
*   We can not use the standard clipping which we use for drawing
*   operations as it is perfectly legal to read pixels outside of
*   the clipping area. We therefor get the bounding rectangle of the
*   device and use it for clipping.
*/
unsigned LCD_GetPixelIndex(int x, int y)
{
    LCD_RECT r;
    GUI_ASSERT_LOCK();
    LCDDEV_L0_GetRect(&r);

    if (x < r.x0)
    {
        return 0;
    }

    if (x > r.x1)
    {
        return 0;
    }

    if (y < r.y0)
    {
        return 0;
    }

    if (y > r.y1)
    {
        return 0;
    }

    return LCDDEV_L0_GetPixelIndex(x, y);
}

/*************************** End of file ****************************/
