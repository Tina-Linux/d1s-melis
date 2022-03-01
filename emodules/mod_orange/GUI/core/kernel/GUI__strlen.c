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
* File    : GUI__strlen.c
* Purpose : Implementation of GUI__strlen
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__strlen
*
* Purpose:
*  Replacement for the strlen function. The advantage is that it can
*  be called with a NULL pointer, in which case -1 is returned.
*/
int GUI__strlen(const char GUI_UNI_PTR *s)
{
    int r = -1;

    if (s)
    {
        do
        {
            r++;
        } while (*s++);
    }

    return r;
}

/*************************** End of file ****************************/
