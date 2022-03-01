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
* File    : GUITOUCH_StoreState.C
* Purpose : Implementation of GUITOUCH_StoreState
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 16:47:34
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
*       GUI_TOUCH_StoreState
*
* NOTE:
*   This is the older version. Prefer GUI_TOUCH_StoreStateEx in new code
*/
void GUI_TOUCH_StoreState(int x, int y)
{
    static GUI_PID_STATE _State;  /* static so we retain coordinates when touch is released */

    if ((x >= 0) && (y >= 0))
    {
        _State.Pressed = 1;
        _State.x = x;
        _State.y = y;
    }
    else
    {
        _State.Pressed = 0;
    }

    GUI_TOUCH_StoreStateEx(&_State);
}

/*************************** End of file ****************************/
