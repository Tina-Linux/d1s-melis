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
* File    : GUI_PID.c
* Purpose : PID (Pointer input device) management
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:51:50
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
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _PID_Load
*
* Purpose:
*   If the window manager is available, write the function pointer for
*   the PID handler.
*/
static void _PID_Load(void)
{
#if (GUI_WINSUPPORT)
    WM_pfHandlePID = WM_HandlePID;
#endif
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_PID_GetState
*/
int GUI_PID_GetState(GUI_PID_STATE *pState)
{
    *pState = _State;
    return (pState->Pressed != 0) ? 1 : 0;
}

/*********************************************************************
*
*       GUI_PID_StoreState
*/
void GUI_PID_StoreState(const GUI_PID_STATE *pState)
{
    _PID_Load();

    if (memcmp(&_State, pState, sizeof(_State)))
    {
        _State = *pState;
        GUI_X_SIGNAL_EVENT();
    }
}

/*************************** End of file ****************************/
