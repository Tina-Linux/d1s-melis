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
* File    : GUI_SetFont.C
* Purpose : Optional routines
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
*       GUI_SetFont
*/
const GUI_FONT GUI_UNI_PTR *GUI_SetFont(const GUI_FONT GUI_UNI_PTR *pNewFont)
{
    const GUI_FONT GUI_UNI_PTR *pOldFont = GUI_Context.pAFont;
    GUI_LOCK();

    if (pNewFont)
    {
        GUI_Context.pAFont = pNewFont;
    }

    GUI_UNLOCK();
    return pOldFont;
}

/*************************** End of file ****************************/
