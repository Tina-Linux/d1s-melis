/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
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
