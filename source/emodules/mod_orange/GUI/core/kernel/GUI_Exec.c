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
* File    : GUI_Exec.c
* Purpose : Implementation of GUI_Exec
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
*       GUI_Exec1
*/
int GUI_Exec1(void)
{
    int r = 0;

    /* Execute background jobs */
    if (GUI_pfTimerExec)
    {
        if ((*GUI_pfTimerExec)())
        {
            r = 1;                  /* We have done something */
        }
    }

#if GUI_WINSUPPORT    /* If 0, WM will not generate any code */

    if (WM_Exec())
    {
        r = 1;
    }

#endif
    return r;
}

/*********************************************************************
*
*       GUI_Exec
*/
int GUI_Exec(void)
{
    int r = 0;

    while (GUI_Exec1())
    {
        r = 1;                  /* We have done something */
    }

    return r;
}

/*************************** End of file ****************************/
