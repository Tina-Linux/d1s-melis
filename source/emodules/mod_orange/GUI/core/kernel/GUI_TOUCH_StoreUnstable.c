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
* File    : GUITOUCH_StoreUnstable.C
* Purpose : Implementation of GUITOUCH_StoreUnstable
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 16:47:34
**********************************************************************************************************************
*/

#include <stdlib.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static int _x, _y;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_TOUCH_StoreUnstable
*/
void GUI_TOUCH_StoreUnstable(int x, int y)
{
    int xDiff, yDiff;
    xDiff = abs(x - _x);
    yDiff = abs(y - _y);

    if (xDiff + yDiff > 2)
    {
        _x = x;
        _y = y;
        GUI_TOUCH_StoreState(x, y);
    }
}

/*********************************************************************
*
*       GUI_TOUCH_GetUnstable
*/
void GUI_TOUCH_GetUnstable(int *px, int *py)
{
    *px = _x;
    *py = _y;
}

/*************************** End of file ****************************/
