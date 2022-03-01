/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_lemon sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : gui_core_i.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _GUI_CORE_I_H_
#define _GUI_CORE_I_H_

#include "gui_i.h"

#include "LCD_Protected.h"
#include "LCD_Private.h"
#include "LCD_L0_Generic.h"
#include "LCD_ConfDefaults.h"
#include "GUIDebug.h"
#include "GUI_Protected.h"
#include "GUI_Private.h"
#include "GUI_FontIntern.h"

#include "MemDev/gui_core_memdev_i.h"
#include "cc/gui_core_cc_i.h"
#include "gui_block_i.h"


int GUI_Core_Init(void);
int GUI_Kernel_Init(void) ;
int GUI_Kernel_Exit(void);

#endif  /* _GUI_CORE_I_H_ */


