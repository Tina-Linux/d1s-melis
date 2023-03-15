/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
