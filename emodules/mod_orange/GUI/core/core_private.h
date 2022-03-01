/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              eMOD sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : core_private.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef __CORE_PRIVATE_H__
#define __CORE_PRIVATE_H__

#include "GUI_Private.h"
#include "core_i.h"

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


int GUI_Core_Init(void);

#endif  /* __CORE_PRIVATE_H__ */


