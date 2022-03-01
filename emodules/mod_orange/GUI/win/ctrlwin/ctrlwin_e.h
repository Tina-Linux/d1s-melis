/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : mod_orange
* File    : ctrlwin_private.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#ifndef __CTRLWIN_E_H__
#define __CTRLWIN_E_H__

extern H_WIN  GUI_CtrlWinCreate(__gui_ctlwincreate_para_t *create_info);
extern __s32  GUI_CtrlWinDelete(H_WIN hWnd);
extern unsigned long  GUI_CtrlWinGetAddData(H_WIN hWnd);
extern __s32  GUI_CtrlWinSetAddData(H_WIN hWnd, unsigned long dwAddData);
extern __s32  GUI_CtrlWinDefaultProc(__gui_msg_t *msg);

#endif /* __WIN_PRIVATE_H__ */
