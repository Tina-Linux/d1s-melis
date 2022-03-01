/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : prog_bar.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include <log.h>
#include "apps.h"
#include "prog_bar_ui.h"
#include "prog_bar.h"
#include "init/init.h"
#include "init/headbar/gscene_headbar.h"

typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;
    __s32 min_val;
    __s32 max_val;
    __s32 cur_val;

    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
    __u8  hide_timmer_id;
    __bool bcreate_finish;//GUI有问题，用来记录是否响应create完毕
} init_prog_bar_scene_t;

static __s32  __init_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved)
{
    H_WIN hparent;
    __gui_msg_t msg;
    hparent = GUI_WinGetParent(hwin);

    if (!hparent)
    {
        __err("hparent is null...\n");
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


static __s32 __prog_bar_init_ui(void)
{
    init_prog_bar_uipara_t *ui_para;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("init_prog_bar_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        __s32 i;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_vol_bg.res_id[i])
            {
                ui_para->uipara_vol_bg.res_hdl[i] = dsk_theme_open(ui_para->uipara_vol_bg.res_id[i]);

                if (!ui_para->uipara_vol_bg.res_hdl[i])
                {
                    __msg("dsk_theme_open fail...\n");
                }
            }
        }
    }

    {
        __s32 i;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_bright_bg.res_id[i])
            {
                ui_para->uipara_bright_bg.res_hdl[i] = dsk_theme_open(ui_para->uipara_bright_bg.res_id[i]);

                if (!ui_para->uipara_bright_bg.res_hdl[i])
                {
                    __msg("dsk_theme_open fail...\n");
                }
            }
        }
    }

    {
        __s32 i;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_prog_bar_bg.res_id[i])
            {
                ui_para->uipara_prog_bar_bg.res_hdl[i] = dsk_theme_open(ui_para->uipara_prog_bar_bg.res_id[i]);

                if (!ui_para->uipara_prog_bar_bg.res_hdl[i])
                {
                    __msg("dsk_theme_open fail...\n");
                }
            }
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < init_prog_bar_icon_num ; i++)
        {
            for (j = 0 ; j < INIT_PROG_BAR_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_id[j])
                {
                    ui_para->uipara_icon[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_icon[i].res_id[j]);

                    if (!ui_para->uipara_icon[i].res_hdl[j])
                    {
                        __msg("dsk_theme_open fail...\n");
                    }
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __prog_bar_deinit_ui(void)
{
    init_prog_bar_uipara_t *ui_para;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("init_prog_bar_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    {
        __s32 i;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_bright_bg.res_hdl[i])
            {
                dsk_theme_close(ui_para->uipara_bright_bg.res_hdl[i]);
                ui_para->uipara_bright_bg.res_hdl[i] = 0;
            }
        }
    }

    {
        __s32 i;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_prog_bar_bg.res_hdl[i])
            {
                dsk_theme_close(ui_para->uipara_prog_bar_bg.res_hdl[i]);
                ui_para->uipara_prog_bar_bg.res_hdl[i] = 0;
            }
        }
    }

    {
        __s32 i;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_prog_bar_bg.res_hdl[i])
            {
                dsk_theme_close(ui_para->uipara_prog_bar_bg.res_hdl[i]);
                ui_para->uipara_prog_bar_bg.res_hdl[i] = 0;
            }
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < init_prog_bar_icon_num ; i++)
        {
            for (j = 0 ; j < INIT_PROG_BAR_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_hdl[j])
                {
                    dsk_theme_close(ui_para->uipara_icon[i].res_hdl[j]);
                    ui_para->uipara_icon[i].res_hdl[j] = 0;
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __prog_bar_draw_bg(init_prog_bar_scene_t *scene_para)
{
    init_prog_bar_uipara_t *ui_para;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("init_prog_bar_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hlyr)
    {
        __msg("scene_para->hlyr is null..., do not paint\n");
        return EPDK_FAIL;
    }

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
    {
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(scene_para->hlyr);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    {
        __s32 i;
        void *pbmp;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_prog_bar_bg.res_hdl[i])
            {
                pbmp = dsk_theme_hdl2buf(ui_para->uipara_prog_bar_bg.res_hdl[i]);

                if (!pbmp)
                {
                    __msg("pbmp is null...\n");
                    return EPDK_FAIL;
                }
                else
                {
                    GUI_ARGB_Draw(pbmp, ui_para->uipara_prog_bar_bg.x, ui_para->uipara_prog_bar_bg.y);
                }
            }
        }
    }
    return EPDK_OK;
}

static __s32 volume_draw_bg(init_prog_bar_scene_t *scene_para)
{
    init_prog_bar_uipara_t *ui_para;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());
    GUI_LyrWinSel(scene_para->hlyr);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    {
        __s32 i;
        void *pbmp;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_vol_bg.res_hdl[i])
            {
                pbmp = dsk_theme_hdl2buf(ui_para->uipara_vol_bg.res_hdl[i]);

                if (!pbmp)
                {
                    __msg("pbmp is null...\n");
                    return EPDK_FAIL;
                }
                else
                {
                    GUI_ARGB_Draw(pbmp, ui_para->uipara_vol_bg.x, ui_para->uipara_vol_bg.y);
                }
            }
        }
    }
    return EPDK_OK;
}


