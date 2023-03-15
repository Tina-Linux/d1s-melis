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
