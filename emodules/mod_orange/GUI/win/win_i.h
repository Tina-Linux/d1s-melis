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
#ifndef __WIN_I_H__
#define __WIN_I_H__

#include "gui_i.h"

extern __u8                          server_id;                                 /*系统服务线程的线程ID*/
extern H_WIN                         capture_wnd;                               /*当前的capture窗口句柄*/
extern H_WIN                         touchfocus_wnd;                               /*当前的capture窗口句柄*/
extern __win_msgqueue_t              server_msgs;                               /*系统服务线程的消息队列*/
extern pguiwin                       active_mainwnd;                            /*当前的activemain窗口句柄*/
extern __gui_lyrwin_t                *sg_InVisLayerList;
extern __gui_lyrwin_t                *sg_VisiLayerList;                         /*图层的Z序列表*/
extern __gui_clipinfo_t              sg_ScrGCRInfo;                             /*屏幕的剪切域信息*/
extern __gui_lyrwin_t                *sg_InVisSpriteList;
extern __gui_lyrwin_t                *sg_VisiSpriteList;

__s32               GUI_LyrGetPrio(H_WIN  hWnd);
static void                add_new_window(__gui_zorderinfo_t *zorder, __gui_zordernode_t *pNode);
static void                remove_window(__gui_zorderinfo_t *zorder, pguiwin pWin);
void                Win_AttachToZorder(H_WIN  hWnd);
void                Win_DeattachFromZorder(__gui_win_t *pWin);
extern void         Win_AttachToParent(__gui_win_t *pWin);
extern void         Win_DeattachFromParent(__gui_win_t *pWin);
extern __gui_win_t  *GUI_ChangActiveManWindow(__gui_win_t  *pWin);
extern void         Win_DeattachFromZorder(__gui_win_t *pWin);
extern H_WIN        GUI_WinCreate(__gui_win_t *create_info, __u32 extNums);
extern __s32        GUI_WinDelete(H_WIN hWnd);
extern void         GUI_FrameWinToScreen(H_WIN hWnd, __gui_rect_t *screct);
extern void         GUI_FrameClentToScreen(H_WIN hWnd, __gui_rect_t *screct);
extern void         GUI_ScreenToFrameWin(H_WIN hWnd, __gui_rect_t *screct, __gui_rect_t *win_rect);
extern void         GUI_WidgetToScreen(H_WIN hWnd, __gui_rect_t *screct);
extern __bool       GUI_WinGetClientRect(H_WIN hWnd, __gui_rect_t *prc);
extern void         GUI_WinLock(void);
extern void         GUI_WinUnlock(void);
extern void         GUI_WinAddNewHostedWin(__gui_win_t *pHosting, __gui_win_t *pHosted);
extern __bool       GUI_WinShow(H_WIN hWnd, int iCmdShow);
#include "win_e.h"

#endif /* __WIN_I_H__ */
