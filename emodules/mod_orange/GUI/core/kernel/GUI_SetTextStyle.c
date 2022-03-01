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
* File    : GUI_SetTextStyle.C
* Purpose : Implementation of various text styles
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 9:56:10
**********************************************************************************************************************
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__cbDrawTextStyle
*/
static void GUI__cbDrawTextStyle(U16 Char)
{
    int x0, x1;
    x1 = GUI_Context.DispPosX[GUI_Context.SelLayer] - 1;
    x0 = x1 - GUI_Context.pAFont->pfGetCharDistX(Char) + 1;

    /* Handle Underline */
    if (GUI_Context.TextStyle & GUI_TS_UNDERLINE)
    {
        int yOff = GUI_Context.pAFont->Baseline;

        if (yOff >= GUI_Context.pAFont->YSize)
        {
            yOff = GUI_Context.pAFont->YSize - 1;
        }

        LCD_DrawHLine(x0, GUI_Context.DispPosY[GUI_Context.SelLayer] + yOff, x1);
    }

    /* Handle strike thru */
    if (GUI_Context.TextStyle & GUI_TS_STRIKETHRU)
    {
        int yOff = GUI_Context.pAFont->Baseline - ((GUI_Context.pAFont->CHeight + 1) / 2);
        LCD_DrawHLine(x0, GUI_Context.DispPosY[GUI_Context.SelLayer] + yOff, x1);
    }

    /* Handle over line */
    if (GUI_Context.TextStyle & GUI_TS_OVERLINE)
    {
        int yOff = GUI_Context.pAFont->Baseline - GUI_Context.pAFont->CHeight - 1;

        if (yOff < 0)
        {
            yOff = 0;
        }

        LCD_DrawHLine(x0, GUI_Context.DispPosY[GUI_Context.SelLayer] + yOff, x1);
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_SetTextStyle
*/
char GUI_SetTextStyle(char Style)
{
    char OldStyle;
    GUI_LOCK();
    OldStyle = GUI_Context.TextStyle;
    //GUI_pfDispCharStyle = GUI__cbDrawTextStyle;    /* Init function pointer (function in this module) */
    GUI_Context.TextStyle = Style;
    GUI_UNLOCK();
    return OldStyle;
}

/*************************** End of file ****************************/
