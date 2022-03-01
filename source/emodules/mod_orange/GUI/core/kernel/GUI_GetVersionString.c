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
* File    : GUI_GetVersionString.c
* Purpose : Version information functions
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
*       Static data
*
**********************************************************************
*/

static char _acVersion[] = {"X.XX\0\0"};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetVersionString
*/
const char *GUI_GetVersionString(void)
{
    _acVersion[0] = '0' + (GUI_VERSION / 10000);
    _acVersion[2] = '0' + (GUI_VERSION /  1000) % 10;
    _acVersion[3] = '0' + (GUI_VERSION /   100) % 10;
#if GUI_VERSION%100
    _acVersion[4] = 'a' - 1 + (GUI_VERSION % 100);
#endif
    return _acVersion;
}

/*************************** End of file ****************************/
