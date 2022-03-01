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
* File    : GUIARCFloat.c
* Purpose : Draw Arc routines based on floating point
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
*       _DrawPie
*/
static void _DrawPie(int x0, int y0, unsigned int r, int Angle0, int Angle1, int Type)
{
    int PenSizeOld;
    PenSizeOld = GUI_GetPenSize();
    GUI_SetPenSize(r);
    r /= 2;
    GL_DrawArc(x0, y0, r, r, Angle0, Angle1);
    GUI_SetPenSize(PenSizeOld);
    GUI_USE_PARA(Type);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DrawPie
*/
void GUI_DrawPie(int x0, int y0, int r, int a0, int a1, int Type)
{
    GUI_LOCK();
#if GUI_WINSUPPORT
    WM_ADDORG(x0, y0);
    WM_ITERATE_START(NULL)
    {
#endif
        _DrawPie(x0, y0, r, a0, a1, Type);
#if GUI_WINSUPPORT
    }
    WM_ITERATE_END();
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
