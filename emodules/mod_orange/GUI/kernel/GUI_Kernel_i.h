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
* File    : gui_core_kernel_i.h
*
* By      : Steven
* Version : v1.0
* Date    : 2009-1-6 9:30:42
**********************************************************************************************************************
*/
#ifndef _GUI_KERNEL_I_H_
#define _GUI_KERNEL_I_H_

#include "gui_i.h"

void    ORANGE_KrnlLock(void);
void    ORANGE_KrnlUnlock(void);
void    ORANGE_KrnlAccept(void);

void     ORANGE_KernelInit(void);
void     ORANGE_KernelExit(void);

#endif  /* _GUI_CORE_KERNEL_I_H_ */


