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
#ifndef __TXT_BOX_C__
#define __TXT_BOX_C__

#include "txtbox_i.h"
#include "txtlayer_process.h"

static __s32 TxtBoxCtrlProc(__gui_msg_t *msg);
/*
**********************************************************************************************************************
*                                               RegisterButtonControl
*
* author:
*
* date:             2009-8-4
*
* Description:      registertxtboxcontrol
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__bool RegisterTxtboxControl(void)
{
    __gui_winclass_t  WndClass;
    WndClass.spClassName = CTRL_TXTBOX;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.WinProc     = TxtBoxCtrlProc;
    return GUI_AddNewControlClass(&WndClass) == ORANGE_OK;
}

//==================================================================
//function name:    GUI_TxtboxCreate
//author:
//date:             2008-12-18
//description:      gui txtboxcreate
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__txtbox_data_t *GUI_TxtboxPrivCreate(H_WIN htxtbox)
{
    __u32                                   num;
    txtitem_t                              *txtitem;
    __rect_t                                rect;
    __txtbox_data_t                         *ptxtbox;
    __txtbox_para_t                         *txtbox_priv;
    __u32                                   item_w;
    __u32                                   size;
    ptxtbox = (__txtbox_data_t *)orange_malloc(sizeof(__txtbox_data_t));

    if (!ptxtbox)
    {
        return NULL;
    }

    orange_memset(ptxtbox, 0, sizeof(__txtbox_data_t));
    txtbox_priv                         = (__txtbox_para_t *)GUI_WinGetAttr(htxtbox);
    ptxtbox->handle                     = htxtbox;
    /*item width equal font_size add four*/
    item_w                              = txtbox_priv->font_size + 2 * txtbox_priv->dist;
    ptxtbox->item_w                     = item_w;

    if (txtbox_priv->nitem == 0)
    {
        __HERE__
        ptxtbox->txtlayer               = (txtlayer_man_t *)TxtLayer_Create(GUI_WinGetLyrWin(htxtbox), (__hdle)ptxtbox, 0);
    }
    else
    {
        __HERE__
        ptxtbox->txtlayer               = (txtlayer_man_t *)TxtLayer_Create(GUI_WinGetLyrWin(htxtbox), (__hdle)ptxtbox, item_w * txtbox_priv->nitem);
    }

    ptxtbox->txtlayer->mode             = txtbox_priv->mode;
    ptxtbox->txtlayer->lastmode         = txtbox_priv->mode;
    ptxtbox->txtlayer->font_size        = txtbox_priv->font_size;
    ptxtbox->txtlayer->bk_color         = txtbox_priv->bk_color;

    if (!ptxtbox->txtlayer)
    {
        __msg("txtlayer create failed!\n");
        goto  error;
    }

    for (num = 0; num < txtbox_priv->nitem; num++)
    {
        rect.width                     = ptxtbox->txtlayer->txtbuffer->rectsz.width;
        rect.height                    = item_w;
        /*TxtItem Create*/
        txtitem                       = txtitem_Create(&rect, &txtbox_priv->items[num]);
        /*TxtItem Add to txtbox*/
        TxtList_AddTail(ptxtbox->txtlayer->txtlist, txtitem);
    }

    ptxtbox->txtlayer->txtlist->PrevItem = ptxtbox->txtlayer->txtlist->head;
    ptxtbox->txtlayer->scnnum            = ptxtbox->txtlayer->scnrect.height / item_w;
    size = (ptxtbox->txtlayer->txtlist->head->rect.height *
            ptxtbox->txtlayer->txtlist->head->rect.width *
            ptxtbox->txtlayer->txtlist->bpp + 7) >> 3;
    ptxtbox->txtlayer->txtlist->ItemSize = size;
    ptxtbox->txtlayer->chg_fb = orange_pmalloc(size);

    if (!ptxtbox->txtlayer->chg_fb)
    {
        goto error;
    }

    ptxtbox->txtlayer->draw_font = txtbox_priv->txtfont;
    ptxtbox->txtlayer->draw_code = txtbox_priv->txt_charen;
    return ptxtbox;
