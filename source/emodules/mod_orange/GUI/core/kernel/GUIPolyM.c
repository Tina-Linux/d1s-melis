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
* File    : GUIPolyR.c
* Purpose : Polygon rotation
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
**********************************************************************************************************************
*/

#include <math.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MagnifyPolygon
*/
void GUI_MagnifyPolygon(GUI_POINT *pDest, const GUI_POINT *pSrc, int NumPoints, int Mag)
{
    int j;

    for (j = 0; j < NumPoints; j++)
    {
        (pDest + j)->x = (pSrc + j)->x * Mag;
        (pDest + j)->y = (pSrc + j)->y * Mag;
    }
}

/*************************** End of file ****************************/
