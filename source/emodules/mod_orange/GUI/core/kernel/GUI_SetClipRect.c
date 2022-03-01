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
* File    : GUI_SetClipRect.c
* Purpose : Implementation of setting cliprect
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
*       GUI_SetClipRect
*
* Purpose:
*   Sets the rectangle used for clipping.
*
* Parameter:
*   pRect - Points to the rectangle to be used for clipping. If the WM
*           is used, the pointer needs to remain valid during the given
*           rectangle should be used for clipping.
*/
const GUI_RECT *GUI_SetClipRect(const GUI_RECT *pRect)
{
    const GUI_RECT *pOldRect;
    GUI_LOCK();
#if GUI_WINSUPPORT
    pOldRect = WM_SetUserClipRect(pRect);
#else
    pOldRect = NULL;
    {
        GUI_RECT Rect;

        if (!pRect)
        {
            Rect.x0 = -4096;
            Rect.y0 = -4096;
            Rect.x1 =  4096;
            Rect.y1 =  4096;
            pRect = &Rect;
        }

        LCD_SetClipRectEx(pRect);
    }
#endif
    GUI_UNLOCK();
    return pOldRect;
}

/*************************** End of file ****************************/

