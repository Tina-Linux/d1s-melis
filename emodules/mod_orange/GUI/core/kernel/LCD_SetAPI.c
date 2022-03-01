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
* File    : LCD_SetAPI.c
* Purpose : Implementation of said function
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
#if GUI_SUPPORT_DEVICES

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_SetAPI
*
* Purpose:
*   Sets the API table for a layer.
*   Some high level software (such as the VNC server, but maybe also the mouse "Cursor" module)
*   may need to override these pointers in order to link itself into the chain
*   of drawing routines.
*/
const tLCDDEV_APIList *LCD_SetAPI(const tLCDDEV_APIList *pAPI, int Index)
{
    const tLCDDEV_APIList *pOldAPI;
    pOldAPI = LCD_aAPI[Index];
__log("iiuii");
    /* Also aplly changes to current context if LCD of this layer is selected */
    if ((GUI_Context.SelLayer == Index) && (GUI_Context.hDevData == 0))
    {
        GUI_Context.pDeviceAPI = pAPI;
    }

    LCD_aAPI[Index] = pAPI;
    return pOldAPI;
}

#else

void LCD_SetAPI_C(void);
void LCD_SetAPI_C(void) {}

#endif

/*************************** End of file ****************************/
