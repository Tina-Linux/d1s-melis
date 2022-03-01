/*
*******************************************************************************************************************
*                                                           Listmenu  Framework
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name£º        txtlist.c
*
*Author£º           William Wang
*
*Version :          1.1.0
*
*Date:              2008-12-17
*
*Description :      txtbox item list function implement
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
#ifndef __TXTLIST_C__
#define __TXTLIST_C__

#include "txtbox_i.h"
#include "txtitem.h"

//==================================================================
//function name:    TxtList_Create
//author:
//date:             2008-12-18
//description:      txtlist create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

txtlist_t      *TxtList_Create(void)
{
    txtlist_t  *NewMenuList = NULL;
    NewMenuList = (txtlist_t *)TXTBOX_MALLOC(sizeof(txtlist_t));

    if (!NewMenuList)
    {
        return NULL;
    }
    else
    {
        NewMenuList->head               = NULL;
        NewMenuList->tail               = NULL;
        NewMenuList->ItemCount          = 0;
        NewMenuList->CurItem            = NULL;
        NewMenuList->PrevItem           = NULL;
        NewMenuList->bpp                = 0;
        return NewMenuList;
    }
}

//==================================================================
//function name:    TxtList_DelAll
//author:
//date:             2008-12-18
//description:      txtlist delall
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 TxtList_DelAll(txtlist_t *txtlist)
{
    txtitem_t *item;
    txtitem_t *nitem;

    if (txtlist->ItemCount != 0)
    {
        item = txtlist->head;

        while (item != txtlist->tail)
        {
            nitem = item->Next;
            txtitem_Destroy(item);
            item = nitem;
        }

        txtitem_Destroy(txtlist->tail);
        txtlist->CurItem = NULL;
        txtlist->head = NULL;
        txtlist->tail = NULL;
        txtlist->ItemCount = 0;
    }

    return OK;
}

//==================================================================
//function name:    TxtList_AddTail
//author:
//date:             2008-12-18
//description:      txtlist addtail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32   TxtList_AddTail(txtlist_t *txtlist,  txtitem_t *txtitem)
{
    if (txtlist->tail != NULL)
    {
        txtitem->itemno                = txtlist->tail->itemno + 1;
        txtlist->tail->Next            = txtitem;
        txtitem->Prev                  = txtlist->tail;
        txtlist->tail                  = txtitem;
        txtitem->Next                  = txtlist->head;
        txtlist->head->Prev            = txtitem;
        txtlist->ItemCount++;
    }
    else
    {
        txtlist->tail                  = txtitem;
        txtlist->head                  = txtitem;
        txtlist->ItemCount++;
        txtitem->Next                  = txtitem;
        txtitem->Prev                  = txtitem;
        txtitem->itemno                = 0;
    }

    return OK;
}

//==================================================================
//function name:    TxtList_SetCurItem
//author:
//date:             2008-12-18
//description:      txtlist setcuritem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32  TxtList_SetCurItem(txtlist_t *txtlist, txtitem_t *txtitem)
{
    if (txtlist == NULL)
    {
        return __LINE__;
    }

    txtlist->CurItem = txtitem;
    return OK;
}

//==================================================================
//function name:    TxtList_GetItemCount
//author:
//date:             2008-12-18
//description:      txtlist getitemcount
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32   TxtList_GetItemCount(txtlist_t *txtlist)
{
    __u32 ItemCount = 0;

    if (txtlist == NULL)
    {
        return 0;
    }

    ItemCount = txtlist->ItemCount;
    return ItemCount;
}

//==================================================================
//function name:    TxtList_Destroy
//author:
//date:             2008-12-18
//description:      txtlist destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32  TxtList_Destroy(txtlist_t *txtlist)
{
    if (!txtlist)
    {
        return __LINE__;
    }
    else
    {
        TxtList_DelAll(txtlist);
        TXTBOX_MEMFREE((void *)txtlist);
    }

    return OK;
}

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------


#endif
