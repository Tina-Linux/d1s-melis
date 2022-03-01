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
* File    : LCDAA.c
* Purpose : Low level antialiasing routines.
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
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
