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
* File    : GUI_DrawVLine.c
* Purpose : Implementation of GUI_DrawVLine
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
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
*       GUI_DrawVLine
*/
void GUI_DrawVLine(int x0, int y0, int y1)
{
#if (GUI_WINSUPPORT)
    GUI_RECT r;
#endif
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    WM_ADDORGY(y1);
    r.x1 = r.x0 = x0;
    r.y0 = y0;
    r.y1 = y1;
    WM_ITERATE_START(&r);
    {
#endif
        LCD_DrawVLine(x0, y0, y1);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