static __s32 bright_draw_bg(init_prog_bar_scene_t *scene_para)
{
    init_prog_bar_uipara_t *ui_para;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());
    GUI_LyrWinSel(scene_para->hlyr);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    {
        __s32 i;
        void *pbmp;

        for (i = 0 ; i < INIT_PROG_BAR_MAX_ICON_RES_NUM ; i++)
        {
            if (ui_para->uipara_bright_bg.res_hdl[i])
            {
                pbmp = dsk_theme_hdl2buf(ui_para->uipara_bright_bg.res_hdl[i]);

                if (!pbmp)
                {
                    __msg("pbmp is null...\n");
                    return EPDK_FAIL;
                }
                else
                {
                    GUI_ARGB_Draw(pbmp, ui_para->uipara_bright_bg.x, ui_para->uipara_bright_bg.y);
                }
            }
        }
    }
    return EPDK_OK;
}
static __s32 __prog_bar_update_prog_bar_ui(init_prog_bar_scene_t *scene_para)
{
    __s32 ret ;
    init_prog_bar_uipara_t *ui_para;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());
    ret = EPDK_OK;

    if (NULL == ui_para)
    {
        __msg("init_prog_bar_get_uipara fail...\n");
        return EPDK_FAIL;
    }

    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hlyr)
    {
        __msg("scene_para->hlyr is null..., do not paint\n");
        return EPDK_FAIL;
    }

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
    {
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(scene_para->hlyr);
    init_memdev_create(scene_para->hlyr);
    //  draw_mem = GUI_MEMDEV_Create(ui_para->uipara_icon[init_prog_bar_icon_prog_bg].x,
    //                               ui_para->uipara_icon[init_prog_bar_icon_prog_bg].y,
    //                               ui_para->uipara_icon[init_prog_bar_icon_prog_bg].w,
    //                               ui_para->uipara_icon[init_prog_bar_icon_prog_bg].h);
    //draw_mem = GUI_MEMDEV_Create(0, 0, 800, 480);
    //  __msg("draw_mem = %x\n ", draw_mem);
    //  GUI_MEMDEV_Select(draw_mem);
    //画进度条背景
    {
        void *pbmp;

        if (!ui_para->uipara_icon[init_prog_bar_icon_prog_bg].res_hdl[0])
        {
            __msg("ui_para->uipara_icon[init_prog_bar_icon_prog_bg].res_hdl[0] is null...\n");
            ret = EPDK_FAIL;
            goto end;
        }

        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[init_prog_bar_icon_prog_bg].res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            ret = EPDK_FAIL;
            goto end;
        }

        GUI_ARGB_Draw(pbmp, ui_para->uipara_icon[init_prog_bar_icon_prog_bg].x, ui_para->uipara_icon[init_prog_bar_icon_prog_bg].y);
    }
    //  GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //  GUI_MEMDEV_CopyToLCD(draw_mem);;
    //  GUI_MEMDEV_Select( NULL );
    //  GUI_MEMDEV_Delete(draw_mem);
    //  draw_mem = NULL;
    //画进度条左边的图标
    {
        void *pbmp;
        __s32 focus;
        focus = 0;

        if (!ui_para->uipara_icon[init_prog_bar_icon_lprog_bar].res_hdl[focus])
        {
            __msg("ui_para->uipara_icon[init_prog_bar_icon_lprog_bar].res_hdl[focus] is null...\n");
            ret = EPDK_FAIL;
            goto end;
        }

        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[init_prog_bar_icon_lprog_bar].res_hdl[focus]);

        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            ret = EPDK_FAIL;
            goto end;
        }

        GUI_ARGB_Draw(pbmp, ui_para->uipara_icon[init_prog_bar_icon_lprog_bar].x, ui_para->uipara_icon[init_prog_bar_icon_lprog_bar].y);
    }

    //画进度条和cursor
    if (scene_para->cur_val >= scene_para->min_val
        && scene_para->cur_val <= scene_para->max_val)
    {
        void *pbmp;

        if (!ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].res_hdl[0])
        {
            __msg("ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].res_hdl[0] is null...\n");
            ret = EPDK_FAIL;
            goto end;
        }

        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...\n");
            ret = EPDK_FAIL;
            goto end;
        }

        {
            __s32 i;
            __s32 n;
            __s32 mid_w;
            __s32 max_mid_w;
            __s32 cursor_w;
            __s32 cursor_pos;
            __s32 bg_pos;
            __s32 bg_w;

            if (scene_para->cur_val == scene_para->min_val)
            {
                n = 0;
            }
            else
            {
                n = (ui_para->uipara_icon[init_prog_bar_icon_prog_bg].w - ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].w) * (scene_para->cur_val - scene_para->min_val)
                    / ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].w / (scene_para->max_val - scene_para->min_val);
            }

            mid_w = ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].w;
            max_mid_w = ui_para->uipara_icon[init_prog_bar_icon_prog_bg].w - ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].w;
            cursor_w = ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].w;
            bg_pos = ui_para->uipara_icon[init_prog_bar_icon_prog_bg].x;
            bg_w = ui_para->uipara_icon[init_prog_bar_icon_prog_bg].w;

            //防止非整数倍时画不满
            if (max_mid_w - n * mid_w < mid_w)
            {
                n++;
                cursor_pos = bg_pos + bg_w - cursor_w;
            }
            else
            {
                cursor_pos = bg_pos + n * mid_w;
            }

            //draw_mem = GUI_MEMDEV_Create(ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].x,
            //                           ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].y,
            //                           ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].w*i,
            //                           ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].h);
            //__msg("draw_mem = %x\n", draw_mem);
            //GUI_MEMDEV_Select(draw_mem);
            for (i = 1 ; i < n ; i++)
            {
                GUI_ARGB_Draw(pbmp, ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].x + i * ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].w
                              , ui_para->uipara_icon[init_prog_bar_icon_mprog_bar].y);
            }

            //GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            //GUI_MEMDEV_CopyToLCD(draw_mem);;
            //GUI_MEMDEV_Select( NULL );
            //GUI_MEMDEV_Delete(draw_mem);
            //draw_mem = NULL;
            //画cursor
            if (!ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].res_hdl[0])
            {
                __msg("ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].res_hdl[0] is null...\n");
                ret = EPDK_FAIL;
                goto end;
            }

            pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].res_hdl[0]);

            if (!pbmp)
            {
                __msg("pbmp is null...\n");
                ret = EPDK_FAIL;
                goto end;
            }

            GUI_ARGB_Draw(pbmp, cursor_pos, ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].y);
        }
    }

