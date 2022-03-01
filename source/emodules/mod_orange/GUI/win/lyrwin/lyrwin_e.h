/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              Lion Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : win
* File    : win.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/
#ifndef __LYRWIN_E_H__
#define __LYRWIN_E_H__
#include <kconfig.h>

extern __s32  GUI_LyrWinSetSrcWindow(H_WIN lyrwin, const RECT *rect);
extern __s32  GUI_LyrWinSetScnWindow(H_WIN lyrwin, const RECT *rect);
extern __s32  GUI_LyrWinGetSrcWindow(H_WIN lyrwin, RECT *rect);
extern __s32  GUI_LyrWinGetScnWindow(H_WIN lyrwin, RECT *rect);
extern __s32  GUI_LyrWinGetFB(H_WIN lyrwin, FB *fb);
extern __s32  GUI_LyrWinSetFB(H_WIN lyrwin, const FB *fb);
extern __s32  GUI_LyrWinSetBackFB(__gui_back_src_t mode, void *src);
extern __s32  GUI_LyrWinSetSta(H_WIN lyrwin, __gui_lyrwin_sta_t status);
extern __s32  GUI_LyrWinSel(H_WIN lyrwin);
extern __s32  GUI_LyrWinCacheOn(void);
extern __s32  GUI_LyrWinCacheOff(void);
extern __s32  GUI_LyrWinFlushFB(H_WIN lyrwin);
extern __s32  GUI_LyrWinSetAlpha(H_WIN lyrwin, __u8 alpha);
extern __s32  GUI_LyrWinAlphaOn(H_WIN lyrwin);
extern __s32  GUI_LyrWinAlphaOff(H_WIN lyrwin);
extern __s32  GUI_LyrWinCKOn(H_WIN lyrwin);
extern __s32  GUI_LyrWinCKOff(H_WIN lyrwin);
extern __s32  GUI_LyrWinSetBottom(H_WIN lyrwin);
extern __s32  GUI_LyrWinSetTop(H_WIN lyrwin);

extern H_LYR GUI_LyrWinGetHWLyr(H_LYR h_lyr);

#endif
