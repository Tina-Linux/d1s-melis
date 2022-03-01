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
* File    : memdev_xy2ptr.c
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 8:41:44
**********************************************************************************************************************
*/

#include "gui_core_memdev_private.h"

#if GUI_SUPPORT_MEMDEV

/*********************************************************************
*
*       modul internal routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV__XY2PTR
*/
void *GUI_MEMDEV__XY2PTR(int x, int y)
{
    GUI_MEMDEV *pDev = GUI_MEMDEV_H2P(GUI_Context.hDevData);
    U8 *pData = (U8 *)(pDev + 1);
#if GUI_DEBUG_LEVEL >= GUI_DEBUG_LEVEL_CHECK_ALL

    if ((x >= pDev->x0 + pDev->XSize) | (x < pDev->x0) | (y >= pDev->y0 + pDev->YSize) | (y < pDev->y0))
    {
        GUI_DEBUG_ERROROUT2("GUI_MEMDEV__XY2PTR: parameters out of bounds", x, y);
    }

#endif
    pData += (y - pDev->y0) * pDev->BytesPerLine;
    return pData + (x - pDev->x0) * (pDev->BitsPerPixel / 8);
}

/*********************************************************************
*
*       GUI_MEMDEV__XY2PTREx
*/
void *GUI_MEMDEV__XY2PTREx(GUI_MEMDEV *pDev, int x, int y)
{
    U8 *pData = (U8 *)(pDev + 1);
    pData += y * pDev->BytesPerLine;
    return pData + x * (pDev->BitsPerPixel / 8);
}

#else

void GUIDEV_XY2PTR_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
