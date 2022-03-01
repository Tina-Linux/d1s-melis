/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : playsta.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include <log.h>
#include "play_sta.h"
#include "common/common.h"

typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;
    movie_playsta_t playsta;

    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
    __u8  hide_timmer_id;
} movie_playsta_scene_t;

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR __playsta_8bpp_layer_create(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_SCALER,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        0xff,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "sub menu layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = rect->width;
    fb.size.height      = rect->height;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe = pipe;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
        layer = NULL;
    }

    return layer;
}

static __s32 __playsta_init_ui(void)
{
    movie_playsta_uipara_t *ui_para;
    ui_para = movie_playsta_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_playsta_get_uipara fail...");
        return EPDK_FAIL;
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_playsta_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_PLAYSTA_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_id[j])
                {
                    ui_para->uipara_icon[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_icon[i].res_id[j]);

                    if (!ui_para->uipara_icon[i].res_hdl[j])
                    {
                        __msg("dsk_theme_open fail...");
                    }
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __playsta_deinit_ui(void)
{
    movie_playsta_uipara_t *ui_para;
    ui_para = movie_playsta_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_playsta_get_uipara fail...");
        return EPDK_FAIL;
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_playsta_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_PLAYSTA_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_hdl[j])
                {
                    dsk_theme_close(ui_para->uipara_icon[i].res_hdl[j]);
                    ui_para->uipara_icon[i].res_hdl[j] = NULL;
                }
            }
        }
    }

    return EPDK_OK;
}

static __s32 __playsta_update_playsta_ui(movie_playsta_scene_t *scene_para)
{
    movie_playsta_uipara_t *ui_para;
    ui_para = movie_playsta_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_playsta_get_uipara fail...");
        return EPDK_FAIL;
    }

    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!scene_para->hlyr)
    {
        __msg("scene_para->hlyr is null..., do not paint");
        return EPDK_FAIL;
    }

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
    {
        __msg("GUI_LYRWIN_STA_ON != scene_para->hlyr, do not paint...");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(scene_para->hlyr);
    GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_SLEEP);
    //com_memdev_create(scene_para->hlyr);
    GUI_SetBkColor(0x0);
    {
        GUI_RECT gui_rect;
        gui_rect.x0 = ui_para->uipara_icon[movie_playsta_icon_lplaysta].x;
        gui_rect.y0 = ui_para->uipara_icon[movie_playsta_icon_lplaysta].y;
        gui_rect.x1 = gui_rect.x0 + ui_para->uipara_icon[movie_playsta_icon_lplaysta].w;
        gui_rect.y1 = gui_rect.y0 + ui_para->uipara_icon[movie_playsta_icon_lplaysta].h;
        GUI_ClearRectEx(&gui_rect);
    }
    {
        void *pbmp;
        __s32 focus;
        __msg("scene_para->playsta=%d", scene_para->playsta);

        if (!ui_para->uipara_icon[movie_playsta_icon_lplaysta].res_hdl[scene_para->playsta])
        {
            __msg("ui_para->uipara_icon[movie_playsta_icon_lplaysta].res_hdl[scene_para->playsta] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[movie_playsta_icon_lplaysta].res_hdl[scene_para->playsta]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, ui_para->uipara_icon[movie_playsta_icon_lplaysta].x, ui_para->uipara_icon[movie_playsta_icon_lplaysta].y);
    }
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
    //com_memdev_delete();
    return EPDK_OK;
}

static __s32 __playsta_install_hide_timmer(movie_playsta_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    scene_para->hide_timmer_id = 0x12;

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                     , 400 * 2, NULL); //3秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __playsta_reset_hide_timmer(movie_playsta_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_ResetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                       , 400 * 2, NULL); //3秒
    }

    return EPDK_OK;
}

