/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __MENU_LIST__C____
#define __MENU_LIST__C____


#include "MenuList.h"
#include "sysmemory.h"


//------------------------------------------------------------------------------------------------------------
//创建MenuList

//==================================================================
//function name:    MenuList_Create
//author:
//date:             2008-12-13
//description:      menulist create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
MENULIST_t  *MenuList_Create()
{
    MENULIST_t  *NewMenuList = NULL;
    NewMenuList = (MENULIST_t *)MEM_MALLOC(sizeof(MENULIST_t));

    if (!NewMenuList)
    {
        return NULL;
    }
    else
    {
        NewMenuList->head               = NULL;
        NewMenuList->load_head          = NULL;
        NewMenuList->load_tail          = NULL;
        NewMenuList->tail               = NULL;
        NewMenuList->ItemCount          = 0;
        NewMenuList->CurItem            = NULL;
        NewMenuList->UnCurItem          = NULL;
        NewMenuList->SelItem            = NULL;
        NewMenuList->UnSelItem          = NULL;
        NewMenuList->HeadItem           = NULL;
        NewMenuList->setCurItem         = 0;
        NewMenuList->bpp                = 0;
        NewMenuList->lyrdist            = 0;
        return NewMenuList;
    }
}

//------------------------------------------------------------------------------------------------------------
//插入item

//==================================================================
//function name:    MenuList_InsertAfter
//author:
//date:             2008-12-13
//description:      menulist insertafter
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 MenuList_InsertAfter(MENULIST_t *menulist, MENUITEM_t *after, MENUITEM_t *menuitem)
{
    MENUITEM_t *Item;

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
//function name:    MenuList_DelItem
//author:
//date:             2008-12-13
//description:      menulist delitem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    MenuList_DelItem(MENULIST_t *menulist, MENUITEM_t *menuitem)
{
    MENUITEM_t *item;
    MENUITEM_t *Cur_Item;
    MENUITEM_t *pitem;
    MENUITEM_t *nitem;
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
            menulist->CurItem = NULL;
            menulist->load_head = NULL;
            menulist->load_tail = NULL;
            return MenuItem_Destroy(menuitem);
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
        return MenuItem_Destroy(menuitem);
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

    return MenuItem_Destroy(menuitem);
}



//------------------------------------------------------------------------------------------------------------
// 删除全部item

//==================================================================
//function name:    MenuList_DelAll
//author:
//date:             2008-12-13
//description:      menulist delall
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    MenuList_DelAll(MENULIST_t *menulist)
{
    MENUITEM_t *item;
    MENUITEM_t *nitem;

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
            MenuItem_Destroy(item);
            item = nitem;
        }

        MenuItem_Destroy(menulist->tail);
        menulist->CurItem = NULL;
        menulist->setCurItem = 0;
        menulist->head = NULL;
        menulist->tail = NULL;
        menulist->ItemCount = 0;
    }

    return OK;
}



//------------------------------------------------------------------------------------------------------------
//添加到尾部

//==================================================================
//function name:    MenuList_AddTail
//author:
//date:             2008-12-13
//description:      menulist addtail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    MenuList_AddTail(MENULIST_t *menulist,  MENUITEM_t *menuitem)
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
        menuitem->itemno                = 0;
    }

    return OK;
}


//------------------------------------------------------------------------------------------------------------
// 添加到头部

//==================================================================
//function name:    MenuList_AddHead
//author:
//date:             2008-12-13
//description:      menulist addhead
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    MenuList_AddHead(MENULIST_t *menulist, MENUITEM_t *menuitem)
{
    MENUITEM_t *after;

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



//------------------------------------------------------------------------------------------------------------
//取得头item的指针

//==================================================================
//function name:    MenuList_GetHead
//author:
//date:             2008-12-13
//description:      menulist gethead
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
MENUITEM_t *MenuList_GetHead(MENULIST_t *menulist)
{
    if (menulist == NULL)
    {
        return NULL;
    }

    return menulist->head;
}


//------------------------------------------------------------------------------------------------------------
//取得当前item

//==================================================================
//function name:    MenuList_GetCurItem
//author:
//date:             2008-12-13
//description:      menulist getcuritem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
H_WIN MenuList_GetCurItem(MENULIST_t *menulist)
{
    if (menulist == NULL)
    {
        return NULL;
    }

    return GUI_ItemP2H(menulist->CurItem);
}


//------------------------------------------------------------------------------------------------------------
//设置当前item

//==================================================================
//function name:    MenuList_SetCurItem
//author:
//date:             2008-12-13
//description:      menulist setcuritem
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 MenuList_SetCurItem(MENULIST_t *menulist, MENUITEM_t *menuitem)
{
    if (menulist == NULL)
    {
        return __LINE__;
    }

    menulist->CurItem = menuitem;
    return OK;
}



//------------------------------------------------------------------------------------------------------------
//取得尾item的指针

//==================================================================
//function name:    MenuList_GetTail
//author:
//date:             2008-12-13
//description:      menulist gettail
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
MENUITEM_t *MenuList_GetTail(MENULIST_t *menulist)
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
//function name:    MenuList_GetItemCount
//author:
//date:             2008-12-13
//description:      menulist getitemcount
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    MenuList_GetItemCount(MENULIST_t *menulist)
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
//function name:    MenuList_Destroy
//author:
//date:             2008-12-13
//description:      menulist destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32   MenuList_Destroy(MENULIST_t *menulist)
{
    //MENUITEM_t  *item;
    //MENUITEM_t  *nitem;
    if (!menulist)
    {
        return __LINE__;
    }
    else
    {
        MenuList_DelAll(menulist);
        MEM_FREE((void *)menulist);
    }

    return OK;
}

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------



#endif //__MENU_LIST__C____