end:
    init_memdev_delete();
    return ret;
}

static __s32 __prog_bar_install_hide_timmer(init_prog_bar_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    scene_para->hide_timmer_id = 0x57;

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                     , 400 * 2, NULL); //2秒
    }
    else
    {
        __msg("timmer already install...\n");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __prog_bar_reset_hide_timmer(init_prog_bar_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_ResetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                       , 400 * 2, NULL); //2秒
    }

    return EPDK_OK;
}

static __s32 __prog_bar_uninstall_hide_timmer(init_prog_bar_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->hide_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __prog_bar_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            init_prog_bar_scene_t *scene_para;
            init_prog_bar_uipara_t *ui_para;
            __msg("__prog_bar_proc GUI_MSG_CREATE begin\n");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...\n");
                return EPDK_FAIL;
            }

            scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
            __prog_bar_init_ui();
            //com_memdev_create
            ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());
            GUI_LyrWinSel(scene_para->hlyr);
            //draw_mem = GUI_MEMDEV_Create(ui_para->uipara_vol_bg.x, ui_para->uipara_vol_bg.y,ui_para->uipara_vol_bg.w, ui_para->uipara_vol_bg.h);
            //ASSERT(draw_mem);
            //GUI_MEMDEV_Select(draw_mem);
            //GUI_LyrWinCacheOff();
            //  com_memdev_delete();
            //GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            //GUI_MEMDEV_CopyToLCD(draw_mem);;
            //GUI_MEMDEV_Select( NULL );
            //GUI_MEMDEV_Delete(draw_mem);
            //draw_mem = NULL;*/
            //__here__;
            init_memdev_create(scene_para->hlyr);

            if (scene_para->scene_id == 0x02)
            {
                //__here__;
                volume_draw_bg(scene_para);
                //__here__;
            }
            else if (scene_para->scene_id == 0x04)
            {
                //__here__;
                bright_draw_bg(scene_para);
                //__here__;
            }
            else
            {
                //__here__;
            }

            __prog_bar_draw_bg(scene_para);
            init_memdev_delete();
            //__here__;
            __prog_bar_update_prog_bar_ui(scene_para);
            __prog_bar_install_hide_timmer(scene_para);
            GUI_WinSetCaptureWin(msg->h_deswin);
            scene_para->bcreate_finish = 1;
            __msg("__prog_bar_proc GUI_MSG_CREATE end\n");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            init_prog_bar_scene_t *scene_para;
            RECT rect1, rect2;
            rect2.x = 0;
            rect2.y = 0;
            rect2.width = 800;
            rect2.height = 46;
            __msg("__prog_bar_proc GUI_MSG_DESTROY begin\n");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...\n");
                return EPDK_FAIL;
            }

            gscene_hbar_set_screen_rect(&rect2);
            __prog_bar_uninstall_hide_timmer(scene_para);
            __prog_bar_deinit_ui();

            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }

            __msg("__prog_bar_proc GUI_MSG_DESTROY end\n");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            static __s32 last_key = -1;
            init_prog_bar_scene_t *scene_para;
            __msg("__prog_bar_proc GUI_MSG_KEY begin\n");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...\n");
                return EPDK_FAIL;
            }

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                __prog_bar_reset_hide_timmer(scene_para);

                if (GUI_MSG_KEY_LEFT == last_key
                    /*||GUI_MSG_KEY_VDEC == last_key*/)
                {
                    if (scene_para->cur_val > scene_para->min_val)
                    {
                        scene_para->cur_val--;
                        __prog_bar_update_prog_bar_ui(scene_para);
                        __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_prog_barset, scene_para->cur_val, 0);
                    }
                }
                else if (GUI_MSG_KEY_RIGHT == last_key
                         /*||GUI_MSG_KEY_VADD == last_key*/)
                {
                    __prog_bar_reset_hide_timmer(scene_para);

                    if (scene_para->cur_val < scene_para->max_val)
                    {
                        scene_para->cur_val++;
                        __prog_bar_update_prog_bar_ui(scene_para);
                        __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_prog_barset, scene_para->cur_val, 0);
                    }
                }
                else if (GUI_MSG_KEY_ESCAPE == last_key
                         || GUI_MSG_KEY_MENU == last_key)
                {
                    __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_timeout, scene_para->cur_val, 0);
                }
            }
            else
            {
                switch (msg->dwAddData1)
                {
                    case GUI_MSG_KEY_LONGLEFT:
                        //case GUI_MSG_KEY_LONGVDEC:
                    {
                        __prog_bar_reset_hide_timmer(scene_para);

                        if (scene_para->cur_val > scene_para->min_val)
                        {
                            scene_para->cur_val--;
                            __prog_bar_update_prog_bar_ui(scene_para);
                            __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_prog_barset, scene_para->cur_val, 0);
                        }

                        break;
                    }

                    case GUI_MSG_KEY_LONGRIGHT:
                        //case GUI_MSG_KEY_LONGVADD:
                    {
                        __prog_bar_reset_hide_timmer(scene_para);

                        if (scene_para->cur_val < scene_para->max_val)
                        {
                            scene_para->cur_val++;
                            __prog_bar_update_prog_bar_ui(scene_para);
                            __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_prog_barset, scene_para->cur_val, 0);
                        }

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                last_key = -1;
            }
            else
            {
                last_key = msg->dwAddData1;
            }

            __msg("__prog_bar_proc GUI_MSG_KEY end\n");
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            init_prog_bar_scene_t *scene_para;
            init_prog_bar_uipara_t *ui_para;
            __s32 x, y;
            __s32 max_mid_w;
            __s32 bg_pos;
            __s32 bg_w;
            __s32 val;
            static __bool touch_down_is_valid = 0;
            __msg("__prog_bar_proc GUI_MSG_TOUCH begin\n");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...\n");
                return EPDK_FAIL;
            }

            ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());

            if (NULL == ui_para)
            {
                __msg("init_prog_bar_get_uipara fail...\n");
                return EPDK_FAIL;
            }

            bg_w = ui_para->uipara_icon[init_prog_bar_icon_prog_bg].w;
            max_mid_w = ui_para->uipara_icon[init_prog_bar_icon_prog_bg].w - ui_para->uipara_icon[init_prog_bar_icon_prog_cursor].w;
            bg_pos = ui_para->uipara_icon[init_prog_bar_icon_prog_bg].x;
            x = LOSWORD(msg->dwAddData2);
            y = HISWORD(msg->dwAddData2);

            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_UP:
                case GUI_MSG_TOUCH_NCUP:
                case GUI_MSG_TOUCH_OVERUP:
                {
                    if (!touch_down_is_valid)
                    {
                        if (x < -5 || x > bg_w + 5
                            || y < 0 || y > 32)
                        {
                            __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_timeout, scene_para->cur_val, 0);
                            return EPDK_OK;
                        }
                    }

                    touch_down_is_valid = 0;
                    break;
                }

                case GUI_MSG_TOUCH_DOWN:
                case GUI_MSG_TOUCH_MOVE:
                case GUI_MSG_TOUCH_LONGDOWN:
                {
                    if (x >= 0 && x <= bg_w
                        && y >= 0 && y < 32
                        && (GUI_MSG_TOUCH_DOWN == msg->dwAddData1 || GUI_MSG_TOUCH_LONGDOWN == msg->dwAddData1))
                    {
                        touch_down_is_valid = 1;
                    }

                    __prog_bar_reset_hide_timmer(scene_para);

                    if (x <= 5)
                    {
                        val = scene_para->min_val;
                    }
                    else if (x >= max_mid_w - 5)
                    {
                        val = scene_para->max_val;
                    }
                    else
                    {
                        val = (x) * (scene_para->max_val - scene_para->min_val) / max_mid_w;
                    }

                    if (val < scene_para->min_val)
                    {
                        val = scene_para->min_val;
                    }

                    if (val > scene_para->max_val)
                    {
                        val = scene_para->max_val;
                    }

                    if (val != scene_para->cur_val
                        && 1 == touch_down_is_valid)
                    {
                        scene_para->cur_val = val;
                        __prog_bar_update_prog_bar_ui(scene_para);
                        __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_prog_barset, scene_para->cur_val, 0);
                    }

                    break;
                }

                default:
                {
                    __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_timeout, scene_para->cur_val, 0);
                    touch_down_is_valid = 0;
                    break;
                }
            }

            __msg("__prog_bar_proc GUI_MSG_TOUCH end\n");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            init_prog_bar_scene_t *scene_para;
            __msg("__prog_bar_proc GUI_MSG_PAINT begin\n");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...\n");
                return EPDK_FAIL;
            }

            if (!scene_para->bcreate_finish)
            {
                return EPDK_FAIL;
            }

            if (GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
            {
                __msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...\n");
                return EPDK_OK;
            }

            //__here__;
            init_memdev_create(scene_para->hlyr);
            __prog_bar_draw_bg(scene_para);
            init_memdev_delete();
            //__here__;
            __prog_bar_update_prog_bar_ui(scene_para);
            __msg("__prog_bar_proc GUI_MSG_PAINT begin\n");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __s32 ret;
            init_prog_bar_scene_t *scene_para;
            __msg("__prog_bar_proc GUI_MSG_TIMER begin\n");
            scene_para = GUI_WinGetAttr(msg->h_deswin);
            ret = -1;

            if (scene_para)
            {
                if (LOWORD(msg->dwAddData1) == scene_para->hide_timmer_id)
                {
                    __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_timeout, 0, 0);
                    ret = EPDK_OK;
                }
            }
            else
            {
                __err("scene_para is null...\n");
            }

            __msg("__prog_bar_proc GUI_MSG_TIMER end\n");

            if (-1 == ret)//未处理，交给子场景处理
            {
                break;
            }
            else//已处理
            {
                return EPDK_OK;
            }
        }

        case GUI_MSG_CLOSE:
        {
            __msg("__prog_bar_proc GUI_MSG_CLOSE begin\n");
            __init_cmd2parent(msg->h_deswin, init_prog_bar_scene_msg_timeout, 0, 0);
            __msg("__prog_bar_proc GUI_MSG_CLOSE end\n");
            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}

