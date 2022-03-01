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
* File    : GUI_SaveContext.c
* Purpose : Code to save and restore GUI context
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:51:50
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
*       GUI_SaveContext
*/
void GUI_SaveContext(GUI_CONTEXT *pContext)
{
    *pContext = GUI_Context;
}

/*********************************************************************
*
*       GUI_RestoreContext
*/
void GUI_RestoreContext(const GUI_CONTEXT *pContext)
{
    GUI_Context = *pContext;
}

/*************************** End of file ****************************/
