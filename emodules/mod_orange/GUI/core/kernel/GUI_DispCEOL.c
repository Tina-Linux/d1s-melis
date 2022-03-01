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
* File    : GUI_DispCEOL.c
* Purpose : Routines to display values clean to end of line
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/



#include "gui_core_kernel_private.h"
#include "string.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispCEOL
*/
void GUI_DispCEOL(void)
{
    int y = GUI_Context.DispPosY[GUI_Context.SelLayer] - GUI_GetYAdjust();
    GUI_ClearRect(GUI_Context.DispPosX[GUI_Context.SelLayer], y, 4000,             /* Max pos x */
                  y + GUI_Context.pAFont->YDist - 1);
}

/*************************** End of file ****************************/
