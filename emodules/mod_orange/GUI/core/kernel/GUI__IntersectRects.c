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
* File    : GUI_IntersectRects.c
* Purpose : Implementation of GUI_IntersectRects
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Macros
*
**********************************************************************
*/

/*********************************************************************
*
*       Public code
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
* Return value:
*   1 if they do intersect,
*   0 if there is no intersection
*
* Add. info:
*   Rectangles are passed as pointers. These pointers need to be valid;
*   a NULL pointer may not be passed. There is no check for NULL pointers
*   implemented in order to avoid avoid performance penalty.
*/
int GUI__IntersectRects(GUI_RECT *pDest, const GUI_RECT *pr0, const GUI_RECT *pr1)
{
    pDest->x0 = MAX(pr0->x0, pr1->x0);
    pDest->y0 = MAX(pr0->y0, pr1->y0);
    pDest->x1 = MIN(pr0->x1, pr1->x1);
    pDest->y1 = MIN(pr0->y1, pr1->y1);

    if (pDest->x1 < pDest->x0)
    {
        return 0;
    }

    if (pDest->y1 < pDest->y0)
    {
        return 0;
    }

    return 1;
}

/*************************** End of file ****************************/
