/*
*******************************************************************************************************************
*                                                           Mod orange
*                                                   the embedded graphic support module
*
*                                            Copyright(C), 2006-2008, SoftWinners Microelectronic Co., Ltd.
*                                                          All Rights Reserved
*
*File Name：     Listmenu.c
*
*Author：        William Wang
*
*Version :       1.1.0
*
*Date:           2008-8-21
*
*Description :   listmenu main function implement
*
*Others :        None at present.
*
* History :
*
* <Author>          <time>      <version>     <description>
*
* William Wang     2008-8-21      1.1.0          Create File
*
*******************************************************************************************************************
*/
#ifndef __LIST__MENU__C____
#define __LIST__MENU__C____

#include "speed_step.h"
#include "ListMenu_i.h"
#include "ListLayer_Process.h"
#include "ListLayer.h"
#include "ListBox.h"
#include "ListBox_Process.h"
#include "ListLayer_Item.h"

static __s32 ListMenuCtrlProc(__gui_msg_t  *msg);
void GUI_LstmCurProc(__listmenu_data_t *pData);
/*
**********************************************************************************************************************
*                                               RegisterButtonControl
*
* author:
*
* date:             2009-8-4
*
* Description:      registerbuttoncontrol
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool RegisterListMenuControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_LISTMENU;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = ListMenuCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == ORANGE_OK;
}

__listmenu_runmsg_t     default_msg;
extern int              step_table[92];
__u32                   GUI_LstmOnMessage(__listmenu_data_t *lstm, __listmenu_runmsg_t *pMsg);
/*
****************************************************************************************************
*
*             GUI_LstmRxThread
*
*  Description:
*       接收线程
*
*  Parameters:
*
*  Return value:
*
****************************************************************************************************
*/
void GUI_LstmRxThread(void *p_arg)
{
    __u8                            err;
    __listmenu_runmsg_t            *frm;

    while (1)
    {
        frm = (__listmenu_runmsg_t *)esKRNL_QPend(((__listmenu_data_t *)p_arg)->listmenu_RxQ, 0, &err);      /*receive listmenu message */

        if (err != OS_NO_ERR)
        {
            SYSTEM_PRINTF("received msg err = %x!", err);
        }
        else
        {
            if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
            {
                esKRNL_TDel(EXEC_prioself);
                return ;
            }

            /*listmenu on message function  */
            GUI_LstmOnMessage((__listmenu_data_t *)p_arg, frm);
        }
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LstmPrivCreate
*
* author:
*
* date:             2009-9-17
*
* Description:      gui lstmprivcreate
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__listmenu_data_t *GUI_LstmPrivCreate(H_WIN hListmenu)
{
    __u8                                    err;
    __u32                                   num;
    MENUITEM_t                              *menuitem;
    __u32                                   ret;
    __rectsz_t                              rect;
    __listmenu_data_t                       *plistmenu;
    __lstm_para_t                           *listmenu_priv;
    __gui_lyrwin_sta_t                      lyrwin_status;
    plistmenu   = (__listmenu_data_t *)orange_malloc(sizeof(__listmenu_data_t));

    if (!plistmenu)
    {
        ORANGE_WRN("malloc listmenu priv data failed!");
        return NULL;
    }

    /*初始化所有项目*/
    plistmenu->run_sem                  = NULL;
    plistmenu->dist_sem                 = NULL;
    plistmenu->move_sem                 = NULL;
    plistmenu->listlayer                = NULL;
    plistmenu->listmenu_RxQ             = NULL;
    plistmenu->listmenu_RxTID           = 0;
    plistmenu->mode                     = 0;
    plistmenu->handle                   = 0;
    plistmenu->user_cb                  = 0;
    plistmenu->status                   = 0;
    plistmenu->request                  = 0;
    plistmenu->msgbuff                  = 0;
    plistmenu->slave_lyr                = 0;
    plistmenu->lastmsg                  = 0xffffffff;
    plistmenu->lastkeyaction            = 0xffffffff;
    listmenu_priv                       = (__lstm_para_t *)GUI_WinGetAttr(hListmenu);

    if (listmenu_priv->nitem != 0)
    {
        if (listmenu_priv->items[0].item_w_focs / listmenu_priv->items[0].item_w_unfocs > 3)
        {
            __msg("foucs width too large width = %d,f_w = %d!", listmenu_priv->items[0].item_w_unfocs,
                  listmenu_priv->items[0].item_w_focs);
            goto error;
        }
    }

    plistmenu->handle                  = hListmenu;
    plistmenu->listlayer               = NULL;
    plistmenu->listmenu_RxQ            = NULL;

    if (listmenu_priv->nitem == 0)
    {
        plistmenu->listlayer           = (LIST_LAYER_MAN_t *)ListLayer_Create(GUI_WinGetLyrWin(hListmenu),
                                         (__hdle)plistmenu, 0, 0);
    }
    else
    {
        plistmenu->listlayer           = (LIST_LAYER_MAN_t *)ListLayer_Create(GUI_WinGetLyrWin(hListmenu), (__hdle)plistmenu,
                                         listmenu_priv->items[0].item_w_unfocs, listmenu_priv->items[0].item_w_focs);
    }

    if (!plistmenu->listlayer)
    {
        __msg("listlayer create failed!");
        goto  error;
    }

    /*为listmenu分配消息buffer*/
    plistmenu->msgbuff                 = (__listmenu_runmsg_t *)orange_malloc(sizeof(__listmenu_runmsg_t) * LISTMENU_FRM_RXQ_DEPTH);

    if (!plistmenu->msgbuff)
    {
        __msg("listmenu malloc message buffer error!");
        goto  error;
    }

    /*create listmenu receive message queue*/
    plistmenu->listmenu_RxQ             = esKRNL_QCreate(LISTMENU_FRM_RXQ_DEPTH);

    if ((plistmenu->listmenu_RxQ         == (__krnl_event_t *)0))
    {
        __msg("listmenu message queue create failed!");
        goto error;
    }

#ifdef SIM_PC_WIN
    __SAVE_MOD_EVN__
#endif
    /* 创建rxtx守护线程             */
    plistmenu->listmenu_RxTID           = esKRNL_TCreate(GUI_LstmRxThread, (void *)plistmenu, 0x2000, 2);
#ifdef SIM_PC_WIN
    __RESTORE_MOD_EVN__
#endif

    if (plistmenu->listmenu_RxTID        == 0)
    {
        __msg("listmenu receive thread create failed!");
        goto error;
    }

    if (plistmenu->listlayer != NULL)
    {
        /* 创建Item 维护线程             */
#ifdef SIM_PC_WIN
        __SAVE_MOD_EVN__
#endif
        plistmenu->listlayer->FoucsTID  = esKRNL_TCreate(ListLayer_FoucsItemThread,
                                          (void *)plistmenu->listlayer, 0x2000,
                                          KRNL_priolevel3);
#ifdef SIM_PC_WIN
        __RESTORE_MOD_EVN__
#endif

        if (plistmenu->listlayer->FoucsTID == 0)
        {
            __msg("listmenu foucs thread create failed!");
            goto error;
        }
    }

    plistmenu->listlayer->bk_color = listmenu_priv->bk_color;
    /*获取滚动速度*/
    plistmenu->listlayer->rollspeed = listmenu_priv->mode & 0x07;

    switch (plistmenu->listlayer->rollspeed)
    {
        case FAST_SPEED:
            plistmenu->listlayer->cursteps = 3;
            plistmenu->listlayer->nextsteps = 3;
            break;

        case LOW_SPEED:
            plistmenu->listlayer->cursteps = 15;
            plistmenu->listlayer->nextsteps = 15;
            break;

        case SPD_LEVEL1:
            plistmenu->listlayer->cursteps = 4;
            plistmenu->listlayer->nextsteps = 4;
            break;

        case SPD_LEVEL2:
            plistmenu->listlayer->cursteps = 5;
            plistmenu->listlayer->nextsteps = 5;
            break;

        case SPD_LEVEL3:
            plistmenu->listlayer->cursteps = 6;
            plistmenu->listlayer->nextsteps = 6;
            break;

        case SPD_LEVEL4:
            plistmenu->listlayer->cursteps = 8;
            plistmenu->listlayer->nextsteps = 8;
            break;

        case SPD_LEVEL5:
            plistmenu->listlayer->cursteps = 10;
            plistmenu->listlayer->nextsteps = 10;
            break;

        case SPD_LEVEL6:
            plistmenu->listlayer->cursteps = 13;
            plistmenu->listlayer->nextsteps = 13;
            break;
    }

    plistmenu->mode    = listmenu_priv->mode;
    plistmenu->run_sem = esKRNL_SemCreate(0);

    if (plistmenu->run_sem == NULL)
    {
        __inf("create run sem! fail");
        goto error;
    }

    /*按键消息信号量*/
    plistmenu->dist_sem = esKRNL_SemCreate(1);

    if (plistmenu->dist_sem == NULL)
    {
        __inf("create key sem! fail");
        goto error;
    }

    /*加载缩略图控制信号量*/
    plistmenu->load_sem = esKRNL_SemCreate(1);

    if (plistmenu->load_sem == NULL)
    {
        __inf("create load sem! fail");
        goto error;
    }

    /*touch滑动消息控制信号量*/
    plistmenu->move_sem = esKRNL_SemCreate(1);

    if (plistmenu->move_sem == NULL)
    {
        __inf("create move sem! fail");
        goto error;
    }

    if (plistmenu->listlayer != NULL)
    {
        if (plistmenu->listlayer->mode == VAR_MODE)
        {
            if (plistmenu->mode & LISTBOX_MOD)
            {
#ifdef SIM_PC_WIN
                __SAVE_MOD_EVN__
#endif
                /*create loaditem thread*/
                plistmenu->listlayer->LoadTID = esKRNL_TCreate(ListBox_LoadItemThread,
                                                (void *)plistmenu->listlayer, 0x8000, KRNL_priolevel4);        /* 创建Item 维护线程             */
#ifdef SIM_PC_WIN
                __RESTORE_MOD_EVN__
#endif
            }
            else
            {
                /*create loaditem thread*/
#ifdef SIM_PC_WIN
                __SAVE_MOD_EVN__
#endif
                plistmenu->listlayer->LoadTID = esKRNL_TCreate(ListLayer_LoadItemThread,
                                                (void *)plistmenu->listlayer, 0x8000, KRNL_priolevel4);        /* 创建Item 维护线程             */
#ifdef SIM_PC_WIN
                __RESTORE_MOD_EVN__
#endif
            }

            if (plistmenu->listlayer->LoadTID == 0)
            {
                __msg("listmenu load item thread create failed!");
                goto error;
            }
        }
        else
        {
            plistmenu->listlayer->LoadTID = 0;
        }
    }

    for (num = 0; num < listmenu_priv->nitem; num++)
    {
        rect.width                     = plistmenu->listlayer->listbuffer->rectsz.width;
        rect.height                    = listmenu_priv->items[num].item_w_unfocs;
        /*MenuItem Create*/
        menuitem                       = MenuItem_Create(&rect, &listmenu_priv->items[num], hListmenu);

        if (!menuitem)
        {
            ORANGE_WRN("create menuitem %d failed!", num);
            goto    error;
        }

        /*MenuItem Add to ListMenu*/
        MenuList_AddTail(plistmenu->listlayer->menulist, menuitem);
    }

    /*初始化listmenu状态*/
    plistmenu->status                  = 0;
    lyrwin_status = GUI_LyrWinGetSta(GUI_WinGetLyrWin(hListmenu));

    if (lyrwin_status == GUI_LYRWIN_STA_SUSPEND)
    {
        plistmenu->lyrsts                  = LISTMENU_SLEEP;
    }
    else
    {
        plistmenu->lyrsts                  = LISTMENU_WAKEUP;
    }

    if (plistmenu->listlayer->plistmenu->mode & LISTBOX_MOD)
    {
        plistmenu->listlayer->headlyr_pos.x = plistmenu->listlayer->userscnrect.x;
        plistmenu->listlayer->headlyr_pos.y = plistmenu->listlayer->userscnrect.y;
        plistmenu->listlayer->taillyr_pos.x = plistmenu->listlayer->userscnrect.x;
        num = (plistmenu->listlayer->scn_height - plistmenu->listlayer->userscnrect.y) / plistmenu->listlayer->menulist->head->rect.height;

        if (num > plistmenu->listlayer->menulist->ItemCount)
        {
            num = 0;
        }
        else
        {
            num = plistmenu->listlayer->menulist->ItemCount - num;
        }

        plistmenu->listlayer->taillyr_pos.y = plistmenu->listlayer->userscnrect.y - num * plistmenu->listlayer->menulist->head->rect.height;
    }
    else
    {
        plistmenu->listlayer->headlyr_pos.x = plistmenu->listlayer->userscnrect.x;
        plistmenu->listlayer->headlyr_pos.y = plistmenu->listlayer->userscnrect.y + listmenu_priv->curitemno * plistmenu->listlayer->menulist->head->rect.height;
        plistmenu->listlayer->taillyr_pos.x = plistmenu->listlayer->userscnrect.x;
        plistmenu->listlayer->taillyr_pos.y = plistmenu->listlayer->userscnrect.y -
                                              (plistmenu->listlayer->menulist->ItemCount - 1 - listmenu_priv->curitemno) *
                                              plistmenu->listlayer->menulist->head->rect.height;
    }

    /*设置曲线参数*/
    plistmenu->gcurv.screen     = plistmenu->listlayer->viewrect.height;
    plistmenu->gcurv.fb_len     = plistmenu->listlayer->menulist->ItemCount * (plistmenu->listlayer->menulist->head->rect.height);
    plistmenu->gcurv.fb_start   = 0;
    plistmenu->gcurv.frame      = plistmenu->listlayer->menulist->head->rect.height;
    plistmenu->gstate           = LSTMGLIDE_IDLE;
    plistmenu->len              = 0;
    plistmenu->index            = 0;
    /*设置listmenu当前的item*/
    ret = GUI_LstmSetCurItemNo(plistmenu, listmenu_priv->curitemno);

    if (ret != ORANGE_OK)
    {
        goto error;
    }

    return plistmenu;
error:

    if (plistmenu->listmenu_RxQ)
    {
        esKRNL_QDel(plistmenu->listmenu_RxQ, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->listmenu_RxTID)
    {
        esKRNL_TDel(plistmenu->listmenu_RxTID);
    }

    if (plistmenu->listlayer)
    {
        ListLayer_Destroy(plistmenu->listlayer);
    }

    if (plistmenu->run_sem)
    {
        esKRNL_SemDel(plistmenu->run_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->dist_sem)
    {
        esKRNL_SemDel(plistmenu->dist_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->move_sem)
    {
        esKRNL_SemDel(plistmenu->move_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->load_sem)
    {
        esKRNL_SemDel(plistmenu->load_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->msgbuff)
    {
        orange_mfree(plistmenu->msgbuff);
    }

    if (plistmenu)
    {
        orange_mfree(plistmenu);
    }

    return NULL;
}
//------------------------------------------------------------------------------------------------------------
//取得当前item

//==================================================================
//function name:    GUI_LstmGetCurItem
//author:
//date:             2008-10-13
//description:      listmenu getcuritem
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
H_ITEM GUI_LstmGetCurItem(H_WIN listmenu)
{
    __listmenu_data_t   *plistmenu;
    MENUITEM_t          *pmenuitem;

    if (listmenu == NULL)
    {
        return NULL;
    }

    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(listmenu);

    if (plistmenu->listlayer->menulist->ItemCount)
    {
        pmenuitem = plistmenu->listlayer->menulist->CurItem;
        return  GUI_ItemP2H(pmenuitem);
    }
    else
    {
        return NULL;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LstmGetChild
*
* author:
*
* date:             2009-2-2
*
* Description:      gui lstm get child
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_ITEM   GUI_LstmGetItem(H_WIN lstm, __u32 no)
{
    __u32               num;
    __listmenu_data_t     *plistmenu;
    MENUITEM_t         *tmpItem;

    if (lstm == NULL)
    {
        return NULL;
    }

    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (no > plistmenu->listlayer->menulist->ItemCount)
    {
        ORANGE_WRN("curitem no beyond the number of item!");
        return NULL;
    }
    else if (plistmenu->listlayer->menulist->ItemCount == 0)
    {
        return NULL;
    }
    else
    {
        tmpItem = plistmenu->listlayer->menulist->head;

        for (num = 0; num < no; num ++)
        {
            tmpItem = tmpItem->Next;
        }

        return GUI_ItemP2H(tmpItem);
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LstmUpdateItem
*
* author:
*
* date:             2009-12-2
*
* Description:      gui lstmupdateitem
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32  GUI_LstmUpdateItem(H_WIN lstm, __u32 no)
{
    __u32                       num;
    __listmenu_data_t          *plistmenu;
    MENUITEM_t                 *tmpItem;
    __draw_para_t               drawpara;

    if (lstm == NULL)
    {
        return 0;
    }

    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (no > plistmenu->listlayer->menulist->ItemCount)
    {
        ORANGE_WRN("curitem no beyond the number of item!");
        return ORANGE_FAIL;
    }
    else if (plistmenu->listlayer->menulist->ItemCount == 0)
    {
        return ORANGE_FAIL;
    }
    else
    {
        tmpItem = plistmenu->listlayer->menulist->head;

        for (num = 0; num < no; num ++)
        {
            tmpItem = tmpItem->Next;
        }

        /*set current draw mode*/
        drawpara.mode   =   GUI_PAIN_MODE_FASTDRAW;
        drawpara.item   =   GUI_ItemP2H(tmpItem);

        /*get the draw item flag*/
        if (plistmenu->lyrsts != LISTMENU_SLEEP)
        {
            /*call menitem callback function*/
            esKRNL_CallBack((__pCBK_t)*tmpItem->cb, &drawpara);

            if (tmpItem == plistmenu->listlayer->menulist->CurItem)
            {
                /*set current draw mode*/
                drawpara.mode   =   GUI_PAIN_MODE_FASTFOCUS;
                drawpara.item   =   GUI_ItemP2H(tmpItem);
                esKRNL_CallBack((__pCBK_t)*tmpItem->cb, &drawpara);
                /*set current draw mode*/
                drawpara.mode   =   GUI_PAIN_MODE_FOCUS;
                drawpara.item   =   GUI_ItemP2H(tmpItem);
                esKRNL_CallBack((__pCBK_t)*tmpItem->cb, &drawpara);
                tmpItem->status = FFOUCS_STS;
                plistmenu->listlayer->setfoucs = VAR_FOUCS;
            }
            else
            {
                tmpItem->status = FLOAD_STS;
            }

            ListLayer_MenuItemUpdateChild(tmpItem);
        }

        return ORANGE_OK;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LstmChildInFb
*
* author:
*
* date:             2009-2-2
*
* Description:      gui lstmchildinfb
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool   GUI_LstmChildInFb(H_WIN  hListmenu, H_ITEM handle)
{
    MENUITEM_t             *headItem;
    MENUITEM_t             *tailItem;
    MENUITEM_t             *tmpItem;
    __listmenu_data_t      *plistmenu;

    if (handle == NULL)
    {
        return EPDK_FALSE;
    }

    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(hListmenu);

    if (plistmenu->listlayer->mode == FIXED_MODE)
    {
        headItem    = plistmenu->listlayer->menulist->head;
        tailItem    = plistmenu->listlayer->menulist->tail;
    }
    else
    {
        headItem    = plistmenu->listlayer->menulist->load_head->Next;
        tailItem    = plistmenu->listlayer->menulist->load_tail->Prev;
    }

    for (tmpItem = headItem; tmpItem != tailItem; tmpItem = tmpItem->Next)
    {
        if (GUI_ItemP2H(tmpItem) == handle)
        {
            return EPDK_TRUE;
        }
    }

    if (GUI_ItemP2H(tmpItem) == handle)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmGetHeadItem
*
* author:
*
* date:             2009-10-10
*
* Description:      gui lstmgetheaditem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_ITEM   GUI_LstmGetSelItem(H_WIN hlistmenu, __s32 x, __s32 y)
{
    MENUITEM_t             *headItem;
    MENUITEM_t             *tailItem;
    MENUITEM_t             *tmpItem;
    __listmenu_data_t      *plistmenu;
    __gui_rect_t            rect;

    if (hlistmenu == NULL)
    {
        return EPDK_FALSE;
    }

    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(hlistmenu);

    if (plistmenu->listlayer->mode == FIXED_MODE)
    {
        headItem    = plistmenu->listlayer->menulist->head;
        tailItem    = plistmenu->listlayer->menulist->tail;
    }
    else
    {
        headItem    = plistmenu->listlayer->menulist->load_head->Next;
        tailItem    = plistmenu->listlayer->menulist->load_tail->Prev;
    }

    for (tmpItem = headItem; tmpItem != tailItem; tmpItem = tmpItem->Next)
    {
        rect.left       = tmpItem->rect.x;
        rect.top        = tmpItem->rect.y;
        rect.right      = tmpItem->rect.x + tmpItem->rect.width - 1;
        rect.bottom     = tmpItem->rect.y + tmpItem->rect.height - 1;

        if (PtInRect(&rect, x, y))
        {
            return GUI_ItemP2H(tmpItem);
        }
        else
        {
            if (plistmenu->listlayer->mode != FIXED_MODE)
            {
                if (tmpItem->rect.y >= plistmenu->listlayer->listbuffer->vrect.height)
                {
                    rect.left       = tmpItem->rect.x;
                    rect.top        = tmpItem->rect.y - plistmenu->listlayer->listbuffer->vrect.height;
                    rect.right      = tmpItem->rect.x + tmpItem->rect.width - 1;
                    rect.bottom     = rect.top + tmpItem->rect.height - 1;

                    if (PtInRect(&rect, x, y))
                    {
                        return GUI_ItemP2H(tmpItem);
                    }
                }
                else if (tmpItem->rect.y < plistmenu->listlayer->viewrect.height)
                {
                    rect.left       = tmpItem->rect.x;
                    rect.top        = tmpItem->rect.y + plistmenu->listlayer->listbuffer->vrect.height;
                    rect.right      = tmpItem->rect.x + tmpItem->rect.width - 1;
                    rect.bottom     = rect.top + tmpItem->rect.height - 1;

                    if (PtInRect(&rect, x, y))
                    {
                        return GUI_ItemP2H(tmpItem);
                    }
                }
            }
        }
    }

    rect.left       = tmpItem->rect.x;
    rect.top        = tmpItem->rect.y;
    rect.right      = tmpItem->rect.x + tmpItem->rect.width - 1;
    rect.bottom     = tmpItem->rect.y + tmpItem->rect.height - 1;

    if (PtInRect(&rect, x, y))
    {
        return GUI_ItemP2H(tmpItem);
    }
    else
    {
        if (plistmenu->listlayer->mode != FIXED_MODE)
        {
            if (tmpItem->rect.y >= plistmenu->listlayer->listbuffer->vrect.height)
            {
                rect.left       = tmpItem->rect.x;
                rect.top        = tmpItem->rect.y - plistmenu->listlayer->listbuffer->vrect.height;
                rect.right      = tmpItem->rect.x + tmpItem->rect.width - 1;
                rect.bottom     = rect.top + tmpItem->rect.height - 1;

                if (PtInRect(&rect, x, y))
                {
                    return GUI_ItemP2H(tmpItem);
                }
            }
            else if (tmpItem->rect.y < plistmenu->listlayer->viewrect.height)
            {
                rect.left       = tmpItem->rect.x;
                rect.top        = tmpItem->rect.y + plistmenu->listlayer->listbuffer->vrect.height;
                rect.right      = tmpItem->rect.x + tmpItem->rect.width - 1;
                rect.bottom     = rect.top + tmpItem->rect.height - 1;

                if (PtInRect(&rect, x, y))
                {
                    return GUI_ItemP2H(tmpItem);
                }
            }
        }
    }

    return NULL;
}

//==================================================================
//function name:    GUI_LstmSetCurItemNo
//author:
//date:             2008-11-8
//description:      listmenu setcuritemno
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32   GUI_LstmSetCurItemNo(__listmenu_data_t *plistmenu, __u32 no)
{
    __u32            num;

    if (no > plistmenu->listlayer->menulist->ItemCount)
    {
        ORANGE_WRN("curitem no beyond the number of item!");
        return __LINE__;
    }
    else if (plistmenu->listlayer->menulist->ItemCount == 0)
    {
        return ORANGE_OK;
    }
    else
    {
        plistmenu->listlayer->menulist->CurItem = plistmenu->listlayer->menulist->head;

        for (num = 0; num < no; num ++)
        {
            plistmenu->listlayer->menulist->CurItem = plistmenu->listlayer->menulist->CurItem->Next;
        }

        plistmenu->listlayer->menulist->setCurItem = 1;
    }

    return  ORANGE_OK;
}
//------------------------------------------------------------------------------------------------------------
//取得item的数目

//==================================================================
//function name:    GUI_LstmGetItemCount
//author:
//date:             2008-10-13
//description:      listmenu getitemcount
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32 GUI_LstmGetItemCount(H_WIN lstm)
{
    __listmenu_data_t          *plistmenu;

    if (lstm == NULL)
    {
        ORANGE_WRN("set current no failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (plistmenu == NULL)
    {
        return __LINE__;
    }

    return  MenuList_GetItemCount(plistmenu->listlayer->menulist);
}

/*
**********************************************************************************************************************
*                                               GUI_LstmGetItemRect
*
* author:
*
* date:             2009-10-13
*
* Description:      gui lstmgetitemrect
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_LstmGetItemRect(H_WIN H_Item, RECT *rect)
{
    MENUITEM_t  *pItem;

    if (!H_Item || !rect)
    {
        ORANGE_WRN("imput parameter error!");
        return EPDK_FAIL;
    }

    pItem = GUI_ItemH2P(H_Item);
    rect->x         = pItem->rect.x;
    rect->y         = pItem->rect.y;
    rect->width     = pItem->rect.width;
    rect->height    = pItem->rect.height;
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmGetItemLayer
*
* author:
*
* date:             2009-10-13
*
* Description:      gui lstmgetitemlayer
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_LYR  GUI_LstmGetItemLyrWin(H_WIN H_Item)
{
    MENUITEM_t  *pItem;

    if (!H_Item)
    {
        ORANGE_WRN("imput parameter error!");
        return NULL;
    }

    pItem = GUI_ItemH2P(H_Item);
    return GUI_WinGetLyrWin(pItem->handle);
}


/*
**********************************************************************************************************************
*                                               GUI_LstmGetHostWin
*
* author:
*
* date:             2009-11-23
*
* Description:      gui lstmgethostwin
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

H_WIN  GUI_LstmGetHostWin(H_WIN H_Item)
{
    MENUITEM_t  *pItem;

    if (!H_Item)
    {
        ORANGE_WRN("imput parameter error!");
        return NULL;
    }

    pItem = GUI_ItemH2P(H_Item);
    return pItem->handle;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmGetItemAttr
*
* author:
*
* date:             2009-10-13
*
* Description:      gui lstmgetitemattr
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

unsigned long  GUI_LstmGetItemAttr(H_WIN H_Item)
{
    MENUITEM_t  *pItem;

    if (!H_Item)
    {
        ORANGE_WRN("imput parameter error!");
        return 0;
    }

    pItem = GUI_ItemH2P(H_Item);
    return (unsigned long)pItem->attr;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmGetItemNo
*
* author:
*
* date:             2010-1-28
*
* Description:      gui lstmgetitemno
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_LstmGetItemNo(H_WIN H_Item)
{
    MENUITEM_t  *pItem;

    if (!H_Item)
    {
        ORANGE_WRN("imput parameter error!");
        return 0;
    }

    pItem = GUI_ItemH2P(H_Item);
    return (__u32)pItem->itemno;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmStopOnLoad
*
* author:
*
* date:             2009-11-17
*
* Description:      停止后台扩散加载
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void  GUI_LstmStopOnLoad(__listmenu_data_t *plistmenu)
{
    __u8        err;

    if (plistmenu->listlayer->mode != FIXED_MODE)
    {
        if (plistmenu->status & LISTMENU_ONLOAD)
        {
            plistmenu->status &= (~LISTMENU_ONLOAD);
            esKRNL_SemPend(plistmenu->load_sem, 0, &err); /*pend pos semphore*/
        }
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LstmPostRunMsg
*
* author:
*
* date:             2009-11-17
*
* Description:      gui lstmpostrunmsg
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void  GUI_LstmPostRunMsg(__listmenu_data_t *plistmenu, __u32 runid)
{
    GUI_LstmStopOnLoad(plistmenu);

    if (runid == LSTMM_RUNUP)
    {
        /*如果是向上消息，更新向上计数值*/
        plistmenu->listlayer->msgupcount ++;
    }
    else if (runid == LSTMM_RUNDOWN)
    {
        /*如果是向下消息，更新向下计数值*/
        plistmenu->listlayer->msgdowncount ++;
    }

    plistmenu->listlayer->msgcount ++;
    plistmenu->msgbuff[plistmenu->listlayer->msgcount % LISTMENU_FRM_RXQ_DEPTH].id        = runid;
    plistmenu->msgbuff[plistmenu->listlayer->msgcount % LISTMENU_FRM_RXQ_DEPTH].dwAddData = 0;
    esKRNL_QPost(plistmenu->listmenu_RxQ, (unsigned long)&plistmenu->msgbuff[plistmenu->listlayer->msgcount % LISTMENU_FRM_RXQ_DEPTH]);
}

/*
**********************************************************************************************************************
*                                               GUI_LstmPostMoveMsg
*
* author:
*
* date:             2009-12-20
*
* Description:      gui lstmpostmovemsg
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void  GUI_LstmPostMoveMsg(__listmenu_data_t *plistmenu, __u32 runid, __u32 dist)
{
    GUI_LstmStopOnLoad(plistmenu);

    if (runid == LSTMM_MOVEUP)
    {
        /*如果是向上消息，更新向上计数值*/
        plistmenu->listlayer->msgupcount ++;
    }
    else
    {
        /*如果是向下消息，更新向下计数值*/
        plistmenu->listlayer->msgdowncount ++;
    }

    plistmenu->listlayer->msgcount ++;
    plistmenu->msgbuff[plistmenu->listlayer->msgcount % LISTMENU_FRM_RXQ_DEPTH].id        = runid;
    plistmenu->msgbuff[plistmenu->listlayer->msgcount % LISTMENU_FRM_RXQ_DEPTH].dwAddData = dist;
    esKRNL_QPost(plistmenu->listmenu_RxQ, (unsigned long)&plistmenu->msgbuff[plistmenu->listlayer->msgcount % LISTMENU_FRM_RXQ_DEPTH]);
}

/*
**********************************************************************************************************************
*                                               GUI_LstmStopProc
*
* author:
*
* date:             2009-11-17
*
* Description:      gui lstmstopproc
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

void  GUI_LstmStopProc(__listmenu_data_t *plistmenu)
{
    __u8        err;

    if (plistmenu->index != plistmenu->len)
    {
        __s32   status;
        status  = ListLayer_GetMoveStatus(plistmenu->listlayer);

        /*等待回弹状态归位，之后再开始运行*/
        while (status == LISTLAYER_ROUNDDOWN || status == LISTLAYER_ROUNDUP)
        {
            status  = ListLayer_GetMoveStatus(plistmenu->listlayer);
            /*无停止消息过来，继续运行*/
            GUI_LstmPostRunMsg(plistmenu, LSTMM_MOVEMSG);
            esKRNL_TimeDly(3);
        };

        plistmenu->listlayer->msgcount          = 0;

        plistmenu->listlayer->msgflush          = 1;

        plistmenu->listlayer->msgdowncount      = 0;

        plistmenu->listlayer->msgupcount        = 0;

        err = esKRNL_QFlush(plistmenu->listmenu_RxQ);

        if (err == OS_NO_ERR)
        {
            plistmenu->listlayer->msgflush      = 0;
            plistmenu->listlayer->msgcount      = 0;
            plistmenu->listlayer->msgdowncount  = 0;
            plistmenu->listlayer->msgupcount    = 0;
        }

        esKRNL_SemPend(plistmenu->move_sem, 0, &err);
        plistmenu->index = plistmenu->len;
        esKRNL_SemPost(plistmenu->move_sem);
    }
    else
    {
        plistmenu->listlayer->msgcount          = 0;
        plistmenu->listlayer->msgflush          = 1;
        plistmenu->listlayer->msgdowncount      = 0;
        plistmenu->listlayer->msgupcount        = 0;
        err = esKRNL_QFlush(plistmenu->listmenu_RxQ);

        if (err == OS_NO_ERR)
        {
            plistmenu->listlayer->msgflush      = 0;
            plistmenu->listlayer->msgcount      = 0;
            plistmenu->listlayer->msgdowncount  = 0;
            plistmenu->listlayer->msgupcount    = 0;
        }
    }

    if (plistmenu->status & LISTMENU_ONRUN)
    {
        esKRNL_SemPend(plistmenu->run_sem, 0, &err); /*pend pos semphore*/
    }
}

//==================================================================
//function name:    GUI_LstmDestroy
//author:
//date:             2008-10-13
//description:      listmenu destroy
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_LstmDestroy(H_WIN hlistmenu)
{
    __u8                        err;
    __listmenu_data_t           *plistmenu;
    __u32                       ret;
    __krnl_q_data_t             qdata;

    if (hlistmenu == NULL)
    {
        return ORANGE_OK;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(hlistmenu);
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);

    if (plistmenu->listmenu_RxTID)
    {
        while (1)
        {
            if (esKRNL_TDelReq(plistmenu->listmenu_RxTID) == OS_TASK_NOT_EXIST)
            {
                break;
            }

            esKRNL_TimeDly(3);
            err = esKRNL_QQuery(plistmenu->listmenu_RxQ, &qdata);

            if (qdata.OSNMsgs == 0)
            {
                default_msg.id          = GUI_MSG_USRDEF;
                esKRNL_QPost(plistmenu->listmenu_RxQ, (unsigned long)&default_msg);
            }
        }
    }

    /*listlayer destory*/
    ret = ListLayer_Destroy(plistmenu->listlayer);

    if (ret != ORANGE_OK)
    {
        return __LINE__;
    }

    if (plistmenu->run_sem)
    {
        esKRNL_SemDel(plistmenu->run_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->dist_sem)
    {
        esKRNL_SemDel(plistmenu->dist_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->load_sem)
    {
        esKRNL_SemDel(plistmenu->load_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->move_sem)
    {
        esKRNL_SemDel(plistmenu->move_sem, OS_DEL_ALWAYS, &err);
    }

    if (plistmenu->msgbuff)
    {
        orange_mfree(plistmenu->msgbuff);
    }

    if (plistmenu->listmenu_RxQ)
    {
        /*delete listmenu receive message queue*/
        esKRNL_QDel(plistmenu->listmenu_RxQ, OS_DEL_ALWAYS, &err);
    }

    orange_mfree(plistmenu);
    plistmenu = NULL;
    return ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmLyrOn
//author:
//date:             2009-1-12
//description:      gui lstmlyron
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_LstmWakeup(__listmenu_data_t *plistmenu)
{
    __u32    ret;
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);

    if (plistmenu->lyrsts == LISTMENU_WAKEUP)
    {
        return ORANGE_OK;
    }

    if (plistmenu->lyrsts == LISTMENU_SLEEP)
    {
        plistmenu->lyrsts = LISTMENU_WAKEUP;
        /*listlayer on draw function*/
        ret = ListLayer_Wakeup(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            plistmenu->lyrsts = LISTMENU_SLEEP;
            return __LINE__;
        }
    }

    return ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmLyrStatus
//author:
//date:             2009-1-12
//description:      gui lstmlyrstatus
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_LstmSleep(__listmenu_data_t *plistmenu)
{
    plistmenu->lyrsts = LISTMENU_SLEEP;
    plistmenu->status &= (~LISTMENU_FOCUS);
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);
    return ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmKeyUp
//author:
//date:             2009-1-12
//description:      gui lstmkeyup
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32  GUI_LstmKeyUp(__listmenu_data_t *plistmenu, __gui_msg_t *message)
{
    __u32                      ret;

    if (plistmenu->lastmsg == message->dwAddData1 && plistmenu->lastkeyaction == KEY_UP_ACTION &&
        plistmenu->listlayer->menulist->CurItem == plistmenu->listlayer->menulist->head &&
        plistmenu->listlayer->menulist->ItemCount != 1 &&
        plistmenu->listlayer->mode != FIXED_MODE)
    {
        __u32                      status;
        /*等待后台线程加载完成*/
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);

        while (status != EPDK_TRUE)
        {
            esKRNL_TimeDly(5);
            status = ListLayer_GetLoadStatus(plistmenu->listlayer);
        }

        GUI_LstmStopProc(plistmenu);

        /*循环菜单处理，从头到尾*/
        if (plistmenu->listlayer->setfoucs == REAL_FOUCS)
        {
            /*取消焦点的一些特效*/
            plistmenu->listlayer->menulist->UnCurItem = plistmenu->listlayer->menulist->CurItem;
            plistmenu->listlayer->unfoucs_pos.x = plistmenu->listlayer->foucs_pos.x;
            plistmenu->listlayer->unfoucs_pos.y = plistmenu->listlayer->foucs_pos.y;
            ListLayer_UnFoucsMenuItem(plistmenu->listlayer, plistmenu->listlayer->menulist->UnCurItem,
                                      0, plistmenu->listlayer->unfoucs_pos.x, plistmenu->listlayer->unfoucs_pos.y);
            plistmenu->listlayer->setfoucs = 0;
        }

        plistmenu->listlayer->menulist->CurItem = plistmenu->listlayer->menulist->tail;
        plistmenu->listlayer->menulist->dist = plistmenu->listlayer->menulist->total_dist;
        plistmenu->listlayer->menulist->setCurItem  = plistmenu->listlayer->menulist->ItemCount;
        GUI_LstmStopOnLoad(plistmenu);
        /*绘制listmenu当前值*/
        ret = ListLayer_OnDraw(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            return __LINE__;
        }

        ListLayer_SetLayerFastFoucs(plistmenu->listlayer, plistmenu->listlayer->menulist->CurItem);

        if (!(plistmenu->listlayer->plistmenu->mode & KEYTONE_OFF))
        {
            _lstm_key_song();
        }

        plistmenu->listlayer->setfoucs = VAR_FOUCS;
    }
    else if (plistmenu->lastmsg == message->dwAddData1 && plistmenu->lastkeyaction == KEY_UP_ACTION &&
             plistmenu->listlayer->menulist->CurItem == plistmenu->listlayer->menulist->head &&
             plistmenu->listlayer->menulist->ItemCount != 1 &&
             plistmenu->listlayer->mode == FIXED_MODE)
    {
        /*循环菜单处理，从头到尾*/
        if (plistmenu->listlayer->setfoucs == REAL_FOUCS)
        {
            /*取消焦点的一些特效*/
            plistmenu->listlayer->menulist->UnCurItem = plistmenu->listlayer->menulist->CurItem;
            plistmenu->listlayer->unfoucs_pos.x = plistmenu->listlayer->foucs_pos.x;
            plistmenu->listlayer->unfoucs_pos.y = plistmenu->listlayer->foucs_pos.y;
            ListLayer_UnFoucsMenuItem(plistmenu->listlayer, plistmenu->listlayer->menulist->UnCurItem,
                                      0, plistmenu->listlayer->unfoucs_pos.x, plistmenu->listlayer->unfoucs_pos.y);
            plistmenu->listlayer->setfoucs = 0;
        }

        plistmenu->listlayer->menulist->CurItem = plistmenu->listlayer->menulist->tail;
        plistmenu->listlayer->menulist->setCurItem  = 1;
        GUI_LstmStopProc(plistmenu);
        /*绘制listmenu当前值*/
        ret = ListLayer_OnDraw(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            return __LINE__;
        }

        ListLayer_SetLayerFastFoucs(plistmenu->listlayer, plistmenu->listlayer->menulist->CurItem);

        if (!(plistmenu->listlayer->plistmenu->mode & KEYTONE_OFF))
        {
            _lstm_key_song();
        }

        plistmenu->listlayer->setfoucs = VAR_FOUCS;
    }
    else if (plistmenu->listmenu_RxQ)
    {
        /*如果是向上消息，更新向上计数值*/
        GUI_LstmPostRunMsg(plistmenu, LSTMM_RUNUP);
        return ORANGE_OK;
    }

    return ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmKeyDown
//author:
//date:             2009-1-12
//description:      gui lstmkeydown
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32  GUI_LstmKeyDown(__listmenu_data_t *plistmenu, __gui_msg_t *message)
{
    if (plistmenu->lastmsg == message->dwAddData1 && plistmenu->lastkeyaction == KEY_UP_ACTION &&
        plistmenu->listlayer->menulist->CurItem == plistmenu->listlayer->menulist->tail &&
        plistmenu->listlayer->menulist->ItemCount != 1 &&
        plistmenu->listlayer->mode != FIXED_MODE)
    {
        __u32                      status;
        __u32                      ret;
        /*等待后台线程加载完成*/
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);

        while (status != EPDK_TRUE)
        {
            esKRNL_TimeDly(5);
            status = ListLayer_GetLoadStatus(plistmenu->listlayer);
        }

        GUI_LstmStopProc(plistmenu);

        /*循环菜单处理，从头到尾*/
        if (plistmenu->listlayer->setfoucs == REAL_FOUCS)
        {
            /*取消焦点的一些特效*/
            plistmenu->listlayer->menulist->UnCurItem = plistmenu->listlayer->menulist->CurItem;
            plistmenu->listlayer->unfoucs_pos.x = plistmenu->listlayer->foucs_pos.x;
            plistmenu->listlayer->unfoucs_pos.y = plistmenu->listlayer->foucs_pos.y;
            ListLayer_UnFoucsMenuItem(plistmenu->listlayer, plistmenu->listlayer->menulist->UnCurItem,
                                      0, plistmenu->listlayer->unfoucs_pos.x, plistmenu->listlayer->unfoucs_pos.y);
            plistmenu->listlayer->setfoucs = 0;
        }

        plistmenu->listlayer->menulist->CurItem = plistmenu->listlayer->menulist->head;
        plistmenu->listlayer->menulist->dist = 0;
        plistmenu->listlayer->menulist->setCurItem  = 1;
        GUI_LstmStopOnLoad(plistmenu);
        /*绘制listmenu当前值*/
        ret = ListLayer_OnDraw(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            return __LINE__;
        }

        ListLayer_SetLayerFastFoucs(plistmenu->listlayer, plistmenu->listlayer->menulist->CurItem);

        if (!(plistmenu->listlayer->plistmenu->mode & KEYTONE_OFF))
        {
            _lstm_key_song();
        }

        plistmenu->listlayer->setfoucs = VAR_FOUCS;
    }
    else if (plistmenu->lastmsg == message->dwAddData1 && plistmenu->lastkeyaction == KEY_UP_ACTION &&
             plistmenu->listlayer->menulist->CurItem == plistmenu->listlayer->menulist->tail &&
             plistmenu->listlayer->menulist->ItemCount != 1 &&
             plistmenu->listlayer->mode == FIXED_MODE)
    {
        __u32                      ret;
        GUI_LstmStopProc(plistmenu);

        /*循环菜单处理，从头到尾*/
        if (plistmenu->listlayer->setfoucs == REAL_FOUCS)
        {
            /*取消焦点的一些特效*/
            plistmenu->listlayer->menulist->UnCurItem = plistmenu->listlayer->menulist->CurItem;
            plistmenu->listlayer->unfoucs_pos.x = plistmenu->listlayer->foucs_pos.x;
            plistmenu->listlayer->unfoucs_pos.y = plistmenu->listlayer->foucs_pos.y;
            ListLayer_UnFoucsMenuItem(plistmenu->listlayer, plistmenu->listlayer->menulist->UnCurItem,
                                      0, plistmenu->listlayer->unfoucs_pos.x, plistmenu->listlayer->unfoucs_pos.y);
            plistmenu->listlayer->setfoucs = 0;
        }

        plistmenu->listlayer->menulist->CurItem = plistmenu->listlayer->menulist->head;
        plistmenu->listlayer->menulist->setCurItem  = 1;
        /*绘制listmenu当前值*/
        ret = ListLayer_OnDraw(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            return __LINE__;
        }

        ListLayer_SetLayerFastFoucs(plistmenu->listlayer, plistmenu->listlayer->menulist->CurItem);

        if (!(plistmenu->listlayer->plistmenu->mode & KEYTONE_OFF))
        {
            _lstm_key_song();
        }

        plistmenu->listlayer->setfoucs = VAR_FOUCS;
    }
    else if (plistmenu->listmenu_RxQ)
    {
        /*如果是向下消息，更新向下计数值*/
        GUI_LstmPostRunMsg(plistmenu, LSTMM_RUNDOWN);
        return ORANGE_OK;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmKeyRise
*
* author:
*
* date:             2008-11-17
*
* Description:      gui lstmkeyrise
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_LstmKeyRise(__listmenu_data_t *plistmenu)
{
    __u8                    err;

    if (plistmenu->lastmsg != GUI_MSG_KEY_UP &&
        plistmenu->lastmsg != GUI_MSG_KEY_DOWN &&
        plistmenu->lastmsg != GUI_MSG_KEY_LONGUP &&
        plistmenu->lastmsg != GUI_MSG_KEY_LONGDOWN)
    {
        return __LINE__;
    }
    else
    {
        GUI_LstmStopProc(plistmenu);

        if (plistmenu->listlayer->mode == FIXED_MODE && plistmenu->listlayer->setfoucs != REAL_FOUCS)
        {
            ListLayer_SetLayerFoucs(plistmenu->listlayer, plistmenu->listlayer->menulist->CurItem);
            plistmenu->listlayer->setfoucs = REAL_FOUCS;
        }

        if (plistmenu->listlayer->mode != FIXED_MODE)
        {
            if (plistmenu->listlayer->menulist->ItemCount != 0)
            {
                plistmenu->listlayer->focus_type = KEY_FOCUS;
                esKRNL_FlagPost(plistmenu->listlayer->foucs_flags,
                                LISTLAYER_DRAW_ITEM_FLAG, OS_FLAG_SET, &err);
            }
        }

        return ORANGE_OK;
    }
}

/*
**********************************************************************************************************************
*                                               GUI_LstmKeyDefault
*
* author:
*
* date:             2008-11-17
*
* Description:      gui lstmkeydefault
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_LstmKeyDefault(__listmenu_data_t *plistmenu)
{
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmMoveUp
*
* author:
*
* date:             2009-12-21
*
* Description:      gui lstmmoveup
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmMoveUp(__listmenu_data_t *plistmenu, __u32 dist)
{
    /*如果是向上消息，更新向上计数值*/
    GUI_LstmPostMoveMsg(plistmenu, LSTMM_MOVEUP, dist);
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmMoveDown
*
* author:
*
* date:             2009-12-21
*
* Description:      gui lstmmovedown
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmMoveDown(__listmenu_data_t *plistmenu, __u32 dist)
{
    /*如果是向上消息，更新向上计数值*/
    GUI_LstmPostMoveMsg(plistmenu, LSTMM_MOVEDOWN, dist);
    return ORANGE_OK;
}


__u32 GUI_LstmMove(__listmenu_data_t *plistmenu)
{
    GUI_LstmPostMoveMsg(plistmenu, LSTMM_MOVEMSG, 0);
    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmMoveStop
*
* author:
*
* date:             2009-12-21
*
* Description:      gui lstmmovestop
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmMoveStop(__listmenu_data_t *plistmenu)
{
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);
    return ORANGE_OK;
}
/*
**********************************************************************************************************************
*                                               GUI_LstmTouchDown
*
* author:
*
* date:             2009-12-21
*
* Description:      gui lstmtouchdown
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmTouchDown(__listmenu_data_t *pData, __gui_msg_t *msg)
{
    __s32               x;
    __s32               y;
    H_ITEM              hitem;
    MENUITEM_t          *phitem;
    __pos_t             sc_pos;
    __pos_t             win_pos;
    __s32               dist;
    __u8                err;

    if (GUI_WinGetCaptureWin() != msg->h_deswin)
    {
        GUI_WinSetCaptureWin(msg->h_deswin);
    }

    GUI_LstmMoveStop(pData);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    win_pos.x   =   x;
    win_pos.y   =   y;
    GUI_WindowToScreen(msg->h_deswin, &win_pos, &sc_pos);
    hitem   = GUI_LstmGetSelItem(msg->h_deswin, x, y);
    phitem  = GUI_ItemH2P(hitem);

    if (pData->listlayer->menulist->CurItem != phitem)
    {
        if (phitem)
        {
            pData->listlayer->unfoucs_pos.x         = pData->listlayer->foucs_pos.x;
            pData->listlayer->unfoucs_pos.y         = pData->listlayer->foucs_pos.y;
            pData->listlayer->menulist->UnCurItem   = pData->listlayer->menulist->CurItem;
            pData->listlayer->menulist->CurItem     = phitem;
            pData->listlayer->foucs_pos.x           = pData->listlayer->menulist->CurItem->rect.x;

            if (pData->listlayer->mode != FIXED_MODE)
            {
                /*焦点的坐标在结束屏位置*/
                if (pData->listlayer->menulist->CurItem->rect.y >= pData->listlayer->listbuffer->vrect.height)
                {
                    /*显示区域在起始屏位置，此时显示起始屏区域，绘制却在结束屏区域*/
                    if (pData->listlayer->viewrect.y < pData->listlayer->viewrect.height)
                    {
                        pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y - pData->listlayer->listbuffer->vrect.height;
                    }
                    else
                    {
                        /*显示区域和绘制区域一致*/
                        pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
                    }
                }/*焦点的坐标在起始屏位置*/
                else if (pData->listlayer->menulist->CurItem->rect.y < pData->listlayer->viewrect.height)
                {
                    if (pData->listlayer->viewrect.y >= pData->listlayer->viewrect.height)
                    {
                        pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y + pData->listlayer->listbuffer->vrect.height;
                    }
                    else
                    {
                        /*显示区域和绘制区域一致*/
                        pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
                    }
                }
                else
                {
                    /*显示区域和绘制区域一致*/
                    pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
                }
            }
            else
            {
                /*显示区域和绘制区域一致*/
                pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
            }

            if (pData->listlayer->setfoucs)
            {
                ListLayer_SetLayerUnFoucs(pData->listlayer);
                pData->listlayer->setfoucs = 0;
            }

            /*查找menuitem里面的子控件*/
            if (phitem->nctrls)
            {
                __u32   count;

                for (count = 0; count < phitem->nctrls; count++)
                {
                    __gui_rect_t    win_rect;
                    RECT            gen_rect;
                    __pos_t         ctrl_pos;
                    GUI_WinGetWinRECT(phitem->child[count].hWnd, &gen_rect);
                    win_rect.left   = gen_rect.x;
                    win_rect.right  = gen_rect.x + gen_rect.width;
                    win_rect.top    = gen_rect.y;
                    win_rect.bottom = gen_rect.y + gen_rect.height;

                    /*找到子控件的情况*/
                    if (PtInRect(&win_rect, win_pos.x, win_pos.y))
                    {
                        __u32               dwAddData;
                        __gui_msg_t         new_msg;
                        /*转换坐标为控件内的坐标*/
                        ctrl_pos.x = win_pos.x - gen_rect.x;
                        ctrl_pos.y = win_pos.x - gen_rect.y;
                        dwAddData = MAKELONG(ctrl_pos.x, ctrl_pos.y);

                        if (GUI_WinGetStyle(phitem->child[count].hWnd) & WS_DISABLED)
                        {
                            return ORANGE_OK;
                        }

                        new_msg.id          = GUI_MSG_TOUCHACTIVE;
                        new_msg.h_deswin    = phitem->child[count].hWnd;
                        new_msg.h_srcwin    = 0;
                        new_msg.dwAddData1  = 0;
                        new_msg.dwAddData2  = 0;
                        new_msg.p_arg       = 0;
                        GUI_SendMessage(&new_msg);
                        new_msg.id           = GUI_MSG_TOUCH;
                        new_msg.h_deswin     = phitem->child[count].hWnd;
                        new_msg.h_srcwin     = msg->h_deswin;
                        new_msg.dwAddData1   = GUI_MSG_TOUCH_DOWN;
                        new_msg.dwAddData2   = (__u32)dwAddData;
                        new_msg.dwReserved   = msg->dwReserved;
                        new_msg.p_arg        = NULL;
                        /*直接发给子控件*/
                        GUI_SendMessage(&new_msg);
                        /*直接返回，此时并不需要再作为unpushed通知码传递给listmenu*/
                        return ORANGE_OK;
                    }
                }
            }
        }
    }
    else
    {
        if (phitem)
        {
            /*查找menuitem里面的子控件*/
            if (phitem->nctrls)
            {
                __u32   count;

                for (count = 0; count < phitem->nctrls; count++)
                {
                    __gui_rect_t    win_rect;
                    RECT            gen_rect;
                    __pos_t         ctrl_pos;
                    GUI_WinGetWinRECT(phitem->child[count].hWnd, &gen_rect);
                    win_rect.left   = gen_rect.x;
                    win_rect.right  = gen_rect.x + gen_rect.width;
                    win_rect.top    = gen_rect.y;
                    win_rect.bottom = gen_rect.y + gen_rect.height;

                    /*找到子控件的情况*/
                    if (PtInRect(&win_rect, win_pos.x, win_pos.y))
                    {
                        __u32               dwAddData;
                        __gui_msg_t         new_msg;
                        /*转换坐标为控件内的坐标*/
                        ctrl_pos.x = win_pos.x - gen_rect.x;
                        ctrl_pos.y = win_pos.x - gen_rect.y;
                        dwAddData = MAKELONG(ctrl_pos.x, ctrl_pos.y);

                        if (GUI_WinGetStyle(phitem->child[count].hWnd) & WS_DISABLED)
                        {
                            return ORANGE_OK;
                        }

                        new_msg.id          = GUI_MSG_TOUCHACTIVE;
                        new_msg.h_deswin    = phitem->child[count].hWnd;
                        new_msg.h_srcwin    = 0;
                        new_msg.dwAddData1  = 0;
                        new_msg.dwAddData2  = 0;
                        new_msg.p_arg       = 0;
                        GUI_SendMessage(&new_msg);
                        new_msg.id           = GUI_MSG_TOUCH;
                        new_msg.h_deswin     = phitem->child[count].hWnd;
                        new_msg.h_srcwin     = msg->h_deswin;
                        new_msg.dwAddData1   = GUI_MSG_TOUCH_DOWN;
                        new_msg.dwAddData2   = (__u32)dwAddData;
                        new_msg.dwReserved   = msg->dwReserved;
                        new_msg.p_arg        = NULL;
                        /*直接发给子控件*/
                        GUI_SendMessage(&new_msg);
                        /*直接返回，此时并不需要再作为unpushed通知码传递给listmenu*/
                        return ORANGE_OK;
                    }
                }
            }
        }
    }

    if (pData->gstate == LSTMGLIDE_IDLE)
    {
        if (pData->index != pData->len)
        {
            esKRNL_SemPend(pData->move_sem, 0, &err);
            pData->index = pData->len;
            esKRNL_SemPost(pData->move_sem);
            pData->gstate = LSTMGLIDE_STOP;
        }
        else
        {
            esKRNL_SemPend(pData->dist_sem, 0, &err);
            dist = ListBox_FlatMoveGetDist(pData->listlayer);
            esKRNL_SemPost(pData->dist_sem);
            pData->gcurv.fb_start = LISTMENU_OTHER_NUM * pData->gcurv.frame - dist;
            lstmglidecuv_init(&(pData->gcurv));
            lstmglidecuv_start(sc_pos.y);
            pData->gstate = LSTMGLIDE_START;
        }
    }

    pData->last_pos.x   = sc_pos.x;
    pData->last_pos.y   = sc_pos.y;
    pData->last_touch   = GUI_MSG_TOUCH_DOWN;

    if (phitem == NULL)
    {
        pData->listlayer->unfoucs_pos.x         = pData->listlayer->foucs_pos.x;
        pData->listlayer->unfoucs_pos.y         = pData->listlayer->foucs_pos.y;
        pData->listlayer->menulist->UnCurItem   = pData->listlayer->menulist->CurItem;
        pData->listlayer->menulist->CurItem     = NULL;

        if (pData->listlayer->menulist->UnCurItem)
        {
            ListLayer_SetLayerUnFoucs(pData->listlayer);
            pData->listlayer->setfoucs = 0;
        }
    }

    {
        __gui_notify_msg_t      notify_msg;
        notify_msg.hWnd         = msg->h_deswin;
        notify_msg.id           = GUI_WinGetItemId(msg->h_deswin);
        notify_msg.msgcode      = LSTMN_PUSHED;
        notify_msg.dwAddData    = msg->dwAddData1;
        notify_msg.dwReserved   = (unsigned long)(GUI_ItemP2H(pData->listlayer->menulist->CurItem));
        GUI_NotifyParent(&notify_msg);
    }

    return  ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmTouchUp
*
* author:
*
* date:             2009-12-21
*
* Description:      gui lstmtouchup
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmTouchUp(__listmenu_data_t *pData, __gui_msg_t *msg)
{
    __s32               x;
    __s32               y;
    __u16               speed;
    __pos_t             sc_pos;
    __pos_t             win_pos;
    __s32               dist;
    __u8                err;
    __bool              flags = ORANGE_FALSE;

    if (GUI_WinGetCaptureWin() != msg->h_deswin)
    {
        return ORANGE_OK;
    }

    GUI_WinReleaseCapture();
    GUI_LstmStopOnLoad(pData);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    win_pos.x   =   x;
    win_pos.y   =   y;

    if (pData->listlayer->setfoucs > 0)
    {
        pData->listlayer->unfoucs_pos.x         = pData->listlayer->foucs_pos.x;
        pData->listlayer->unfoucs_pos.y         = pData->listlayer->foucs_pos.y;
        pData->listlayer->menulist->UnCurItem   = pData->listlayer->menulist->CurItem;
        ListLayer_SetLayerUnFoucs(pData->listlayer);
        pData->listlayer->setfoucs = 0;
    }

    GUI_WindowToScreen(msg->h_deswin, &win_pos, &sc_pos);

    if (pData->gstate == LSTMGLIDE_START)
    {
        esKRNL_SemPend(pData->move_sem, 0, &err);
        pData->len      = lstmglidecuv_up(sc_pos.y, pData->last_pos.y, LOSWORD(msg->dwReserved), HISWORD(msg->dwReserved), pData->step, STEP_ARRAY);
        pData->index    = 0;
        esKRNL_SemPost(pData->move_sem);
        //GUI_LstmMove(pData);
        pData->gstate = LSTMGLIDE_IDLE;
    }
    else if (pData->gstate == LSTMGLIDE_MOVE)
    {
        pData->listlayer->menulist->CurItem     = NULL;
        esKRNL_SemPend(pData->move_sem, 0, &err);
        pData->len      = lstmglidecuv_up(sc_pos.y, pData->last_pos.y, LOSWORD(msg->dwReserved), HISWORD(msg->dwReserved), pData->step, STEP_ARRAY);
        pData->index    = 0;
        esKRNL_SemPost(pData->move_sem);
        GUI_LstmMove(pData);
        pData->gstate = LSTMGLIDE_IDLE;
    }
    else if (pData->gstate == LSTMGLIDE_STOP)
    {
        esKRNL_SemPend(pData->dist_sem, 0, &err);
        dist = ListBox_FlatMoveGetDist(pData->listlayer);
        esKRNL_SemPost(pData->dist_sem);
        pData->gcurv.fb_start = LISTMENU_OTHER_NUM * pData->gcurv.frame - dist;
        lstmglidecuv_init(&(pData->gcurv));
        lstmglidecuv_start(pData->last_pos.y);
        esKRNL_SemPend(pData->move_sem, 0, &err);
        pData->len      = lstmglidecuv_up(sc_pos.y, pData->last_pos.y, LOSWORD(msg->dwReserved), HISWORD(msg->dwReserved), pData->step, STEP_ARRAY);
        pData->index    = 0;
        esKRNL_SemPost(pData->move_sem);
        GUI_LstmMove(pData);
        pData->gstate = LSTMGLIDE_IDLE;
        flags = ORANGE_TRUE;
    }

    pData->last_pos.x   = sc_pos.x;
    pData->last_pos.y   = sc_pos.y;

    if (flags == ORANGE_FALSE)
    {
        if (pData->last_touch != GUI_MSG_TOUCH_MOVE && speed == 0)
        {
            __gui_notify_msg_t      notify_msg;
            notify_msg.hWnd     = msg->h_deswin;
            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
            notify_msg.msgcode  = LSTMN_UNPUSHED;
            notify_msg.dwAddData = pData->last_touch;
            GUI_NotifyParent(&notify_msg);
        }
        else
        {
            __gui_notify_msg_t      notify_msg;
            notify_msg.hWnd     = msg->h_deswin;
            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
            notify_msg.msgcode  = LSTMN_UNPUSHED;
            notify_msg.dwAddData = GUI_MSG_TOUCH_MOVE;
            GUI_NotifyParent(&notify_msg);
        }
    }

    pData->last_touch   = GUI_MSG_TOUCH_UP;
    return  ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmTouchMove
*
* author:
*
* date:             2009-12-21
*
* Description:      gui lstmtouchmove
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmTouchMove(__listmenu_data_t *pData, __gui_msg_t *msg)
{
    __s32               x;
    __s32               y;
    __pos_t             sc_pos;
    __pos_t             win_pos;
    __u8                err;
    __s32               dist;

    if (GUI_WinGetCaptureWin() != msg->h_deswin)
    {
        GUI_WinSetCaptureWin(msg->h_deswin);
    }

    GUI_LstmStopOnLoad(pData);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    win_pos.x   =   x;
    win_pos.y   =   y;

    if (pData->listlayer->setfoucs > 0)
    {
        pData->listlayer->unfoucs_pos.x         = pData->listlayer->foucs_pos.x;
        pData->listlayer->unfoucs_pos.y         = pData->listlayer->foucs_pos.y;
        pData->listlayer->menulist->UnCurItem   = pData->listlayer->menulist->CurItem;
        ListLayer_SetLayerUnFoucs(pData->listlayer);
        pData->listlayer->setfoucs = 0;
    }

    pData->listlayer->menulist->CurItem     = NULL;
    GUI_WindowToScreen(msg->h_deswin, &win_pos, &sc_pos);

    if (pData->gstate == LSTMGLIDE_START)
    {
        esKRNL_SemPend(pData->move_sem, 0, &err);
        pData->len      = lstmglidecuv_move(sc_pos.y, pData->last_pos.y, pData->step, STEP_ARRAY);
        pData->index    = 0;
        esKRNL_SemPost(pData->move_sem);
        GUI_LstmMove(pData);
        pData->gstate = LSTMGLIDE_MOVE;
    }
    else if (pData->gstate == LSTMGLIDE_MOVE)
    {
        esKRNL_SemPend(pData->move_sem, 0, &err);
        pData->len      = lstmglidecuv_move(sc_pos.y, pData->last_pos.y, pData->step, STEP_ARRAY);
        pData->index    = 0;
        esKRNL_SemPost(pData->move_sem);
        GUI_LstmMove(pData);
    }
    else if (pData->gstate == LSTMGLIDE_STOP)
    {
        esKRNL_SemPend(pData->dist_sem, 0, &err);
        dist = ListBox_FlatMoveGetDist(pData->listlayer);
        esKRNL_SemPost(pData->dist_sem);
        pData->gcurv.fb_start = LISTMENU_OTHER_NUM * pData->gcurv.frame - dist;
        lstmglidecuv_init(&(pData->gcurv));
        lstmglidecuv_start(pData->last_pos.y);
        esKRNL_SemPend(pData->move_sem, 0, &err);
        pData->len      = lstmglidecuv_move(sc_pos.y, pData->last_pos.y, pData->step, STEP_ARRAY);
        pData->index    = 0;
        esKRNL_SemPost(pData->move_sem);
        GUI_LstmMove(pData);
        pData->gstate = LSTMGLIDE_MOVE;
    }

    pData->last_pos.x   = sc_pos.x;
    pData->last_pos.y   = sc_pos.y;
    pData->last_touch   = GUI_MSG_TOUCH_MOVE;
    return  ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmOnDraw
//author:
//date:             2008-10-13
//description:      listmenu ondraw
//parameters:
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_LstmOnDraw(__listmenu_data_t *plistmenu)
{
    __u32            ret;

    if (!plistmenu)
    {
        return __LINE__;
    }

    /*get the handle of listlayer*/
    SYSTEM_PRINTF("start listlayer ondraw!");
    /*listlayer on draw function*/
    ret = ListLayer_OnDraw(plistmenu->listlayer);

    if (ret != ORANGE_OK)
    {
        return __LINE__;
    }

    return ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmSetFoucs
//author:
//date:             2009-1-11
//description:      gui lstmsetfoucs
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_LstmSetFoucs(__listmenu_data_t *plistmenu)
{
    __u32            ret;
    __u8             err;

    if (!plistmenu)
    {
        return __LINE__;
    }

    /*listlayer on draw function*/
    ret = ListLayer_SetWinFoucs(plistmenu->listlayer);

    if (ret != ORANGE_OK)
    {
        return __LINE__;
    }

    if (plistmenu->listlayer->mode != FIXED_MODE)
    {
        if (plistmenu->listlayer->menulist->ItemCount != 0)
        {
            plistmenu->listlayer->focus_type = KEY_FOCUS;
            esKRNL_FlagPost(plistmenu->listlayer->foucs_flags,
                            LISTLAYER_DRAW_ITEM_FLAG, OS_FLAG_SET, &err);
        }
    }

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmSetUnFoucs
*
* author:
*
* date:             2009-9-20
*
* Description:      gui lstmsetunfoucs
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_LstmSetUnFoucs(__listmenu_data_t *plistmenu)
{
    __u32            ret;

    if (!plistmenu)
    {
        return __LINE__;
    }

    /*listlayer on draw function*/
    ret = ListLayer_SetWinUnFoucs(plistmenu->listlayer);

    if (ret != ORANGE_OK)
    {
        return __LINE__;
    }

    return ORANGE_OK;
}

//------------------------------------------------------------------------------------------------------------
// 响应消息

//==================================================================
//function name:    GUI_LstmOnMessage
//author:
//date:             2008-10-13
//description:      listmenu on message funtion
//parameters:       listmenu  the handle of listmenu
//                  pMsg      the message address of listmenu need to receive
//return:           if success return
//                  if fail return the number of fail
//modify history:
//==================================================================
//------------------------------------------------------------------------------------------------------------
__u32  GUI_LstmOnMessage(__listmenu_data_t *plistmenu, __listmenu_runmsg_t *pMsg)
{
    __u8                    err;
    OS_SEM_DATA             sem_data;

    if (!plistmenu)
    {
        return __LINE__;
    }

    if (pMsg->id == GUI_MSG_USRDEF)
    {
        return ORANGE_OK;
    }

    err = esKRNL_SemQuery(plistmenu->run_sem, &sem_data);

    if (sem_data.OSCnt == 0)
    {
        esKRNL_SemPost(plistmenu->run_sem); /*post run semphore*/
    }

    esKRNL_SemPend(plistmenu->run_sem, 0, &err); /*pend pos semphore*/
    plistmenu->status |= LISTMENU_ONRUN;
    /*listmenu on message function  */
    ListLayer_OnMesssage(plistmenu->listlayer, pMsg);     /*call listlayer message function*/
    plistmenu->status &= (~LISTMENU_ONRUN);
    esKRNL_SemPost(plistmenu->run_sem); /*post run semphore*/
    return ORANGE_OK;
}

//==================================================================
//function name:    GUI_LstmSetCurNo
//author:
//date:             2008-12-30
//description:      gui lstmsetcurno
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32   GUI_LstmDrawCurNo(H_WIN lstm, __u32 curno)
{
    __u32                    ret;
    __listmenu_data_t          *plistmenu;
    __bool                   status;

    if (lstm == NULL)
    {
        ORANGE_WRN("set current no failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);
    GUI_LstmStopOnLoad(plistmenu);
    /*等待后台线程加载完成*/
    status = ListLayer_GetLoadStatus(plistmenu->listlayer);

    while (status != EPDK_TRUE)
    {
        esKRNL_TimeDly(5);
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);
    }

    /*设置listmenu当前值*/
    ret = GUI_LstmSetCurItemNo(plistmenu, curno);

    if (ret != ORANGE_OK)
    {
        ORANGE_WRN("lstm set current no failed");
        return __LINE__;
    }

    /*绘制listmenu当前值*/
    ret = ListLayer_OnDraw(plistmenu->listlayer);

    if (ret != ORANGE_OK)
    {
        return __LINE__;
    }

    /*绘制完成之后设置焦点状态*/
    ret = GUI_LstmSetFoucs(plistmenu);

    if (ret != ORANGE_OK)
    {
        ORANGE_WRN("listmenu set focus failed!");
        return __LINE__;
    }

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmSetCurNo
*
* author:
*
* date:             2009-2-27
*
* Description:      gui lstmsetcurno
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32   GUI_LstmSetCurNo(H_WIN lstm, __u32 curno)
{
    __u32                    ret;
    __listmenu_data_t          *plistmenu;
    __bool                   status;

    if (lstm == NULL)
    {
        ORANGE_WRN("set current no failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (plistmenu->listlayer->menulist->CurItem->itemno != curno)
    {
        /*等待后台线程加载完成*/
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);

        while (status != EPDK_TRUE)
        {
            esKRNL_TimeDly(5);
            status = ListLayer_GetLoadStatus(plistmenu->listlayer);
        }

        /*设置listmenu当前值*/
        ret = GUI_LstmSetCurItemNo(plistmenu, curno);

        if (ret != ORANGE_OK)
        {
            ORANGE_WRN("lstm set current no failed");
            return __LINE__;
        }

        if (plistmenu->lyrsts != LISTMENU_SLEEP)
        {
            /*绘制listmenu当前值*/
            ret = ListLayer_OnDraw(plistmenu->listlayer);

            if (ret != ORANGE_OK)
            {
                return __LINE__;
            }
        }
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_lstmDelCurItem
*
* author:
*
* date:             2009-9-21
*
* Description:      gui lstmdelcuritem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32   GUI_LstmDelCurItem(H_WIN lstm)
{
    __u32                    ret;
    __listmenu_data_t          *plistmenu;
    __bool                   status;

    if (lstm == NULL)
    {
        ORANGE_WRN("lstm is null failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);
    /*等待后台线程加载完成*/
    status = ListLayer_GetLoadStatus(plistmenu->listlayer);

    while (status != EPDK_TRUE)
    {
        esKRNL_TimeDly(5);
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);
    }

    if (plistmenu->listlayer->menulist->CurItem == NULL)
    {
        GUI_LstmCurProc(plistmenu);
    }

    if (plistmenu->mode & LISTBOX_MOD)
    {
        /*删除listmenu当前值*/
        ret = ListLayer_BoxDelCurItem(plistmenu->listlayer);
    }
    else
    {
        /*删除listmenu当前值*/
        ret = ListLayer_DelCurItem(plistmenu->listlayer);
    }

    if (ret != ORANGE_OK)
    {
        ORANGE_WRN("lstm delete current item failed");
        return __LINE__;
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_lstmDelCurItem
*
* author:
*
* date:             2009-9-21
*
* Description:      gui lstmdelcuritem
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32  GUI_LstmAddItemTail(H_WIN lstm, __lstm_item_attr_t *NewItem)
{
    __u32                    ret;
    __listmenu_data_t        *plistmenu;
    __bool                   status;

    if (lstm == NULL)
    {
        ORANGE_WRN("lstm is null failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);
    GUI_LstmStopProc(plistmenu);
    GUI_LstmStopOnLoad(plistmenu);
    /*等待后台线程加载完成*/
    status = ListLayer_GetLoadStatus(plistmenu->listlayer);

    while (status != EPDK_TRUE)
    {
        esKRNL_TimeDly(5);
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);
    }

    if (plistmenu->listlayer->menulist->CurItem == NULL)
    {
        GUI_LstmCurProc(plistmenu);
    }

    if (plistmenu->mode & LISTBOX_MOD)
    {
        /*删除listmenu当前值*/
        ret = ListLayer_BoxAddItemTail(plistmenu->listlayer, NewItem);
    }
    else
    {
        /*删除listmenu当前值*/
        ret = ListLayer_AddItemTail(plistmenu->listlayer, NewItem);
    }

    if (ret != ORANGE_OK)
    {
        ORANGE_WRN("lstm add current item failed");
        return __LINE__;
    }

    return ORANGE_OK;
}


/*
**********************************************************************************************************************
*                                               GUI_LstmSetCurNo
*
* author:
*
* date:             2009-2-27
*
* Description:      GUI_LstmNextPage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32   GUI_LstmNextPage(H_WIN lstm)
{
    __u32                    ret;
    __listmenu_data_t        *plistmenu;
    __bool                   status;

    if (lstm == NULL)
    {
        ORANGE_WRN("set current no failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (!(plistmenu->mode & LISTBOX_MOD))
    {
        return __LINE__;
    }

    GUI_LstmStopOnLoad(plistmenu);
    status = ListLayer_GetLoadStatus(plistmenu->listlayer);

    while (status != EPDK_TRUE)
    {
        esKRNL_TimeDly(5);
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);
    }

    if (plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*绘制listmenu当前值*/
        ret = ListLayer_VarNextPage(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            ORANGE_WRN("lstm set current no failed");
            return __LINE__;
        }

        ret = GUI_LstmSetFoucs(plistmenu);

        if (ret != ORANGE_OK)
        {
            __inf("set foucs error!");
            return  __LINE__;
        }
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmSetCurNo
*
* author:
*
* date:             2009-2-27
*
* Description:      GUI_LstmPrevPage
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32   GUI_LstmPrevPage(H_WIN lstm)
{
    __u32                    ret;
    __listmenu_data_t        *plistmenu;
    __bool                   status;

    if (lstm == NULL)
    {
        ORANGE_WRN("set current no failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);

    if (!(plistmenu->mode & LISTBOX_MOD))
    {
        return __LINE__;
    }

    GUI_LstmStopOnLoad(plistmenu);
    status = ListLayer_GetLoadStatus(plistmenu->listlayer);

    while (status != EPDK_TRUE)
    {
        esKRNL_TimeDly(5);
        status = ListLayer_GetLoadStatus(plistmenu->listlayer);
    }

    if (plistmenu->lyrsts != LISTMENU_SLEEP)
    {
        /*绘制listmenu当前值*/
        ret = ListLayer_VarPrevPage(plistmenu->listlayer);

        if (ret != ORANGE_OK)
        {
            ORANGE_WRN("lstm set current no failed");
            return __LINE__;
        }

        ret = GUI_LstmSetFoucs(plistmenu);

        if (ret != ORANGE_OK)
        {
            __inf("set foucs error!");
            return  __LINE__;
        }
    }

    return ORANGE_OK;
}

/*
**********************************************************************************************************************
*                                               GUI_LstmStopSlip
*
* author:
*
* date:             2009-2-13
*
* Description:      gui lstmstopslip
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32   GUI_LstmStopSlip(H_WIN lstm)
{
    __listmenu_data_t          *plistmenu;

    if (lstm == NULL)
    {
        ORANGE_WRN("set current no failed");
        return __LINE__;
    }

    /*获取listmenu变量*/
    plistmenu = (__listmenu_data_t *)GUI_CtrlWinGetAddData(lstm);
    GUI_LstmStopProc(plistmenu);
    return ORANGE_OK;
}


void GUI_LstmCurProc(__listmenu_data_t *pData)
{
    if (pData->listlayer->menulist->CurItem == NULL)
    {
        pData->listlayer->menulist->CurItem = pData->listlayer->menulist->SelItem;
        pData->listlayer->foucs_pos.x           = pData->listlayer->menulist->CurItem->rect.x;

        if (pData->listlayer->mode != FIXED_MODE)
        {
            /*焦点的坐标在结束屏位置*/
            if (pData->listlayer->menulist->CurItem->rect.y >= pData->listlayer->listbuffer->vrect.height)
            {
                /*显示区域在起始屏位置，此时显示起始屏区域，绘制却在结束屏区域*/
                if (pData->listlayer->viewrect.y < pData->listlayer->viewrect.height)
                {
                    pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y - pData->listlayer->listbuffer->vrect.height;
                }
                else
                {
                    /*显示区域和绘制区域一致*/
                    pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
                }
            }/*焦点的坐标在起始屏位置*/
            else if (pData->listlayer->menulist->CurItem->rect.y < pData->listlayer->viewrect.height)
            {
                if (pData->listlayer->viewrect.y >= pData->listlayer->viewrect.height)
                {
                    pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y + pData->listlayer->listbuffer->vrect.height;
                }
                else
                {
                    /*显示区域和绘制区域一致*/
                    pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
                }
            }
            else
            {
                /*显示区域和绘制区域一致*/
                pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
            }
        }
        else
        {
            /*显示区域和绘制区域一致*/
            pData->listlayer->foucs_pos.y           = pData->listlayer->menulist->CurItem->rect.y;
        }
    }
}

/*
**********************************************************************************************************************
*                                               ListMenuCtrlProc
*
* author:
*
* date:             2009-9-14
*
* Description:      listmenuctrlproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 ListMenuCtrlProc(__gui_msg_t *msg)
{
    __listmenu_data_t        *pData;
    __s32                    ret;

    switch (msg->id)
    {
        case  GUI_MSG_CREATE:
            pData = (__listmenu_data_t *) GUI_LstmPrivCreate(msg->h_deswin);

            if (pData == NULL)
            {
                ORANGE_WRN("create listmenu private failed");
                return ORANGE_FAIL;
            }

            GUI_CtrlWinSetAddData(msg->h_deswin, (unsigned long)pData);
            return ORANGE_OK;

        case  GUI_MSG_DESTROY:
            GUI_LstmDestroy(msg->h_deswin);
            return ORANGE_OK;

        case  GUI_MSG_SET_FOCUS:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (!(pData->status & LISTMENU_FOCUS))
            {
                GUI_LstmStopOnLoad(pData);
                /*处理设置焦点消息*/
                ret = GUI_LstmSetFoucs(pData);

                if (ret != ORANGE_OK)
                {
                    __inf("set foucs error!");
                    return  ORANGE_FAIL;
                }

                pData->status |= LISTMENU_FOCUS;
            }
        }

        return ORANGE_OK;

        case  GUI_MSG_SET_UNFOCUS:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->status & LISTMENU_FOCUS)
            {
                GUI_LstmStopOnLoad(pData);
                /*处理设置焦点消息*/
                ret = GUI_LstmSetUnFoucs(pData);

                if (ret != ORANGE_OK)
                {
                    __inf("set unfoucs error!");
                    return  ORANGE_FAIL;
                }

                pData->status &= (~LISTMENU_FOCUS);
            }
        }

        return ORANGE_OK;

        case  GUI_MSG_PAINT:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->lyrsts == LISTMENU_SLEEP)
            {
                ORANGE_WRN("listmenu sleep!");
                return ORANGE_FAIL;
            }

            GUI_LstmStopOnLoad(pData);
            /*处理绘制消息*/
            ret = GUI_LstmOnDraw(pData);

            if (ret != ORANGE_OK)
            {
                __inf("on pain error!");
                return  ORANGE_FAIL;
            }
        }

        return ORANGE_OK;

        case GUI_MSG_KEY:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->listlayer->menulist->CurItem == NULL)
            {
                GUI_LstmCurProc(pData);
            }

            /*获取键盘ID值*/
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_UP    :
                case GUI_MSG_KEY_LONGUP:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->listlayer->menulist->ItemCount)
                        {
                            __gui_notify_msg_t      notify_msg;
                            GUI_LstmKeyRise(pData);

                            if (GUI_WinGetExStyle(msg->h_deswin) & WS_EX_CHILDPROC)
                            {
                                /*子窗口优先处理RISE消息*/
                                if (pData->listlayer->menulist->CurItem->child[0].hWnd)
                                {
                                    __gui_msg_t   new_msg;
                                    new_msg.id              = msg->id;
                                    new_msg.dwAddData1      = msg->dwAddData1;
                                    new_msg.h_deswin        = pData->listlayer->menulist->CurItem->child[0].hWnd;
                                    new_msg.h_srcwin        = msg->h_deswin;
                                    new_msg.dwAddData2      = msg->dwAddData2;
                                    new_msg.p_arg           = NULL;
                                    GUI_SendMessage(&new_msg);
                                }
                                else
                                {
                                    notify_msg.hWnd     = msg->h_deswin;
                                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                    notify_msg.msgcode  = LSTMN_UNPUSHED;
                                    notify_msg.dwAddData = pData->lastmsg;
                                    GUI_NotifyParent(&notify_msg);
                                }
                            }
                            else
                            {
                                /*父窗口优先处理RISE消息*/
                                notify_msg.hWnd     = msg->h_deswin;
                                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                notify_msg.msgcode  = LSTMN_UNPUSHED;
                                notify_msg.dwAddData = pData->lastmsg;
                                GUI_NotifyParent(&notify_msg);
                            }
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->listlayer->menulist->ItemCount)
                        {
                            GUI_LstmKeyUp(pData, msg);
                        }
                    }

                    pData->lastmsg          = msg->dwAddData1;
                    pData->lastkeyaction    = msg->dwAddData2;
                    return  ORANGE_OK;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_LONGDOWN:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->listlayer->menulist->ItemCount)
                        {
                            __gui_notify_msg_t      notify_msg;
                            GUI_LstmKeyRise(pData);

                            if (GUI_WinGetExStyle(msg->h_deswin) & WS_EX_CHILDPROC)
                            {
                                /*子窗口优先处理RISE消息*/
                                if (pData->listlayer->menulist->CurItem->child[0].hWnd)
                                {
                                    __gui_msg_t   new_msg;
                                    new_msg.id              = msg->id;
                                    new_msg.dwAddData1      = msg->dwAddData1;
                                    new_msg.h_deswin        = pData->listlayer->menulist->CurItem->child[0].hWnd;
                                    new_msg.h_srcwin        = msg->h_deswin;
                                    new_msg.dwAddData2      = msg->dwAddData2;
                                    new_msg.p_arg           = NULL;
                                    GUI_SendMessage(&new_msg);
                                }
                                else
                                {
                                    notify_msg.hWnd     = msg->h_deswin;
                                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                    notify_msg.msgcode  = LSTMN_UNPUSHED;
                                    notify_msg.dwAddData = pData->lastmsg;
                                    GUI_NotifyParent(&notify_msg);
                                }
                            }
                            else
                            {
                                /*父窗口优先处理RISE消息*/
                                notify_msg.hWnd     = msg->h_deswin;
                                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                notify_msg.msgcode  = LSTMN_UNPUSHED;
                                notify_msg.dwAddData = pData->lastmsg;
                                GUI_NotifyParent(&notify_msg);
                            }
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->listlayer->menulist->ItemCount)
                        {
                            /*如果是上下键值则加入消息队列处理*/
                            GUI_LstmKeyDown(pData, msg);
                        }
                    }

                    pData->lastmsg          = msg->dwAddData1;
                    pData->lastkeyaction    = msg->dwAddData2;
                    return ORANGE_OK;
#if 0

                case GUI_MSG_KEY_RISE:
                    if (pData->listlayer->menulist->ItemCount)
                    {
                        __gui_notify_msg_t      notify_msg;
                        GUI_LstmKeyRise(pData);

                        if (GUI_WinGetExStyle(msg->h_deswin) & WS_EX_CHILDPROC)
                        {
                            /*子窗口优先处理RISE消息*/
                            if (pData->listlayer->menulist->CurItem->child[0].hWnd)
                            {
                                __gui_msg_t   new_msg;
                                new_msg.id              = msg->id;
                                new_msg.dwAddData1      = msg->dwAddData1;
                                new_msg.h_deswin        = pData->listlayer->menulist->CurItem->child[0].hWnd;
                                new_msg.h_srcwin        = msg->h_deswin;
                                new_msg.dwAddData2      = msg->dwAddData2;
                                new_msg.p_arg           = NULL;
                                GUI_SendMessage(&new_msg);
                            }
                            else
                            {
                                notify_msg.hWnd     = msg->h_deswin;
                                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                notify_msg.msgcode  = LSTMN_UNPUSHED;
                                notify_msg.dwAddData = pData->lastmsg;
                                GUI_NotifyParent(&notify_msg);
                            }
                        }
                        else
                        {
                            /*父窗口优先处理RISE消息*/
                            notify_msg.hWnd     = msg->h_deswin;
                            notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                            notify_msg.msgcode  = LSTMN_UNPUSHED;
                            notify_msg.dwAddData = pData->lastmsg;
                            GUI_NotifyParent(&notify_msg);
                        }
                    }

                    pData->lastmsg  = GUI_MSG_KEY_RISE;
                    return ORANGE_OK;
#endif

                case GUI_MSG_KEY_ENTER:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_RIGHT:
                default:
                    if (msg->dwAddData2 == KEY_UP_ACTION)
                    {
                        if (pData->listlayer->menulist->ItemCount)
                        {
                            __gui_notify_msg_t      notify_msg;
                            GUI_LstmKeyRise(pData);

                            if (GUI_WinGetExStyle(msg->h_deswin) & WS_EX_CHILDPROC)
                            {
                                /*子窗口优先处理RISE消息*/
                                if (pData->listlayer->menulist->CurItem->child[0].hWnd)
                                {
                                    __gui_msg_t   new_msg;
                                    new_msg.id              = msg->id;
                                    new_msg.dwAddData1      = msg->dwAddData1;
                                    new_msg.h_deswin        = pData->listlayer->menulist->CurItem->child[0].hWnd;
                                    new_msg.h_srcwin        = msg->h_deswin;
                                    new_msg.dwAddData2      = msg->dwAddData2;
                                    new_msg.p_arg           = NULL;
                                    GUI_SendMessage(&new_msg);
                                }
                                else
                                {
                                    notify_msg.hWnd     = msg->h_deswin;
                                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                    notify_msg.msgcode  = LSTMN_UNPUSHED;
                                    notify_msg.dwAddData = pData->lastmsg;
                                    GUI_NotifyParent(&notify_msg);
                                }
                            }
                            else
                            {
                                /*父窗口优先处理RISE消息*/
                                notify_msg.hWnd     = msg->h_deswin;
                                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                notify_msg.msgcode  = LSTMN_UNPUSHED;
                                notify_msg.dwAddData = pData->lastmsg;
                                GUI_NotifyParent(&notify_msg);
                            }
                        }
                    }

                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (pData->listlayer->menulist->ItemCount)
                        {
                            __gui_notify_msg_t      notify_msg;
                            GUI_LstmKeyDefault(pData);

                            if (GUI_WinGetExStyle(msg->h_deswin) & WS_EX_CHILDPROC)
                            {
                                /*子窗口优先处理ENTER消息*/
                                if (pData->listlayer->menulist->CurItem->child[0].hWnd)
                                {
                                    __gui_msg_t   new_msg;
                                    new_msg.id              = msg->id;
                                    new_msg.dwAddData1      = msg->dwAddData1;
                                    new_msg.h_deswin        = pData->listlayer->menulist->CurItem->child[0].hWnd;
                                    new_msg.h_srcwin        = msg->h_deswin;
                                    new_msg.dwAddData2      = msg->dwAddData2;
                                    new_msg.p_arg           = NULL;
                                    GUI_SendMessage(&new_msg);
                                }
                                else
                                {
                                    notify_msg.hWnd     = msg->h_deswin;
                                    notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                    notify_msg.msgcode  = LSTMN_PUSHED;
                                    notify_msg.dwAddData = msg->dwAddData1;
                                    GUI_NotifyParent(&notify_msg);
                                }
                            }
                            else
                            {
                                /*父窗口优先处理ENTER消息*/
                                notify_msg.hWnd     = msg->h_deswin;
                                notify_msg.id       = GUI_WinGetItemId(msg->h_deswin);
                                notify_msg.msgcode  = LSTMN_PUSHED;
                                notify_msg.dwAddData = msg->dwAddData1;
                                GUI_NotifyParent(&notify_msg);
                            }
                        }
                    }

                    pData->lastmsg          = msg->dwAddData1;
                    pData->lastkeyaction    = msg->dwAddData2;
                    return ORANGE_OK;
            }
        }

        case GUI_MSG_TOUCH:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (!pData->listlayer->menulist->ItemCount)
            {
                break;
            }

            /*获取键盘ID值*/
            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_UP    :
                {
                    GUI_LstmTouchUp(pData, msg);
                }
                break;

                case GUI_MSG_TOUCH_DOWN:
                {
                    /*移动之后的按键不处理*/
                    if (pData->last_touch != GUI_MSG_TOUCH_MOVE)
                    {
                        GUI_LstmTouchDown(pData, msg);
                    }
                }
                break;

                case GUI_MSG_TOUCH_MOVE:
                {
                    GUI_LstmTouchMove(pData, msg);
                }
                break;

                case GUI_MSG_TOUCH_LONGDOWN:

                    /*移动之后的长按键不处理*/
                    if (pData->last_touch != GUI_MSG_TOUCH_MOVE)
                    {
                        if (pData->listlayer->setfoucs == 0)
                        {
                            ListLayer_SetLayerFastFoucs(pData->listlayer, pData->listlayer->menulist->CurItem);
                            ListLayer_SetLayerFoucs(pData->listlayer, pData->listlayer->menulist->CurItem);
                            pData->listlayer->setfoucs = REAL_FOUCS;
                        }
                    }
                break;
                default:
                    if (pData->last_touch    != GUI_MSG_TOUCH_MOVE)
                    {
                        __gui_notify_msg_t      notify_msg;
                        notify_msg.hWnd         = msg->h_deswin;
                        notify_msg.id           = GUI_WinGetItemId(msg->h_deswin);
                        notify_msg.msgcode      = LSTMN_PUSHED;
                        notify_msg.dwAddData    = msg->dwAddData1;
                        notify_msg.dwReserved   = (unsigned long)(GUI_ItemP2H(pData->listlayer->menulist->CurItem));
                        GUI_NotifyParent(&notify_msg);
                        pData->last_touch   = msg->dwAddData1;
                    }

                    break;
            }
        }

        return ORANGE_OK;

        case GUI_MSG_WIN_WAKEUP:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            ORANGE_WRN("listmenu layer on!");
            ret = GUI_LstmWakeup(pData);

            if (ret != ORANGE_OK)
            {
                ORANGE_WRN("listmenu layer on failed!");
                return ORANGE_FAIL;
            }
        }

        return ORANGE_OK;

        case GUI_MSG_WIN_SLEEP:
        {
            __listmenu_data_t   *pData;
            pData = (__listmenu_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);
            ORANGE_WRN("listmenu layer on!");
            ret = GUI_LstmSleep(pData);

            if (ret != ORANGE_OK)
            {
                ORANGE_WRN("listmenu layer on failed!");
                return ORANGE_FAIL;
            }
        }

        return ORANGE_OK;

        /*menuitem上的子控件通知消息的处理函数*/
        case GUI_MSG_COMMAND:
        {
            __gui_msg_t     notifymsg;
            /*直接将子控件的通知码往listmenu控件的父窗口传递
            传递的通知码和id保持不变，这需要应用程序保证在listmenu中所有的控件的id，唯一*/
            notifymsg.h_srcwin  = msg->h_deswin;
            notifymsg.h_deswin  = GUI_WinGetParent(msg->h_deswin);
            notifymsg.id        = GUI_MSG_COMMAND;
            notifymsg.dwAddData1 = msg->dwAddData1;
            notifymsg.dwAddData2 = msg->dwAddData2;
            notifymsg.dwReserved = msg->dwReserved;
            notifymsg.p_arg     = NULL;
            //GUI_SendNotifyMessage (&notifymsg);
            GUI_SendMessage(&notifymsg);
        }

        return ORANGE_OK;
    }

    return  ORANGE_OK;
}

#endif //__LIST__MENU__H____




