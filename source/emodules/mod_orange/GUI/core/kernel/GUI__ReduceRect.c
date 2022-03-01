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
* File    : GUI__ReduceRect.c
* Purpose : Implementation of GUI__ReduceRect
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
*       GUI__ReduceRect
*/
void GUI__ReduceRect(GUI_RECT *pDest, const GUI_RECT *pRect, int Dist)
{
    pDest->x0 = pRect->x0 + Dist;
    pDest->x1 = pRect->x1 - Dist;
    pDest->y0 = pRect->y0 + Dist;
    pDest->y1 = pRect->y1 - Dist;
}

/*************************** End of file ****************************/
