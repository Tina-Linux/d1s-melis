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
* File    : GUI_GetBitmapPixelColor.c
* Purpose : Gets the color from one pixel of a bitmap
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
*       GUI_GetBitmapPixelColor
*/
GUI_COLOR GUI_GetBitmapPixelColor(const GUI_BITMAP GUI_UNI_PTR *pBMP, unsigned x, unsigned y)
{
    int Index = GUI_GetBitmapPixelIndex(pBMP, x, y);

    if (pBMP->pPal->HasTrans && (Index == 0))
    {
        return GUI_INVALID_COLOR;
    }

    return pBMP->pPal->pPalEntries[Index];
}

/*************************** End of file ****************************/
