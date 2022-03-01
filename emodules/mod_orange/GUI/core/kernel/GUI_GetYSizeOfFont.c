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
* File    : GUI_GetYSizeOfFont.c
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
*       GUI_GetYSizeOfFont
*/
int GUI_GetYSizeOfFont(const GUI_FONT GUI_UNI_PTR *pFont)
{
    return pFont->YSize * pFont->YMag;
}

/*********************************************************************
*
*       GUI_GetYDistOfFont
*/
int GUI_GetYDistOfFont(const GUI_FONT GUI_UNI_PTR *pFont)
{
    return pFont->YDist * pFont->YMag;
}

/*************************** End of file ****************************/
