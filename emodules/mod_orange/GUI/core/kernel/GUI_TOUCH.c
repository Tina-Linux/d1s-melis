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
* File    : GUITOUCH.C
* Purpose :  Generic touch screen routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:50:57
**********************************************************************************************************************
*/

#include <string.h>
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
*       GUI_TOUCH_GetState
*/
int GUI_TOUCH_GetState(GUI_PID_STATE *pState)
{
    *pState = _State;
    return (_State.Pressed != 0) ? 1 : 0;
}

/*********************************************************************
*
*       GUI_TOUCH_StoreStateEx
*/
void GUI_TOUCH_StoreStateEx(const GUI_PID_STATE *pState)
{
    if (memcmp(pState, &_State, sizeof(_State)))
    {
        _State = *pState;
        GUI_PID_StoreState(pState);
    }
}

/*************************** End of file ****************************/
