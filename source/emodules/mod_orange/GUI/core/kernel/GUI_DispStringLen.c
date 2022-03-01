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
* File    : GUI_DispStringLen.c
* Purpose : Implementation of optional routines
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
*       GUI_DispStringLen
*/
void GUI_DispStringLen(const char GUI_UNI_PTR *s, int MaxNumChars)
{
    U16 Char;
    GUI_LOCK();

    while (MaxNumChars && ((Char = GUI_UC__GetCharCodeInc(&s)) != 0))
    {
        GUI_DispChar(Char);
        MaxNumChars--;
    }

    while (MaxNumChars--)
    {
        GUI_DispChar(' ');
    }

    GUI_UNLOCK();
}

/*************************** End of file ****************************/
