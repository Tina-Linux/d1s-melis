/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    siconlist.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2010-5-20
*
*Description :  siconlist implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2010-5-20         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __SICONLIST_C__
#define __SICONLIST_C__

#include "sicons_i.h"

//------------------------------------------------------------------------------------------------------------
//创建iconList

//==================================================================
//function name:    SIconList_Create
//author:
//date:             2008-12-13
//description:      iconlist create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__icons_iconlist_t  *SIconList_Create(void)
{
    __icons_iconlist_t  *NewiconList = NULL;
    NewiconList = (__icons_iconlist_t *)sicons_malloc(sizeof(__icons_iconlist_t));

    if (!NewiconList)
    {
        return NULL;
    }
    else
    {
        NewiconList->head               = NULL;
        NewiconList->tail               = NULL;
        NewiconList->ItemCount          = 0;
        NewiconList->HeadItem           = NULL;
        return NewiconList;
    }
}

//------------------------------------------------------------------------------------------------------------
//插入item

//==================================================================
//function name:    SIconList_InsertAfter
//author:
//date:             2008-12-13
//description:      iconlist insertafter
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 SIconList_InsertAfter(__icons_iconlist_t *iconlist, __icons_iconitem_t *after, __icons_iconitem_t *iconitem)
{
    __icons_iconitem_t *Item;

    if ((!iconlist) || (!after) || (!iconitem))
    {
        return __LINE__;
    }

    if (after->Next == iconlist->head)
    {
        after->Next             = iconitem;
        iconitem->Prev          = after;
        iconitem->Next          = iconlist->head;
        iconlist->head->Prev    = iconitem;
        iconlist->tail          = iconitem;
        iconitem->itemno        = after->itemno + 1;
        iconlist->ItemCount++;
    }
    else
    {
        after->Next->Prev       = iconitem;
        iconitem->Next          = after->Next;
        after->Next             = iconitem;
        iconitem->Prev          = after;
        iconlist->ItemCount++;
        Item                    = iconitem;
        Item->itemno            = after->itemno;

        for (; Item != iconlist->head;)
        {
            Item->itemno        = Item->itemno + 1;
            Item                = Item->Next;
        }
    }

    return OK;
}


//------------------------------------------------------------------------------------------------------------
//删除item

//==================================================================
//function name:    SIconList_DelItem
//author:
//date:             2008-12-13
//description:      iconlist delitem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SIconList_DelItem(__icons_iconlist_t *iconlist, __icons_iconitem_t *iconitem)
{
    __icons_iconitem_t *item;
    __icons_iconitem_t *Cur_Item;
    __icons_iconitem_t *pitem;
    __icons_iconitem_t *nitem;
    //void *base;

    if (!iconlist)
    {
        return __LINE__;
    }

    if (!iconitem)
    {
        return OK;
    }

    if (iconlist->ItemCount == 1)
    {
        if (iconlist->head == iconitem)
        {
            iconlist->head = NULL;
            iconlist->tail = NULL;
            iconlist->ItemCount--;
            return SIconItem_Destroy(iconitem);
        }
        else
        {
            __wrn("iconitem not in iconlist!\n");
            return __LINE__;
        }
    }

    if (iconlist->tail == iconitem)
    {
        iconlist->tail                  = iconitem->Prev;
        iconlist->tail->Next            = iconlist->head;
        iconlist->head->Prev            = iconlist->tail;
        iconlist->ItemCount--;
        return SIconItem_Destroy(iconitem);
    }

    item                                = iconlist->head;

    while (item != iconlist->tail)
    {
        if (item == iconitem)
        {
            Cur_Item                    = iconitem->Next;

            for (; Cur_Item != iconlist->head;)
            {
                Cur_Item->itemno        = Cur_Item->itemno - 1;
                Cur_Item                = Cur_Item->Next;
            }

            if (item == iconlist->head)
            {
                iconlist->head          = iconitem->Next;
                iconlist->head->Prev    = iconlist->tail;
                iconlist->tail->Next    = iconlist->head;
                iconlist->ItemCount--;
            }
            else
            {
                pitem                   = iconitem->Prev;
                nitem                   = iconitem->Next;
                pitem->Next             = nitem;
                nitem->Prev             = pitem;
                iconlist->ItemCount--;
            }

            break;
        }

        item                            = item->Next;
    }

    return SIconItem_Destroy(iconitem);
}



