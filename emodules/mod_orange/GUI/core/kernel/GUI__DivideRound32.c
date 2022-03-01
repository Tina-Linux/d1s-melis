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
* File    : GUI__DivideRound32.c
* Purpose : Implementation of GUI__DivideRound32
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
*       GUI__DivideRound32
*
* This routine is used internally for computations. Primary goal is
* to minimize the effects of rounding which occur if we simply
* divide.
*/
I32 GUI__DivideRound32(I32 a, I32 b)
{
    if (b)
    {
        if (a < 0)
        {
            return (a - (b >> 1)) / b;
        }
        else
        {
            return (a + (b >> 1)) / b;
        }
    }

    return 0;
}

/*************************** End of file ****************************/
