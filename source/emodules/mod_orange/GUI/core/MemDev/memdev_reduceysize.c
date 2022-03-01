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
* File    : memdev_reduceysize.c
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
*       GUI_MEMDEV_ReduceYSize
*/
void GUI_MEMDEV_ReduceYSize(GUI_MEMDEV_Handle hMem, int YSize)
{
    /* Make sure memory handle is valid */
    if (!hMem)
    {
        hMem = GUI_Context.hDevData;
    }

    if (!hMem)
    {
        return;
    }

    {
        GUI_MEMDEV *pDevData;
        GUI_LOCK();
        pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */

        if (YSize < pDevData->YSize)
        {
            pDevData->YSize = YSize;
        }

        GUI_UNLOCK();
    }
}

#else

void GUIDEV_ReduceYSize(void) {} /* avoid empty object files */

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