error:

    if (ptxtbox->txtlayer)
    {
        if (ptxtbox->txtlayer->chg_fb)
        {
            orange_pfree(ptxtbox->txtlayer->chg_fb, ptxtbox->txtlayer->txtlist->ItemSize);
            ptxtbox->txtlayer->chg_fb = NULL;
        }

        TxtLayer_Destroy(ptxtbox->txtlayer);
    }

    if (ptxtbox)
    {
        orange_mfree(ptxtbox);
    }

    return NULL;
}

//==================================================================
//function name:    GUI_TxtboxGetItemCount
//author:
//date:             2008-12-18
//description:      gui txtboxgetitemcount
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_TxtboxGetItemCount(__txtbox_data_t *ptxtbox)
{
    if (ptxtbox == NULL)
    {
        return __LINE__;
    }

    return  TxtList_GetItemCount(ptxtbox->txtlayer->txtlist);
}

void GUI_TxtboxSetTextAlign(H_WIN htxtbox, int Align)
{
}

//==================================================================
//function name:    GUI_TxtboxDelete
//author:
//date:             2008-12-18
//description:      gui txtboxdelete
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_TxtboxDelete(H_WIN htxtbox)
{
    //   __u8             err;
    __txtbox_data_t  *ptxtbox;
    __u32            ret;

    if (htxtbox == NULL)
    {
        return OK;
    }

    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(htxtbox);
    /*txtlayer destory*/
    ret = TxtLayer_Destroy(ptxtbox->txtlayer);

    if (ret != OK)
    {
        return __LINE__;
    }

    ///*delete txtbox receive thread*/
    orange_mfree(ptxtbox);
    ptxtbox = NULL;
    return OK;
}

/*
**********************************************************************************************************************
*                                               GUI_TxtboxSetStamp
*
* author:
*
* date:             2009-2-22
*
* Description:      gui txtboxsetstamp
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__u32 GUI_TxtboxSetStamp(H_WIN htxtbox, __u32 stamp)
{
    __txtbox_data_t      *ptxtbox;
    __u32              ret;
    __pos_t            curpos;

    if (htxtbox == NULL)
    {
        return __LINE__;
    }

    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(htxtbox);
    ret = TxtLayer_StampToPos(ptxtbox->txtlayer, stamp, &curpos);

    if (ret != OK)
    {
        return __LINE__;
    }

    ret = TxtLayer_SwitchFB(ptxtbox->txtlayer, curpos.x, curpos.y);

    if (ret != OK)
    {
        return __LINE__;
    }

    ret = TxtLayer_StampProcess(ptxtbox->txtlayer, stamp);

    if (ret != OK)
    {
        return __LINE__;
    }

    return OK;
}

//==================================================================
//function name:    GUI_TxtboxDestroy
//author:
//date:             2008-12-18
//description:      gui txtboxdestroy
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_TxtboxDestroy(H_WIN htxtbox)
{
    __txtbox_data_t      *ptxtbox;
    //__u32              ret;
    //__pos_t            curpos;

    if (htxtbox == NULL)
    {
        return __LINE__;
    }

    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(htxtbox);

    if (ptxtbox->txtlayer)
    {
        if (ptxtbox->txtlayer->chg_fb)
        {
            orange_pfree(ptxtbox->txtlayer->chg_fb, ptxtbox->txtlayer->txtlist->ItemSize);
            ptxtbox->txtlayer->chg_fb = NULL;
        }

        TxtLayer_Destroy(ptxtbox->txtlayer);
    }

    if (ptxtbox)
    {
        orange_mfree(ptxtbox);
    }

    return OK;
}

//==================================================================
//function name:    GUI_TxtboxOnDraw
//author:
//date:             2008-12-18
//description:      gui txtboxondraw
//parameters:
//return:           if success return DIS_SUCCESS
//                  if fail return the number of fail
//modify history:
//==================================================================

__u32 GUI_TxtboxOnDraw(H_WIN htxtbox)
{
    __u32             ret;
    __txtbox_data_t      *ptxtbox;
    //    __pos_t            curpos;

    if (htxtbox == NULL)
    {
        return __LINE__;
    }

    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(htxtbox);

    if (!ptxtbox)
    {
        return __LINE__;
    }

    /*get the handle of txtlayer*/
    SYSTEM_PRINTF("start txtlayer ondraw!\n");
    /*txtlayer on draw function*/
    ret = TxtLayer_OnDraw(ptxtbox->txtlayer);

    if (ret != OK)
    {
        return __LINE__;
    }

    return OK;
}

