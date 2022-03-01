/*
*******************************************************************************************************************
*                                                               Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    GUI_Timer.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-11-2
*
*Description :  gui timer const function definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-11-2         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __GUI_TIMER_H__
#define __GUI_TIMER_H__

#include "GUI_Kernel_i.h"

__bool          GUI_SetTimer(H_WIN hWnd, __u16 id, __u32 speed, __pGUI_TIMERPROC timer_proc);
void            GUI_DispatchTimerMessage(__u32 inter);
__gui_timer_t  *gui_get_timer(__s32 slot);
__s32           gui_get_timer_slot(H_WIN hWnd, __u16 id);
void            gui_remove_timer(__gui_timer_t *timer, __s32 slot);
void            gui_move_timer_last(__gui_timer_t *timer, __s32 slot);
__s32           GUI_KillTimer(H_WIN hWnd, __u16 id);
__bool          GUI_ResetTimer(H_WIN hWnd, __u16 id, __u32 speed, __pGUI_TIMERPROC timer_proc);
__bool          GUI_IsTimerInstalled(H_WIN hWnd, __u16 id);
__bool          GUI_HaveFreeTimer(void);
__u32           GUI_GetTickCount(void);
__u32           GUI_GetTimerExpired(H_WIN hWnd, __u16 id);
#endif
