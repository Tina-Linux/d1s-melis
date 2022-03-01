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
* File    : GUIAlloc.c
* Purpose : Dynamic memory management
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include <stddef.h>           /* needed for definition of NULL */
#include <string.h>           /* for memcpy, memset */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Internal memory management
*
**********************************************************************
*/

/*********************************************************************
*
*       GUI_ALLOC_AllocZero
*/
GUI_HMEM GUI_ALLOC_AllocZero(GUI_ALLOC_DATATYPE Size)
{
    GUI_HMEM hMem;
    GUI_LOCK();
    GUI_DEBUG_LOG2("\nGUI_ALLOC_Alloc... requesting %d, %d avail", Size, GUI_ALLOC_GetMaxSize());
    hMem = GUI_ALLOC_AllocNoInit(Size);

    if (hMem)
    {
        GUI_MEMSET((U8 *)GUI_ALLOC_h2p(hMem), 0, Size);  /* Zeroinit ! */
    }

    GUI_UNLOCK();
    return hMem;
}

/*************************** End of file ****************************/
