/*
*******************************************************************************************************************
*                                                           Module Orange
*                                                   the embedded Graphical module
*
*                                            Copyright(C), 2006-2008, Softwinner Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：    smenuitem.c
*
*Author：       William Wang
*
*Version :      1.1.0
*
*Date:          2010-5-20
*
*Description :  menuitem implement
*
*Others :       None at present.
*
* History :
*
* <Author>          <time>         <version>     <description>
*
* William Wang     2010-5-20         1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __SMENUITEM_C__
#define __SMENUITEM_C__

#include "slistbox_i.h"

/*
**********************************************************************************************************************
*                                               SItem_Destory
*
* author:
*
* date:             2010-5-21
*
* Description:      sitem destory
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 SItem_Destory(__listbox_sitem_t *sitem)
{
    if (sitem)
    {
        slistbox_free(sitem);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

/*
**********************************************************************************************************************
*                                               SItem_Create
*
* author:
*
* date:             2010-5-21
*
* Description:      sitem create
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__listbox_sitem_t *SItem_Create(__listbox_sitem_t *sitem_attr)
{
    __listbox_sitem_t         *newitem = NULL;

    if (sitem_attr == NULL)
    {
        return NULL;
    }

    newitem                 = (__listbox_sitem_t *)slistbox_malloc(sizeof(__listbox_sitem_t));

    if (!newitem)
    {
        goto error;
    }

    newitem->menuitem       = sitem_attr->menuitem;
    /*set menuitem lion handle*/
    newitem->cb             = sitem_attr->cb;
    newitem->rect           = sitem_attr->rect;
    return newitem;
error:

    if (newitem)
    {
        slistbox_free(newitem);
    }

    return NULL;
}

//------------------------------------------------------------------------------------------------------------
// 创建menuitem

//==================================================================
//function name:    SMenuItem_Create
//author:
//date:             2008-12-13
//description:      menuitem create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__listbox_menuitem_t *SMenuItem_Create(__slistbox_item_attr_t *item_attr, H_WIN  hWnd)
{
    __listbox_menuitem_t      *newitem = NULL;

    if (item_attr == NULL)
    {
        return NULL;
    }

    newitem                 = (__listbox_menuitem_t *)slistbox_malloc(sizeof(__listbox_menuitem_t));

    if (!newitem)
    {
        goto error;
    }

    newitem->Next           = NULL;
    newitem->Prev           = NULL;
    /*set menuitem lion handle*/
    newitem->cb             = (ListboxItem_DrawCB)esKRNL_GetCallBack((__pCBK_t)item_attr->cb);
    newitem->handle         = hWnd;
    newitem->attr           = item_attr->p_arg;
    newitem->enable         = item_attr->enable;
    newitem->item_w         = item_attr->item_w;
    return newitem;
error:

    if (newitem)
    {
        slistbox_free(newitem);
    }

    return NULL;
}


//------------------------------------------------------------------------------------------------------------
//销毁menuItem

//==================================================================
//function name:    SMenuItem_Destroy
//author:
//date:             2008-12-13
//description:      menuitem destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    SMenuItem_Destroy(__listbox_menuitem_t *menuitem)
{
    if (menuitem == NULL)
    {
        return OK;
    }
    else
    {
        slistbox_free((__listbox_menuitem_t *)menuitem);
        return OK;
    }
}

//------------------------------------------------------------------------------------------------------------
//取得上一个menuitem

//==================================================================
//function name:    SMenuItem_GetPrev
//author:
//date:             2008-12-13
//description:      menuitem getprev
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__listbox_menuitem_t *SMenuItem_GetPrev(__listbox_menuitem_t *menuitem)
{
    if (menuitem == NULL)
    {
        return NULL;
    }
    else
    {
        return menuitem->Prev;
    }
}


//------------------------------------------------------------------------------------------------------------
//取得下一个menuitem

//==================================================================
//function name:    MenuItem_GetNext
//author:
//date:             2008-12-13
//description:      menuitem getnext
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__listbox_menuitem_t *SMenuItem_GetNext(__listbox_menuitem_t *menuitem)
{
    if (menuitem == NULL)
    {
        return NULL;
    }
    else
    {
        return menuitem->Next;
    }
}

#endif
