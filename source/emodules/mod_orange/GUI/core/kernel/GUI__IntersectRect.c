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
* File    : GUI_IntersectRect.c
* Purpose : Implementation of GUI_IntersectRect
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__IntersectRects
*
* Purpose:
*   Calc intersection of rectangles
*
* Add. info:
*   Rectangles are passed as pointers. These pointers need to be valid;
*   a NULL pointer may not be passed. There is no check for NULL pointers
*   implemented in order to avoid avoid performance penalty.
*   There is a similar function available, GUI__IntersectRects(),
*   which takes 3 parameters and
*   has a return value. Note that this one should be preferred because
*   it is considerably faster and the call requires one parameter less.
*/
void GUI__IntersectRect(GUI_RECT *pDest, const GUI_RECT *pr0)
{
    if (pDest->x0 < pr0->x0)
    {
        pDest->x0 = pr0->x0;
    }

    if (pDest->y0 < pr0->y0)
    {
        pDest->y0 = pr0->y0;
    }

    if (pDest->x1 > pr0->x1)
    {
        pDest->x1 = pr0->x1;
    }

    if (pDest->y1 > pr0->y1)
    {
        pDest->y1 = pr0->y1;
    }
}

/*************************** End of file ****************************/
