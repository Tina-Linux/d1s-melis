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
* File    : GUIRealloc.c
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
*       Public routines: Using internal memory management
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_ALLOC_Realloc
*
* Purpose:
*   Reallocate a memory block. This is typically used to grow memory
*   blocks. The contents of the old memory block are copied into the
*   new block (or as much as fits in case of shrinkage).
*   In case of error the old memory block (and its handle) remain
*   unchanged.
*
* Return value:
*   On success:    Handle of newly allocated memory block
*   On error:      0
*/
GUI_HMEM GUI_ALLOC_Realloc(GUI_HMEM hOld, int NewSize)
{
    GUI_HMEM hNew;
    hNew = GUI_ALLOC_AllocNoInit(NewSize);

    if (hNew && hOld)
    {
        void *pNew, *pOld;
        int Size, OldSize;
        OldSize = GUI_ALLOC_GetSize(hOld);
        Size = (OldSize < NewSize) ? OldSize : NewSize;
        GUI_LOCK();
        pNew = GUI_ALLOC_h2p(hNew);
        pOld = GUI_ALLOC_h2p(hOld);
        memcpy(pNew, pOld, Size);
        GUI_UNLOCK();
        GUI_ALLOC_Free(hOld);
    }

    return hNew;
}

/*************************** End of file ****************************/
