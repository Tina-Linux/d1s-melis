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
* File    : GUI_AlphaBlend.c
* Purpose : Implementation of AlphaBlend
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
*       GUI_OpenAlphaBlend
*/
void GUI_OpenAlphaBlend()
{
    GUI_LOCK();
    {
        GUI_Context.BlendFlag = 1;
    }
    GUI_UNLOCK();
}


/*********************************************************************
*
*       GUI_CloseAlphaBlend
*/
void GUI_CloseAlphaBlend()
{
    GUI_LOCK();
    {
        GUI_Context.BlendFlag = 0;
    }
    GUI_UNLOCK();
}


/*************************** End of file ****************************/
