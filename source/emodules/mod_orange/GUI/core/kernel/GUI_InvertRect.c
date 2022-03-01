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
* File    : GUI_InvertRect.c
* Purpose : Implementation file for GUI_InvertRect
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
*       GUI_InvertRect
*/
void GUI_InvertRect(int x0, int y0, int x1, int y1)
{
    GUI_DRAWMODE PrevDraw;
#if (GUI_WINSUPPORT)
    GUI_RECT r;
#endif
    GUI_LOCK();
    PrevDraw = GUI_SetDrawMode(GUI_DRAWMODE_XOR);
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    WM_ADDORG(x1, y1);
    r.x0 = x0;
    r.x1 = x1;
    r.y0 = y0;
    r.y1 = y1;
    WM_ITERATE_START(&r);
    {
#endif
        LCD_FillRect(x0, y0, x1, y1);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_SetDrawMode(PrevDraw);
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
