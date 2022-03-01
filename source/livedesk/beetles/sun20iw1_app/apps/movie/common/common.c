/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : common.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include <log.h>
#include "beetles_app.h"

static __krnl_event_t *g_movie_robin_sem = NULL;

__s32 __movie_robin_sem_init(void)
{
    if (0 == g_movie_robin_sem)
    {
        g_movie_robin_sem = esKRNL_SemCreate(1);
    }

    if (g_movie_robin_sem)
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}

__s32 __movie_robin_sem_deinit(void)
{
    if (g_movie_robin_sem)
    {
        __u8 err;
        esKRNL_SemDel(g_movie_robin_sem, OS_DEL_ALWAYS, &err);
        g_movie_robin_sem = NULL;
    }

    return EPDK_OK;
}

__s32 __movie_robin_pend(void)
{
    __u8 err;
    esKRNL_SemPend(g_movie_robin_sem, 0, &err);
    return 0;
}

__s32 __movie_robin_accept(void)
{
    __u16 sem_nr;
    sem_nr = esKRNL_SemAccept(g_movie_robin_sem);
    return sem_nr;
}

__s32 __movie_robin_post(void)
{
    esKRNL_SemPost(g_movie_robin_sem);
    return 0;
}

__s32  movie_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
{
    H_WIN hparent;
    __gui_msg_t msg;
    hparent = GUI_WinGetParent(hwin);

    if (!hparent)
    {
        __err("hparent is null...");
        return EPDK_FAIL;
    }

    msg.h_deswin = hparent;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;
    GUI_SendNotifyMessage(&msg);
    return EPDK_OK;
}

__s32  movie_cmd2parent_syn(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
{
    H_WIN hparent;
    __gui_msg_t msg;
    hparent = GUI_WinGetParent(hwin);

    if (!hparent)
    {
        __err("hparent is null...");
        return EPDK_FAIL;
    }

    msg.h_deswin = hparent;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;
    GUI_SendMessage(&msg);
    return EPDK_OK;
}

__s32 __movie_set_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}

__s32 __movie_point_in_rect(__s32 x, __s32 y, RECT *rect
                            , __s32 left_margin, __s32 top_margin, __s32 right_margin,
                            __s32 bottom_margin)
{
    if (x > rect->x - left_margin
        && y > rect->y - top_margin
        && x < rect->x + rect->width + right_margin
        && y < rect->y + rect->height + bottom_margin)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

__u32 abs(__s32 v)
{
    if (v >= 0)
    {
        return v;
    }

    return -v;
}
