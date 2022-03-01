/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    smenulist.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2010-5-20
*
*Description :  smenulist implement
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
#ifndef __SMENULIST_C__
#define __SMENULIST_C__

#include "slistbox_i.h"

//------------------------------------------------------------------------------------------------------------
//创建MenuList

//==================================================================
//function name:    SMenuList_Create
//author:
//date:             2008-12-13
//description:      menulist create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__listbox_menulist_t  *SMenuList_Create(void)
{
    __listbox_menulist_t  *NewMenuList = NULL;
    NewMenuList = (__listbox_menulist_t *)slistbox_malloc(sizeof(__listbox_menulist_t));

    if (!NewMenuList)
    {
        return NULL;
    }
    else
    {
        NewMenuList->head               = NULL;
        NewMenuList->tail               = NULL;
        NewMenuList->ItemCount          = 0;
        NewMenuList->HeadItem           = NULL;
        return NewMenuList;
    }
}

//------------------------------------------------------------------------------------------------------------
//插入item

//==================================================================
//function name:    SMenuList_InsertAfter
//author:
//date:             2008-12-13
//description:      menulist insertafter
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 SMenuList_InsertAfter(__listbox_menulist_t *menulist, __listbox_menuitem_t *after, __listbox_menuitem_t *menuitem)
{
    __listbox_menuitem_t *Item;

    if ((!menulist) || (!after) || (!menuitem))
    {
        return __LINE__;
    }

    if (after->Next == menulist->head)
    {
        after->Next             = menuitem;
        menuitem->Prev          = after;
        menuitem->Next          = menulist->head;
        menulist->head->Prev    = menuitem;
        menulist->tail          = menuitem;
        menuitem->itemno        = after->itemno + 1;
        menulist->ItemCount++;
    }
    else
    {
        after->Next->Prev       = menuitem;
        menuitem->Next          = after->Next;
        after->Next             = menuitem;
        menuitem->Prev          = after;
        menulist->ItemCount++;
        Item                    = menuitem;
        Item->itemno            = after->itemno;

        for (; Item != menulist->head;)
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
//function name:    SMenuList_DelItem
//author:
//date:             2008-12-13
//description:      menulist delitem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SMenuList_DelItem(__listbox_menulist_t *menulist, __listbox_menuitem_t *menuitem)
{
    __listbox_menuitem_t *item;
    __listbox_menuitem_t *Cur_Item;
    __listbox_menuitem_t *pitem;
    __listbox_menuitem_t *nitem;
    //void *base;

    if (!menulist)
    {
        return __LINE__;
    }

    if (!menuitem)
    {
        return OK;
    }

    if (menulist->ItemCount == 1)
    {
        if (menulist->head == menuitem)
        {
            menulist->head = NULL;
            menulist->tail = NULL;
            menulist->ItemCount--;
            return SMenuItem_Destroy(menuitem);
        }
        else
        {
            __wrn("menuitem not in menulist!\n");
            return __LINE__;
        }
    }

    if (menulist->tail == menuitem)
    {
        menulist->tail                  = menuitem->Prev;
        menulist->tail->Next            = menulist->head;
        menulist->head->Prev            = menulist->tail;
        menulist->ItemCount--;
        return SMenuItem_Destroy(menuitem);
    }

    item                                = menulist->head;

    while (item != menulist->tail)
    {
        if (item == menuitem)
        {
            Cur_Item                    = menuitem->Next;

            for (; Cur_Item != menulist->head;)
            {
                Cur_Item->itemno        = Cur_Item->itemno - 1;
                Cur_Item                = Cur_Item->Next;
            }

            if (item == menulist->head)
            {
                menulist->head          = menuitem->Next;
                menulist->head->Prev    = menulist->tail;
                menulist->tail->Next    = menulist->head;
                menulist->ItemCount--;
            }
            else
            {
                pitem                   = menuitem->Prev;
                nitem                   = menuitem->Next;
                pitem->Next             = nitem;
                nitem->Prev             = pitem;
                menulist->ItemCount--;
            }

            break;
        }

        item                            = item->Next;
    }

    return SMenuItem_Destroy(menuitem);
}



//------------------------------------------------------------------------------------------------------------
// 删除全部item

//==================================================================
//function name:    SMenuList_DelAll
//author:
//date:             2008-12-13
//description:      menulist delall
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SMenuList_DelAll(__listbox_menulist_t *menulist)
{
    __listbox_menuitem_t *item;
    __listbox_menuitem_t *nitem;

    if (menulist == NULL)
    {
        return __LINE__;
    }

    if (menulist->ItemCount != 0)
    {
        item = menulist->head;

        while (item != menulist->tail)
        {
            nitem = item->Next;
            SMenuItem_Destroy(item);
            item = nitem;
        }

        SMenuItem_Destroy(menulist->tail);
        menulist->head      = NULL;
        menulist->tail      = NULL;
        menulist->HeadItem  = NULL;
        menulist->ItemCount = 0;
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               SMenulist_FindNextEnable
*
* author:
*
* date:             2010-5-22
*
* Description:      smenulist findnextenable
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__listbox_menuitem_t *SMenuList_FindNextEnable(__listbox_menulist_t *menulist, __listbox_menuitem_t *cur_item)
{
    __u32                         count = 0;
    __listbox_menuitem_t         *next_item;

    for (next_item = cur_item->Next; count < menulist->ItemCount; next_item = next_item->Next)
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
*                                               SMenulist_FindPrevEnable
*
* author:
*
* date:             2010-5-22
*
* Description:      smenulist findprevenable
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__listbox_menuitem_t *SMenuList_FindPrevEnable(__listbox_menulist_t *menulist, __listbox_menuitem_t *cur_item)
{
    __u32                         count = 0;
    __listbox_menuitem_t         *prev_item;

    for (prev_item = cur_item->Prev; count < menulist->ItemCount; prev_item = prev_item->Prev)
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
*                                               SMenuList_SetHeadItemFromItem
*
* author:
*
* date:             2010-5-22
*
* Description:      smenulist setheaditemfromitem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SMenuList_SetHeadItemFromItem(__listbox_menulist_t *menulist, __listbox_menuitem_t *cur_item)
{
    __s32                   dist;
    __s32                   head_no;
    __listbox_menuitem_t    *head_item;
    dist = cur_item->itemno - menulist->HeadItem->itemno + 1;

    if (dist > (__s32)menulist->pagenum)
    {
        head_no = cur_item->itemno - (menulist->pagenum - 1);
    }
    else if (dist <= 0)
    {
        head_no = cur_item->itemno;
    }
    else
    {
        return  dist - 1;
    }

    head_item = SMenuList_GetMenuitem(menulist, head_no);

    if (head_item)
    {
        menulist->HeadItem = head_item;
        return  cur_item->itemno - head_item->itemno;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SMenuList_GetCurPageNum
*
* author:
*
* date:             2010-5-22
*
* Description:      smenulist getcurpagenum
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  SMenuList_GetCurPageNum(__listbox_menulist_t *menulist)
{
    __u32   num;
    num = menulist->ItemCount - 1 - menulist->HeadItem->itemno;

    if (num < menulist->pagenum)
    {
        return (num + 1);
    }

    return menulist->pagenum;
}

//------------------------------------------------------------------------------------------------------------
//添加到尾部

//==================================================================
//function name:    SMenuList_AddTail
//author:
//date:             2008-12-13
//description:      menulist addtail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SMenuList_AddTail(__listbox_menulist_t *menulist,  __listbox_menuitem_t *menuitem)
{
    if ((menulist == NULL) || (menuitem == NULL))
    {
        return __LINE__;
    }

    if (menulist->tail != NULL)
    {
        menuitem->itemno                = menulist->tail->itemno + 1;
        menulist->tail->Next            = menuitem;
        menuitem->Prev                  = menulist->tail;
        menulist->tail                  = menuitem;
        menuitem->Next                  = menulist->head;
        menulist->head->Prev            = menuitem;
        menulist->ItemCount++;
    }
    else
    {
        menulist->tail                  = menuitem;
        menulist->head                  = menuitem;
        menulist->ItemCount++;
        menuitem->Next                  = menuitem;
        menuitem->Prev                  = menuitem;
        menulist->HeadItem              = menuitem;
        menuitem->itemno                = 0;
    }

    return OK;
}


//------------------------------------------------------------------------------------------------------------
// 添加到头部

//==================================================================
//function name:    SMenuList_AddHead
//author:
//date:             2008-12-13
//description:      menulist addhead
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SMenuList_AddHead(__listbox_menulist_t *menulist, __listbox_menuitem_t *menuitem)
{
    __listbox_menuitem_t *after;

    if ((menulist == NULL) || (menuitem == NULL))
    {
        return __LINE__;
    }

    if (menulist->head != NULL)
    {
        menuitem->itemno                = 0;
        menulist->head->Prev            = menuitem;
        menuitem->Next                  = menulist->head;
        menulist->head                  = menuitem;
        menuitem->Prev                  = menulist->tail;
        menulist->tail->Next            = menuitem;
        menulist->ItemCount++;
        after                           = menuitem->Next;

        for (; after != menulist->head;)
        {
            after->itemno               = after->itemno + 1;
            after                       = after->Next;
        }
    }
    else
    {
        menulist->head                  = menuitem;
        menulist->tail                  = menuitem;
        menulist->ItemCount++;
        menuitem->Next                  = menuitem;
        menuitem->Prev                  = menuitem;
        menuitem->itemno                = 0;
    }

    return OK;
}

/*
**********************************************************************************************************************
*                                               SMenulist_GetMenuitem
*
* author:
*
* date:             2010-5-21
*
* Description:      smenulist getmenuitem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__listbox_menuitem_t *SMenuList_GetMenuitem(__listbox_menulist_t *menulist, __s32 index)
{
    __u32                   num;
    __listbox_menuitem_t   *ret_item = NULL;

    if (index >= menulist->ItemCount || index < 0)
    {
        __wrn("input index ovrlap!\n");
        return NULL;
    }

    ret_item = menulist->head;

    for (num = 1; num <= index; num++)
    {
        ret_item = ret_item->Next;
    }

    return  ret_item;
}
//------------------------------------------------------------------------------------------------------------
//取得头item的指针

//==================================================================
//function name:    SMenuList_GetHead
//author:
//date:             2008-12-13
//description:      menulist gethead
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__listbox_menuitem_t *SMenuList_GetHead(__listbox_menulist_t *menulist)
{
    if (menulist == NULL)
    {
        return NULL;
    }

    return menulist->head;
}

//------------------------------------------------------------------------------------------------------------
//取得尾item的指针

//==================================================================
//function name:    SMenuList_GetTail
//author:
//date:             2008-12-13
//description:      menulist gettail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__listbox_menuitem_t *SMenuList_GetTail(__listbox_menulist_t *menulist)
{
    if (menulist == NULL)
    {
        return NULL;
    }

    return menulist->tail;
}



//------------------------------------------------------------------------------------------------------------
//取得item的数目

//==================================================================
//function name:    SMenuList_GetItemCount
//author:
//date:             2008-12-13
//description:      menulist getitemcount
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SMenuList_GetItemCount(__listbox_menulist_t *menulist)
{
    __u32 ItemCount = 0;

    if (menulist == NULL)
    {
        return 0;
    }

    ItemCount = menulist->ItemCount;
    return ItemCount;
}

//------------------------------------------------------------------------------------------------------------
//销毁MenuList

//==================================================================
//function name:    SMenuList_Destroy
//author:
//date:             2008-12-13
//description:      menulist destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32   SMenuList_Destroy(__listbox_menulist_t *menulist)
{
    //__listbox_menuitem_t  *item;
    //__listbox_menuitem_t  *nitem;
    if (!menulist)
    {
        return __LINE__;
    }
    else
    {
        SMenuList_DelAll(menulist);
        slistbox_free((void *)menulist);
    }

    return OK;
}

#endif
