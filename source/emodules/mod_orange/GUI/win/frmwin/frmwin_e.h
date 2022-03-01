/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : orange
* File    : frmwin_private.h
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/

#ifndef __FRMWIN_E_H__
#define __FRMWIN_E_H__

extern void     GUI_FrmWinDestroyAllCtrlWins(H_WIN hDlg);
extern __s32    GUI_FrmWinDefaultProc(__gui_msg_t *msg);
extern H_WIN    GUI_FrmWinCreate(__gui_framewincreate_para_t *create_info);
extern __s32    GUI_FrmWinDelete(H_WIN hWnd);

#endif /* __FRMWIN_PRIVATE_H__ */
