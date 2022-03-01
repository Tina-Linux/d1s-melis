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
* File    : GUI_GetFont.c
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
*       GUI_GetFont
*/
const GUI_FONT GUI_UNI_PTR *GUI_GetFont(void)
{
    const GUI_FONT GUI_UNI_PTR *r;
    GUI_LOCK();
    r = GUI_Context.pAFont;
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
