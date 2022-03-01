/*
*******************************************************************************************************************
*                                                           display driver
*                                                   the display dirver support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：      MenuItem.c
*
*Author：         William Wang
*
*Version :        1.1.0
*
*Date:            2008-11-24
*
*Description :    menuitem implement definition
*
*Others :         None at present.
*
* History :
*
* <Author>          <time>          <version>     <description>
*
* William Wang     2008-11-24         1.1.0        Create File
*
*******************************************************************************************************************
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


