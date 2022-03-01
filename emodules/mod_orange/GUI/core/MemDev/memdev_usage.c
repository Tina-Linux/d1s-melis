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
* File    : memdev_usage.c
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
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_USAGE_Select
*/
void GUI_USAGE_Select(GUI_USAGE_Handle hUsage)
{
    GUI_MEMDEV *pDev;
    GUI_LOCK();
    pDev = GUI_MEMDEV_H2P(GUI_Context.hDevData);
    pDev->hUsage = hUsage;
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_USAGE_DecUseCnt
*
* Purpose: Decrements the usage count and deletes the usage object if
*          the counter reaches 0.
*/
void GUI_USAGE_DecUseCnt(GUI_USAGE_Handle  hUsage)
{
    GUI_USAGE *pThis;
    GUI_LOCK();
    pThis = GUI_USAGE_H2P(hUsage);

    if (--pThis->UseCnt == 0)
    {
        GUI_ALLOC_Free(hUsage);
    }

    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_USAGE_AddRect
*
* Parameters:
*   hUsage: Handle to usage object. May not be 0 !
*/
void GUI_USAGE_AddRect(GUI_USAGE  *pUsage, int x0, int y0, int xSize, int ySize)
{
    do
    {
        GUI_USAGE_AddHLine(pUsage, x0, y0++, xSize);
    } while (--ySize);
}

#else

void GUIDEV_Usage(void) {} /* avoid empty object files */

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
