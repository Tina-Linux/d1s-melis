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
* File    : GUI_CursorCrossMI.c
* Purpose : Defines the cross cursor, medium
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"

GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapCrossMI =
{
    21,                   /* XSize */
    21,                   /* YSize */
    6,                    /* BytesPerLine */
    2,                    /* BitsPerPixel */
    GUI_Pixels_CrossM,    /* Pointer to picture data (indices) */
    &GUI_CursorPalI,      /* Pointer to palette */
    NULL
};


GUI_CONST_STORAGE GUI_CURSOR GUI_CursorCrossMI =
{
    &GUI_BitmapCrossMI, 10, 10
};


/*************************** End of file ****************************/
