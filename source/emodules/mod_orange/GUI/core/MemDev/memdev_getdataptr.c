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
* File    : memdev_getdataptr.c
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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MEMDEV_GetDataPtr
*/
void *GUI_MEMDEV_GetDataPtr(GUI_MEMDEV_Handle hMem)
{
    GUI_MEMDEV *pDev;
    void *pData;
    GUI_LOCK();                   /* Needed so the memory management does not complain */
    pDev  = GUI_MEMDEV_H2P(hMem);
    GUI_UNLOCK();
    pData = (void *)(pDev + 1);
    return pData;
}


#else

void GUI_MEMDEV_GetDataPtr_C(void) {}

#endif /* GUI_SUPPORT_MEMDEV */

/*************************** end of file ****************************/
