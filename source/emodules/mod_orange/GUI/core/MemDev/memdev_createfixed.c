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
* File    : memdev_createfixed.c
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
*       Exported code
*
**********************************************************************
*/

GUI_MEMDEV_Handle GUI_MEMDEV_CreateFixed(int x0, int y0,
        int xsize, int ysize, int Flags,
        const tLCDDEV_APIList *pMemDevAPI,
        const LCD_API_COLOR_CONV *pColorConvAPI)
{
    GUI_MEMDEV_Handle hMemDev;
    GUI_LOCK();
    hMemDev = GUI_MEMDEV__CreateFixed(x0, y0, xsize, ysize, Flags, pMemDevAPI,
                                      pColorConvAPI->pfColor2Index,
                                      pColorConvAPI->pfIndex2Color,
                                      pColorConvAPI->pfGetIndexMask,
                                      pMemDevAPI->pfGetPixelIndex);
    GUI_UNLOCK();
    return hMemDev;
}

#else

void GUIDEV_CreateFixed_C(void);
void GUIDEV_CreateFixed_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
