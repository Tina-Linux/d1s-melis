/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtlist.h
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      txtbox itemlist definition
*
*Others :           None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-12-17         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __TXTLIST_H__
#define __TXTLIST_H__

#include "txtcfg.h"
#include "txtitem.h"
//------------------------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------------------------
typedef struct tag_TXTLIST
{
    txtitem_t          *head;
    txtitem_t          *tail;
    txtitem_t          *CurItem;
    txtitem_t          *FoucsItem;
    __u32               count;
    __u32               ItemCount;
    __u32               bpp;
    __u32               ItemSize;
    txtitem_t          *PrevItem;
} txtlist_t;


txtlist_t      *TxtList_Create(void);
__u32           TxtList_DelAll(txtlist_t *txtlist);
__u32           TxtList_AddTail(txtlist_t *txtlist,  txtitem_t *txtitem);
__u32           TxtList_SetCurItem(txtlist_t *txtlist, txtitem_t *txtitem);
__u32           TxtList_GetItemCount(txtlist_t *txtlist);
__u32           TxtList_Destroy(txtlist_t *txtlist);

#endif
