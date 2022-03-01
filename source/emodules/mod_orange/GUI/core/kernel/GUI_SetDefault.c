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
* File    : GUI_SetDefault.C
* Purpose : Implementation of GUI_SetDefault
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
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
*       GL_SetDefault
*/
void GL_SetDefault(void)
{
    GUI_SetBkColor(GUI_DEFAULT_BKCOLOR);
    GUI_SetColor(GUI_DEFAULT_COLOR);
    GUI_SetPenSize(1);
    GUI_SetTextAlign(0);
    GUI_SetTextMode(0);
    GUI_SetDrawMode(0);
    GUI_SetFont(GUI_DEFAULT_FONT);
    GUI_SetLineStyle(GUI_LS_SOLID);
}

/*********************************************************************
*
*       GUI_SetDefault
*/
void GUI_SetDefault(void)
{
    GUI_LOCK();
    GL_SetDefault();
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
