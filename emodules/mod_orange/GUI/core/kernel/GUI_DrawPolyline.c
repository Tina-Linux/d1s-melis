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
* File    : GUI_DrawPolyLine.c
* Purpose : Implementation of GUI_DrawPolyline
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 11:42:00
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _DrawPolyLine
*
* Draw Poly line, which does not have to be a closed shape
*/
static void _DrawPolyLine(const GUI_POINT *pPoints, int NumPoints, int x0, int y0)
{
    GL_MoveTo(pPoints->x + x0, pPoints->y + y0);

    while (--NumPoints > 0)
    {
        pPoints++;
        GL_DrawLineTo(pPoints->x + x0, pPoints->y + y0);
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
*       GUI_DrawPolyLine
*/
void GUI_DrawPolyLine(const GUI_POINT *pPoints, int NumPoints, int x0, int y0)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    WM_ADDORG(x0, y0);
    WM_ITERATE_START(NULL);
    {
#endif
        _DrawPolyLine(pPoints, NumPoints, x0, y0);
#if (GUI_WINSUPPORT)
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
