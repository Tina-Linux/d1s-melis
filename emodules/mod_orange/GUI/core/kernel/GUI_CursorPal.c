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
* File    : GUI_CursorPal.c
* Purpose : Defines the colors for most cursors
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static GUI_CONST_STORAGE GUI_COLOR _aColor[] =
{
    0x0000FF, 0x000000, 0xFFFFFF
};

GUI_CONST_STORAGE GUI_LOGPALETTE GUI_CursorPal =
{
    3,    /* number of entries */
    1,    /* Has transparency */
    &_aColor[0]
};

/*************************** End of file ****************************/
