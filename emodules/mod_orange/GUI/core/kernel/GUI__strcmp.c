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
* File    : GUI__strcmp.c
* Purpose : Implementation of said function
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
*       public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__strcmp
*
* Purpose:
*   Replacement for the strcmp function. The advantage is that it can
*   be called with NULL pointer(s).
*   A NULL string is treated as a an empty string ("").
* Return value:
*   0 if identical
*   1 else
*/
int GUI__strcmp(const char GUI_UNI_PTR *s0, const char GUI_UNI_PTR *s1)
{
    if (s0 == NULL)
    {
        s0 = "";
    }

    if (s1 == NULL)
    {
        s1 = "";
    }

    do
    {
        if (*s0 != *s1)
        {
            return 1;
        }

        s1++;
    } while (*++s0);

    if (*s1)
    {
        return 1;    /* Not equal, since s1 is longer than s0 */
    }

    return 0;      /* Equal ! */
}

/*********************************************************************
*
*       GUI__strcmp_hp
*
* Purpose:
*  Replacement for the strcmp function. The advantage is that it can
*  be called with NULL pointer or 0 handle.
*  A NULL string is treated as a an empty string ("").
* Return value:
*  0 if identical
*  1 else
*/
int GUI__strcmp_hp(GUI_HMEM  hs0, const char GUI_UNI_PTR *s1)
{
    const char *s0 = NULL;

    if (hs0)
    {
        s0 = (const char *)GUI_ALLOC_h2p(hs0);
    }

    return GUI__strcmp(s0, s1);
}

/*************************** End of file ****************************/
