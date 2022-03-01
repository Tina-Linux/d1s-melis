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
* File    : GUI_IsInFont.c
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_IsInFont
*/
char GUI_IsInFont(const GUI_FONT GUI_UNI_PTR *pFont, U16 c)
{
    if (pFont == NULL)
    {
        pFont = GUI_Context.pAFont;
    }

    return pFont->pfIsInFont(pFont, c);
}

/*************************** End of file ****************************/
