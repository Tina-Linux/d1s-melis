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
* File    : LCD_SelectLCD.c
* Purpose : Implementation of said routine
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
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       _SelectLCD
*/
void LCD_SelectLCD(void)
{
#if GUI_SUPPORT_DEVICES
#if GUI_NUM_LAYERS == 1
    GUI_Context.pDeviceAPI   = LCD_aAPI[0];   // RGB565565
#else

    switch (GUI_LayerInfo.colorMode[GUI_Context.SelLayer])
    {
        case   PIXEL_COLOR_RGB565:
            GUI_Context.pDeviceAPI   = LCD_aAPI[0];
            break;

        case   PIXEL_COLOR_RGB0888:
        case   PIXEL_COLOR_ARGB8888:
            GUI_Context.pDeviceAPI   = LCD_aAPI[1];
            break;

        case   PIXEL_MONO_8BPP:
            GUI_Context.pDeviceAPI   = LCD_aAPI[2];
            break;

        case   PIXEL_MONO_2BPP:
            GUI_Context.pDeviceAPI   = LCD_aAPI[3];
            break;
    }

#endif
    GUI_Context.hDevData = 0;
#endif
    GUI_Context.pClipRect_HL[GUI_Context.SelLayer] = &GUI_Context.ClipRect[GUI_Context.SelLayer];
    LCD_SetClipRectMax();
}

/*************************** End of file ****************************/
