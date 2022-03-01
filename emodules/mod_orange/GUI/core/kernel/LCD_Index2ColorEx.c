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
* File    : LCD_Index2ColorEx.c
* Purpose : Implementation of LCD_Index2ColorEx
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/

#include "gui_core_kernel_private.h"     /* Required for configuration, APIList */

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_Index2ColorEx
*
* Purpose:
*   Convert Index to color for the given display
*/
LCD_COLOR LCD_Index2ColorEx(int i, unsigned int LayerIndex)
{
#if GUI_SUPPORT_DEVICES
    LCD_COLOR r = 0;

    if (LayerIndex < GUI_NUM_LAYERS)
    {
        r = LCD_aAPI[LayerIndex]->pfIndex2Color(i);
    }

    return r;
#else
    GUI_USE_PARA(LayerIndex);
    return LCD_Index2Color(i);
#endif
}

/*************************** End of file ****************************/
