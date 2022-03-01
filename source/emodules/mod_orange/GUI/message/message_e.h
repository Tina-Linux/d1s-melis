/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    message_e.h
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2009-8-4
*
*Description :  gui 外部头文件引用定义
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
#ifndef __MESSAGE_E_H__
#define __MESSAGE_E_H__

#include "gui_i.h"


extern __bool       InitFreeQMSGList(void);
extern __bool       DestroyFreeQMSGList(void);
extern __bool       GUI_HavePendingMessage(H_WIN hWnd);
extern __s32        GUI_BroadcastMessage(__gui_msg_t *pmsg);
extern __s32        GUI_PostSyncMessage(__gui_msg_t *pmsg);
extern __s32        GUI_SendMessgae(__gui_msg_t *msg);
extern __s32        GUI_PostMessage(__gui_msg_t *msg);
extern __s32        GUI_SendNotifyMessage(__gui_msg_t *msg);
extern __s32        GUI_GetMessage(__gui_msg_t *msg, H_WIN  hManWin);
extern __s32        GUI_DispatchMessage(__gui_msg_t *msg);
extern __s32        GUI_PostQuitMessage(H_WIN  hWnd);
extern __s32        GUI_SendAsyncMessage(__gui_msg_t *msg);
extern __s32        GUI_ThrowAwayMessages(H_WIN hWnd);
extern void         GUI_NotifyMSGQ(__win_msgqueue_t *qmsg);

#endif /* __WIN_PRIVATE_H__ */
