/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        manwin_e.h
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2009-8-1
*
*Description :      manwin extern function definition
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>        <version>     <description>
*
* William Wang     2009-8-1         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __MANWIN_E_H__
#define __MANWIN_E_H__

extern H_WIN    GUI_GetActiveManWin(void);
extern H_WIN    GUI_SetActiveManWin(H_WIN actwin);
extern H_WIN    GUI_GetTouchFocusWin(__s32 x, __s32 y);
extern __s32    GUI_ManWinThreadCleanup(H_WIN hMainWnd);

extern __s32    GUI_ManWinDefaultProc(__gui_msg_t *msg) ;
extern H_WIN    GUI_ManWinCreate(__gui_manwincreate_para_t *create_info);
extern __s32    GUI_ManWinDelete(H_WIN hWnd);

#endif /* __WIN_PRIVATE_H__ */
