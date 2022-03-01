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
* File    : GUI__GetNumChars.c
* Purpose : Implementation of character and string services
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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*      GUI__GetNumChars
*/
int GUI__GetNumChars(const char GUI_UNI_PTR *s)
{
    int NumChars = 0;

    if (s)
    {
        while (GUI_UC__GetCharCodeInc(&s))
        {
            NumChars++;
        }
    }

    return NumChars;
}

/*************************** End of file ****************************/
