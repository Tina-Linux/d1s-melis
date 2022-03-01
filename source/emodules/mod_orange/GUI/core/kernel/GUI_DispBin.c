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
* File    : GUI_DispBin.c
* Purpose : Routines to display values as binary
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"
#include "string.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*     GUI_DispBin
*/
void GUI_DispBin(U32 v, U8 Len)
{
    char ac[33];
    char *s = ac;
    GUI_AddBin(v, Len, &s);
    GUI_DispString(ac);
}

/*********************************************************************
*
*     GUI_DispBinAt
*/
void GUI_DispBinAt(U32 v, I16P x, I16P y, U8 Len)
{
    char ac[33];
    char *s = ac;
    GUI_AddBin(v, Len, &s);
    GUI_DispStringAt(ac, x, y);
}

/*************************** End of file ****************************/