static __s32 __playsta_uninstall_hide_timmer(movie_playsta_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->hide_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __playsta_change_sta(movie_playsta_scene_t *scene_para)
{
    __s32 ret;

    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (movie_playsta_play == scene_para->playsta)
    {
        __playsta_uninstall_hide_timmer(scene_para);
        scene_para->playsta = movie_playsta_pause;
    }
    else if (movie_playsta_pause == scene_para->playsta)
    {
        __playsta_install_hide_timmer(scene_para);
        scene_para->playsta = movie_playsta_play;
    }
    else
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    ret = __playsta_update_playsta_ui(scene_para);
    return ret;
}

static __s32 __playsta_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            movie_playsta_scene_t *scene_para;
            //__msg("__playsta_proc GUI_MSG_CREATE begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
            __playsta_init_ui();
            __playsta_update_playsta_ui(scene_para);
            __playsta_install_hide_timmer(scene_para);
            //__msg("__playsta_proc GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            movie_playsta_scene_t *scene_para;
            //__msg("__playsta_proc GUI_MSG_DESTROY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            __playsta_uninstall_hide_timmer(scene_para);
            __playsta_deinit_ui();
            __msg("__playsta_proc GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            static __s32 last_key = -1;
            movie_playsta_scene_t *scene_para;
            //  __msg("__playsta_proc GUI_MSG_KEY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                if (GUI_MSG_KEY_ENTER == last_key)
                {
                    __playsta_reset_hide_timmer(scene_para);
                    __playsta_change_sta(scene_para);
                    movie_cmd2parent(msg->h_deswin, movie_playsta_scene_msg_clicked, scene_para->playsta, 0);
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

            __msg("__playsta_proc GUI_MSG_KEY end");
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            movie_playsta_scene_t *scene_para;
            //__msg("__playsta_proc GUI_MSG_TOUCH begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
            {
                __playsta_reset_hide_timmer(scene_para);
                __playsta_change_sta(scene_para);
                movie_cmd2parent(msg->h_deswin, movie_playsta_scene_msg_clicked, scene_para->playsta, 0);
            }

            __msg("__playsta_proc GUI_MSG_TOUCH end");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            movie_playsta_scene_t *scene_para;
            //          __msg("__playsta_proc GUI_MSG_PAINT begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            if (GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
            {
                __msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
                return EPDK_OK;
            }

            __playsta_update_playsta_ui(scene_para);
            //      __msg("__playsta_proc GUI_MSG_PAINT begin");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __s32 ret;
            movie_playsta_scene_t *scene_para;
            //          __msg("__playsta_proc GUI_MSG_TIMER begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);
            ret = -1;

            if (scene_para)
            {
                if (LOWORD(msg->dwAddData1) == scene_para->hide_timmer_id)
                {
                    if (movie_playsta_play == scene_para->playsta)
                    {
                        movie_cmd2parent(msg->h_deswin, movie_playsta_scene_msg_timeout, 0, 0);
                    }

                    ret = EPDK_OK;
                }
            }
            else
            {
                __err("scene_para is null...");
            }

            //  __msg("__playsta_proc GUI_MSG_TIMER end");

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
            //__msg("__playsta_proc GUI_MSG_CLOSE begin");
            movie_cmd2parent(msg->h_deswin, movie_playsta_scene_msg_timeout, 0, 0);
            __msg("__playsta_proc GUI_MSG_CLOSE end");
            break;
        }

        default:
        {
            break;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_playsta_scene_create(movie_playsta_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_playsta_scene_t *sence_para;
    movie_playsta_uipara_t *ui_para;
    RECT lyr_rect;
    sence_para = (movie_playsta_scene_t *)esMEMS_Malloc(0, sizeof(movie_playsta_scene_t));

    if (!sence_para)
    {
        __msg("mem not enough...");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_playsta_scene_t));
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->playsta = create_para->playsta;
    ui_para = movie_playsta_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("movie_spsc_get_uipara fail...");
        return NULL;
    }

    lyr_rect.x      = ui_para->uipara_lyr.x;
    lyr_rect.y      = ui_para->uipara_lyr.y;
    lyr_rect.width  = ui_para->uipara_lyr.w;
    lyr_rect.height = ui_para->uipara_lyr.h;
    sence_para->hlyr = __playsta_8bpp_layer_create(&lyr_rect, ui_para->pipe);

    if (NULL == sence_para->hlyr)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("sence_para->hlyr is null...");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          =   "movie_playsta_win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = sence_para->scene_id;
    framewin_para.hHosting      =  sence_para->hparent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__playsta_proc);
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
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("GUI_FrmWinCreate fail...");
        return NULL;
    }

    if (sence_para->hlyr)
    {
        GUI_LyrWinSetSta(sence_para->hlyr, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(sence_para->hlyr);
    }
    else
    {
        __err("invalid para...");
    }

    return sence_para;
}

__s32 movie_playsta_scene_delete(void *handle)
{
    movie_playsta_scene_t *scene_para;
    scene_para = (movie_playsta_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_FrmWinDelete(scene_para->hfrm);

    if (!scene_para->hlyr)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_LyrWinDelete(scene_para->hlyr);
    eLIBs_memset(scene_para, 0, sizeof(movie_playsta_scene_t));//防止重复释放
    esMEMS_Mfree(0, scene_para);
    return EPDK_OK;
}

__s32 movie_playsta_scene_set_focus(void *handle)
{
    movie_playsta_scene_t *scene_para;
    scene_para = (movie_playsta_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_WinSetFocusChild(scene_para->hfrm);
    return EPDK_OK;
}

unsigned long movie_playsta_scene_get_hwnd(void *handle)
{
    movie_playsta_scene_t *scene_para;
    scene_para = (movie_playsta_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return NULL;
    }

    return (unsigned long)scene_para->hfrm;
}

__s32 movie_playsta_scene_change_sta(void *handle)
{
    movie_playsta_scene_t *scene_para;
    scene_para = (movie_playsta_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    __playsta_change_sta(scene_para);
    return EPDK_OK;
}


__s32 movie_playsta_scene_get_lyr_scn(RECT *rect)
{
    movie_playsta_uipara_t *ui_para;
    ui_para = movie_playsta_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return NULL;
    }

    rect->x = ui_para->uipara_lyr.x;
    rect->y = ui_para->uipara_lyr.y;
    rect->width = ui_para->uipara_lyr.w;
    rect->height = ui_para->uipara_lyr.h;
    return EPDK_OK;
}

