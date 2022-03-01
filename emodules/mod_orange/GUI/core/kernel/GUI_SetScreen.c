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
* File    : GUI_SetScreen.C
* Purpose : Change LCD Direction and set LCD Size
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:53:53
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include <string.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SetLCDSizeY
*/
static char _SetLCDSizeY(int y)
{
    Screen_Context.LCDYSize[GUI_Context.SelLayer] = y;
    return 0;
}

/*********************************************************************
*
*       _SetLCDSizeX
*/
static char _SetLCDSizeX(int x)
{
    Screen_Context.LCDXSize[GUI_Context.SelLayer] = x;
    return 0;
}

static char _SetLCDDirection(U8 dir)
{
    U8 old_dir;
    old_dir = Screen_Context.LCDDirection[GUI_Context.SelLayer];
    Screen_Context.LCDDirection[GUI_Context.SelLayer] = dir;
    return old_dir;
}

static char _GetLCDDirection(void)
{
    return Screen_Context.LCDDirection[GUI_Context.SelLayer];
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/


/*********************************************************************
*
*       GUI_SetLCDDirection
*/

char GUI_SetLCDDirection(U8 dir)
{
    char r;
    GUI_LOCK();
    r = _SetLCDDirection(dir);
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GotoXY
*/
char GUI_SetLCDSize(int x, int y)
{
    char r;
    GUI_LOCK();
    r  = _SetLCDSizeX(x);
    r |= _SetLCDSizeY(y);
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
