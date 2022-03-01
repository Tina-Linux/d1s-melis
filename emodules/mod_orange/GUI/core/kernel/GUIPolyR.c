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
* Date    : 2009-2-19 10:58:42
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
*       GUI_RotatePolygon
*/
void GUI_RotatePolygon(GUI_POINT *pDest, const GUI_POINT *pSrc, int NumPoints, float Angle)
{
    int j;
    float fcos = cos(Angle);
    float fsin = sin(Angle);

    for (j = 0; j < NumPoints; j++)
    {
        int x = (pSrc + j)->x;
        int y = (pSrc + j)->y;
        (pDest + j)->x =  x * fcos + y * fsin;
        (pDest + j)->y = -x * fsin + y * fcos;
    }
}

/*************************** End of file ****************************/
