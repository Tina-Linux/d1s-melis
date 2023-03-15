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
#ifndef __MENU_ITEM__C____
#define __MENU_ITEM__C____

#include "MenuItem.h"

//------------------------------------------------------------------------------------------------------------
// 创建menuitem

//==================================================================
//function name:    MenuItem_Create
//author:
//date:             2008-12-13
//description:      menuitem create
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
MENUITEM_t *MenuItem_Create(__rectsz_t   *rectsz, __lstm_item_attr_t *item_attr, H_WIN handle)
{
    __u32           count;
    MENUITEM_t      *newitem = NULL;
    __u32           i;
    __u8            nctrls;

    if (rectsz == NULL || item_attr == NULL)
    {
        return NULL;
    }

    if (item_attr->nctrl && item_attr->ctrls == NULL)
    {
        ORANGE_WRN("input parameter error!\n");
        return NULL;
    }

    if (item_attr->nctrl > MENUITEM_MAXWIN)
    {
        nctrls = MENUITEM_MAXWIN;
    }
    else
    {
        nctrls = item_attr->nctrl;
    }

    newitem                 = (MENUITEM_t *)orange_malloc(sizeof(MENUITEM_t));

    if (!newitem)
    {
        goto error;
    }

    for (count = 0; count < MENUITEM_MAXWIN; count++)
    {
        newitem->child[count].hWnd  = 0;
    }

    newitem->nctrls         = 0;
    newitem->rect.width     = rectsz->width;
    newitem->rect.height    = rectsz->height;

    if (nctrls)
    {
        for (count = 0; count < nctrls; count++)
        {
            __gui_ctlwincreate_para_t       create_info;
            create_info.name                = item_attr->ctrls[count].name;
            create_info.spClassName         = item_attr->ctrls[count].spClassName;
            create_info.id                  = item_attr->ctrls[count].id;
            create_info.spCaption           = NULL;
            create_info.dwStyle             = item_attr->ctrls[count].dwStyle & (~(WS_VISIBLE | WS_LYRWIN));
            create_info.dwExStyle           = item_attr->ctrls[count].dwExStyle;
            /*暂不处理窗口越界的情况william 2009-10-28*/
            create_info.ctl_rect.x          = item_attr->ctrls[count].ctl_rect.x;
            create_info.ctl_rect.y          = item_attr->ctrls[count].ctl_rect.y;
            create_info.ctl_rect.width      = item_attr->ctrls[count].ctl_rect.width;
            create_info.ctl_rect.height     = item_attr->ctrls[count].ctl_rect.height;
            create_info.attr                = item_attr->ctrls[count].attr;
            create_info.hParent             = handle;
            create_info.hLayer              = NULL;
            newitem->child[count].hWnd      = GUI_CtrlWinCreate(&create_info);

            if (!newitem->child[count].hWnd)
            {
                ORANGE_WRN("create widget failed!\n");

                for (i = 0; i < count ; i++)
                {
                    if (newitem->child[count].hWnd)
                    {
                        GUI_CtrlWinDelete(newitem->child[count].hWnd);
                    }
                }

                goto error;
            }

            newitem->child[count].win_rect.x        = create_info.ctl_rect.x;
            newitem->child[count].win_rect.y        = create_info.ctl_rect.y;
            newitem->child[count].win_rect.width    = create_info.ctl_rect.width;
            newitem->child[count].win_rect.height   = create_info.ctl_rect.height;
            newitem->nctrls++;
            /*控件窗口的私有控制信息里面保存MENUITEM的句柄*/
            GUI_WinSetAddData(newitem->child[count].hWnd, (unsigned long)((H_ITEM)newitem));
        }
    }

    newitem->nctrls         = nctrls;
    newitem->Next           = NULL;
    newitem->Prev           = NULL;
    /*set menuitem lion handle*/
    newitem->cb             = (__pITEM_DRAW_CB)esKRNL_GetCallBack((__pCBK_t)item_attr->cb);
    newitem->foucs_w        = item_attr->item_w_focs;
    newitem->status         = 0;
    newitem->handle         = handle;
    newitem->attr           = item_attr->p_arg;
    return newitem;
error:

    if (newitem)
    {
        orange_mfree(newitem);
    }

    return NULL;
}


//------------------------------------------------------------------------------------------------------------
//销毁menuItem

//==================================================================
//function name:    MenuItem_Destroy
//author:
//date:             2008-12-13
//description:      menuitem destroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32    MenuItem_Destroy(MENUITEM_t *menuitem)
{
    __u32   count;

    if (menuitem == NULL)
    {
        return OK;
    }
    else
    {
        for (count = 0; count < menuitem->nctrls; count++)
        {
            if (menuitem->child[count].hWnd)
            {
                GUI_CtrlWinDelete(menuitem->child[count].hWnd);
            }
        }

        orange_mfree((MENUITEM_t *)menuitem);
        return OK;
    }
}

//------------------------------------------------------------------------------------------------------------
//取得上一个menuitem

//==================================================================
//function name:    MenuItem_GetPrev
//author:
//date:             2008-12-13
//description:      menuitem getprev
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
MENUITEM_t *MenuItem_GetPrev(MENUITEM_t *menuitem)
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
//function name:    Menuitem_GetNext
//author:
//date:             2008-12-13
//description:      menuitem getnext
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
MENUITEM_t *Menuitem_GetNext(MENUITEM_t *menuitem)
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

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif //__MENU_ITEM__C____
