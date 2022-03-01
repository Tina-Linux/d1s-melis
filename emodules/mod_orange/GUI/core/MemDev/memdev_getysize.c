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
* File    : memdev_getysize.c
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
*       GUI_MEMDEV_GetYSize
*/
int GUI_MEMDEV_GetYSize(GUI_MEMDEV_Handle hMem)
{
    int r = 0;
    GUI_MEMDEV *pDevData;
    GUI_LOCK();

    if (hMem == 0)
    {
        hMem = GUI_Context.hDevData;
    }

    if (hMem)
    {
        pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */
        r = pDevData->YSize;
    }

    GUI_UNLOCK();
    return r;
}

#else

void GUIDEV_GetYSize(void) {} /* avoid empty object files */

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
