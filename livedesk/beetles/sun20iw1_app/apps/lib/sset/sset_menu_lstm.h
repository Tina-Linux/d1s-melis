/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : sset_menu_lstm.h
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/
#ifndef __SSET_MENU_LSTM_H__
#define __SSET_MENU_LSTM_H__

/*
*************************************************************************************
*                                       SSET_MenuLstm_Create
*
*Description: SSET子菜单创建
*
*Arguments  : hparent
*             howner
*             pitem
*
*Return     : NULL
*
*************************************************************************************
*/
H_WIN SSET_MenuLstm_Create(H_WIN hparent, H_WIN hlayer, __sset_item_t *pitem);
/*
*************************************************************************************
*                                       SSET_MenuLstm_Destroy
*
*Description: SSET子菜单销毁
*
*Arguments  : h_win  : 要销毁的窗口句柄
*
*Return     : NULL
*
*************************************************************************************
*/
__s32 SSET_MenuLstm_Destroy(H_WIN h_win);
/*
*************************************************************************************
*                                       SSET_MenuLstm_SetFocus
*
*Description:
*
*Arguments  :
*
*Return     :
*
*************************************************************************************
*/
__s32 SSET_MenuLstm_SetFocus(H_WIN h_win, __u32 fcs_id);
/*
************************************************************************************************************************
*                                       SSET_SubMenuUpdateItem
*
*Description: 更新子菜单submenu的一个条目
*
*Arguments  :
*
*Return     :
*
************************************************************************************************************************
*/
__s32 SSET_MenuLstm_UpdateItem(H_WIN h_win, __u32 item_no);


#endif  /* __SSET_MENU_LSTM_H__ */
