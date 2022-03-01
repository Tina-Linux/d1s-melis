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
* File    : GUI_DispChar.c
* Purpose : Implementation of optional routines
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/


#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       CL_DispChar
*/
#if (GUI_WINSUPPORT)
static void CL_DispChar(U16 c)
{
    GUI_RECT r;
    WM_ADDORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
    r.x1 = (r.x0 = GUI_Context.DispPosX[GUI_Context.SelLayer]) + GUI_GetCharDistX(c) - 1;
    r.y1 = (r.y0 = GUI_Context.DispPosY[GUI_Context.SelLayer]) + GUI_GetFontSizeY()  - 1;
    WM_ITERATE_START(&r)
    {
        GL_DispChar(c);
    }
    WM_ITERATE_END();

    if (c != '\n')
    {
        GUI_Context.DispPosX[GUI_Context.SelLayer] = r.x1 + 1;
    }

    WM_SUBORG(GUI_Context.DispPosX[GUI_Context.SelLayer], GUI_Context.DispPosY[GUI_Context.SelLayer]);
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_DispChar
*/
void GUI_DispChar(U16 c)
{
    GUI_LOCK();
#if (GUI_WINSUPPORT)
    CL_DispChar(c);
#else
    GL_DispChar(c);
#endif
    GUI_UNLOCK();
}

/*********************************************************************
*
*       GUI_DispCharAt
*/
void GUI_DispCharAt(U16 c, I16P x, I16P y)
{
    GUI_LOCK();
    GUI_Context.DispPosX[GUI_Context.SelLayer] = x;
    GUI_Context.DispPosY[GUI_Context.SelLayer] = y;
#if (GUI_WINSUPPORT)
    CL_DispChar(c);
#else
    GL_DispChar(c);
#endif
    GUI_UNLOCK();
}

/*************************** End of file ****************************/
