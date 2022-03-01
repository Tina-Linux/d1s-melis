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
* File    : memdev_write.c
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
*       Exported routines
*
**********************************************************************
*/
/*********************************************************************
*
*        GUI_MEMDEV_WriteAt
*/
void GUI_MEMDEV_WriteAt(GUI_MEMDEV_Handle hMem, int x, int y)
{
    if (hMem)
    {
        GUI_MEMDEV *pDevData;
#if (GUI_WINSUPPORT)
        GUI_RECT r;
#endif
        GUI_LOCK();
        pDevData = (GUI_MEMDEV *) GUI_ALLOC_h2p(hMem); /* Convert to pointer */

        if (x == GUI_POS_AUTO)
        {
            x = pDevData->x0;
            y = pDevData->y0;
        }

#if (GUI_WINSUPPORT)
        /* Calculate rectangle */
        r.x1 = (r.x0 = x) + pDevData->XSize - 1;
        r.y1 = (r.y0 = y) + pDevData->YSize - 1;;
        /* Do the drawing. WIndow manager has to be on */
        WM_ITERATE_START(&r)
        {
            GUI_MEMDEV__WriteToActiveAt(hMem, x, y);
        }
        WM_ITERATE_END();
#else
        GUI_MEMDEV__WriteToActiveAt(hMem, x, y);
#endif
        GUI_UNLOCK();
    }
}

/*********************************************************************
*
*       GUI_MEMDEV_Write
*/
void GUI_MEMDEV_Write(GUI_MEMDEV_Handle hMem)
{
    GUI_MEMDEV_WriteAt(hMem, GUI_POS_AUTO, GUI_POS_AUTO);
}

#else

void GUIDEV_Write_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
