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
* File    : GUI_GetFontInfo.c
* Purpose : Get GUI_FONTINFO structure
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_kernel_private.h"
#include <stddef.h>

#if GUI_COMPILER_SUPPORTS_FP

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetFontInfo
*/
void GUI_GetFontInfo(const GUI_FONT GUI_UNI_PTR *pFont, GUI_FONTINFO *pFontInfo)
{
    GUI_LOCK();

    if (pFont == NULL)
    {
        pFont = GUI_Context.pAFont;
    }

    pFontInfo->Baseline = pFont->Baseline;
    pFontInfo->CHeight  = pFont->CHeight;
    pFontInfo->LHeight  = pFont->LHeight;
    pFont->pfGetFontInfo(pFont, pFontInfo);
    GUI_UNLOCK();
}

#endif

/*************************** End of file ****************************/
