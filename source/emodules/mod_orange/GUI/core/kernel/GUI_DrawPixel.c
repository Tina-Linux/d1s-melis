/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lemoDn
* File    : GUI_DrawPixel.c
* Purpose : Implementation of GUI_DrawPixel
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 11:42:00
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DrawPixel
*/
void GUI_DrawPixel(int x, int y)
{
#if (GUI_WINSUPPORT)
    GUI_RECT r;
#endif
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x, y);
    r.x0 = r.x1 = x;
    r.y0 = r.y1 = y;
    WM_ITERATE_START(&r);
    {
#endif
        LCD_HL_DrawPixel(x, y);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
