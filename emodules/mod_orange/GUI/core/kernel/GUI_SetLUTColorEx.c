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
* File    : GUI_SetLUTColorEx.C
* Purpose : Implementation of GUI_SetLUTColorEx
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:05:00
**********************************************************************************************************************
*/

#include <stddef.h>
#include "gui_core_kernel_private.h"
#if GUI_COMPILER_SUPPORTS_FP

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static const LCD_LUT_INFO _aLutInfo[GUI_NUM_LAYERS] =
{
#if GUI_NUM_LAYERS > 1
    {
#if (LCD_FIXEDPALETTE == 0) && (LCD_PHYSCOLORS_IN_RAM)
        LCD_PhysColors, (I16)(1 << LCD_BITSPERPIXEL_0)
#else
        NULL, 0
#endif
    },
    {
#if (LCD_FIXEDPALETTE_1 == 0) && (LCD_PHYSCOLORS_IN_RAM)
        LCD_PhysColors_1, (I16)(1 << LCD_BITSPERPIXEL_1)
#else
        NULL, 0
#endif
    },
#if GUI_NUM_LAYERS > 2
    {
#if (LCD_FIXEDPALETTE_1 == 0) && (LCD_PHYSCOLORS_IN_RAM)
        LCD_PhysColors_2, (I16)(1 << LCD_BITSPERPIXEL_2)
#else
        NULL, 0
#endif
    },
#endif
#if GUI_NUM_LAYERS > 3
    {
#if (LCD_FIXEDPALETTE_3 == 0) && (LCD_PHYSCOLORS_IN_RAM)
        LCD_PhysColors_3, (I16)(1 << LCD_BITSPERPIXEL_3)
#else
        NULL, 0
#endif
    },
#endif
#if GUI_NUM_LAYERS > 4
    {
#if (LCD_FIXEDPALETTE_4 == 0) && (LCD_PHYSCOLORS_IN_RAM)
        LCD_PhysColors_4, (I16)(1 << LCD_BITSPERPIXEL_4)
#else
        NULL, 0
#endif
    },
#endif
#else
    {
#if (LCD_FIXEDPALETTE == 0) && (LCD_PHYSCOLORS_IN_RAM)
        LCD_PhysColors, 1 << LCD_BITSPERPIXEL
#else
        NULL, 0
#endif
    }
#endif
};

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SetLUTColorEx
*/
void GUI_SetLUTColorEx(U8 Pos, LCD_COLOR Color, unsigned int LayerIndex)
{
    if (LayerIndex < GUI_NUM_LAYERS)
    {
        if (Pos < _aLutInfo[LayerIndex].NumEntries)
        {
            LCD_aAPI[LayerIndex]->pfSetLUTEntry(Pos, Color);
            _aLutInfo[LayerIndex].paColor[Pos] = Color;
        }
    }
}

#endif

/*************************** End of file ****************************/
