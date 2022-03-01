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
* File    : memdev_setorg.c
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
*       GUI_MEMDEV_SetOrg
*/
void GUI_MEMDEV_SetOrg(GUI_MEMDEV_Handle hMem, int x0, int y0)
{
    /* Make sure memory handle is valid */
    if (!hMem)
    {
        if ((hMem = GUI_Context.hDevData) == 0)
        {
            return;
        }
    }

    GUI_LOCK();
    {
        GUI_MEMDEV *pDev = GUI_MEMDEV_H2P(hMem);  /* Convert to pointer */
        pDev->y0 = y0;
        pDev->x0 = x0;
        LCD_SetClipRectMax();

        /* Move usage along */
        if (pDev->hUsage)
        {
            GUI_USAGE *pUsage = GUI_USAGE_H2P(pDev->hUsage);

            if (((pUsage->XSize = pDev->XSize) != 0) && ((pUsage->YSize = pDev->YSize) != 0))
            {
                pUsage->x0 = x0;
                pUsage->y0 = y0;
            }
        }
    }
    GUI_UNLOCK();
}

#else

void GUIDEV_SetOrg(void) {} /* avoid empty object files */

#endif /* GUI_MEMDEV_SUPPORT */

/*************************** end of file ****************************/
