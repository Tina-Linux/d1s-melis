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
* File    : GUI_CursorCrossLI.c
* Purpose : Defines the cross cursor, large
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapCrossLI =
{
    31,                   /* XSize */
    31,                   /* YSize */
    8,                    /* BytesPerLine */
    2,                    /* BitsPerPixel */
    GUI_Pixels_CrossL,    /* Pointer to picture data (indices) */
    &GUI_CursorPalI,      /* Pointer to palette */
    NULL
};


GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossLI =
{
    &GUI_BitmapCrossLI, 15, 15
};


/*************************** End of file ****************************/
