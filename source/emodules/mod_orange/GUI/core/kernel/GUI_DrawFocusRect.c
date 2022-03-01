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
* File    : GUI_DrawFocusRect.c
* Purpose : Implementation of GUI_DrawFocusRect
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 11:42:00
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawFocusRect
*/
static void _DrawFocusRect(const GUI_RECT *pr)
{
    int i;

    for (i = pr->x0; i <= pr->x1; i += 2)
    {
        LCD_DrawPixel(i, pr->y0);
        LCD_DrawPixel(i, pr->y1);
    }

    for (i = pr->y0; i <= pr->y1; i += 2)
    {
        LCD_DrawPixel(pr->x0, i);
        LCD_DrawPixel(pr->x1, i);
    }
}
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DrawFocusRect
*/
void GUI_DrawFocusRect(const GUI_RECT *pRect, int Dist)
{
    GUI_RECT r;
    GUI__ReduceRect(&r, pRect, Dist);
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(r.x0, r.y0);
    WM_ADDORG(r.x1, r.y1);
    WM_ITERATE_START(&r);
    {
#endif
        _DrawFocusRect(&r);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