__u32 GUI_TxtboxSetMode(H_WIN htxtbox, __u8 mode)
{
    __txtbox_data_t      *ptxtbox;
    //    __pos_t            curpos;

    if (htxtbox == NULL)
    {
        return __LINE__;
    }

    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(htxtbox);

    if (mode >= 1)
    {
        mode = 1;
    }
    else
    {
        mode = 0;
    }

    ptxtbox->txtlayer->lastmode = ptxtbox->txtlayer->mode;
    ptxtbox->txtlayer->mode     = mode;
    return OK;
}

__u32 GUI_TxtboxGetMode(H_WIN htxtbox)
{
    __txtbox_data_t      *ptxtbox;
    //    __pos_t            curpos;

    if (htxtbox == NULL)
    {
        return __LINE__;
    }

    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(htxtbox);
    return ptxtbox->txtlayer->mode;
}

/*
**********************************************************************************************************************
*                                               ListMenuCtrlProc
*
* author:
*
* date:             2009-9-14
*
* Description:      txtboxctrlproc
*
* parameters:
*
* return:           if success return ERDK_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

static __s32 TxtBoxCtrlProc(__gui_msg_t *msg)
{
    __txtbox_data_t          *pData;
    __s32                    ret;

    switch (msg->id)
    {
        case  GUI_MSG_CREATE:
            pData = (__txtbox_data_t *) GUI_TxtboxPrivCreate(msg->h_deswin);

            if (pData == NULL)
            {
                __wrn("create listmenu private failed");
                return ORANGE_FAIL;
            }

            GUI_CtrlWinSetAddData(msg->h_deswin, (unsigned long)pData);
            return ORANGE_OK;

        case  GUI_MSG_DESTROY:
            GUI_TxtboxDestroy(msg->h_deswin);
            return ORANGE_OK;

        case  GUI_MSG_SET_FOCUS:
            return ORANGE_OK;

        case  GUI_MSG_SET_UNFOCUS:
            return ORANGE_OK;

        case  GUI_MSG_PAINT:
        {
            __txtbox_data_t   *pData;
            pData = (__txtbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (pData->lyrsts == TXTBOX_SLEEP)
            {
                __wrn("txtbox sleep!\n");
                return ORANGE_FAIL;
            }

            /*处理绘制消息*/
            ret = GUI_TxtboxOnDraw(msg->h_deswin);

            if (ret != OK)
            {
                __inf("on pain error!\n");
                return  ORANGE_FAIL;
            }
        }

        return ORANGE_OK;

        case GUI_MSG_KEY:
            return ORANGE_OK;

        case GUI_MSG_TOUCH:
        {
            __txtbox_data_t   *pData;
            pData = (__txtbox_data_t *)GUI_CtrlWinGetAddData(msg->h_deswin);

            if (!pData->txtlayer->txtlist->ItemCount)
            {
                break;
            }

            /*获取键盘ID值*/
            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_UP    :
                {
                    if (GUI_WinGetCaptureWin() != msg->h_deswin)
                    {
                        break;
                    }

                    GUI_WinReleaseCapture();
                }

                return  ORANGE_OK;

                case GUI_MSG_TOUCH_DOWN:
                    return ORANGE_OK;

                case GUI_MSG_TOUCH_MOVE:
                    return ORANGE_OK;

                default:
                    return ORANGE_OK;
            }
        }

        return ORANGE_OK;

        case GUI_MSG_WIN_WAKEUP:
            return ORANGE_OK;

        case GUI_MSG_WIN_SLEEP:
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

