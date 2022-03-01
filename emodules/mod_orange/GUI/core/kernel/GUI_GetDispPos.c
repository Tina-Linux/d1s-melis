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
* File    : GUI_GetDispPos.c
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetDispPosX
*
* Purpose:
*   Return X-component of current display position
*/
int GUI_GetDispPosX(void)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.DispPosX[GUI_Context.SelLayer];
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetDispPosY
*
* Purpose:
*   Return Y-component of current display position
*/
int GUI_GetDispPosY(void)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.DispPosY[GUI_Context.SelLayer];
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
