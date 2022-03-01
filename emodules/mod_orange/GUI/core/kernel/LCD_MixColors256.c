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
* File    : LCD_MixColors256.c
* Purpose : Implementation of LCD_MixColors256
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
*       LCD_MixColors256
*
* Purpose:
*   Mix 2 colors.
*
* Parameters:
*   Intens:    Intensity of first color in 257 steps, from 0 to 256, where 256 equals 100%
*/
LCD_COLOR LCD_MixColors256(LCD_COLOR Color, LCD_COLOR BkColor, unsigned Intens)
{
    /* Calc Color seperations for FgColor first */
    U32 R = (Color & 0xff)    * Intens;
    U32 G = (Color & 0xff00)  * Intens;
    U32 B = (Color & 0xff0000) * Intens;
    /* Add Color seperations for BkColor */
    Intens = 256 - Intens;
    R += (BkColor & 0xff)     * Intens;
    G += (BkColor & 0xff00)   * Intens;
    B += (BkColor & 0xff0000) * Intens;
    R = (R >> 8);
    G = (G >> 8) & 0xff00;
    B = (B >> 8) & 0xff0000;
    return R + G + B;
}

/*************************** End of file ****************************/
