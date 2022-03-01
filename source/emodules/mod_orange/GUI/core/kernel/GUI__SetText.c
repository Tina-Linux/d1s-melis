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
* File    : GUI__SetText.c
* Purpose : Implementation of said function
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include <string.h>
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__SetText
*/
int GUI__SetText(GUI_HMEM *phText, const char *s)
{
    int r = 0;

    if (GUI__strcmp_hp(*phText, s) != 0)              /* Make sure we have a quick out if nothing changes */
    {
        GUI_HMEM hMem;
        hMem = GUI_ALLOC_AllocNoInit(GUI__strlen(s) + 1);

        if (hMem)
        {
            char *pMem;
            pMem = (char *) GUI_ALLOC_h2p(hMem);
            strcpy(pMem, s);
            GUI_ALLOC_FreePtr(phText);
            *phText = hMem;
            r = 1;
        }
    }

    return r;
}

/*************************** End of file ****************************/
