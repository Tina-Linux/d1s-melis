/*
*********************************************************************************************************
*                                                   eMOD
*                                  the Easy Portable/Player Operation System
*                                              mod_lemon sub-system
*
*                               (c) Copyright 2006-2007, Steven.ZGJ China
*                                           All Rights Reserved
*
* File   : gui_i.h
* Version: V1.0
* By     : steven.zgj
*********************************************************************************************************
*/

#ifndef _GUI_E_H_
#define _GUI_E_H_

int     GUI_Init(void);
int     GUI_Exit(void);
__s32   GUI_CharSetToEncode(__s32 charset_enm);

#include "win/win_e.h"
//#include "message/message_e.h"

//#include "core\\core_i.h"

#endif  /* _GUI_I_H_ */


