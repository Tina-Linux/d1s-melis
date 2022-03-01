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
* File    : icnm.c
*
* By      : Steven
* Version : v1.0
* Date    : 2008-10-4 11:24:32
**********************************************************************************************************************
*/
#include "icnm_private.h"

SIZE ICNM_icontbl[16] =                             /* icon 的大小表                                                */
{
    {80, 128}, {78, 125}, {76, 122}, {74, 118}, {72, 115}, {70, 112}, {68, 109}, {66, 106},
    {64, 102}, {62, 99},  {60, 96},  {58, 93},  {56, 90},  {54, 86},  {52, 83},  {50, 80}
};

__u32 ICNM_pos[] =                                  /* icon 的大小表                                                */
{
    0, 0, 0, 0, 0, 0, 1, 2, 4, 7, 10, 13, 17, 21, 25, 30, 35, 39, 43, 46, 50, 53, 56, 58, 59, 60, 60, 60, 60, 60, 60
};

__u32 ICNM_steptbl[] =
{
    0, 7, 12, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};

extern GUI_FONT      SWFFont;

/* static */
/*
**********************************************************************************************************************
*                                               ICNM_icon_show_thread
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      : this thread is used to create all the scaled icons
*
**********************************************************************************************************************
*/
static void ICNM_icon_show_thread(void *p_arg)
{
    __u8            i;
    __lion_icnm_t *icnm = (__lion_icnm_t *)p_arg;
    __u32           curitem;
    GUI_SetBkColor(0x00ffffff);
    GUI_ClearRect(0, 0, 600, 300);

    while (1)
    {
        esKRNL_TimeDly(1);
        curitem = icnm->curitem;

        for (i = 0; i < icnm->nitem; i++)
        {
            __lion_msg_t msg;
            __lion_win_painpara_t painpara;
            painpara.mode = LION_PAIN_MODE_FOUCS;

            if (curitem == i)
            {
                painpara.aux  = ICNM_steptbl[0];
            }
            else
            {
                if (i > curitem)
                {
                    painpara.aux  = ICNM_steptbl[i - curitem];
                }
                else
                {
                    painpara.aux  = 0 - ICNM_steptbl[curitem - i];
                }
            }

            msg.id       = LION_MSG_ONPAIN;
            msg.h_srcwin = icnm->h_win;
            msg.h_deswin = (H_WIN)(icnm->items[i]);
            msg.p_arg    = (void *)&painpara;

            while (LION_LockAccept() == EPDK_FAIL)
            {
                if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
                {
                    esKRNL_TDel(OS_PRIO_SELF);
                    return;
                }

                esKRNL_TimeDly(1);
            }

            LION_SendMessage(&msg);
            LION_Unlock();
        }
    }
}
/*
**********************************************************************************************************************
*                                               IMENU_icon_prescale
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      : this thread is used to create all the scaled icons
*
**********************************************************************************************************************
*/
static void ICNM_icon_prescale_thread(void *p_arg)
{
    __u8            i;
    __lion_icnm_t *icnm = (__lion_icnm_t *)p_arg;

    /* 预先产生变化的图                         */
    for (i = 0; i < icnm->nitem; i++)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        if (ICNM_prescale_icons(icnm->items[i]) == EPDK_FAIL)
        {
            __err("prescale icon fail!");
        }
    }

    icnm->tid_prescale = 0xff;
    esKRNL_TDel(OS_PRIO_SELF);
    return ;
}
/*
**********************************************************************************************************************
*                                               LION_IcnmDelete
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
static __s32 LION_IcnmDelete(H_WIN h_icnm)
{
    __lion_icnm_t *p_icnm;
#if LION_CHECK_ARG_EN > 0

    if (!h_icnm)
    {
        __wrn("h_icnm empty!");
        return EPDK_FAIL;
    }

#endif
    p_icnm = (__lion_icnm_t *)LION_WinGetAttr(h_icnm);

    while (1)
    {
        if (p_icnm->tid_show != 0xff)
        {
            if (esKRNL_TDelReq(p_icnm->tid_show) == OS_TASK_NOT_EXIST)
            {
                break;
            }
        }
        else
        {
            break;
        }

        esKRNL_TimeDly(1);
    }

    while (1)
    {
        if (p_icnm->tid_prescale != 0xff)
        {
            if (esKRNL_TDelReq(p_icnm->tid_prescale) == OS_TASK_NOT_EXIST)
            {
                break;
            }
        }
        else
        {
            break;
        }

        esKRNL_TimeDly(1);
    }

    esMEMS_Mfree(0, (void *)p_icnm->items);
    esMEMS_Mfree(0, (void *)p_icnm);
    return EPDK_OK;
}
/*
**********************************************************************************************************************
*                                               cb_Iconmenu
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
static void cb_Iconmenu(__lion_msg_t *msg)
{
    __s32       cpu_sr;
    __lion_icnm_t *icnm = (__lion_icnm_t *)(((__lion_win_t *)(msg->h_deswin))->attr);

    switch (msg->id)
    {
        case LION_MSG_INIT_WIN:
            return;

        case LION_MSG_DELETE_WIN:
            LION_IcnmDelete(msg->h_deswin);
            return;

        case (LION_MSG_USRDEF + 0):
            LION_LyrSel(((__lion_win_t *)(msg->h_deswin))->h_layer);
            GUI_UC_SetEncodeGB2312();
            GUI_SetFont(&SWFFont);
            GUI_SetBkColor(0x48ffffff);
            GUI_ClearRect(200, 40, 500, 80);
            GUI_DispStringAt(msg->p_arg, 250, 40);
            return;

        case LION_MSG_KEY:
            switch (((__lion_keyinfo_t *)(msg->p_arg))->key)
            {
                case LION_MSG_KEY_RIGHT:
                {
                    __gui_msg_t msg;
                    msg.id  = GUI_MSG_KEYTONE;
                    msg.h_srcwin = 0;
                    msg.h_deswin = GUI_WinGetBackWin();
                    msg.p_arg    = (void *)0;
                    GUI_SendMessage(&msg);
                }

                ENTER_CRITICAL(cpu_sr);

                if (icnm->curitem < 6)
                {
                    icnm->curitem++;
                }

                EXIT_CRITICAL(cpu_sr);
                return;

                case LION_MSG_KEY_LEFT:
                {
                    __gui_msg_t msg;
                    msg.id  = GUI_MSG_KEYTONE;
                    msg.h_srcwin = 0;
                    msg.h_deswin = GUI_WinGetBackWin();
                    msg.p_arg    = (void *)0;
                    GUI_SendMessage(&msg);
                }

                ENTER_CRITICAL(cpu_sr);

                if (icnm->curitem > 0)
                {
                    icnm->curitem--;
                }

                EXIT_CRITICAL(cpu_sr);
                return;

                default:
                    LION_WinDefaultProc(msg);
                    return ;
            }

        default:
            LION_WinDefaultProc(msg);
            break;
    }

    return ;
}

/*
**********************************************************************************************************************
*                                               LION_IcnmCreate
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
H_WIN LION_IcnmCreate(__lion_icnm_create_para_t *p_icnm_para, H_WIN h_parent)
{
    __u32            i;
    __lion_win_t    *p_win;
    __lion_icnm_t   *p_icnm;
#if LION_CHECK_ARG_EN > 0

    if (!p_icnm_para)
    {
        __wrn("p_button_para null!");
        return (H_WIN)0;
    }

#endif
    /* create icon menu handle                      */
    p_icnm = (__lion_icnm_t *)esMEMS_Malloc(0, sizeof(__lion_icnm_t));

    if (!p_icnm)
    {
        __wrn("malloc fail!");
        return (H_WIN)0;
    }

    /* create win                                   */
    {
        __lion_wincreate_para_t winpara;
        winpara.name  = p_icnm_para->name;
        winpara.rect  = p_icnm_para->rect;
        winpara.h_lyr = p_icnm_para->h_lyr;
        winpara.cb    = cb_Iconmenu;
        winpara.attr  = p_icnm;
        p_win         = (__lion_win_t *)LION_WinCreate(&winpara, h_parent);
    }
    /* init name                                    */
    eLIBs_strncpy(p_icnm->name, p_icnm_para->name, LION_NAME_MAX - 1);
    /* init nitem/curitem                           */
    p_icnm->nitem   = p_icnm_para->attr->nitem;
    p_icnm->curitem = p_icnm_para->attr->curitem;
    /* malloc items pointers array                  */
    p_icnm->items = (__lion_win_t **)esMEMS_Malloc(0, 4 * (p_icnm->nitem));

    if (!p_icnm->items)
    {
        __wrn("malloc fail!");
        esMEMS_Mfree(0, (void *)p_icnm);
        return (H_WIN)0;
    }

    eLIBs_memclr(p_icnm->items, 4 * (p_icnm->nitem));

    /* create items                                 */
    for (i = 0; i < p_icnm->nitem; i++)
    {
        __lion_icnm_itemcreate_para_t itempara;
        itempara.name        = p_icnm_para->attr->items[i].name;
        itempara.title       = p_icnm_para->attr->items[i].title;
        itempara.rect.x      = 60 + 72 * i;
        itempara.rect.y      = 100;
        itempara.rect.width  = 60;
        itempara.rect.height = 140;
        itempara.h_lyr       = p_icnm_para->h_lyr;
        itempara.cb          = 0;
        itempara.attr        = &(p_icnm_para->attr->items[i]);
        itempara.index       = i;
        p_icnm->items[i]     = ICNM_item_create(&itempara, (H_WIN)p_win);

        if (!p_icnm->items[i])
        {
            __wrn("because win cannot be created, create imenu fail!");
            return (H_WIN)0;
        }
    }

    /* start thread                                 */
#ifdef SIM_PC_WIN
    __SAVE_MOD_EVN__
#endif
    p_icnm->tid_show     = esKRNL_TCreate(ICNM_icon_show_thread,
                                          (void *)p_icnm,
                                          0x400,
                                          (EPOS_curid << 8) | KRNL_priolevel2);
    p_icnm->tid_prescale = esKRNL_TCreate(ICNM_icon_prescale_thread,
                                          (void *)p_icnm,
                                          0x400,
                                          (EPOS_curid << 8) | KRNL_priolevel2);
#ifdef SIM_PC_WIN
    __RESTORE_MOD_EVN__
#endif
    return (H_WIN)p_win;
}


/*
**********************************************************************************************************************
*                                               LION_IcnmGetCurItemIndex
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
__u8   LION_IcnmGetCurItemIndex(H_WIN h_icnm)
{
    __lion_icnm_t *p_icnm;
#if LION_CHECK_ARG_EN > 0

    if (!h_icnm)
    {
        __wrn("h_icnm empty!");
        return 0xff;
    }

#endif
    p_icnm = ((__lion_win_t *)h_icnm)->attr;
    return p_icnm->curitem;
}
