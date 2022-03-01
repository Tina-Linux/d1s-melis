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
#ifndef _GUI_KERNEL_E_H_
#define _GUI_KERNEL_E_H_

ORANGE_HMEM             ORANGE_ALLOC_AllocInit(const void *pInitData, ORANGE_ALLOC_DATATYPE Size);
ORANGE_HMEM             ORANGE_ALLOC_AllocNoInit(ORANGE_ALLOC_DATATYPE size);
ORANGE_HMEM             ORANGE_ALLOC_AllocZero(ORANGE_ALLOC_DATATYPE size);
void                    ORANGE_ALLOC_Free(ORANGE_HMEM  hMem);
void                    ORANGE_ALLOC_FreePtr(ORANGE_HMEM *phMem);
ORANGE_ALLOC_DATATYPE   ORANGE_ALLOC_GetSize(ORANGE_HMEM  hMem);
ORANGE_ALLOC_DATATYPE   ORANGE_ALLOC_GetMaxSize(void);
void                   *ORANGE_ALLOC_h2p(ORANGE_HMEM  hMem);
void                    ORANGE_ALLOC_Init(void);
ORANGE_HMEM             ORANGE_ALLOC_Realloc(ORANGE_HMEM hOld, int NewSize);



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
__s32           gui_timer_init(void);

void            GUI_NestUnlock(__gui_nestlock_t *nestlock);
void            GUI_NestLock(__gui_nestlock_t *nestlock, void *context) ;
void            GUI_NestLockInit(__gui_nestlock_t *nestlock) ;
void            GUI_NestLockExit(__gui_nestlock_t *nestlock);
__s32           GUI_NestLockAccept(__gui_nestlock_t *nestlock, void *context);
#endif  /* _GUI_CORE_KERNEL_PRIVATE_H_ */


