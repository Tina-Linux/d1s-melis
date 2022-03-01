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
* File    : GUI_SetKeyMsgHook.c
* Purpose : Implementation of GUI_SetKeyMsgHook
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
*       GUI_SetKeyMsgHook
*/
GUI_KEY_MSG_HOOK *GUI_SetKeyMsgHook(GUI_KEY_MSG_HOOK *pHook)
{
    GUI_KEY_MSG_HOOK *r;
    r = GUI_pfKeyMsgHook;
    GUI_pfKeyMsgHook = pHook;
    return r;
}

/*************************** End of file ****************************/
