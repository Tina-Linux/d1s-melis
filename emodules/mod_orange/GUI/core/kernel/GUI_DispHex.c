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
* File    : GUI_DispHex.c
* Purpose : Routines to display hex
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public routines
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispHex
*/
void GUI_DispHex(U32 v, U8 Len)
{
    char ac[9];
    char *s = ac;
    GUI_AddHex(v, Len, &s);
    GUI_DispString(ac);
}

/*********************************************************************
*
*       GUI_DispHexAt
*/
void GUI_DispHexAt(U32 v, I16P x, I16P y, U8 Len)
{
    char ac[9];
    char *s = ac;
    GUI_AddHex(v, Len, &s);
    GUI_DispStringAt(ac, x, y);
}

/*************************** End of file ****************************/

