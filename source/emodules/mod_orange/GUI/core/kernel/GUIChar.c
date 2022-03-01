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
* File    : GUIChar.C
* Purpose : Implementation of character and string services
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 15:54:04
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
*       GUI_DispNextLine
*/
void GUI_DispNextLine(void)
{
    GUI_LOCK();
    GUI_Context.DispPosY[GUI_Context.SelLayer] += GUI_GetFontDistY();
    GUI_Context.DispPosX[GUI_Context.SelLayer]  = GUI_Context.LBorder;
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GL_DispChar
*/
void GL_DispChar(U16 c)
{
    /* check for control characters */
    if (c == '\n')
    {
        GUI_DispNextLine();
    }
    else
    {
        if (c != '\r')
        {
            GUI_LOCK();
            //c = eLIBs_GB2312_to_Unicode(c);
            GUI_Context.pAFont->pfDispChar(c);
            //if (GUI_pfDispCharStyle) {
            //GUI_pfDispCharStyle(c);
            //}
            GUI_UNLOCK();
        }
    }
}

/*********************************************************************
*
*       GUI_GetYAdjust
*
* Returns adjustment in vertical (Y) direction
*
* Note: The return value needs to be subtracted from
*       the y-position of the character.
*/
int GUI_GetYAdjust(void)
{
    int r = 0;
    GUI_LOCK();

    switch (GUI_Context.TextAlign & GUI_TA_VERTICAL)
    {
        case GUI_TA_BOTTOM:
            r = GUI_Context.pAFont->YSize - 1;
            break;

        case GUI_TA_VCENTER:
            r = GUI_Context.pAFont->YSize / 2;
            break;

        case GUI_TA_BASELINE:
            r = GUI_Context.pAFont->YSize / 2;
    }

    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetFontDistY
*/
int GUI_GetFontDistY(void)
{
    int r;
    GUI_LOCK();
    //  r = GUI_Context.pAFont->YDist;
    r = GUI_Context.pAFont->YDist * GUI_Context.pAFont->YMag;

    if (GUI_Context.FontMode == LCD_FONTMODE_32BPPMAG2 || GUI_Context.FontMode == LCD_FONTMODE_8BPP32_FRAME_MAG)
    {
        r = r * 2;
    }

    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetCharDistX
*/
int GUI_GetCharDistX(U16 c)
{
    int r;
    GUI_LOCK();
    r = GUI_Context.pAFont->pfGetCharDistX(c);
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
