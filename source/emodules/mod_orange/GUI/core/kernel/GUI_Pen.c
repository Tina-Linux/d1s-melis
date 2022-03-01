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
* File    : GUI_Pen.c
* Purpose : Getting / Setting pen attributes
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-2-19 10:58:42
**********************************************************************************************************************
*/
#include "gui_core_kernel_private.h"

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetPenSize
*/
U8 GUI_GetPenSize(void)
{
    U8 r;
    GUI_LOCK();
    r = GUI_Context.PenSize;
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_GetPenShape
*/
U8 GUI_GetPenShape(void)
{
    U8 r;
    GUI_LOCK();
    r = GUI_Context.PenShape;
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_SetPenSize
*/
U8 GUI_SetPenSize(U8 PenSize)
{
    U8 r;
    GUI_LOCK();
    r = GUI_Context.PenSize;
    GUI_Context.PenSize = PenSize;
    GUI_UNLOCK();
    return r;
}

/*********************************************************************
*
*       GUI_SetPenShape
*/
U8 GUI_SetPenShape(U8 PenShape)
{
    U8 r;
    GUI_LOCK();
    r = GUI_Context.PenShape;
    GUI_Context.PenShape = PenShape;
    GUI_UNLOCK();
    return r;
}

/*************************** End of file ****************************/
