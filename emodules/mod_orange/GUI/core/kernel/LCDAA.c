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
#define LCD_C

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define RETURN_IF_Y_OUT() \
    if (y < GUI_Context.ClipRect[GUI_Context.SelLayer].y0)                     \
        return;                                            \
    if (y > GUI_Context.ClipRect[GUI_Context.SelLayer].y1)                     \
        return;

#define RETURN_IF_X_OUT() \
    if (x < GUI_Context.ClipRect[GUI_Context.SelLayer].x0) return;             \
    if (x > GUI_Context.ClipRect[GUI_Context.SelLayer].x1) return;

#define CLIP_X() \
    if (x0 < GUI_Context.ClipRect[GUI_Context.SelLayer].x0) { x0 = GUI_Context.ClipRect[GUI_Context.SelLayer].x0; } \
    if (x1 > GUI_Context.ClipRect[GUI_Context.SelLayer].x1) { x1 = GUI_Context.ClipRect[GUI_Context.SelLayer].x1; }

#define CLIP_Y() \
    if (y0 < GUI_Context.ClipRect[GUI_Context.SelLayer].y0) { y0 = GUI_Context.ClipRect[GUI_Context.SelLayer].y0; } \
    if (y1 > GUI_Context.ClipRect[GUI_Context.SelLayer].y1) { y1 = GUI_Context.ClipRect[GUI_Context.SelLayer].y1; }

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_AA_MixColors
*/
LCD_COLOR LCD_AA_MixColors(LCD_COLOR Color, LCD_COLOR BkColor, U8 Intens)
{
    /* Calc Color seperations for FgColor first */
    I32 R = (Color & 0xff) * Intens;
    I32 G = (Color & 0xff00) * Intens;
    I32 B = (Color & 0xff0000) * Intens;
    /* Add Color seperations for BkColor */
    Intens = 15 - Intens;
    R += (BkColor & 0xff) * Intens;
    G += (BkColor & 0xff00) * Intens;
    B += (BkColor & 0xff0000) * Intens;
    R = (R / 15) & 0xff;
    G = (G / 15) & 0xff00;
    B = (B / 15) & 0xff0000;
    Color = R + G + B;
    return Color;
}

/*********************************************************************
*
*       LCD_SetPixelAA
*/
void LCD_SetPixelAA(int x, int y, U8 Intens)
{
    if (Intens == 0)
    {
        return;
    }

    RETURN_IF_Y_OUT();
    RETURN_IF_X_OUT();

    if (Intens >= 15)
    {
        LCDDEV_L0_SetPixelIndex(x, y, LCD_COLORINDEX);
    }
    else
    {
        LCD_COLOR Color = LCD_Index2Color(LCD_COLORINDEX);
        LCD_COLOR BkColor =  LCD_GetPixelColor(x, y);
        Color = LCD_AA_MixColors(Color, BkColor, Intens);
        LCDDEV_L0_SetPixelIndex(x, y, LCD_Color2Index(Color));
    }
}

/*********************************************************************
*
*       LCD_SetPixelAA_NoTrans
*/
void LCD_SetPixelAA_NoTrans(int x, int y, U8 Intens)
{
    RETURN_IF_Y_OUT();
    RETURN_IF_X_OUT();

    if (Intens == 0)
    {
        LCDDEV_L0_SetPixelIndex(x, y, LCD_BKCOLORINDEX);
    }
    else if (Intens == 15)
    {
        LCDDEV_L0_SetPixelIndex(x, y, LCD_COLORINDEX);
    }
    else
    {
        LCD_COLOR Color = LCD_AA_MixColors(LCD_Index2Color(LCD_COLORINDEX),
                                           LCD_Index2Color(LCD_BKCOLORINDEX),
                                           Intens);
        LCDDEV_L0_SetPixelIndex(x, y, LCD_Color2Index(Color));
    }
}

/*************************** End of file ****************************/
