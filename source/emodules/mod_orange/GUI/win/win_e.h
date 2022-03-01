/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º    win_e.h
*
*Author£º       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-11
*
*Description :  windows extern function and variable definition
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2009-8-11         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __WIN_E_H__
#define __WIN_E_H__

extern int          GUI_DestroyAutoPanel(void *hdle);
extern void        *GUI_CreateAutoPanel(int id);
extern __s32        GUI_WinInit(void);
extern __s32        GUI_WinExit(void);
extern H_WIN        GUI_SetFocusChild(H_WIN hWnd);
extern H_WIN        GUI_GetFocusChild(H_WIN hWnd);
extern H_WIN        GUI_GetActiveControl(H_WIN hWnd);
extern __bool       GUI_IsWindow(H_WIN hWnd);
extern __u32        GUI_SysToGUIColor(__color_t *color);

extern __s32        GUI_LyrGetPrio(H_LYR  hLyr);
extern __s32        Layer_AttachToVisableList(H_LYR  hLyr);
extern __s32        Layer_DeattachFromVisList(H_LYR hLyr);
extern __s32        Layer_AttachToInVisList(H_LYR  hLyr);
extern __s32        Layer_DeattachFromInVisList(H_LYR  hLyr);
extern __s32        Layer_ListSetTop(H_LYR  hLyr);
extern __s32        Layer_ListSetBottom(H_LYR  hLyr);

extern void         Win_AttachToZorder(H_WIN hWnd);

extern __u32        GUI_WinGetWinThreadId(H_WIN  hWnd);
extern void         GUI_ClientToScreen(H_WIN hWnd, __pos_t *client_pos, __pos_t *sc_pos);
extern void         GUI_ScreenToClient(H_WIN hWnd, __pos_t *sc_pos, __pos_t *client_pos);
extern void         GUI_WindowToScreen(H_WIN hWnd, __pos_t *win_pos, __pos_t *sc_pos);
extern void         GUI_ScreenToWindow(H_WIN hWnd, __pos_t *sc_pos, __pos_t *win_pos);
extern void         GUI_FBToScreen(H_WIN hWnd, __pos_t *fb_pos, __pos_t *sc_pos);
extern void         GUI_ScreenToFB(H_WIN hWnd, __pos_t *sc_pos, __pos_t *fb_pos);

extern void         GUI_WinFBToClient(H_WIN hWnd, const __gui_rect_t *rcFB, __gui_rect_t *rcClient);

#include "ctrlwin/ctrlwin_e.h"
#include "manwin/manwin_e.h"
#include "frmwin/frmwin_e.h"
#include "lyrwin/lyrwin_e.h"
#endif /* __WIN_PRIVATE_H__ */
