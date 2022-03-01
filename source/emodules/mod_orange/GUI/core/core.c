/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009,  Miracle.MCL.China
*                                             All Rights Reserved
*
* Moudle  : lemon
* File    : gui_core.c
*
* By      : Miracle
* Version : v1.0
* Date    : 2009-1-6 11:55:42
**********************************************************************************************************************
*/
#include "core_i.h"
#include <kconfig.h>

extern void  GUI_LongStringInit(void);
extern void  GUI_LongStringExit(void);
/*
**********************************************************************************************************************
*                                               GUI_Core_Init
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int GUI_Core_Init(void)
{
    int r;
    r = GUI_Kernel_Init();
#ifdef CONFIG_GUI_CORE_LONGNAME_EN
    GUI_LongStringInit();
#endif
    return r;
}
/*
**********************************************************************************************************************
*                                               GUI_Core_Exit
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
int GUI_Core_Exit(void)
{
#ifdef CONFIG_GUI_CORE_LONGNAME_EN
    GUI_LongStringExit();
#endif
    return GUI_Kernel_Exit();
}
