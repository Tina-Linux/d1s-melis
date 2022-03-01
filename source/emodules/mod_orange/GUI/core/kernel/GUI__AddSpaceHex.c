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
* File    : GUI__AddSpaceHex.c
* Purpose : Internal function
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/



#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Public code (module internal)
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__AddSpaceHex
*/
void GUI__AddSpaceHex(U32 v, U8 Len, char **ps)
{
    char *s = *ps;
    *s++ = ' ';
    *ps = s;
    GUI_AddHex(v, Len, ps);
}

/*************************** End of file ****************************/
