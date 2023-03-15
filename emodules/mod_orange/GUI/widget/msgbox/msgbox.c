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
#include "msgbox_private.h"
/*
**********************************************************************************************************************
*                                               GUI_IcnmDelete
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static __s32 GUI_MboxDelete(H_WIN h_mbox)
{
    __gui_mbox_t *p_mbox;
#if GUI_CHECK_ARG_EN > 0

    if (!h_mbox)
    {
        __wrn("h_mbox empty!\n");
        return EPDK_FAIL;
    }

#endif
    p_mbox = (__gui_mbox_t *)GUI_WinGetAttr(h_mbox);
    esMEMS_Mfree(0, (void *)p_mbox);
    return EPDK_OK;
}

__s32 GUI_MboxDestory(H_WIN h_mbox)
{
    if (h_mbox == 0)
    {
        return EPDK_FAIL;
    }

    GUI_WinDelete(h_mbox);
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               cb_Mbox
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void cb_Mbox(__gui_msg_t *msg)
{
    H_WIN       curfcs;
    __gui_mbox_t *mbox = (__gui_mbox_t *)(((__gui_win_t *)(msg->h_deswin))->attr);

    if (mbox->mbox_cb != 0)
    {
        //mbox->mbox_cb(msg);
        esKRNL_CallBack((__pCBK_t)*mbox->mbox_cb, msg);
    }

    switch (msg->id)
    {
        case GUI_MSG_INIT_WIN:
            return;

        case GUI_MSG_DELETE_WIN:
            GUI_MboxDelete(msg->h_deswin);
            return;

        case GUI_MSG_ONPAIN:
            if (mbox->yesbutton)
            {
                __gui_win_painpara_t para;
                __gui_msg_t          new_msg;
                para.mode = GUI_PAIN_MODE_UNFOUCS;
                new_msg.p_arg = &para;
                new_msg.id   = GUI_MSG_ONPAIN;
                new_msg.h_srcwin = mbox->box_win;
                new_msg.h_deswin = mbox->yesbutton;
                GUI_SendMessage(&new_msg);
            }

            if (mbox->nobutton)
            {
                __gui_win_painpara_t para;
                __gui_msg_t          new_msg;
                para.mode = GUI_PAIN_MODE_UNFOUCS;
                new_msg.p_arg = &para;
                new_msg.id   = GUI_MSG_ONPAIN;
                new_msg.h_srcwin = mbox->box_win;
                new_msg.h_deswin = mbox->nobutton;
                GUI_SendMessage(&new_msg);
            }

            if (mbox->cancelbutton)
            {
                __gui_win_painpara_t para;
                __gui_msg_t          new_msg;
                para.mode = GUI_PAIN_MODE_UNFOUCS;
                new_msg.p_arg = &para;
                new_msg.id   = GUI_MSG_ONPAIN;
                new_msg.h_srcwin = mbox->box_win;
                new_msg.h_deswin = mbox->cancelbutton;
                GUI_SendMessage(&new_msg);
            }

            return ;

        case GUI_MSG_SET_FOCUS:
            if (mbox->flags != NO_BUTTON)
            {
                GUI_WinSetFocus(mbox->yesbutton);
            }

            return ;

        case GUI_MSG_KEY:
            switch (((__gui_keyinfo_t *)(msg->p_arg))->key)
            {
                case GUI_MSG_KEY_ENTER:
                    //GUI_WinDelete(msg->h_deswin);
                    return;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_RIGHT:
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (mbox->flags == NO_BUTTON)
                        {
                            return ;
                        }
                        else if (mbox->flags == YES_BUTTON)
                        {
                            return ;
                        }
                        else if (mbox->flags == YES_NO_BUTTON)
                        {
                            curfcs = GUI_WinGetFocus();

                            if (curfcs == mbox->yesbutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->yesbutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->nobutton);
                            }
                            else
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->nobutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->yesbutton);
                            }
                        }
                        else if (mbox->flags == YES_NO_CANCEL_BUTTON)
                        {
                            curfcs = GUI_WinGetFocus();

                            if (curfcs == mbox->yesbutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->yesbutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->nobutton);
                            }
                            else if (curfcs == mbox->nobutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->nobutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->cancelbutton);
                            }
                            else if (curfcs == mbox->cancelbutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->cancelbutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->yesbutton);
                            }
                        }
                    }

                    return;

                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LEFT:
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        if (mbox->flags == NO_BUTTON)
                        {
                            return ;
                        }
                        else if (mbox->flags == YES_BUTTON)
                        {
                            return ;
                        }
                        else if (mbox->flags == YES_NO_BUTTON)
                        {
                            curfcs = GUI_WinGetFocus();

                            if (curfcs == mbox->yesbutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->yesbutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->nobutton);
                            }
                            else
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->nobutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->yesbutton);
                            }
                        }
                        else if (mbox->flags == YES_NO_CANCEL_BUTTON)
                        {
                            curfcs = GUI_WinGetFocus();

                            if (curfcs == mbox->yesbutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->yesbutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->cancelbutton);
                            }
                            else if (curfcs == mbox->nobutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->nobutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->yesbutton);
                            }
                            else if (curfcs == mbox->cancelbutton)
                            {
                                __gui_win_painpara_t para;
                                __gui_msg_t          new_msg;
                                para.mode = GUI_PAIN_MODE_UNFOUCS;
                                new_msg.p_arg = &para;
                                new_msg.id   = GUI_MSG_ONPAIN;
                                new_msg.h_srcwin = mbox->box_win;
                                new_msg.h_deswin = mbox->cancelbutton;
                                GUI_SendMessage(&new_msg);
                                GUI_WinSetFocus(mbox->nobutton);
                            }
                        }
                    }

                    return;

                default:
                    return ;
            }

        default:
            break;
    }

    return ;
}
/*
**********************************************************************************************************************
*                                               cb_Mbox
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void cb_YesButton(__gui_msg_t *msg)
{
    __gui_mbox_t *mbox = (__gui_mbox_t *)(((__gui_win_t *)(msg->h_deswin))->parent->attr);

    if (mbox->yes_usercb)
    {
        //mbox->yes_usercb(msg);
        esKRNL_CallBack((__pCBK_t)*mbox->yes_usercb, msg);
    }

    switch (msg->id)
    {
        case GUI_MSG_INIT_WIN:
            return;

        case GUI_MSG_DELETE_WIN:
            return;

        case GUI_MSG_ONPAIN:
            return;

        case GUI_MSG_SET_FOCUS:
            return ;

        case GUI_MSG_KEY:
            switch (((__gui_keyinfo_t *)(msg->p_arg))->key)
            {
                case GUI_MSG_KEY_ENTER:
                    //GUI_WinDelete(mbox->box_win);
                    return;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_RIGHT:
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        GUI_WinDefaultProc(msg);
                    }

                    return ;

                default:
                    return ;
            }

        default:
            break;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               cb_Nobutton
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static void cb_NoButton(__gui_msg_t *msg)
{
    __gui_mbox_t *mbox = (__gui_mbox_t *)(((__gui_win_t *)(msg->h_deswin))->parent->attr);

    if (mbox->yes_usercb)
    {
        //mbox->yes_usercb(msg);
        esKRNL_CallBack((__pCBK_t)*mbox->yes_usercb, msg);
    }

    switch (msg->id)
    {
        case GUI_MSG_INIT_WIN:
            return;

        case GUI_MSG_DELETE_WIN:
            return;

        case GUI_MSG_ONPAIN:
            return;

        case GUI_MSG_SET_FOCUS:
            return ;

        case GUI_MSG_KEY:
            switch (((__gui_keyinfo_t *)(msg->p_arg))->key)
            {
                case GUI_MSG_KEY_ENTER:
                    //GUI_WinDelete(mbox->box_win);
                    return;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_RIGHT:
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        GUI_WinDefaultProc(msg);
                    }

                    return ;

                default:
                    return ;
            }

        default:
            break;
    }

    return ;
}

static void cb_CancelButton(__gui_msg_t *msg)
{
    __gui_mbox_t *mbox = (__gui_mbox_t *)(((__gui_win_t *)(msg->h_deswin))->parent->attr);

    if (mbox->yes_usercb)
    {
        //mbox->yes_usercb(msg);
        esKRNL_CallBack((__pCBK_t)*mbox->yes_usercb, msg);
    }

    switch (msg->id)
    {
        case GUI_MSG_INIT_WIN:
            return;

        case GUI_MSG_DELETE_WIN:
            return;

        case GUI_MSG_ONPAIN:
            return;

        case GUI_MSG_SET_FOCUS:
            return ;

        case GUI_MSG_KEY:
            switch (((__gui_keyinfo_t *)(msg->p_arg))->key)
            {
                case GUI_MSG_KEY_ENTER:
                    //GUI_WinDelete(mbox->box_win);
                    return;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_RIGHT:
                    if (msg->dwAddData2 == KEY_DOWN_ACTION)
                    {
                        GUI_WinDefaultProc(msg);
                    }

                    return ;

                default:
                    return ;
            }

        default:
            break;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               GUI_MboxCreate
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
H_WIN GUI_MboxCreate(__gui_mbox_create_para_t *p_mbox_para, H_WIN h_parent)
{
    __gui_win_t              *p_win = NULL;
    __gui_mbox_t             *p_mbox = NULL;
#if GUI_CHECK_ARG_EN > 0

    if (!p_mbox_para)
    {
        __wrn("p_mbox_para null!\n");
        return (H_WIN)0;
    }

#endif
    /* create icon menu handle                      */
    p_mbox = (__gui_mbox_t *)esMEMS_Malloc(0, sizeof(__gui_mbox_t));

    if (!p_mbox)
    {
        __wrn("malloc fail!\n");
        return (H_WIN)0;
    }

    p_mbox->nbutton      = p_mbox_para->attr->nbutton;
    p_mbox->mbox_cb      = p_mbox_para->cb;
    p_mbox->layer        = p_mbox_para->h_lyr;
    p_mbox->usr_attr     = p_mbox_para->attr->p_arg;
    p_mbox->cancelbutton = 0;
    p_mbox->nobutton     = 0;
    p_mbox->yesbutton    = 0;
    p_mbox->cancel_usercb = NULL;
    p_mbox->yes_usercb   = NULL;
    p_mbox->no_usercb    = NULL;

    if (p_mbox_para->attr->nbutton == 0)
    {
        p_mbox->flags = NO_BUTTON;
    }
    else if (p_mbox_para->attr->nbutton == 1)
    {
        p_mbox->flags = YES_BUTTON;
    }
    else if (p_mbox_para->attr->nbutton == 2)
    {
        p_mbox->flags = YES_NO_BUTTON;
    }
    else
    {
        p_mbox->flags = YES_NO_CANCEL_BUTTON;
    }

    /* create win                                   */
    {
        __gui_wincreate_para_t winpara;
        winpara.name  = p_mbox_para->name;
        winpara.rect  = p_mbox_para->rect;
        winpara.h_lyr = p_mbox_para->h_lyr;
        winpara.cb    = cb_Mbox;
        winpara.attr  = (void *)p_mbox;
        p_win         = (__gui_win_t *)GUI_WinCreate(&winpara, h_parent);
    }

    if (!p_win)
    {
        __wrn("cannot create message box!\n");
        return (H_WIN)0;
    }

    /* init name                                    */
    eLIBs_strncpy(p_mbox->name, p_mbox_para->name, GUI_NAME_MAX - 1);

    /* create child windows                         */
    switch (p_mbox->flags)
    {
        case NO_BUTTON:
            break;

        case YES_BUTTON:
        {
            __gui_wincreate_para_t winpara;

            if (p_mbox_para->attr->buttons == NULL)
            {
                __wrn("NO button parameter error!\n");
                goto error;
            }

            winpara.rect.x      = p_mbox_para->attr->buttons[0].rect.x;
            winpara.rect.y      = p_mbox_para->attr->buttons[0].rect.y;
            winpara.rect.width  = p_mbox_para->attr->buttons[0].rect.width;
            winpara.rect.height = p_mbox_para->attr->buttons[0].rect.height;
            winpara.h_lyr       = p_mbox_para->h_lyr;
            winpara.cb          = cb_YesButton;
            winpara.attr        = p_mbox_para->attr->buttons[0].attr;
            p_mbox->yes_usercb  = p_mbox_para->attr->buttons[0].cb;
            p_mbox->yesbutton   = GUI_WinCreate(&winpara, (H_WIN)p_win);

            if (p_mbox->yesbutton == (H_WIN)0)
            {
                __wrn("yes button create failed!\n");
                goto error;
            }

            break;
        }

        case YES_NO_BUTTON:
        {
            __gui_wincreate_para_t winpara;

            if (p_mbox_para->attr->buttons == NULL)
            {
                __wrn("NO yes button parameter!\n");
                goto error;
            }

            if (p_mbox_para->attr->buttons == NULL)
            {
                __wrn("NO no button parameter!\n");
                goto error;
            }

            winpara.rect.x      = p_mbox_para->attr->buttons[0].rect.x;
            winpara.rect.y      = p_mbox_para->attr->buttons[0].rect.y;
            winpara.rect.width  = p_mbox_para->attr->buttons[0].rect.width;
            winpara.rect.height = p_mbox_para->attr->buttons[0].rect.height;
            winpara.h_lyr       = p_mbox_para->h_lyr;
            winpara.cb          = cb_YesButton;
            winpara.attr        = p_mbox_para->attr->buttons[0].attr;
            p_mbox->yes_usercb  = p_mbox_para->attr->buttons[0].cb;
            p_mbox->yesbutton   = GUI_WinCreate(&winpara, (H_WIN)p_win);

            if (p_mbox->yesbutton == (H_WIN)0)
            {
                __wrn("yes button create failed!\n");
                goto error;
            }

            winpara.rect.x      = p_mbox_para->attr->buttons[1].rect.x;
            winpara.rect.y      = p_mbox_para->attr->buttons[1].rect.y;
            winpara.rect.width  = p_mbox_para->attr->buttons[1].rect.width;
            winpara.rect.height = p_mbox_para->attr->buttons[1].rect.height;
            winpara.h_lyr       = p_mbox_para->h_lyr;
            winpara.cb          = cb_NoButton;
            winpara.attr        = p_mbox_para->attr->buttons[1].attr;
            p_mbox->no_usercb  = p_mbox_para->attr->buttons[1].cb;
            p_mbox->nobutton   = GUI_WinCreate(&winpara, (H_WIN)p_win);

            if (p_mbox->nobutton == (H_WIN)0)
            {
                __wrn("yes button create failed!\n");
                goto error;
            }

            break;
        }

        case YES_NO_CANCEL_BUTTON:
        {
            __gui_wincreate_para_t winpara;

            if (p_mbox_para->attr->buttons == NULL)
            {
                __wrn("NO yes button parameter!\n");
                goto error;
            }

            if (p_mbox_para->attr->buttons == NULL)
            {
                __wrn("NO no button parameter!\n");
                goto error;
            }

            if (p_mbox_para->attr->buttons == NULL)
            {
                __wrn("NO no button parameter!\n");
                goto error;
            }

            winpara.rect.x      = p_mbox_para->attr->buttons[0].rect.x;
            winpara.rect.y      = p_mbox_para->attr->buttons[0].rect.y;
            winpara.rect.width  = p_mbox_para->attr->buttons[0].rect.width;
            winpara.rect.height = p_mbox_para->attr->buttons[0].rect.height;
            winpara.h_lyr       = p_mbox_para->h_lyr;
            winpara.cb          = cb_YesButton;
            winpara.attr        = p_mbox_para->attr->buttons[0].attr;
            p_mbox->yes_usercb  = p_mbox_para->attr->buttons[0].cb;
            p_mbox->yesbutton   = GUI_WinCreate(&winpara, (H_WIN)p_win);

            if (p_mbox->yesbutton == (H_WIN)0)
            {
                __wrn("yes button create failed!\n");
                goto error;
            }

            winpara.rect.x      = p_mbox_para->attr->buttons[1].rect.x;
            winpara.rect.y      = p_mbox_para->attr->buttons[1].rect.y;
            winpara.rect.width  = p_mbox_para->attr->buttons[1].rect.width;
            winpara.rect.height = p_mbox_para->attr->buttons[1].rect.height;
            winpara.h_lyr       = p_mbox_para->h_lyr;
            winpara.cb          = cb_NoButton;
            winpara.attr        = p_mbox_para->attr->buttons[1].attr;
            p_mbox->no_usercb  = p_mbox_para->attr->buttons[1].cb;
            p_mbox->nobutton   = GUI_WinCreate(&winpara, (H_WIN)p_win);

            if (p_mbox->nobutton == (H_WIN)0)
            {
                __wrn("no button create failed!\n");
                goto error;
            }

            winpara.rect.x      = p_mbox_para->attr->buttons[2].rect.x;
            winpara.rect.y      = p_mbox_para->attr->buttons[2].rect.y;
            winpara.rect.width  = p_mbox_para->attr->buttons[2].rect.width;
            winpara.rect.height = p_mbox_para->attr->buttons[2].rect.height;
            winpara.h_lyr       = p_mbox_para->h_lyr;
            winpara.cb          = cb_CancelButton;
            winpara.attr        = p_mbox_para->attr->buttons[2].attr;
            p_mbox->cancel_usercb  = p_mbox_para->attr->buttons[2].cb;
            p_mbox->cancelbutton   = GUI_WinCreate(&winpara, (H_WIN)p_win);

            if (p_mbox->cancelbutton == (H_WIN)0)
            {
                __wrn("cancel button create failed!\n");
                goto error;
            }

            break;
        }

        default:
            __wrn("too many buttons!\n");
            goto error;
    }

    p_mbox->box_win = (H_WIN)p_win;
    return (H_WIN)p_win;
error:

    if (p_mbox)
    {
        if (p_mbox->yesbutton)
        {
            GUI_WinDelete(p_mbox->yesbutton);
        }

        if (p_mbox->nobutton)
        {
            GUI_WinDelete(p_mbox->nobutton);
        }

        if (p_mbox->cancelbutton)
        {
            GUI_WinDelete(p_mbox->cancelbutton);
        }

        esMEMS_Mfree(0, p_mbox);
    }

    if (p_win)
    {
        GUI_WinDelete((H_WIN)p_win);
    }

    return (H_WIN)0;
}
