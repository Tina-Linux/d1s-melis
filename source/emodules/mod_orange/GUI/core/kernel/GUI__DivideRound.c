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
* File    : GUI__DivideRound.c
* Purpose : Implementation of GUI__DivideRound
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
*       GUI__DivideRound
*
* This routine is used internally for computations. Primary goal is
* to minimize the effects of rounding which occur if we simply
* divide.
*/
int GUI__DivideRound(int a, int b)
{
    int r = 0;

    if (b)
    {
        r = ((a + b / 2) / b);
    }

    return r;
}

/*************************** End of file ****************************/
