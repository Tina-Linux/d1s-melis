/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    gui_win_i.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  gui 窗口外部头文件引用信息
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>                  <version>     <description>
*
* William Wang     2009-8-4         1.1.0          Create File
*
*******************************************************************************************************************
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
