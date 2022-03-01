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
* File    : GUI_SetDecChar.C
* Purpose : Routines to set the character used for decimal point
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
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
*       GUI_SetDecChar
*/
char GUI_SetDecChar(char c)
{
    char r = GUI_DecChar;
    GUI_DecChar = c;
    return r;
}

/*********************************************************************
*
*       GUI_GetDecChar
*/
char GUI_GetDecChar(void)
{
    return GUI_DecChar;
}

/*************************** End of file ****************************/