/*
**********************************************************************************************************************
*                                               GUI_TxtBoxSetPara
*
* author:
*
* date:             2009-11-30
*
* Description:      gui txtboxsetpara
*
* parameters:
*
* return:           if success return ORANGE_OK
*                   if fail return the number of fail
* modify history:
**********************************************************************************************************************
*/

__s32 GUI_TxtBoxSetPara(H_WIN hbox, __txtbox_para_t *para)
{
    __u32                                   num;
    txtitem_t                              *txtitem;
    __rect_t                                rect;
    __txtbox_data_t                         *ptxtbox;
    __txtbox_para_t                         *txtbox_priv;
    __u32                                   item_w;
    __u32                                   size;
#if GUI_CHECK_ARG_EN > 0

    if (!hbox || !para)
    {
        ORANGE_WRN("h_lyr empty!\n");
        return ORANGE_FAIL;
    }

#endif
    txtbox_priv = (__txtbox_para_t *)para;
    ptxtbox = (__txtbox_data_t *)GUI_CtrlWinGetAddData(hbox);
    TxtList_DelAll(ptxtbox->txtlayer->txtlist);
    /*item width equal font_size add four*/
    item_w                              = txtbox_priv->font_size + 2 * txtbox_priv->dist;
    ptxtbox->item_w                     = item_w;
    ptxtbox->txtlayer->mode             = txtbox_priv->mode;
    ptxtbox->txtlayer->lastmode         = txtbox_priv->mode;
    ptxtbox->txtlayer->font_size        = txtbox_priv->font_size;

    for (num = 0; num < txtbox_priv->nitem; num++)
    {
        rect.width                     = ptxtbox->txtlayer->txtbuffer->rectsz.width;
        rect.height                    = item_w;
        /*TxtItem Create*/
        txtitem                       = txtitem_Create(&rect, &txtbox_priv->items[num]);
        /*TxtItem Add to txtbox*/
        TxtList_AddTail(ptxtbox->txtlayer->txtlist, txtitem);
    }

    ptxtbox->txtlayer->txtlist->PrevItem = ptxtbox->txtlayer->txtlist->head;
    ptxtbox->txtlayer->scnnum            = ptxtbox->txtlayer->scnrect.height / item_w;
    size = (ptxtbox->txtlayer->txtlist->head->rect.height *
            ptxtbox->txtlayer->txtlist->head->rect.width *
            ptxtbox->txtlayer->txtlist->bpp + 7) >> 3;

    if (ptxtbox->txtlayer->chg_fb)
    {
        orange_pfree(ptxtbox->txtlayer->chg_fb, ptxtbox->txtlayer->txtlist->ItemSize);
    }

    ptxtbox->txtlayer->txtlist->ItemSize = size;
    ptxtbox->txtlayer->chg_fb = orange_pmalloc(size);

    if (!ptxtbox->txtlayer->chg_fb)
    {
        goto error;
    }

    ptxtbox->txtlayer->draw_font = txtbox_priv->txtfont;
    ptxtbox->txtlayer->draw_code = txtbox_priv->txt_charen;
error:

    if (ptxtbox->txtlayer->txtlist->ItemCount)
    {
        TxtList_DelAll(ptxtbox->txtlayer->txtlist);
    }

    return ORANGE_FAIL;
}

//------------------------------------------------------------------------------------------------------------
// THE END !
//------------------------------------------------------------------------------------------------------------

#endif
