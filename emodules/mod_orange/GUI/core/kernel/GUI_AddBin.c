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
* File    : GUI_AddBin.c
* Purpose : Converts binary value 2 string
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
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
*       GUI_AddBin
*/
void GUI_AddBin(U32 v, U8 Len, char **ps)
{
    char *s = *ps;
#if GUI_DEBUG_LEVEL >1

    if (Len > 32)
    {
        GUI_DEBUG_WARN("Can not display more than 32 bin. digits");
        Len = 32;
    }

#endif
    (*ps) += Len;
    **ps   = '\0';     /* Make sure string is 0-terminated */

    while (Len--)
    {
        *(s + Len) = (char)('0' + (v & 1));
        v >>= 1;
    }
}

/*************************** End of file ****************************/
