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
* File    : GUI_GetClientRect.c
* Purpose : Implementation of GUI_GetClientRect
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
*       GUI_GetClientRect
*/
void GUI_GetClientRect(GUI_RECT *pRect)
{
    if (!pRect)
    {
        return;
    }

#if GUI_WINSUPPORT
    WM_GetClientRect(pRect);
#else
    pRect->x0 = 0;
    pRect->y0 = 0;
    pRect->x1 = LCD_GET_XSIZE();
    pRect->y1 = LCD_GET_YSIZE();
#endif
}

/*************************** End of file ****************************/
