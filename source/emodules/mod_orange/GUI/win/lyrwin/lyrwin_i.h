/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : layer_private.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-30 8:37:07
**********************************************************************************************************************
*/

#ifndef __LAYER_I_H__
#define __LAYER_I_H__

#include "../win_i.h"
#include "fb_ops.h"

#define GUI_CURSOR_NOINIT           0
#define GUI_CURSOR_SHOW             1
#define GUI_CURSOR_HIDE             2

#define GUI_LYRWIN_STA_ONEX         (GUI_LYRWIN_STA_ERR + 1)
#define GUI_LYRWIN_STA_OFFEX        (GUI_LYRWIN_STA_ERR + 2)
#define GUI_LYRWIN_STA_SLEEPEX      (GUI_LYRWIN_STA_ERR + 3)
#define GUI_LYRWIN_STA_SUSPENDEX    (GUI_LYRWIN_STA_ERR + 4)

void    GUI_LyrLock(void);
void    GUI_LyrUnlock(void);
__u16   GUI_LyrLockAccept(void);
void    GUI_LyrInitOS(void);

extern  __s32 GUI_LogicLayerSetScnWin(__u8 dir, __mp *disp_handle, __hdle hlayer, __rect_t *rect);

extern  __s32 GUI_LogicLayerSetSrcWin(__u8 dir, __mp *disp_handle, __hdle hlayer, __rect_t *rect);

extern  __s32 GUI_LogicLayerSetPara(__u8 dir, __mp *disp_handle, __hdle hlayer, __disp_layer_para_t *player);

extern  __s32 GUI_LogicLayerRequest(__u8 dir, __mp *disp_handle, __hdle hlayer, __disp_layer_para_t *player);

extern  __s32 GUI_LogicLayerSetFb(__u8 dir, __mp *disp_handle, __hdle hlayer, FB *pfb);

#endif  /* #define __LAYER_PRIVATE_H__  */
