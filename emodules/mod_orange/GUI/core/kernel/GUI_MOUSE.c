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
* File    : GUI_MOUSE.c
* Purpose : Generic mouse routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <string.h>           /* memcmp */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static GUI_PID_STATE _State;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_MOUSE_GetState
*/
int GUI_MOUSE_GetState(GUI_PID_STATE *pState)
{
    *pState = _State;
    return (_State.Pressed != 0) ? 1 : 0;
}

/*********************************************************************
*
*       GUI_MOUSE_StoreState
*/
void GUI_MOUSE_StoreState(const GUI_PID_STATE *pState)
{
    if (memcmp(pState, &_State, sizeof(_State)))
    {
        _State = *pState;
        GUI_PID_StoreState(pState);
    }
}

/*************************** End of file ****************************/

