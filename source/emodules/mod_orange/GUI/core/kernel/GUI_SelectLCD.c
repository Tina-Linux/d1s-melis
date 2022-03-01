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
* File    : GUI_SelectLCD.c
* Purpose : Implementation of said routine
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:51:50
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
*       GUI_SelectLCD
*/
void GUI_SelectLCD(void)
{
    GUI_LOCK();
#if GUI_SUPPORT_DEVICES
    LCD_SelectLCD();
    LCD_UpdateColorIndices();
#if GUI_WINSUPPORT
    WM_Activate();
#endif
#endif
    GUI_UNLOCK();
#ifdef SIM_PC_WIN
    //SetUpdateScreenEvent(1);
    RefreshDeContent(1);
#endif
}

/*************************** End of file ****************************/
