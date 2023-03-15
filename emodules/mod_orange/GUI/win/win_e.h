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
