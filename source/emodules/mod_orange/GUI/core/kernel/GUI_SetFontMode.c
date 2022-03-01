/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Miracle.MCL.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : GUI_SetFontMode.C
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-3-9 11:09:08
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
*       GUI_SetDrawMode
*/
GUI_FONTMODE GUI_SetFontMode(GUI_FONTMODE fm)
{
    GUI_FONTMODE OldMode;
    GUI_LOCK();
    {
        OldMode = LCD_SetFontMode(fm);
    }
    GUI_UNLOCK();
    return OldMode;
}

/*************************** End of file ****************************/