//------------------------------------------------------------------------------------------------------------
// 删除全部item

//==================================================================
//function name:    SIconList_DelAll
//author:
//date:             2008-12-13
//description:      iconlist delall
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SIconList_DelAll(__icons_iconlist_t *iconlist)
{
    __icons_iconitem_t *item;
    __icons_iconitem_t *nitem;

    if (iconlist == NULL)
    {
        return __LINE__;
    }

    if (iconlist->ItemCount != 0)
    {
        item = iconlist->head;

        while (item != iconlist->tail)
        {
            nitem = item->Next;
            SIconItem_Destroy(item);
            item = nitem;
        }

        SIconItem_Destroy(iconlist->tail);
        iconlist->head      = NULL;
        iconlist->tail      = NULL;
        iconlist->HeadItem  = NULL;
        iconlist->ItemCount = 0;
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               Siconlist_FindNextEnable
*
* author:
*
* date:             2010-5-22
*
* Description:      siconlist findnextenable
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__icons_iconitem_t *SIconList_FindNextEnable(__icons_iconlist_t *iconlist, __icons_iconitem_t *cur_item)
{
    __u32                         count = 0;
    __icons_iconitem_t         *next_item;

    for (next_item = cur_item->Next; count < iconlist->ItemCount; next_item = next_item->Next)
    {
        if (next_item->enable)
        {
            return next_item;
        }

        count++;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               Siconlist_FindPrevEnable
*
* author:
*
* date:             2010-5-22
*
* Description:      siconlist findprevenable
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__icons_iconitem_t *SIconList_FindPrevEnable(__icons_iconlist_t *iconlist, __icons_iconitem_t *cur_item)
{
    __u32                         count = 0;
    __icons_iconitem_t         *prev_item;

    for (prev_item = cur_item->Prev; count < iconlist->ItemCount; prev_item = prev_item->Prev)
    {
        if (prev_item->enable)
        {
            return prev_item;
        }

        count++;
    }

    return NULL;
}

/*
**********************************************************************************************************************
*                                               SIconList_SetHeadItemFromItem
*
* author:
*
* date:             2010-5-22
*
* Description:      siconlist setheaditemfromitem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIconList_SetHeadItemFromItem(__icons_iconlist_t *iconlist, __icons_iconitem_t *cur_item)
{
    __s32                   dist;
    __s32                   head_no;
    __icons_iconitem_t    *head_item;
    dist = cur_item->itemno - iconlist->HeadItem->itemno + 1;

    if (dist > (__s32)iconlist->pagenum)
    {
        head_no = cur_item->itemno - (iconlist->pagenum - 1);
    }
    else if (dist <= 0)
    {
        head_no = cur_item->itemno;
    }
    else
    {
        return  dist - 1;
    }

    head_item = SIconList_GetIconitem(iconlist, head_no);

    if (head_item)
    {
        iconlist->HeadItem = head_item;
        return  cur_item->itemno - head_item->itemno;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SIconList_GetCurPageNum
*
* author:
*
* date:             2010-5-22
*
* Description:      siconlist getcurpagenum
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SIconList_GetCurPageNum(__icons_iconlist_t *iconlist)
{
    __u32   num;
    num = iconlist->ItemCount - 1 - iconlist->HeadItem->itemno;

    if (num < iconlist->pagenum)
    {
        return (num + 1);
    }

    return iconlist->pagenum;
}

//------------------------------------------------------------------------------------------------------------
//添加到尾部

//==================================================================
//function name:    SIconList_AddTail
//author:
//date:             2008-12-13
//description:      iconlist addtail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SIconList_AddTail(__icons_iconlist_t *iconlist,  __icons_iconitem_t *iconitem)
{
    if ((iconlist == NULL) || (iconitem == NULL))
    {
        return __LINE__;
    }

    if (iconlist->tail != NULL)
    {
        iconitem->itemno                = iconlist->tail->itemno + 1;
        iconlist->tail->Next            = iconitem;
        iconitem->Prev                  = iconlist->tail;
        iconlist->tail                  = iconitem;
        iconitem->Next                  = iconlist->head;
        iconlist->head->Prev            = iconitem;
        iconlist->ItemCount++;
    }
    else
    {
        iconlist->tail                  = iconitem;
        iconlist->head                  = iconitem;
        iconlist->ItemCount++;
        iconitem->Next                  = iconitem;
        iconitem->Prev                  = iconitem;
        iconlist->HeadItem              = iconitem;
        iconitem->itemno                = 0;
    }

    return OK;
}


//------------------------------------------------------------------------------------------------------------
// 添加到头部

//==================================================================
//function name:    SIconList_AddHead
//author:
//date:             2008-12-13
//description:      iconlist addhead
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SIconList_AddHead(__icons_iconlist_t *iconlist, __icons_iconitem_t *iconitem)
{
    __icons_iconitem_t *after;

    if ((iconlist == NULL) || (iconitem == NULL))
    {
        return __LINE__;
    }

    if (iconlist->head != NULL)
    {
        iconitem->itemno                = 0;
        iconlist->head->Prev            = iconitem;
        iconitem->Next                  = iconlist->head;
        iconlist->head                  = iconitem;
        iconitem->Prev                  = iconlist->tail;
        iconlist->tail->Next            = iconitem;
        iconlist->ItemCount++;
        after                           = iconitem->Next;

        for (; after != iconlist->head;)
        {
            after->itemno               = after->itemno + 1;
            after                       = after->Next;
        }
    }
    else
    {
        iconlist->head                  = iconitem;
        iconlist->tail                  = iconitem;
        iconlist->ItemCount++;
        iconitem->Next                  = iconitem;
        iconitem->Prev                  = iconitem;
        iconitem->itemno                = 0;
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               Siconlist_Geticonitem
*
* author:
*
* date:             2010-5-21
*
* Description:      siconlist geticonitem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__icons_iconitem_t *SIconList_GetIconitem(__icons_iconlist_t *iconlist, __s32 index)
{
    __u32                   num;
    __icons_iconitem_t   *ret_item = NULL;

    if (index >= iconlist->ItemCount || index < 0)
    {
        __wrn("input index ovrlap!\n");
        return NULL;
    }

    ret_item = iconlist->head;

    for (num = 1; num <= index; num++)
    {
        ret_item = ret_item->Next;
    }

    return  ret_item;
}
//------------------------------------------------------------------------------------------------------------
//取得头item的指针

//==================================================================
//function name:    SIconList_GetHead
//author:
//date:             2008-12-13
//description:      iconlist gethead
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__icons_iconitem_t *SIconList_GetHead(__icons_iconlist_t *iconlist)
{
    if (iconlist == NULL)
    {
        return NULL;
    }

    return iconlist->head;
}

//------------------------------------------------------------------------------------------------------------
//取得尾item的指针

//==================================================================
//function name:    SIconList_GetTail
//author:
//date:             2008-12-13
//description:      iconlist gettail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__icons_iconitem_t *SIconList_GetTail(__icons_iconlist_t *iconlist)
{
    if (iconlist == NULL)
    {
        return NULL;
    }

    return iconlist->tail;
}



//------------------------------------------------------------------------------------------------------------
//取得item的数目

//==================================================================
//function name:    SIconList_GetItemCount
//author:
//date:             2008-12-13
//description:      iconlist getitemcount
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SIconList_GetItemCount(__icons_iconlist_t *iconlist)
{
    __u32 ItemCount = 0;

    if (iconlist == NULL)
    {
        return 0;
    }

    ItemCount = iconlist->ItemCount;
    return ItemCount;
}

//------------------------------------------------------------------------------------------------------------
//销毁iconList

//==================================================================
//function name:    SIconList_Destroy
//author:
//date:             2008-12-13
//description:      iconlist destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32   SIconList_Destroy(__icons_iconlist_t *iconlist)
{
    //__icons_iconitem_t  *item;
    //__icons_iconitem_t  *nitem;
    if (!iconlist)
    {
        return __LINE__;
    }
    else
    {
        SIconList_DelAll(iconlist);
        sicons_free((void *)iconlist);
    }

    return OK;
}

#endif
