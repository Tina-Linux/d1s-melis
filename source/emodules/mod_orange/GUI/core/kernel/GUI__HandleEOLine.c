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
* File    : GUI__HandleEOLine.c
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */

#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       GUI__HandleEOLine
*
* Is called when processing strings which may consist of
* multiple lines after a line has been processed. It will
* a) Swall the line feed character (if it is there)
* b) Return 1 if end of string, otherwise 0
*/
int GUI__HandleEOLine(const char GUI_UNI_PTR **ps)
{
    const char GUI_UNI_PTR *s = *ps;
    char c = *s++;

    if (c == 0)
    {
        return 1;
    }

    if ((c == '\n') || (c == '\r')) //modified by Derek 2010.12.02.11:35
    {
        *ps = s;
    }

    return 0;
}

/*************************** End of file ****************************/
