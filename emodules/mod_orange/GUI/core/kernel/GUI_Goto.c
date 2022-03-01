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
* File    : GUI_Goto.c
* Purpose : Implementation of GUI_Goto... routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
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
*       _GotoY
*/
static char _GotoY(int y)
{
    GUI_Context.DispPosY[GUI_Context.SelLayer] = y;
    return 0;
}

/*********************************************************************
*
*       _GotoX
*/
static char _GotoX(int x)
{
    GUI_Context.DispPosX[GUI_Context.SelLayer] = x;
    return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GotoY
*/
char GUI_GotoY(int y)
{
    char r;
    GUI_LOCK();
    r = _GotoY(y);
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GotoX
*/
char GUI_GotoX(int x)
{
    char r;
    GUI_LOCK();
    r = _GotoX(x);
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GotoXY
*/
char GUI_GotoXY(int x, int y)
{
    char r;
    GUI_LOCK();
    r  = _GotoX(x);
    r |= _GotoY(y);
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
