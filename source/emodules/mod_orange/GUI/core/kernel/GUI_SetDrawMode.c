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
* File    : GUI_SetDrawMode.C
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
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
GUI_DRAWMODE GUI_SetDrawMode(GUI_DRAWMODE dm)
{
    GUI_DRAWMODE OldMode;
    GUI_LOCK();
    {
        OldMode = LCD_SetDrawMode(dm);
    }
    GUI_UNLOCK();
    return OldMode;
}

/*************************** End of file ****************************/