__s32 init_prog_bar_scene_update_data(void *handle, init_prog_bar_create_para_t *create_para)
{
    __s32 ret;
    init_prog_bar_scene_t *scene_para;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    if (!create_para)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    scene_para = (init_prog_bar_scene_t *)handle;

    if (!scene_para->hfrm)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    scene_para->min_val = create_para->min_val;
    scene_para->cur_val = create_para->cur_val;
    scene_para->max_val = create_para->max_val;
    ret = __prog_bar_update_prog_bar_ui(scene_para);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    ret = __prog_bar_reset_hide_timmer(scene_para);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

void *init_prog_bar_scene_create(init_prog_bar_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    init_prog_bar_scene_t *sence_para;
    init_prog_bar_uipara_t *ui_para;
    RECT lyr_rect;
    FB fb;
    sence_para = (init_prog_bar_scene_t *)esMEMS_Balloc(sizeof(init_prog_bar_scene_t));

    if (!sence_para)
    {
        __msg("mem not enough...\n");
        return NULL;
    }

    //__here__;
    ui_para = init_prog_bar_get_uipara(GUI_GetScnDir());

    if (!ui_para)
    {
        __msg("invalid para...\n");
        return NULL;
    }

    //__msg("ui_para->uipara_frm.x = %d\n", ui_para->uipara_frm.x);
    //__msg("ui_para->uipara_frm.y = %d\n", ui_para->uipara_frm.y);
    //__msg("ui_para->uipara_frm.width = %d\n", ui_para->uipara_frm.w);
    //__msg("ui_para->uipara_frm.height = %d\n", ui_para->uipara_frm.h);
    eLIBs_memset(sence_para, 0, sizeof(init_prog_bar_scene_t));
    GUI_LyrWinGetFB(create_para->hlyr, &fb);
    //__msg("fb.size.height =%d\n",fb.size.height );
    //__msg("fb.size.width  =%d\n",fb.size.width );
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->max_val = create_para->max_val;
    sence_para->min_val = create_para->min_val;
    sence_para->cur_val = create_para->cur_val;
    sence_para->hlyr    = create_para->hlyr;
    sence_para->bcreate_finish = EPDK_FALSE;
    //__here__;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          =   "init_prog_bar_win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = sence_para->scene_id;
    framewin_para.hHosting      =  sence_para->hparent;
    framewin_para.FrameWinProc  = __prog_bar_proc;
    framewin_para.rect.x        = ui_para->uipara_frm.x;
    framewin_para.rect.y        = ui_para->uipara_frm.y;
    framewin_para.rect.width    = ui_para->uipara_frm.w;
    framewin_para.rect.height   = ui_para->uipara_frm.h;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)sence_para;
    framewin_para.hLayer        = sence_para->hlyr;
    sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);

    if (NULL == sence_para->hfrm)
    {
        esMEMS_Bfree(sence_para, sizeof(init_prog_bar_scene_t));
        sence_para = NULL;
        __msg("GUI_FrmWinCreate fail...\n");
        return NULL;
    }

    return sence_para;
}

__s32 init_prog_bar_scene_delete(void *handle)
{
    init_prog_bar_scene_t *scene_para;
    scene_para = (init_prog_bar_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    GUI_FrmWinDelete(scene_para->hfrm);
    eLIBs_memset(scene_para, 0, sizeof(init_prog_bar_scene_t));//防止重复释放
    esMEMS_Bfree(scene_para, sizeof(init_prog_bar_scene_t));
    return EPDK_OK;
}

__s32 init_prog_bar_scene_set_focus(void *handle)
{
    init_prog_bar_scene_t *scene_para;
    scene_para = (init_prog_bar_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..\n");
        return EPDK_FAIL;
    }

    GUI_WinSetFocusChild(scene_para->hfrm);
    return EPDK_OK;
}

__s32 init_prog_bar_scene_get_id(void *handle)
{
    init_prog_bar_scene_t *scene_para;
    scene_para = (init_prog_bar_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..\n");
        return 0;
    }

    return scene_para->scene_id;
}


