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
* File    : memdev_clear.c
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
*       GUI_MEMDEV_Clear
*/
void GUI_MEMDEV_Clear(GUI_MEMDEV_Handle hMem)
{
    if (!hMem)
    {
        if ((hMem = GUI_Context.hDevData) == 0)
        {
            return;
        }
    }

    {
        GUI_MEMDEV *pDev;
        GUI_USAGE_h hUsage;
        GUI_LOCK();
        pDev = GUI_MEMDEV_H2P(hMem);  /* Convert to pointer */
        hUsage = pDev->hUsage;

        if (hUsage)
        {
            GUI_USAGE *pUsage = GUI_USAGE_H2P(hUsage);
            GUI_USAGE_Clear(pUsage);
        }

        GUI_UNLOCK();
    }
}

#else

void GUIDEV_Clear(void) {} /* avoid empty object files */

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
