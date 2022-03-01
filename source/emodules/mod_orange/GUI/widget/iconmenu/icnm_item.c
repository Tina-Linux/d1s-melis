/*
**********************************************************************************************************************
*                                                    ePDK
*                                    the Easy Portable/Player Develop Kits
*                                              eMOD Sub-System
*
*                                   (c) Copyright 2007-2009, Steven.ZGJ.China
*                                             All Rights Reserved
*
* Moudle  : lion
* File    : icon.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-11-30 10:45:42
**********************************************************************************************************************
*/
#include "icnm_private.h"
/* static */
/*
**********************************************************************************************************************
*                                               _draw
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
static __u32 abs(__s32 v)
{
    if (v >= 0)
    {
        return v;
    }

    return -v;
}
static void _draw(__lion_msg_t *msg)
{
    __u32   tmp;
    __lion_win_painpara_t *onpain_para;
    __lion_win_t           *p_win  = (__lion_win_t *)msg->h_deswin;
    __lion_icnm_item_t     *p_item = (__lion_icnm_item_t *)p_win->attr;
    onpain_para = msg->p_arg;

    if (onpain_para->aux != p_item->end)
    {
        p_item->step  = 0;
        p_item->start = p_item->current;
        p_item->end   = onpain_para->aux;
    }

    if (p_item->start == p_item->end)
    {
        return;
    }

    if (p_item->step != 16)
    {
        if (onpain_para->mode == LION_PAIN_MODE_FOUCS)
        {
            p_item->step++;
        }

        if (p_item->current == 0)
        {
            if (p_item->drawflag)
            {
                __lion_msg_t mymsg;
                mymsg.id = LION_MSG_USRDEF + 0;
                mymsg.p_arg = ((__lion_icnm_item_t *)(((__lion_win_t *)(msg->h_deswin))->attr))->title;
                mymsg.h_srcwin = 0;
                mymsg.h_deswin = ((H_WIN)((__lion_win_t *)(msg->h_deswin))->parent);
                LION_SendMessage(&mymsg);
                p_item->drawflag = 0;
            }
        }
        else
        {
            p_item->drawflag = 1;
        }

        tmp = ((p_item->end - p_item->start) * p_item->step + p_item->start * 16) / 16;

        /* update current                           */
        if (p_item->current != tmp)
        {
            /* 显示当前的图 */
            LION_LyrSel(p_win->h_layer);

            if (!p_item->s_icon[abs(p_item->current)].argbbuf)
            {
                ICNM_prescale_icon(p_win, abs(p_item->current));
            }

            if (1)
            {
                FB fb;
                GUI_SetBkColor(0x00ffffff);
                GUI_ClearRect(p_win->rect.x,
                              p_win->rect.y,
                              (p_win->rect.x + p_item->s_icon[abs(tmp)].size.width + 4),
                              (p_win->rect.y + p_win->rect.height));
                p_win->rect.x = p_win->rect.x + ICNM_pos[tmp + 15] - ICNM_pos[p_item->current + 15];
                p_win->rect.width = p_win->rect.width + (p_item->current - tmp) * 2;
                fb.size                  = p_item->s_icon[abs(tmp)].size;
                fb.addr[0]               = p_item->s_icon[abs(tmp)].argbbuf;
                fb.fmt.type              = FB_TYPE_RGB;
                fb.fmt.fmt.rgb.pixelfmt  = PIXEL_COLOR_ARGB8888;
                fb.fmt.fmt.rgb.br_swap   = 0;
                fb.fmt.fmt.rgb.pixseq    = 2;
                GUI_BitString_DrawEx(&fb, p_win->rect.x, p_win->rect.y);
            }

            //ICNM_showiconname(item->name);
            p_item->current = tmp;
        }
    }

    return;
}
/*
**********************************************************************************************************************
*                                               cb_Icon
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
static void cb_Icon(__lion_msg_t *msg)
{
    switch (msg->id)
    {
        case LION_MSG_DELETE_WIN:
            ICNM_item_delete(msg->h_deswin);
            return;

        case LION_MSG_ONPAIN:
            _draw(msg);
            return;

        default:
            LION_WinDefaultProc(msg);
            break;
    }

    return ;
}
/* public */
/*
**********************************************************************************************************************
*                                               ICNM_item_create
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
__lion_win_t *ICNM_item_create(__lion_icnm_itemcreate_para_t *p_para, H_WIN h_parent)
{
    __u32                           i;
    __lion_win_t                   *p_win;
    __lion_icnm_item_t             *p_item;
    __lion_icnm_itemcreate_attr_t *p_attr;
#if LION_CHECK_ARG_EN > 0

    if (!p_para)
    {
        __wrn("p_button_para null!\n");
        return (H_WIN)0;
    }

#endif
    p_item  = (void *)esMEMS_Malloc(0, sizeof(__lion_icnm_item_t));

    if (!p_item)
    {
        __err("malloc err!\n");
    }

    eLIBs_memclr((void *)p_item, sizeof(__lion_icnm_item_t));
    p_attr = p_para->attr;
    eLIBs_memcpy(p_item->name, p_attr->name, LION_NAME_MAX);
    eLIBs_memcpy(p_item->title, p_attr->title, LION_TITLE_MAX);
    p_item->mode     = LION_PAIN_MODE_UNFOUCS;
    p_item->current  = 15;
    p_item->step     = 0;
    p_item->start    = 15;
    p_item->end      = 15;
    p_item->index    = p_para->index;
    p_item->icon[0] = p_attr->icon[0];
    p_item->icon[0].argbbuf = (void *)((__u32)p_attr->icon[0].argbbuf + 0x38);

    for (i = 0; i < 16; i++)
    {
        p_item->s_icon[i].argbbuf = 0;
    }

    if (1)
    {
        __lion_wincreate_para_t winpara;
        winpara.name  = p_para->name;
        winpara.rect  = p_para->rect;
        winpara.h_lyr = p_para->h_lyr;
        winpara.cb    = cb_Icon;
        winpara.attr  = (void *)p_item;
        p_win = (__lion_win_t *)LION_WinCreate(&winpara, h_parent);
    }

    if (!p_win)
    {
        __wrn("because win cannot be created, create button fail!\n");
        return (H_WIN)0;
    }

    return p_win;
}
/*
**********************************************************************************************************************
*                                               ICNM_icon_delete
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
__s32 ICNM_item_delete(H_WIN h_item)
{
    __u32   i;
    __lion_icnm_item_t *p_item;
#if LION_CHECK_ARG_EN > 0

    if (!h_item)
    {
        __wrn("p_button_para null!\n");
        return (H_WIN)0;
    }

#endif
    p_item = (__lion_icnm_item_t *)(((__lion_win_t *)h_item)->attr);

    for (i = 0; i < 16; i++)
    {
        if (p_item->s_icon[i].argbbuf)
        {
            esMEMS_Bfree(p_item->s_icon[i].argbbuf,
                         ICNM_icontbl[i].width * ICNM_icontbl[i].height * 4);
        }
    }

    esMEMS_Mfree(0, (void *)p_item);
    return EPDK_OK;
}

/*
**********************************************************************************************************************
*                                               ICNM_prescale_icon
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
__s32 ICNM_prescale_icon(__lion_win_t *p_icon, __u8 iconindex)
{
    __lion_icnm_item_t   *p_item = (__lion_icnm_item_t *)(p_icon->attr);

    /* 预先产生变化的图                             */
    if (!p_item->s_icon[iconindex].argbbuf)         /* make sure icon not be created by someone else already        */
    {
        __s32               cpu_sr;
        __lion_icnm_icon_t  icon;
        icon.argbbuf = esMEMS_Balloc(ICNM_icontbl[iconindex].width * ICNM_icontbl[iconindex].height * 4);

        if (!icon.argbbuf)
        {
            __wrn("balloc tmp fail!\n");
            return EPDK_FAIL;
        }

        icon.size = ICNM_icontbl[iconindex];
        ICNM_Scaler(&icon, &(p_item->icon[0]));
        ENTER_CRITICAL(cpu_sr);

        /* make sure icon not created by else   */
        if (!p_item->s_icon[iconindex].argbbuf)
        {
            p_item->s_icon[iconindex] = icon;
            icon.argbbuf = 0;
        }

        EXIT_CRITICAL(cpu_sr);

        /* if tmp not equ. 0, means s_icon[i] is created by someone else                                        */
        if (icon.argbbuf)
        {
            esMEMS_Bfree(icon.argbbuf, ICNM_icontbl[iconindex].width * ICNM_icontbl[iconindex].height * 4);
        }
    }

    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               ICNM_prescale_icons
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
__s32 ICNM_prescale_icons(__lion_win_t *p_icon)
{
    __u8    i;

    /* 预先产生变化的图                         */
    for (i = 0; i < 16; i++)
    {
        if (ICNM_prescale_icon(p_icon, i) == EPDK_FAIL)
        {
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}
