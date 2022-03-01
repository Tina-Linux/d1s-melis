/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : app_movie.c
* By        : Bayden.chen
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2011-05-05   Bayden.chen  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include <emodules/mod_display.h>
#include "app_movie_i.h"
#include "dfs_posix.h"

typedef struct
{
    //ctrl scene
    void *spsc_scene;//manwin

    void *playsta_scene;//frmwin

    __s32 sence_flag;   //0为set 1 为spsc
    void *ff_rr_show_scene;//frmwin
    movie_ff_rr_show_interval_t *ff_rr_show_interval_uipara;
    void *sub_show_scene;//frmwin
    __u32 sub_state;
    __u32 sub_pos;
    __u32 sub_color;
    __u32 sub_size;
    __u32 sub_stream;//0代表国语 1代表粤语

    void *prog_scene;//frmwin

    H_WIN sset_sence;//sset子场景
    __sset_create_para_t *sset_create_para;//sset创建参数
    __u32 channel;//左右声道\立体声

    H_WIN breakplay_scene;//该值直接就是frmwin句柄
    H_WIN playerror_scene;//该值直接就是frmwin句柄
    H_WIN playend_scene;//该值直接就是frmwin句柄
    H_WIN h_frm_alarm_coming ;  //闹钟定时时间到的时候弹出的对话框的窗口句柄
    H_WIN movie_manager;//movie的manwin

    __mp *hdisp;//显示驱动句柄
    __hdle   hvedio_lyr;//视频图层

    __u8     auto_play_timmer_id;
    __u8     set_cb_timmer_id;
    __u8     auto_ffrr_timmer_id;
    __u32    ff_rr_cnt;

    __bool   exit_movie;//是否退出movie
    __s32    err_cnt;//播放错误的文件个数
    __s32    rr_flag;//是否进行了快退操作，用于快退到尽头时，做判断用
    __u32   current_npl_index ; //当前播放的npl 索引
    __movie_create_mode_e mode;//视频模式
    __movie_ab_mode_e    ab_mode;


    //数字选取===================================
    __s32  ndigit_movie_num; //数字选取序号
    __u8   digit_timmer_id;  //数字选取timmer id号
    H_LYR  hdigit_layer;     //显示当前数字的图层
    __u8   auto_del_hbar_timmer_id;  //自动删除顶栏
    //=============================================


    __s32    root_type;

} movie_ctrl_t;

static movie_ctrl_t *g_movie_ctrl = NULL;
static __s32 is_touch_ff_rr_valid = 0;//代表判断当前是否可以进行触摸快进快退

static __s32 __movie_playend_scene_create(movie_ctrl_t *movie_ctrl);
static __s32 __movie_has_sub_scene(movie_ctrl_t *movie_ctrl, __u32 sub_id);

//删除movie的子场景
static __s32 __movie_delete_subscene_by_id(movie_ctrl_t *movie_ctrl, __u32 sub_id);

static __s32 __movie_playerror_scene_create(movie_ctrl_t *movie_ctrl);

//是否有除字幕以外的子场景
static __bool __app_has_visuable_sub_scene_except_x(movie_ctrl_t *movie_ctrl, __u32 sub_scene);


static __s32 __app_movie_show_dialog(H_WIN movie_manager, __s32 win_id, __s32  item_id);
static __s32 __movie_tvout_tp_is_in_ellipse(__s32 a, __s32 b, __s32 ox, __s32 oy,
        __s32 x, __s32 y);
static __s32 __movie_ff_rr_show_scene_delete(movie_ctrl_t *movie_ctrl);
static __s32 __movie_tvout_tp_is_in_ellipse(__s32 a, __s32 b, __s32 ox, __s32 oy,
        __s32 x, __s32 y);
static __s32 __movie_ff_rr_show_scene_delete(movie_ctrl_t *movie_ctrl);
static __s32 __movie_playerror_scene_delete(movie_ctrl_t *movie_ctrl);
static __s32 __movie_prog_scene_delete(movie_ctrl_t *movie_ctrl);
static __s32 __movie_sub_show_scene_create(movie_ctrl_t *movie_ctrl);



#define GUI_MSG_SET_TVOUT_TP_HOOK_MSG (GUI_MSG_USER_DEF+1)


static H_WIN h_infor;

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR movie_8bpp_layer_create(RECT *rect, __s32 pipe)
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
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
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
        __err("GUI_LyrWinCreate fail... !");
    }

    return layer;
}
__s32  movie_cmd2sset(H_WIN hwin, __s32 id, __s32 data1, __s32 data2, __s32 reserved)
{
    __gui_msg_t msg;
    msg.h_deswin = hwin;
    msg.h_srcwin = NULL;
    msg.id = id;
    msg.dwAddData1 = data1;
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;
    GUI_SendNotifyMessage(&msg);
    return EPDK_OK;
}
__s32  movie_cmd2sset_syn(H_WIN hwin, __s32 id, __s32 data1, __s32 data2, __s32 reserved)
{
    __gui_msg_t msg;
    msg.h_deswin = hwin;
    msg.h_srcwin = NULL;
    msg.id = id;
    msg.dwAddData1 = data1;
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;
    GUI_SendMessage(&msg);
    return EPDK_OK;
}

static __bool is_on_tv(void)
{
    __u32 output;
    output = dsk_display_get_output_type();

    if (DISP_OUTPUT_TYPE_LCD == output
        || DISP_OUTPUT_TYPE_NONE == output)
    {
        __wrn("not on tv now...");
        return EPDK_FALSE;
    }
    else
    {
        __wrn("on tv now...");
        return EPDK_TRUE;
    }
}


/////////////////////////////////////////////////////chengf  add
__s32 app_movie_get_root_type(H_WIN movie_man_win, __u32 *root_type)
{
    movie_ctrl_t   *movie_ctrl = NULL;

    if (movie_man_win == NULL)
    {
        return EPDK_FAIL;
    }

    movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(movie_man_win);

    if (!movie_ctrl)
    {
        __wrn("*************movie_ctrl is null...");
        return EPDK_FAIL;
    }

    if (root_type)
    {
        *root_type = movie_ctrl->root_type;
    }

    return EPDK_OK;
}
/////////////////////////////////////////////////


static void __app_movie_set_play_mode(void)
{
    reg_movie_para_t *para = NULL;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __log("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __log("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para != NULL)
    {
        robin_set_play_mode(para->rotate_mode);
        __log("para->rotate_mode=%d", para->rotate_mode);
    }
    else
    {
        robin_set_play_mode(RAT_PLAY_MODE_ROTATE_ALL);
        __log("para->rotate_mode=%d", RAT_PLAY_MODE_ROTATE_ALL);
    }
}

static void __app_movie_set_zoom_mode(void)
{
    reg_movie_para_t *para = NULL;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __log("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __log("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para != NULL)
    {
        robin_set_zoom(para->zoom_mode);
        __log("para->zoom_mode=%d", para->zoom_mode);
    }
}

static __u32 __app_movie_get_sub_show_state(void)
{
    reg_movie_para_t *para;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
        __msg("para->channel = %d", para->channel);
        __msg("para->rotate_mode = %d", para->rotate_mode);
        __msg("para->sub_color = %x", para->sub_color);
        __msg("para->sub_pos = %d", para->sub_pos);
        __msg("para->sub_state = %d", para->sub_state);
        __msg("para->sub_stream = %d", para->sub_stream);
        __msg("para->zoom_mode = %d", para->zoom_mode);
    }

    if (para)
    {
        __msg("para->sub_state=%d", para->sub_state);
        return para->sub_state;
    }
    else
    {
        return 1;
    }
}

static __u32 __app_movie_get_sub_stream_index(void)
{
    reg_movie_para_t *para;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para)
    {
        __msg("para->sub_stream=%d", para->sub_stream);
        return para->sub_stream;
    }
    else
    {
        return 1;
    }
}

static __u32 __app_movie_get_channel(void)
{
    reg_movie_para_t *para;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para)
    {
        __msg("para->sub_stream=%d", para->channel);
        return para->channel;
    }
    else
    {
        return 0;
    }
}


//=====================chengf3  add  begin
static __u32 __app_movie_get_track(void)
{
    reg_movie_para_t *para;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __wrn("********* load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __wrn("========= load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para)
    {
        __wrn("========= load para->track = %d", para->track);
        return para->track;
    }
    else
    {
        __wrn("********* invalid para");
        return 0;
    }
}


static void __movie_save_track(__s32 track)
{
    reg_movie_para_t *para;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (para)
    {
        para->track = track;
        __wrn("====== save track = %d", para->track);
    }
}

//=====================chengf3  add  end


static __u32 __app_movie_get_sub_show_pos(void)
{
    reg_movie_para_t *para;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para)
    {
        __msg("para->sub_pos=%d", para->sub_pos);
        return para->sub_pos;
    }
    else
    {
        return 0;
    }
}

static __u32 __app_movie_get_sub_show_color(void)
{
    reg_movie_para_t *para;

    if (dsk_reg_get_app_restore_flag(REG_APP_MOVIE))
    {
        __msg("load default setting...");
        para = (reg_movie_para_t *)dsk_reg_get_default_para_by_app(REG_APP_MOVIE);
    }
    else
    {
        __msg("load current setting...");
        para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);
    }

    if (para)
    {
        __msg("para->sub_color=%x", para->sub_color);
        return para->sub_color;
    }
    else
    {
        return GUI_WHITE;
    }
}

static void __movie_save_play_mode(__s32 play_mode)
{
    reg_movie_para_t *para;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (para)
    {
        para->rotate_mode = play_mode;
        __msg("para->rotate_mode=%d", para->rotate_mode);
    }
}

static void __movie_save_zoom_mode(__s32 zoom_mode)
{
    reg_movie_para_t *para;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (para)
    {
        para->zoom_mode = zoom_mode;
        __msg("para->zoom_mode=%d", para->zoom_mode);
    }
}

static void __movie_save_channel(__s32 channel)
{
    reg_movie_para_t *para;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (para)
    {
        para->channel = channel;
    }
}
static void __movie_save_sub_pos(__s32 sub_pos)
{
    reg_movie_para_t *para = NULL;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (para != NULL)
    {
        para->sub_pos = sub_pos;
    }
}
static void app_movie_is_movie_pause(movie_ctrl_t *movie_ctrl)
{
    __cedar_status_t fsm_sta;
    __msg(" MOVIE_SSET_NC_PLAY");
    fsm_sta = robin_get_fsm_status();

    if (CEDAR_STAT_PAUSE == fsm_sta)
    {
        movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                       ID_MOVIE_SSET_PAUSE_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS);
        movie_cmd2sset(movie_ctrl->sset_sence, SSET_STRING_UPDATE,
                       STRING_MOVIE_STOP, 0, 0);
    }
}

static __s32 __app_movie_reg_para_init(movie_ctrl_t *movie_ctrl)
{
    __app_movie_set_play_mode();
    __app_movie_set_zoom_mode();

    movie_ctrl->sub_pos   = __app_movie_get_sub_show_pos();
    movie_ctrl->sub_color = __app_movie_get_sub_show_color();
    movie_ctrl->sub_state = __app_movie_get_sub_show_state();
    movie_ctrl->sub_stream = __app_movie_get_sub_stream_index();
    movie_ctrl->sub_size  = 16;

    dsk_reg_set_app_restore_flag(REG_APP_MOVIE, 0);

    return EPDK_OK;
}

static __s32 __movie_ctrl_para_init(movie_ctrl_t *movie_ctrl)
{
    if (NULL == movie_ctrl)
    {
        __err("movie_ctrl is null...");
        return EPDK_FAIL;
    }

    movie_ctrl->hdisp = esKSRV_Get_Display_Hld();

    if (NULL == movie_ctrl->hdisp)
    {
        __msg("open disp fail...");
    }

    __msg("open disp success...");
    {
        __s32 ret;
        __s32 screen_width, screen_height;
        RECT vedio_rect;
        robin_open_arg_t arg = {0};
        arg.cedar_stop_mode = CEDAR_STOP_MODE_UNINSTALL_PLUGINS;
        arg.switch_vplay_mode = CEDAR_FILE_SWITCH_VPLY_MODE_GAP;
        arg.reserve_mem_size = 5500 * 1024; //3500*1024;//2000*1024; //1700*1024;
        ret = robin_open(ROBIN_MODE_VIDEO_MAX, &arg);

        if (EPDK_OK != ret)
        {
            __msg("robin_open fail...");
            return EPDK_FAIL;
        }
        __msg("robin_open success...");
        robin_clear_feedback_msgQ();
        dsk_display_get_size(&screen_width, &screen_height);
        vedio_rect.x = 0;
        vedio_rect.y = 0;
        vedio_rect.width = screen_width;
        vedio_rect.height = screen_height;
        __msg("vedio screen=%d %d %d %d", vedio_rect.x, vedio_rect.y, vedio_rect.width, vedio_rect.height);
        movie_ctrl->hvedio_lyr = robin_request_video_layer(&vedio_rect, 0, 0xff);

        if (NULL == movie_ctrl->hvedio_lyr)
        {
            __msg("robin_request_vedio_layer fail...");
            return EPDK_FAIL;
        }

        __msg("robin_request_vedio_layer success...");
    }
    if (is_on_tv())
    {
        unsigned long arg[3] = {0};
        arg[0] = (unsigned long)movie_ctrl->hvedio_lyr;
        esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_VIDEO_SET_FB, 0, (void *)arg);

        movie_ctrl->mode = MOVIE_MODE_TV;
    }

    {
        __s32 ret;
        unsigned long arg[3] = {0};
        arg[0] = (unsigned long)movie_ctrl->hvedio_lyr;
        ret = esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_LAYER_BOTTOM, 0, (void *)arg);

        if (0 != ret)
        {
            __msg("DISP_CMD_LAYER_BOTTOM fail...");
        }
        else
        {
            __msg("DISP_CMD_LAYER_BOTTOM success...");
        }
    }

    movie_ctrl->auto_play_timmer_id = 0x10;
    movie_ctrl->set_cb_timmer_id    = 0x40;
    movie_ctrl->digit_timmer_id     = 0x41; // 30234384
    movie_ctrl->err_cnt             = 0;
    movie_ctrl->rr_flag             = 0;
    movie_ctrl->auto_ffrr_timmer_id             = 0x43;
    movie_ctrl->auto_del_hbar_timmer_id = 0x42; //
    //movie_ctrl->ff_rr_show_interval_uipara = movie_ff_rr_show_get_interval_uipara(GUI_GetScnDir());
    //__movie_install_auto_del_habr_timmer(movie_ctrl);
    __msg("__movie_para_init success...");
    return EPDK_OK;
}

static __s32 __movie_ctrl_para_deinit(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (is_on_tv())
    {
        unsigned long arg[3] = {0};
        arg[0] = (unsigned long)movie_ctrl->hvedio_lyr;
        esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_VIDEO_STOP, 0, (void *)arg);
    }

    if (movie_ctrl->hvedio_lyr)
    {
        robin_release_video_layer(movie_ctrl->hvedio_lyr);
        movie_ctrl->hvedio_lyr = NULL;
    }

    if (movie_ctrl->hdisp)
    {
        //eLIBs_fclose(movie_ctrl->hdisp);
        movie_ctrl->hdisp = NULL;
    }

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
    {
        GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __movie_install_set_cb_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->set_cb_timmer_id))
    {
        GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->set_cb_timmer_id
                     , 100, NULL);//2.5秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_uninstall_set_cb_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->set_cb_timmer_id))
    {
        GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->set_cb_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __movie_install_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
    {
        GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
                     , 200, NULL);//0.5秒
    }
    else
    {
        __msg("timmer already install...");
        GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id
                       , 200, NULL);//0.5秒
        return EPDK_OK;
    }

    return EPDK_OK;
}

static __s32 __movie_uninstall_auto_play_timmer(movie_ctrl_t *movie_ctrl)
{
    if (!movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id))
    {
        GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_play_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __movie_install_auto_del_habr_timmer(movie_ctrl_t *movie_ctrl) //add by chenguofeng 2013.2.23
{
    if (!movie_ctrl)
    {
        __err("invalid movie_ctrl para ");
        return EPDK_FAIL;
    }

    __wrn("%s %s", __FILE__, __FUNCTION__);

    if (!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_del_hbar_timmer_id))
    {
        GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_del_hbar_timmer_id, 1000, NULL);//5秒
    }
    else
    {
        //__wrn("auto_del_habr_timmer already install");
        GUI_ResetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_del_hbar_timmer_id, 1000, NULL);//5秒
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_uninstall_auto_del_habr_timmer(movie_ctrl_t *movie_ctrl) //add by chenguofeng 2013.2.23
{
    if (!movie_ctrl)
    {
        __err("invalid movie_ctrl para ");
        return EPDK_FAIL;
    }

    __wrn("%s %s", __FILE__, __FUNCTION__);

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_del_hbar_timmer_id))
    {
        GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_del_hbar_timmer_id);
    }

    return EPDK_OK;
}


static __s32 __movie_load_breakpoint_info(__s32 index, __cedar_tag_inf_t *tag_inf)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return -1;
    }

    //__here__;
    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
    {
        return -1;
    }

    //__here__;
    if (get_item_content_from_fifo_db(NAME_MOVIE_BKPOINT, filename, tag_inf))
    {
        return -1;
    }

    //__here__;
    return 0;
}

static __s32 __movie_save_breakpoint_info(__s32 index)
{
    __cedar_tag_inf_t tag_inf;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __s32 ret;
    /*get the tag*/
    ret = robin_get_tag(&tag_inf);

    if (0 != ret)
    {
        __msg("robin_get_tag fail...");
        return -1;
    }

    //__here__;
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return -1;
    }

    __msg("begin save breakpoint, index = %d, filename=%s", index, filename);

    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
    {
        ret = create_fifo_db(NAME_MOVIE_BKPOINT, 4, sizeof(__cedar_tag_inf_t));

        if (0 != ret)
        {
            __msg("create_fifo_db fail...");
            return -1;
        }
    }

    //__here__;
    ret = add_item_to_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);
    __msg("add_item_to_fifo_db, ret=%d", ret);
    return ret;
}

static __s32 __movie_delete_breakpoint_info(__s32 index)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __cedar_tag_inf_t tag_inf;
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return -1;
    }

    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
    {
        return -1;
    }

    ret = get_item_content_from_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);

    if (-1 == ret)
    {
        return 0;
    }

    if (delete_item_from_fifo_db(NAME_MOVIE_BKPOINT, filename))
    {
        return -1;
    }

    return 0;
}

static __s32 __movie_play_file(movie_ctrl_t *movie_ctrl, __s32 index, __bool breakplay)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __movie_install_auto_play_timmer(movie_ctrl);
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return EPDK_FAIL;
    }

    __msg("breakplay=%d", breakplay);
    __msg("filename=%s", filename);

    if (EPDK_TRUE == breakplay)
    {
        __cedar_tag_inf_t tag_inf;
        eLIBs_memset(&tag_inf, 0x00, sizeof(tag_inf));
        ret = __movie_load_breakpoint_info(index, &tag_inf);

        if (EPDK_OK == ret)
        {
            __msg("has breakpoint info, begin breakplay... index = %d filename=%s", index, filename);
            robin_set_cmd_stop();
            //robin_clear_feedback_msgQ();//清空错误信息消息队列
            ret = robin_play_file(filename, &tag_inf);

            if (EPDK_FAIL == ret)
            {
                return EPDK_FAIL;
            }

            robin_set_cmd_play();
        }
        else
        {
            robin_set_cmd_stop();
            //robin_clear_feedback_msgQ();//清空错误信息消息队列
            ret = robin_play_file(filename, NULL);

            if (EPDK_FAIL == ret)
            {
                return EPDK_FAIL;
            }

            robin_set_cmd_play();
            __msg("wrn, no breakpoint info, not breakplay... index = %d filename=%s", index, filename);
        }
    }
    else
    {
        robin_set_cmd_stop();
        //robin_clear_feedback_msgQ();//清空错误信息消息队列
        ret = robin_play_file(filename, NULL);
        __msg("ret=%d", ret);

        if (EPDK_FAIL == ret)
        {
            return EPDK_FAIL;
        }

        robin_set_cmd_play();
        __msg("ret=%d", ret);
    }

    if (robin_subtitle_exist() == EPDK_TRUE)
    {
        if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) != EPDK_TRUE)
        {
            __movie_sub_show_scene_create(movie_ctrl);
            //__movie_install_auto_play_timmer(movie_ctrl);
        }
    }
    else
    {
        if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) == EPDK_TRUE)
        {
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
            //__movie_uninstall_auto_play_timmer(movie_ctrl);
        }
    }

#if OUTAGE_SAVE_CUR_FILE
    dsk_reg_save_cur_play_info(REG_APP_MOVIE, index, filename, movie_ctrl->root_type);
    //__movie_save_breakpoint_info(index);
    dsk_reg_flush();
#endif
    return ret;
}

static __s32 __movie_play_next(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __s32 index;
    __s32 total;

    if (MOVIE_MODE_AB != movie_ctrl->ab_mode)
    {
        __gui_msg_t my_msg_to_sset;
        movie_ctrl->ab_mode = MOVIE_MODE_AB;
        robin_cancle_ab_v2();
        eLIBs_memset(&my_msg_to_sset, 0, sizeof(__gui_msg_t));
        my_msg_to_sset.id = SSET_STRING_UPDATE;
        my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
        my_msg_to_sset.h_srcwin = NULL;
        my_msg_to_sset.dwAddData1 = STRING_MOVIE_AB;
        GUI_SendMessage(&my_msg_to_sset);
    }

    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    __movie_delete_breakpoint_info(index);
    total = robin_npl_get_total_count();

    if (-1 == total)
    {
        __msg("robin_npl_get_total_count fail...");
        return EPDK_FAIL;
    }

    index++;

    if (index >= total)
    {
        index = 0;
    }

    robin_npl_set_cur(index);
#if HT_1628_LED_EX
    SetMovieNum(APP_MOVIE_ID, ((index + 1) & 0x0fff) | 0x2000);
#endif
    ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);
    return ret;
}


static __s32 __movie_play_prev(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __s32 index;
    __s32 total;

    if (MOVIE_MODE_AB != movie_ctrl->ab_mode)
    {
        __gui_msg_t my_msg_to_sset;
        movie_ctrl->ab_mode = MOVIE_MODE_AB;
        robin_cancle_ab_v2();
        eLIBs_memset(&my_msg_to_sset, 0, sizeof(__gui_msg_t));
        my_msg_to_sset.id = SSET_STRING_UPDATE;
        my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
        my_msg_to_sset.h_srcwin = NULL;
        my_msg_to_sset.dwAddData1 = STRING_MOVIE_AB;
        GUI_SendMessage(&my_msg_to_sset);
    }

    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    total = robin_npl_get_total_count();

    if (-1 == total)
    {
        __msg("robin_npl_get_total_count fail...");
        return EPDK_FAIL;
    }

    index--;

    if (index < 0)
    {
        index = total - 1;
    }

    if (index < 0)
    {
        __msg("total movie count = 0");
        index = 0;
    }

#if HT_1628_LED_EX
    SetMovieNum(APP_MOVIE_ID, ((index + 1) & 0x0fff) | 0x2000);
#endif
    robin_npl_set_cur(index);
    ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);
    return ret;
}

static __s32 __movie_cur_file_has_breakpoint(void)
{
    __s32 ret = EPDK_FALSE;
    __s32 index = 0;
    __cedar_tag_inf_t tag_inf;
#if defined(CONFIG_SUNXI_QA_TEST)
	return EPDK_FALSE;//qa测试不使用断点播放?
#endif
    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __log("robin_npl_get_cur fail...");
        return EPDK_FALSE;
    }

    __log("before __movie_load_breakpoint_info");
    eLIBs_memset(&tag_inf, 0x00, sizeof(tag_inf));
    ret = __movie_load_breakpoint_info(index, &tag_inf);

    if (0 == ret)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

static __s32 __movie_start_play(movie_ctrl_t *movie_ctrl, __bool breakplay)
{
    __s32 ret = 0;
    __s32 index = -1;
    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    ret = __movie_play_file(movie_ctrl, index, breakplay);
    return ret;
}

static __s32 __movie_auto_play_next(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __s32 index;
    __s32 total;
    index = robin_npl_get_cur();

    if (-1 != index)
    {
        //__here__;
        __movie_delete_breakpoint_info(index);
        //__here__;
    }

    if (MOVIE_MODE_AB != movie_ctrl->ab_mode)
    {
        __gui_msg_t my_msg_to_sset;
        movie_ctrl->ab_mode = MOVIE_MODE_AB;
        robin_cancle_ab_v2();
        eLIBs_memset(&my_msg_to_sset, 0, sizeof(__gui_msg_t));
        my_msg_to_sset.id = SSET_STRING_UPDATE;
        my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
        my_msg_to_sset.h_srcwin = NULL;
        my_msg_to_sset.dwAddData1 = STRING_MOVIE_AB;
        GUI_SendMessage(&my_msg_to_sset);
    }

    index = robin_npl_get_next();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    __movie_play_file(movie_ctrl, index, EPDK_FALSE);
    //__msg("ret=%d", ret);
    return EPDK_OK;//强制返回ok，cedar_set_play可能返回失败
}


static __s32 __movie_stop_play(movie_ctrl_t *movie_ctrl)
{
    robin_set_cmd_stop();
    return EPDK_OK;
}

static __s32 __movie_pause_play(movie_ctrl_t *movie_ctrl)
{
    robin_set_cmd_pause();
    return EPDK_OK;
}

static __s32 __movie_reseum_play(movie_ctrl_t *movie_ctrl)
{
    robin_set_cmd_play();
    return EPDK_OK;
}

//flag:0表示退出到explorer，1表示退出到home
static __s32 __movie_exit_movie(movie_ctrl_t *movie_ctrl, __s32 flag)
{
    __msg("before __movie_exit_movie");

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (movie_ctrl->exit_movie)
    {
        __msg("movie already exit...");
        return EPDK_FAIL;
    }

    movie_ctrl->exit_movie = 1;

    if (NULL == movie_ctrl->movie_manager)
    {
        __err("movie_ctrl->movie_manager is null...");
        return EPDK_FAIL;
    }

    __movie_uninstall_auto_play_timmer(movie_ctrl);
    //__movie_pause_play(movie_ctrl);视频库修改了，要求获取断点信息的时候只能是play状态，所以不能暂停
    {
        __gui_msg_t msg;
        eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
        msg.id = GUI_MSG_CLOSE;
        msg.dwAddData1 = flag;
        msg.h_deswin = movie_ctrl->movie_manager;
        GUI_SendMessage(&msg);
    }
    __msg("after __movie_exit_movie");
    return EPDK_OK;
}

static __bool  __movie_check_for_play_err(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    ret = robin_get_feedback_msg();

    if (ret < 0)
    {
        robin_clear_feedback_msgQ();//清空错误信息消息队列
        __msg("robin_get_feedback_msg < 0, movie play err...");
        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
        __movie_playerror_scene_create(movie_ctrl);
        esKRNL_TimeDly(100);
        __movie_playerror_scene_delete(movie_ctrl);
        movie_ctrl->err_cnt++;

        if (movie_ctrl->err_cnt >= robin_npl_get_total_count()
            || RAT_PLAY_MODE_ROTATE_ONE == robin_get_play_mode()
            || RAT_PLAY_MODE_ONLY_ONCE == robin_get_play_mode())
        {
            __movie_exit_movie(movie_ctrl, 0);
            return EPDK_FALSE;
        }
    }
    else
    {
        movie_ctrl->err_cnt = 0;
    }

    return EPDK_TRUE;
}

static __s32 __movie_check_for_auto_play(movie_ctrl_t *movie_ctrl)
{
    static __s32 stop_cnt = 2;//连续2次检测到stop，才播下一首，防止还没播起来就跳到下一首
    static __s32 cur_stop_cnt = 0;

    if (!movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        __cedar_status_t fsm_sta;
        fsm_sta = robin_get_fsm_status();
        __msg("fsm_sta= %d", fsm_sta);

        switch (fsm_sta)
        {
            case CEDAR_STAT_STOP:
            {
                __s32 ret;

                //如果存在FF RR SHOW，出现STOP状态就要删除
                if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID))
                {
                    __movie_prog_scene_delete(movie_ctrl);
                }

                if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_FF_RR_SHOW_ID))
                {
                    __movie_ff_rr_show_scene_delete(movie_ctrl);
                    is_touch_ff_rr_valid = 0;
                }

                cur_stop_cnt++;

                if (cur_stop_cnt < stop_cnt)
                {
                    break;
                }

                cur_stop_cnt = 0;
                __msg("CEDAR_STAT_STOP, start play next...");
                __msg("movie_ctrl->rr_flag=%d", movie_ctrl->rr_flag);

                if (1 == movie_ctrl->rr_flag)
                {
                    __s32 index;
                    index = robin_npl_get_cur();
                    __msg("index=%d", index);

                    if (-1 != index)
                    {
                        __movie_play_file(movie_ctrl, index, EPDK_FALSE);
                    }

                    //movie_ctrl->rr_flag = 0;
                }
                else
                {
                    ret = __movie_auto_play_next(movie_ctrl);
                    __msg("ret=%d", ret);

                    if (EPDK_OK != ret)//播放完毕,退出应用
                    {
                        __movie_playend_scene_create(movie_ctrl);
                        esKRNL_TimeDly(100);
                        __movie_exit_movie(movie_ctrl, 0);
                    }
                    else
                    {
                        {
                            //通知子场景更新UI
                            __gui_msg_t msg;
                            eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
                            __msg("movie_ctrl->spsc_scene=%x", movie_ctrl->spsc_scene);

                            if (movie_ctrl->spsc_scene)
                            {
                                msg.h_deswin = movie_spsc_scene_get_hwnd(movie_ctrl->spsc_scene);

                                if (msg.h_deswin)
                                {
                                    msg.id = movie_spsc_scene_update_fileinfo;
                                    msg.dwAddData1 = 0;
                                    GUI_SendMessage(&msg);
                                }
                            }
                        }
                    }
                }

                break;
            }

            case CEDAR_STAT_PLAY:
            {
                movie_ctrl->rr_flag = 0;
                //                __msg("movie_ctrl->rr_flag=%d", movie_ctrl->rr_flag);
                {
                    //存在能正常播放的文件，把该值复位一下
                    movie_ctrl->err_cnt = 0;
                }
                break;
            }

            case CEDAR_STAT_RR:
            {
                movie_ctrl->rr_flag = 1;
                __msg("movie_ctrl->rr_flag=%d", movie_ctrl->rr_flag);
                break;
            }

            default:
            {
                movie_ctrl->rr_flag = 0;
                //                __msg("fsm_sta=%d, movie_ctrl->rr_flag=%d", fsm_sta, movie_ctrl->rr_flag);
                break;
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        movie_spsc_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_spsc_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_SPSC_ID;
        create_para.sub_state = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);
        movie_ctrl->spsc_scene = movie_spsc_scene_create(&create_para);

        if (NULL == movie_ctrl->spsc_scene)
        {
            __msg("movie_spsc_scene_create fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->spsc_scene)
        {
            movie_spsc_scene_delete(movie_ctrl->spsc_scene);
            movie_ctrl->spsc_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playsta_scene_create(movie_ctrl_t *movie_ctrl, movie_playsta_t playsta)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (NULL == movie_ctrl->playsta_scene)
    {
        movie_playsta_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_playsta_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_PLAYSTA_ID;
        create_para.playsta = playsta;
        movie_ctrl->playsta_scene = movie_playsta_scene_create(&create_para);

        if (NULL == movie_ctrl->playsta_scene)
        {
            __msg("movie_playsta_scene_create fail...");
            return EPDK_FAIL;
        }

        movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
    }

    return EPDK_OK;
}

static __s32 __movie_playsta_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
#if 1

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playsta_scene)
        {
            {
                movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
            }
            movie_playsta_scene_delete(movie_ctrl->playsta_scene);
            movie_ctrl->playsta_scene = NULL;
        }
    }

#endif
    return EPDK_OK;
}

static __s32 __movie_playsta_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playsta_scene)
        {
            movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_show_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        movie_sub_show_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_sub_show_create_para_t));
        create_para.hparent  = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_SUB_SHOW_ID;
        create_para.sub_pos  = movie_ctrl->sub_pos;
        create_para.sub_color = movie_ctrl->sub_color;
        create_para.sub_size = movie_ctrl->sub_size;
        movie_ctrl->sub_show_scene = movie_sub_show_scene_create(&create_para);

        if (NULL == movie_ctrl->sub_show_scene)
        {
            __msg("movie_sub_show_scene_create fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sub_show_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->sub_show_scene)
        {
            GUI_WinSetFocusChild(movie_ctrl->sset_sence);
            movie_sub_show_scene_delete(movie_ctrl->sub_show_scene);
            movie_ctrl->sub_show_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_prog_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        movie_prog_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_prog_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_PROG_ID;
        movie_ctrl->prog_scene = movie_prog_scene_create(&create_para);

        if (NULL == movie_ctrl->prog_scene)
        {
            __msg("movie_volume_scene_create fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_prog_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->prog_scene)
        {
            movie_prog_scene_delete(movie_ctrl->prog_scene);
            movie_ctrl->prog_scene = NULL;
        }
    }

    if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
    {
        GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id);
        movie_ctrl->ff_rr_cnt = 0;
    }

    return EPDK_OK;
}

static __s32 __movie_prog_scene_update_ui(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (NULL == movie_ctrl->prog_scene)
    {
        __log("===============");
        __log("invalid para...");
        __log("===============");
        return EPDK_FAIL;
    }

    return movie_prog_scene_update_ui(movie_ctrl->prog_scene);
}

static __s32 __movie_prog_scene_enable_auto_hide(movie_ctrl_t *movie_ctrl, __bool auto_hide)
{
    __s32 ret;
    __wrn("__movie_prog_scene_enable_auto_hide");

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->prog_scene)
        {
            movie_prog_scene_enable_auto_hide(movie_ctrl->prog_scene, auto_hide);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_prog_scene_set_focus(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->prog_scene)
        {
            movie_prog_scene_set_focus(movie_ctrl->prog_scene);
        }
    }

    return EPDK_OK;
}


static __s32 __movie_breakplay_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (NULL == movie_ctrl->breakplay_scene)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_BREAKPLAY};
            default_dialog(movie_ctrl->breakplay_scene, movie_ctrl->movie_manager, MOVIE_BREAKPLAY_DIALOG_ID, ADLG_YESNO, lang_id);
        }

        //movie_ctrl->breakplay_scene = create_breakplay_dlg();
        if (NULL == movie_ctrl->breakplay_scene)
        {
            __msg("default_dialog fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_breakplay_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->breakplay_scene)
        {
            app_dialog_destroy(movie_ctrl->breakplay_scene);
            movie_ctrl->breakplay_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playerror_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (NULL == movie_ctrl->playerror_scene)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_FILE_NOT_SUPPORT};
            default_dialog(movie_ctrl->playerror_scene, movie_ctrl->movie_manager, MOVIE_PLAYERROR_DIALOG_ID, ADLG_OK, lang_id);
        }

        //movie_ctrl->breakplay_scene = create_breakplay_dlg();
        if (NULL == movie_ctrl->playerror_scene)
        {
            __msg("default_dialog fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playerror_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playerror_scene)
        {
            app_dialog_destroy(movie_ctrl->playerror_scene);
            movie_ctrl->playerror_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playend_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (NULL == movie_ctrl->playend_scene)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MOVIE_PLAY_END};
            default_dialog(movie_ctrl->playend_scene, movie_ctrl->movie_manager, MOVIE_PLAYEND_DIALOG_ID, ADLG_OK, lang_id);
        }

        if (NULL == movie_ctrl->playend_scene)
        {
            __msg("default_dialog fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_playend_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->playend_scene)
        {
            app_dialog_destroy(movie_ctrl->playend_scene);
            movie_ctrl->playend_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_sset_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (NULL == movie_ctrl->sset_sence)
        {
            if (!movie_ctrl->sset_create_para)
            {
                __movie_sset_list_para_t sset_list_para;
                eLIBs_memset(&sset_list_para, 0, sizeof(__movie_sset_list_para_t));
                sset_list_para.channel = __app_movie_get_channel();
                sset_list_para.playmode = robin_get_play_mode();
                sset_list_para.track = __app_movie_get_track(); //chengf3  add
                sset_list_para.zoom = robin_get_zoom();
                sset_list_para.sub_pos = movie_ctrl->sub_pos;
                movie_ctrl->sset_create_para = Movie_SSET_Build_CreatePara(movie_ctrl->movie_manager, MOVIE_SSET_ID
                                               , &sset_list_para);
            }

            if (!movie_ctrl->sset_create_para)
            {
                __msg("Movie_SSET_Build_CreatePara fail...");
                return EPDK_FAIL;
            }
        }

        if (NULL == movie_ctrl->sset_sence)
        {
            movie_ctrl->sset_sence = SSET_Create(movie_ctrl->sset_create_para);
        }

        if (NULL == movie_ctrl->sset_sence)
        {
            __msg("default_dialog fail...");
            return EPDK_FAIL;
        }

        ret = SSET_Set_Nodelist("movie", NODELIST_ON);

        if (EPDK_FAIL == ret)
        {
            return EPDK_FAIL;
        }
    }

    app_movie_is_movie_pause(movie_ctrl);
    return EPDK_OK;
}

static __s32 __movie_sset_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->sset_sence)
        {
            SSET_Destroy(movie_ctrl->sset_sence);
            movie_ctrl->sset_sence = NULL;
        }

        if (movie_ctrl->sset_create_para)
        {
            Movie_SSET_Destroy_CreatePara(movie_ctrl->sset_create_para);
            movie_ctrl->sset_create_para = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_ff_rr_show_scene_create(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __wrn("[__movie_ff_rr_show_scene_create] called");

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        movie_ff_rr_show_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_ff_rr_show_create_para_t));
        create_para.hparent = movie_ctrl->movie_manager;
        create_para.scene_id = MOVIE_FF_RR_SHOW_ID;
        create_para.sub_state = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID);
        movie_ctrl->ff_rr_show_scene = movie_ff_rr_show_scene_create(&create_para);

        if (NULL == movie_ctrl->ff_rr_show_scene)
        {
            __msg("movie_ff_rr_show_scene_create fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_ff_rr_show_scene_delete(movie_ctrl_t *movie_ctrl)
{
    __s32 ret;
    __wrn("[__movie_ff_rr_show_scene_delete] called");

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        if (movie_ctrl->ff_rr_show_scene)
        {
            movie_ff_rr_show_scene_delete(movie_ctrl->ff_rr_show_scene);
            movie_ctrl->ff_rr_show_scene = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 __movie_get_ff_rr_show_info_by_piont(__s32 cur_x, __s32 cur_y, movie_ctrl_t *movie_ctrl, movie_ff_rr_show_info_t *ff_rr_show_info)
{
    static __s32 dirtion = 0;
    static __s32 ff_rr_speed = 0;
    __s32 i = 0;
    __wrn("[__movie_get_ff_rr_show_info_by_piont] called");

    if (NULL == movie_ctrl)
    {
        __err("__movie_get_ff_rr_show_info_by_piont, movie_ctrl == NULL...");
        return EPDK_FAIL;
    }

    if (NULL == movie_ctrl->ff_rr_show_interval_uipara)
    {
        __err("__movie_get_ff_rr_show_info_by_piont, movie_ctrl->ff_rr_show_interval_uipara == NULL...");
        return EPDK_FAIL;
    }

    if (NULL == ff_rr_show_info)
    {
        __err("__movie_get_ff_rr_show_info_by_piont, ff_rr_show_info == NULL...");
        return EPDK_FAIL;
    }

    if (cur_x >= movie_ctrl->ff_rr_show_interval_uipara->interval[INTERVAL_NUM / 2 - 1] \
        && cur_x <= (movie_ctrl->ff_rr_show_interval_uipara->ellipse.x + movie_ctrl->ff_rr_show_interval_uipara->ellipse.a))
    {
        return EPDK_FAIL;
    }

    for (; i < INTERVAL_NUM; i++)
    {
        if (cur_x < movie_ctrl->ff_rr_show_interval_uipara->interval[i])
        {
            break;
        }
    }

    if (i >= 8)
    {
        return EPDK_FAIL;
    }

    switch (i)
    {
        case 0:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_RR;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_LEFT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_4;
        }
        break;

        case 1:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_RR;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_LEFT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_3;
        }
        break;

        case 2:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_RR;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_LEFT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_2;
        }
        break;

        case 3:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_RR;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_LEFT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_1;
        }
        break;

        case 4:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_FF;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_RIGHT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_1;
        }
        break;

        case 5:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_FF;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_RIGHT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_2;
        }
        break;

        case 6:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_FF;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_RIGHT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_3;
        }
        break;

        case 7:
        {
            ff_rr_show_info->text_id = STRING_MOVIE_FF;
            ff_rr_show_info->direction = FF_RR_SHOW_DIRECTION_RIGHT;
            ff_rr_show_info->ff_rr_speed = FF_RR_SHOW_SPEED_4;
        }
        break;

        default:
            return EPDK_FAIL;
    }

    if (dirtion == ff_rr_show_info->direction && ff_rr_speed == ff_rr_show_info->ff_rr_speed)
    {
        return EPDK_FAIL;
    }
    else
    {
        robin_set_ff_rr_speed(ff_rr_show_info->ff_rr_speed);

        if (dirtion != ff_rr_show_info->direction)
        {
            robin_set_cmd_play();
        }

        if (FF_RR_SHOW_DIRECTION_LEFT == ff_rr_show_info->direction)
        {
            robin_set_cmd_rr();
        }
        else if (FF_RR_SHOW_DIRECTION_RIGHT == ff_rr_show_info->direction)
        {
            robin_set_cmd_ff();
            __log("%s %d ", __FILE__, __LINE__);
        }

        dirtion = ff_rr_show_info->direction;
        ff_rr_speed = ff_rr_show_info->ff_rr_speed;
    }

    return EPDK_OK;
}

static __s32 __app_movie_touch_ff_rr_identify(__gui_msg_t *msg, movie_ff_rr_show_info_t *ff_rr_show_info)
{
    static __s32 ignore_msg = EPDK_NO;
    __s32 cur_x, cur_y;
    movie_ctrl_t *movie_ctrl;
    __s32 ret = -1;

    if (NULL == msg)
    {
        __err("__app_movie_touch_ff_rr_identify, msg == NULL...");
        return EPDK_FAIL;
    }

    if (NULL == ff_rr_show_info)
    {
        __err("__app_movie_touch_ff_rr_identify, ff_rr_show_info == NULL...");
        return EPDK_FAIL;
    }

    if (EPDK_YES == ignore_msg)
    {
        __err("__app_movie_touch_ff_rr_identify, ignore msg...");
        return EPDK_FAIL;
    }

    cur_x = LOSWORD(msg->dwAddData2);
    cur_y = HISWORD(msg->dwAddData2);
    movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (GUI_MSG_TOUCH_LONGDOWN == msg->dwAddData1
        || GUI_MSG_TOUCH_OVERLONGDOWN == msg->dwAddData1)
    {
        __wrn("__app_movie_touch_ff_rr_identify, touch down...");
        __wrn("ignore_msg = %d", ignore_msg);

        if (msg->h_deswin != GUI_WinGetCaptureWin())
        {
            //  GUI_WinSetCaptureWin(msg->h_deswin);
        }

        if (0 == is_touch_ff_rr_valid)
        {
            ret = __movie_tvout_tp_is_in_ellipse(movie_ctrl->ff_rr_show_interval_uipara->ellipse.a, \
                                                 movie_ctrl->ff_rr_show_interval_uipara->ellipse.b, \
                                                 movie_ctrl->ff_rr_show_interval_uipara->ellipse.x, \
                                                 movie_ctrl->ff_rr_show_interval_uipara->ellipse.y, \
                                                 cur_x, \
                                                 cur_y);

            if (EPDK_FALSE == ret)
            {
                __wrn("Point outside the ellipse");
                is_touch_ff_rr_valid = 0;
                return EPDK_FAIL;
            }

            is_touch_ff_rr_valid = 1;
            ignore_msg = EPDK_YES;

            if (EPDK_FALSE == __movie_has_sub_scene(movie_ctrl, MOVIE_FF_RR_SHOW_ID))
            {
                if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID))
                {
                    /*是否是sset 中是tbar 的情况*/
                    if (SSET_court_is_tbar() == EPDK_TRUE)
                    {
                        /*SHOW状态下隐藏*/
                        if (SSET_tbar_is_show() == EPDK_TRUE)
                        {
                            SSET_hide_tbar();
                        }

                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                    }
                    else
                    {
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                    }
                }

                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                __movie_ff_rr_show_scene_create(movie_ctrl);
                ff_rr_show_info->text_id = STRING_MOVIE_FF_RR_START;
                ff_rr_show_info->direction = 0;
                ff_rr_show_info->ff_rr_speed = 0;
                movie_ff_rr_show_scene_update_string(movie_ctrl->ff_rr_show_scene, ff_rr_show_info);

                if (EPDK_FALSE == __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID))
                {
                    __movie_prog_scene_create(movie_ctrl);
                    __movie_prog_scene_set_focus(movie_ctrl);
                }
            }

            ignore_msg = EPDK_NO;
        }
    }
    else if (GUI_MSG_TOUCH_UP == msg->dwAddData1
             || GUI_MSG_TOUCH_OVERUP == msg->dwAddData1)
    {
        __cedar_status_t fsm_sta;
        fsm_sta = robin_get_fsm_status();
        __wrn("__app_movie_touch_ff_rr_identify, touch up...");

        if (msg->h_deswin == GUI_WinGetCaptureWin())
        {
            // GUI_WinReleaseCapture();
        }

        __wrn("is_touch_valid=%d", is_touch_ff_rr_valid);

        if (1 == is_touch_ff_rr_valid)
        {
            if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID))
            {
                __movie_prog_scene_delete(movie_ctrl);
            }

            if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_FF_RR_SHOW_ID))
            {
                __movie_ff_rr_show_scene_delete(movie_ctrl);
            }
        }

        if (CEDAR_STAT_FF == fsm_sta || CEDAR_STAT_RR == fsm_sta)
        {
            robin_set_cmd_play();
        }

        is_touch_ff_rr_valid = 0;
    }
    else if (GUI_MSG_TOUCH_MOVE == msg->dwAddData1
             || GUI_MSG_TOUCH_OVERMOVE == msg->dwAddData1)
    {
        __wrn("__app_movie_touch_ff_rr_identify, touch move...");
        __wrn("is_touch_ff_rr_valid = %d", is_touch_ff_rr_valid);

        if (1 == is_touch_ff_rr_valid)
        {
            return __movie_get_ff_rr_show_info_by_piont(cur_x, cur_y, movie_ctrl, ff_rr_show_info);
        }
    }
    else
    {
        __wrn("__app_movie_touch_ff_rr_identify, other touch msg...");
    }

    return EPDK_FAIL;
}

//判断movie manwin是否有指定的子场景之一
static __s32 __movie_has_sub_scene(movie_ctrl_t *movie_ctrl, __u32 sub_id)
{
    H_WIN child;
    char  winname[256] = {0};
    __u32 child_id;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FALSE;
    }

    if (!movie_ctrl->movie_manager)
    {
        __err("invalid para...");
        return EPDK_FALSE;
    }

    child = GUI_WinGetFirstChild(movie_ctrl->movie_manager);

    while (child != NULL)
    {
        child_id = GUI_WinGetItemId(child);

        if ((child_id & sub_id))
        {
            return EPDK_TRUE;
        }

        child = GUI_WinGetNextBro(child);
    }

    return EPDK_FALSE;
}

//删除movie的子场景
static __s32 __movie_delete_subscene_by_id(movie_ctrl_t *movie_ctrl, __u32 sub_id)
{
    H_WIN child, next_child;
    char  winname[256];
    __u32 child_id;

    if (NULL == movie_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!movie_ctrl->movie_manager)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    child = GUI_WinGetFirstChild(movie_ctrl->movie_manager);

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);
        next_child = GUI_WinGetNextBro(child);
        child_id = GUI_WinGetItemId(child);

        if ((child_id & sub_id))//需要删除
        {
            __wrn("@@@@@@@@@@@ delete movie sub scene, winname = %s,child_id = %d", winname, child_id);

            switch (child_id)
            {
                case MOVIE_SPSC_ID:
                {
                    __movie_spsc_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_PLAYSTA_ID:
                {
                    __movie_playsta_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_SUB_SHOW_ID:
                {
                    //__here__;
                    __wrn("__movie_sub_show_scene_delete:");
                    __movie_sub_show_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_PROG_ID:
                {
                    __movie_prog_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_BREAKPLAY_DIALOG_ID:
                {
                    __movie_breakplay_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_PLAYERROR_DIALOG_ID:
                {
                    __movie_playerror_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_PLAYEND_DIALOG_ID:
                {
                    __movie_playend_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_SSET_ID:
                {
                    __movie_sset_scene_delete(movie_ctrl);
                    break;
                }

                case MOVIE_FF_RR_SHOW_ID:
                {
                    __movie_ff_rr_show_scene_delete(movie_ctrl);
                    is_touch_ff_rr_valid = 0;
                    break;
                }

                default:
                {
                    __err("unknown child id, child_id=%d", child_id);
                    return EPDK_FAIL;
                }
            }
        }

        child = next_child;
    }

    return EPDK_OK;
}

static __s32 __movie_delete_scene_berore_switch_tv(movie_ctrl_t *movie_ctrl)
{
    //删除场景和部分资源
    //__here__;
    //__movie_uninstall_auto_play_timmer(movie_ctrl);
    {
        //save breakpoint
        __s32 index;
        index = robin_npl_get_cur();

        //__here__;
        if (-1 != index)
        {
            //__here__;
            __movie_save_breakpoint_info(index);
        }
    }
    //__here__;
    __movie_stop_play(movie_ctrl);
    //__here__;
    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
    //__here__;
    __movie_ctrl_para_deinit(movie_ctrl);
    robin_close();
    return EPDK_OK;
}

static __s32 __movie_create_scene_after_switch_tv(movie_ctrl_t *movie_ctrl)
{
    //创建部分场景和资源
    __movie_ctrl_para_init(movie_ctrl);
    __app_movie_reg_para_init(movie_ctrl);
    __movie_start_play(movie_ctrl, EPDK_TRUE);

    if (1 == movie_ctrl->sub_state)
    {
        if (robin_subtitle_exist() == EPDK_TRUE)
        {
            if (EPDK_FALSE == __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID))
            {
                __movie_sub_show_scene_create(movie_ctrl);
                //__movie_install_auto_play_timmer(movie_ctrl);
            }
        }
        else
        {
            if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID))
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
                //__movie_uninstall_auto_play_timmer(movie_ctrl);
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_switch_output(movie_ctrl_t *movie_ctrl)
{
    //切换输出
    {
        reg_system_para_t *para = NULL;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para != NULL)
        {
            __msg("para->pal_ntsc = %d", para->pal_ntsc);

            if (0 == para->pal_ntsc)
            {
                if (para->output == LION_DISP_LCD)
                {
                    g_display_switch_output(LION_DISP_TV_NTSC);
                }
                else
                {
                    g_display_switch_output(LION_DISP_LCD);
                }
            }
            else
            {
                if (para->output == LION_DISP_LCD)
                {
                    g_display_switch_output(LION_DISP_TV_PAL);
                }
                else
                {
                    g_display_switch_output(LION_DISP_LCD);
                }
            }
        }
    }
    return EPDK_OK;
}

static __s32 __movie_tvout_tp_is_in_ellipse(__s32 a, __s32 b, __s32 ox, __s32 oy,
        __s32 x, __s32 y)
{
    // __wrn("a=%d, b=%d, ox=%d, oy=%d, x=%d, y=%d", a, b, ox, oy, x, y);
    // __wrn("b*b*(x-ox)*(x-ox)=%d", b*b*(x-ox)*(x-ox));
    // __wrn("a*a*(y-oy)*(y-oy)=%d", a*a*(y-oy)*(y-oy));
    // __wrn("a*a*b*b=%d", a*a*b*b);
    if (b * b * (x - ox) * (x - ox) + a * a * (y - oy) * (y - oy) <= a * a * b * b)
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

typedef struct
{
    __s32 a;
    __s32 b;
    __s32 ox;
    __s32 oy;
    __s32 val;
} __movie_ellipse_t;

static __s32 __movie_tvout_tp_calc_key_val(__s32 x, __s32 y)
{
    __s32 i;
    __s32 ret;
    static __movie_ellipse_t ellipse[] =
    {
        {MOVIE_LCD_WIDTH / 5,      MOVIE_LCD_HEIGHT / 3,     0,                       MOVIE_LCD_HEIGHT / 2,   GUI_MSG_KEY_LEFT}, //左
        //{MOVIE_LCD_WIDTH/3.2,    MOVIE_LCD_HEIGHT/4.2,   MOVIE_LCD_WIDTH/2,       0,                    GUI_MSG_KEY_UP},//上
        {MOVIE_LCD_WIDTH / 5,      MOVIE_LCD_HEIGHT / 3,     MOVIE_LCD_WIDTH,         MOVIE_LCD_HEIGHT / 2,   GUI_MSG_KEY_RIGHT}, //右
        //{MOVIE_LCD_WIDTH/3.2,    MOVIE_LCD_HEIGHT/4.2,   MOVIE_LCD_WIDTH/2,       MOVIE_LCD_HEIGHT,     GUI_MSG_KEY_DOWN},//下
        {MOVIE_LCD_WIDTH / 4,      MOVIE_LCD_HEIGHT / 4.6,   MOVIE_LCD_WIDTH / 2,       MOVIE_LCD_HEIGHT / 2,   GUI_MSG_KEY_ENTER}, //中
    };

    for (i = 0 ; i < BEETLES_TBL_SIZE(ellipse) ; i++)
    {
        ret = __movie_tvout_tp_is_in_ellipse(ellipse[i].a, ellipse[i].b, ellipse[i].ox, ellipse[i].oy, x, y);

        if (EPDK_TRUE == ret)
        {
            // __wrn("__movie_tvout_tp_is_in_ellipse found correspond region, i=%d, ellipse[i].val=0x%x", i, ellipse[i].val);
            return ellipse[i].val;
        }
    }

    return -1;
}

static __s32 __movie_tvout_tp_hook(__gui_msg_t *msg)
{
    static __s32 touch_down_key_val = -1;
    __wrn("__movie_tvout_tp_hook");

    if (NULL == msg)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    if (NULL != msg->p_arg)
    {
        __s32 *p;
        p = (__s32 *)(msg->p_arg);
        touch_down_key_val = p[0];
        __wrn("__movie_tvout_tp_hook, static val reinit...");
        return EPDK_OK;
    }

    if (GUI_MSG_USER_DEF != msg->id)
    {
        __wrn("msg type error...");
    }

    if (NULL == g_movie_ctrl)
    {
        __wrn("g_movie_ctrl is null...");
    }

    {
        __s32 dwAddData1;
        __s32 dwAddData2;
        __gui_msg_t mymsg;
        __s32 x, y;
        __s32 cur_key_val = -1;
        __s32 key_action;
        static __s32 longdown_cnt = 0;
        __s32 longdown_delay = 3;//每3个touch long down当作一次key long down
        dwAddData1 = msg->dwAddData1;
        dwAddData2 = msg->dwAddData2;
        x = LOSWORD(dwAddData2);
        y = HISWORD(dwAddData2);
        __wrn("***************x=%d, y=%d************", x, y);
        cur_key_val =  __movie_tvout_tp_calc_key_val(x, y);

        if (-1 == cur_key_val)
        {
            __wrn("not in eny region...");
        }

        if (GUI_MSG_TOUCH_DOWN == dwAddData1)
        {
            if (-1 != touch_down_key_val) //tp会连续不断发touch down下来，转换为key long down
            {
                __wrn("key down alreadly come...");
                longdown_cnt++;

                if (0 == (longdown_cnt % longdown_delay))
                {
                    key_action = KEY_REPEAT_ACTION;
                    cur_key_val = touch_down_key_val;
                }
                else
                {
                    cur_key_val = -1;
                }
            }
            else
            {
                key_action = KEY_DOWN_ACTION;
                touch_down_key_val = cur_key_val;//tp按下时记录keydown val
            }
        }
        else if (GUI_MSG_TOUCH_UP == dwAddData1)
        {
            key_action = KEY_UP_ACTION;
            cur_key_val = touch_down_key_val;//tp抬起时设置keyup val为上次key down val
            touch_down_key_val = -1;//上次key down val置空
        }
        else if (GUI_MSG_TOUCH_MOVE == dwAddData1) //发生了移动，当前消息无效
        {
            cur_key_val = -1;
            __wrn("move msg come, invalidate cur key msg...");
        }
        else
        {
            __wrn("dwAddData1 invalid...");
            return EPDK_FAIL;
        }

        if (-1 == cur_key_val)
        {
            __wrn("after process, cur_key_val is -1...");
            return EPDK_OK;
        }

        eLIBs_memcpy(&mymsg, msg, sizeof(__gui_msg_t));
        mymsg.h_deswin = g_movie_ctrl->movie_manager;
        mymsg.id = GUI_MSG_KEY;
        mymsg.dwAddData1 = cur_key_val;
        mymsg.dwAddData2 = key_action;
        __wrn("tvout tp,GUI_PostMessage to movie manwin, mymsg.dwAddData1=0x%x, mymsg.dwAddData2=0x%x", mymsg.dwAddData1, mymsg.dwAddData2);
        GUI_PostMessage(&mymsg);
        esKRNL_TimeDly(10);
    }

    return EPDK_OK;
}



static __s32 __movie_switch_to_tv_or_lcd(movie_ctrl_t *movie_ctrl)
{
    __movie_delete_scene_berore_switch_tv(movie_ctrl);
    __movie_switch_output(movie_ctrl);
    gscene_hbar_set_state(HBAR_ST_HIDE);

    //return EPDK_OK;
    if (EPDK_TRUE == is_on_tv())
    {
        __gui_msg_t msg = {0};
        __s32 args[3] = { -1};
        msg.p_arg = args;
        __movie_tvout_tp_hook(&msg);
        __movie_install_set_cb_timmer(movie_ctrl);
        dsk_amplifier_onoff(0);//关闭功放
    }
    else
    {
        __wrn("before msg_emit_register_tvout_tp_hook");
        msg_emit_register_tvout_tp_hook(NULL);
        dsk_amplifier_onoff(1);//打开功放
    }

    __movie_create_scene_after_switch_tv(movie_ctrl);
    return EPDK_OK;
}

static __s32 __movie_save_last_playing_file(__gui_msg_t *msg)
{
    __s32 index;
    char file[RAT_MAX_FULL_PATH_LEN] = {0};
    root_para_t *root_para = NULL;
    root_para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);
    index = robin_npl_get_cur();

    if (-1 != index)
    {
        __s32 ret;
        ret = robin_npl_index2file(index, file);

        if (EPDK_OK == ret)
        {
            ret = dsk_reg_save_cur_play_info(REG_APP_MOVIE, index,
                                             file, root_para->root_type);

            if (EPDK_OK == ret)
            {
                __msg("dsk_reg_save_cur_play_info success,index=%d, file=%s, root=%d",
                      index, file, root_para->root_type);
                return EPDK_OK;
            }
            else
            {
                __msg("dsk_reg_save_cur_play_info fail...");
                return EPDK_FAIL;
            }
        }
        else
        {
            __msg("robin_npl_index2file fail...");
            return EPDK_FAIL;
        }
    }
    else
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    return EPDK_FAIL;
}

static __s32 __spsc_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch (msg_id)
    {
        case spsc_scene_msg_timeout:
        {
            __s32 ret;
            __msg("spsc_scene_msg_timeout");
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
            {
                //如果处于暂停状态，则创建播放状态子场景
                __cedar_status_t  status;
                status = robin_get_fsm_status();

                if (CEDAR_STAT_PAUSE == status)
                {
                    __msg("spsc time out, cedar in pause status, create play/pause scene...");
                    __movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
                }
            }
            return EPDK_OK;
        }

        case spsc_scene_msg_rr:
        {
            movie_ctrl->rr_flag = 1;
            return EPDK_OK;
        }

        case spsc_scene_msg_switch:
        {
            __s32 ret;
            __msg("spsc_scene_msg_switch ");
            movie_ctrl->sence_flag = 0;
            //__here__;
            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

            if (EPDK_FALSE == ret)
            {
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                if (EPDK_TRUE == ret)
                {
                    __movie_spsc_hide_layer_win(movie_ctrl->spsc_scene);
                }

                //__here__;
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                //__here__;
                __movie_sset_scene_create(movie_ctrl);
                //__here__;
                return EPDK_OK;
            }
            else
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));

                //是否是sset 中是tbar 的情况
                if (SSET_court_is_tbar() == EPDK_TRUE)
                {
                    //隐藏状态下show 它出来
                    if (SSET_tbar_is_show() == EPDK_FALSE)
                    {
                        SSET_show_tbar();
                        return EPDK_OK;
                    }
                }
                else
                {
                    __gui_msg_t my_msg_to_sset;
                    my_msg_to_sset.id = SSET_APP2SSET_PREV;
                    my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
                    my_msg_to_sset.h_srcwin = NULL;
                    GUI_SendMessage(&my_msg_to_sset);
                    return  EPDK_OK;
                }
            }

            return EPDK_OK;
        }

        case spsc_scene_msg_infor:
        {
            char *pstr;
            char *str_tmp;
            __msg("spsc_scene_msg_infor");
            __movie_spsc_scene_delete(movie_ctrl);

            if (1)
            {
                __s32 ret;
                __s32 index;
                char str_name[RAT_MAX_FULL_PATH_LEN] = {0};
                char str_name_tmp[1024] = {0};
                char strlentmp[RAT_MAX_FULL_PATH_LEN] = {0};
                char str_filelen[RAT_MAX_FULL_PATH_LEN] = {0};
                __s32 ret2;
                char video_bps[256] = {0};
                char strtmp[32];
                index = robin_npl_get_cur();

                if (-1 == index)
                {
                    __msg("robin_npl_get_cur fail...");
                    return EPDK_FAIL;
                }

                ret = robin_npl_index2file(index, str_name);

                if (-1 == ret)
                {
                    __msg("robin_npl_index2file fail, index=%d...", index);
                    return EPDK_FAIL;
                }

                __msg("str_name = %s", str_name);
                dsk_langres_get_menu_text(STRING_MOVIE_TITLE, str_name_tmp, sizeof(str_name_tmp));
                //__here__;
                __msg("str_name_tmp = %s", str_name_tmp);
                //eLIBs_memcpy(pstr, str_name_tmp, sizeof(str_name_tmp));
                pstr = str_name_tmp;
                __msg("pstr = %s", pstr);
                str_tmp = eLIBs_strchrlast(str_name, '\\');

                if (!str_tmp)
                {
                    __msg("eLIBs_strchrlast fail...");
                    return EPDK_FAIL;
                }

                __msg("pstr = %s", str_tmp);
                str_tmp++;
                __msg("pstr = %s", str_tmp);
                //eLIBs_strcat(pstr, str_tmp);
                eLIBs_strcat(pstr, "");
                eLIBs_strcat(pstr, "");
                ret = eLIBs_GetFileSize(str_name);
                filesize2str(ret, strlentmp);
                dsk_langres_get_menu_text(STRING_MOVIE_FILE_INFO, str_filelen, sizeof(str_filelen));
                eLIBs_strcat(str_filelen, strlentmp);
                __msg("str=%s", str_filelen);
                eLIBs_strcat(pstr, str_filelen);
                __msg("pstr = %s", pstr);
                eLIBs_strcat(pstr, "");
                ret2 = robin_get_video_bps();
                __msg("************robin_get_video_bps=%d*******", ret);

                if (0 == ret2)
                {
                    eLIBs_strcpy(strtmp, "");
                }
                else
                {
                    eLIBs_sprintf(strtmp, "%dkbps", (ret2 + 1023) / 1024);
                }

                //__here__;
                dsk_langres_get_menu_text(STRING_MOVIE_VIDEO_INFO, video_bps, sizeof(video_bps));
                eLIBs_strcat(video_bps, strtmp);
                __msg("str=%s", video_bps);
                eLIBs_strcat(pstr, video_bps);
                __msg("pstr = %s", pstr);
            }

            if (1)
            {
                __s32 lang_id[] = {STRING_MOVIE_TIPS, 0};
                __msg("++++++movie_spsc_ctrl_icon_infor++++++++");
                default_dialog_ex(h_infor, (H_WIN)movie_ctrl->movie_manager, MOVIE_SPSC_INFOR_ID, ADLG_OK, lang_id, pstr);
#if  0
                {
                    __gui_msg_t msg;
                    H_WIN hparent;
                    hparent = GUI_WinGetParent((H_WIN)movie_ctrl->movie_manager);

                    if (!hparent)
                    {
                        __err("hparent is null...");
                        return EPDK_FAIL;
                    }

                    __msg("++++++++++++++++++++++++++++++");
                    __msg("GUI_WinGetItemId(h_win) = %x", GUI_WinGetItemId(h_infor));
                    msg.h_deswin = (H_WIN)movie_ctrl->movie_manager;
                    msg.h_srcwin = NULL;
                    msg.id = GUI_MSG_COMMAND;
                    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(h_infor), 0);;
                    msg.dwAddData2 = 0;
                    msg.dwReserved = 0;
                    msg.p_arg      = (void *)(pstr);
                    GUI_SendMessage(&msg);
                }
#endif
                esKRNL_TimeDly(400);
                app_dialog_destroy(h_infor);
            }

            return EPDK_OK;
        }

        case spsc_scene_msg_tvout:
        {
            __movie_switch_to_tv_or_lcd(movie_ctrl);
            __msg("spsc_scene_msg_tvout");
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return EPDK_FAIL;
}

static __s32 __playsta_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch (msg_id)
    {
        case movie_playsta_scene_msg_timeout:
        {
            __s32 ret;
            __msg("movie_playsta_scene_msg_timeout");
            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYSTA_ID);

            if (EPDK_TRUE == ret)
            {
                //__here__;
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
            }

            return EPDK_OK;
        }

        case movie_playsta_scene_msg_clicked:
        {
            __msg("movie_playsta_scene_msg_playstaset");

            if (movie_playsta_play == dwAddData2)
            {
                robin_set_cmd_play();
            }
            else if (movie_playsta_pause == dwAddData2)
            {
                robin_set_cmd_pause();
            }
            else
            {
                __err("dwAddData2 invalid...");
            }

            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return EPDK_FAIL;
}

static __s32 __prog_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch (msg_id)
    {
        case movie_prog_scene_msg_timeout:
        {
            __s32 ret;
            __wrn("movie_prog_scene_msg_timeout");
            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);
            __wrn("ret=%d", ret);

            if (EPDK_TRUE == ret)
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
            }

            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return EPDK_FAIL;
}

static __s32 __movie_process_after_start_play(movie_ctrl_t *movie_ctrl)
{
    //创建字幕
    if (1 == movie_ctrl->sub_state)
    {
        if (robin_subtitle_exist() == EPDK_TRUE)
        {
            if (EPDK_FALSE == __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID))
            {
                __movie_sub_show_scene_create(movie_ctrl);
                //__movie_install_auto_play_timmer(movie_ctrl);
            }
        }
        else
        {
            if (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID))
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
                //__movie_uninstall_auto_play_timmer(movie_ctrl);
            }
        }
    }

    //while(1);
    //__movie_spsc_scene_create(movie_ctrl);//刚进入视频时，不显示控制菜单
    return EPDK_OK;
}

static __s32 __breakplay_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    switch (msg_id)
    {
        case ADLG_CMD_EXIT:
        {
            if (ADLG_IDYES == dwAddData2) //断点播放
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
                __movie_start_play(movie_ctrl, EPDK_TRUE);

                // __movie_process_after_start_play(movie_ctrl);
                if (robin_subtitle_exist() == EPDK_TRUE)
                {
                    if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) != EPDK_TRUE)
                    {
                        __movie_sub_show_scene_create(movie_ctrl);
                        //__movie_install_auto_play_timmer(movie_ctrl);
                    }
                }
                else
                {
                    if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) == EPDK_TRUE)
                    {
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
                        //__movie_uninstall_auto_play_timmer(movie_ctrl);
                    }
                }

                return EPDK_OK;
            }
            else//非断点播放
            {
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
                __movie_start_play(movie_ctrl, EPDK_FALSE);

                //__movie_process_after_start_play(movie_ctrl);
                if (robin_subtitle_exist() == EPDK_TRUE)
                {
                    if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) != EPDK_TRUE)
                    {
                        __movie_sub_show_scene_create(movie_ctrl);
                        //__movie_install_auto_play_timmer(movie_ctrl);
                    }
                }
                else
                {
                    if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) == EPDK_TRUE)
                    {
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
                        //__movie_uninstall_auto_play_timmer(movie_ctrl);
                    }
                }

                return EPDK_OK;
            }
        }

        default:
        {
            break;
        }
    }

    return EPDK_FAIL;
}

static __s32 __sset_scene_cmd_proc(movie_ctrl_t *movie_ctrl, __s32 msg_id, __s32 dwAddData2)
{
    reg_movie_para_t *para = NULL;
    para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

    if (MOVIE_SSET_NC_ID_UNKNOW == dwAddData2)
    {
        __wrn("MOVIE_SSET_NC_ID_UNKNOW == dwAddData2");
        return EPDK_OK;
    }

    switch (msg_id)
    {
        case SSET_NC_TIMER:
        {
            __wrn("@@@@@@@@@@@ SSET_NC_TIMER");
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SSET_ID);
            return EPDK_OK;
        }

        case MOVIE_SSET_NC_RETURN:
        {
            __msg(" MOVIE_SSET_NC_RETURN");
            __movie_exit_movie(movie_ctrl, 0);
            return EPDK_OK;
        }

        case MOVIE_SSET_NC_PRE:
        {
            __cedar_status_t fsm_sta;
            __msg(" MOVIE_SSET_NC_PRE");
            fsm_sta = robin_get_fsm_status();

            if (CEDAR_STAT_PLAY == fsm_sta)
            {
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                               ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
            }
            else if (CEDAR_STAT_PAUSE == fsm_sta)
            {
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                               ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
            }

            movie_cmd2sset(movie_ctrl->sset_sence, SSET_STRING_UPDATE,
                           STRING_MOVIE_PLAY, MOVIE_SSET_NC_NEXT, 1);

            if (CEDAR_STAT_PLAY == fsm_sta
                || CEDAR_STAT_PAUSE == fsm_sta)
            {
                __movie_play_prev(movie_ctrl);
                __msg("after __movie_play_prev");
            }
            else
            {
                __msg("fsm not in play/pause status, do not play next...");
            }

            return EPDK_OK;
        }

        case MOVIE_SSET_NC_PLAY:   //播放暂停
        {
            __cedar_status_t fsm_sta;
            __msg(" MOVIE_SSET_NC_PLAY");
            fsm_sta = robin_get_fsm_status();

            if (CEDAR_STAT_PLAY == fsm_sta)
            {
                robin_set_cmd_pause();
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                               ID_MOVIE_SSET_PAUSE_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS);
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_STRING_UPDATE,
                               STRING_MOVIE_STOP, 0, 0);
                __msg("after robin_set_cmd_play");
                __movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
            }
            else if (CEDAR_STAT_PAUSE == fsm_sta)
            {
                robin_set_cmd_play();
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                               ID_MOVIE_SSET_PLAY_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS);
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_STRING_UPDATE,
                               STRING_MOVIE_PLAY, 0, 0);
                __msg("after robin_set_cmd_play");
            }

            if (movie_ctrl->sset_sence) // 30234384
            {
                GUI_WinSetFocusChild(movie_ctrl->sset_sence);
            }

            return EPDK_OK;
        }

        case MOVIE_SSET_NC_NEXT:
        {
            __cedar_status_t fsm_sta;
            __msg(" MOVIE_SSET_NC_NEXT");
            fsm_sta = robin_get_fsm_status();

            if (CEDAR_STAT_PLAY == fsm_sta)
            {
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                               ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
            }
            else if (CEDAR_STAT_PAUSE == fsm_sta)
            {
                movie_cmd2sset(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                               ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
            }

            movie_cmd2sset(movie_ctrl->sset_sence, SSET_STRING_UPDATE,
                           STRING_MOVIE_PLAY, MOVIE_SSET_NC_NEXT, 1);

            if (CEDAR_STAT_PLAY == fsm_sta
                || CEDAR_STAT_PAUSE == fsm_sta)
            {
                __movie_play_next(movie_ctrl);
                __msg("after __movie_play_next");
            }
            else
            {
                __msg("fsm not in play/pause status, do not play next...");
            }

            return EPDK_OK;
        }

        case MOVIE_SSET_NC_PLAYMODE:
        {
            if (MOVIE_SSET_NC_ID_UNKNOW == dwAddData2)
            {
                return EPDK_OK;
            }

            if (dwAddData2 >= RAT_PLAY_MODE_MAX || dwAddData2 < 0)
            {
                __msg("dwAddData2 is invalid...");
                dwAddData2 = 0;
            }

            if (dwAddData2 == RAT_PLAY_MODE_EXIT)
            {
                app_movie_notify_delete_sub_scene(movie_ctrl->movie_manager);
            }
            else
            {
                __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_PLAY_MODE_ID, dwAddData2);
                robin_set_play_mode(dwAddData2);
                __movie_save_play_mode(dwAddData2);
            }

            return EPDK_OK;
        }

        case MOVIE_SSET_NC_CHANNEL:
        {
            if (MOVIE_SSET_NC_ID_UNKNOW == dwAddData2)
            {
                return EPDK_OK;
            }

            if (dwAddData2 == AUDIO_DEV_CHANNEL_EXIT)//exit
            {
                app_movie_notify_delete_sub_scene(movie_ctrl->movie_manager);
                return EPDK_OK;
            }

            if (dwAddData2 > 2
                || dwAddData2 < 0)
            {
                __msg("dwAddData2 is invalid...");
                dwAddData2 = 0;
            }

            __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_CHANNEL_ID, dwAddData2);
            robin_set_channel(dwAddData2);
            __movie_save_channel(dwAddData2);
            return EPDK_OK;
        }

        case MOVIE_SSET_NC_TRACK:
        {
            __s32 track;
            __audstream_profile_t   track_para;

            if (MOVIE_SSET_NC_ID_UNKNOW == dwAddData2)
            {
                return EPDK_OK;
            }

            eLIBs_memset(&track_para, 0x00, sizeof(track_para));
            robin_get_track_info(&track_para);
            __msg("track_para.nAudStrmNum=%d", track_para.nAudStrmNum);

            if (track_para.nAudStrmNum <= dwAddData2) //exit
            {
                app_movie_notify_delete_sub_scene(movie_ctrl->movie_manager);
                return EPDK_OK;
            }

            if (track_para.nAudStrmNum == 0)
            {
                return EPDK_OK;
            }

            if (dwAddData2 >= track_para.nAudStrmNum
                || dwAddData2 < 0)
            {
                __msg("dwAddData2 is invalid...");
                dwAddData2 = 0;
            }

            __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_TRACK_ID, dwAddData2);
            robin_select_track(dwAddData2);
            return EPDK_OK;
        }

        case MOVIE_SSET_NC_ZOOM:
        {
            __msg("++++++MOVIE_SSET_NC_ZOOM++++++");
            __msg("dwAddData2 = %d", dwAddData2);

            if (MOVIE_SSET_NC_ID_UNKNOW == dwAddData2)
            {
                return EPDK_OK;
            }

            if (dwAddData2 == ROBIN_ZOOM_EXIT)//exit
            {
                //__here__;
                __wrn("============app_movie_notify_delete_sub_scene ");
                app_movie_notify_delete_sub_scene(movie_ctrl->movie_manager);
                return EPDK_OK;
            }

            if (dwAddData2 < 0 || dwAddData2 > 4)
            {
                __msg("dwAddData2 is invalid...");
                return EPDK_OK;
            }

            __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_ZOOM_ID, dwAddData2);
            robin_set_zoom(dwAddData2);
            __movie_save_zoom_mode(dwAddData2);
            return EPDK_OK;
        }

        case MOVIE_SSET_NC_SUBTITLE:
        {
            if (MOVIE_SSET_NC_ID_UNKNOW == dwAddData2)
            {
                return EPDK_OK;
            }

            if (dwAddData2 == 3)//exit
            {
                app_movie_notify_delete_sub_scene(movie_ctrl->movie_manager);
                return EPDK_OK;
            }

            __wrn("dwAddData2=%d", dwAddData2);

            if (dwAddData2 < 0 || dwAddData2 > 2)
            {
                __msg("dwAddData2 is invalid...");
                return EPDK_OK;
            }

            __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_SUBTITLE_ID, dwAddData2);
            __wrn("movie_ctrl->sub_pos=%d", movie_ctrl->sub_pos);

            if (movie_ctrl->sub_pos == dwAddData2)
            {
                __wrn("sub pos not need change...");
                return EPDK_OK;
            }

            movie_ctrl->sub_pos = dwAddData2;

            if (1 == movie_ctrl->sub_state) //如果存在字幕场景，则需删除后重键，使得其颜色变化
            {
                if (dwAddData2 >= 0 && dwAddData2 <= 2)
                {
                    if (robin_subtitle_exist() == EPDK_TRUE)
                    {
                        if (EPDK_TRUE ==  __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID))
                        {
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SHOW_ID);
                        }

                        __movie_sub_show_scene_create(movie_ctrl);
                        SSET_set_curt_lyr_top();
                    }
                    else
                    {
                        if (EPDK_TRUE ==  __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SHOW_ID))
                        {
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
                            //__movie_uninstall_auto_play_timmer(movie_ctrl);
                        }
                    }

                    __movie_save_sub_pos(dwAddData2);
                }
                else
                {
                    __err("dwAddData2 invalid...");
                }
            }

            return EPDK_OK;
        }

        /*case MOVIE_SSET_NC_TVOUT:
        {
            __movie_switch_to_tv_or_lcd(movie_ctrl);
            return EPDK_OK;
        }*/
        case MOVIE_SSET_NC_AB:
        {
            int index;
            __s32 id_string = 0;
            index = movie_ctrl->sset_create_para->attr->n;

            if (MOVIE_MODE_AB == movie_ctrl->ab_mode)
            {
                movie_ctrl->ab_mode = MOVIE_MODE_A;
                robin_set_ab_a_v2();
                robin_set_audio_ab_mode(0);
                //movie_ctrl->sset_create_para->attr->childarray[index-1].item_attr.string[0]
                id_string   = STRING_MOVIE_A;
                //movie_ctrl->sset_create_para->attr->childarray[index-1].item_attr.string[1]
                //  = STRING_MOVIE_A;
            }
            else if (MOVIE_MODE_A == movie_ctrl->ab_mode)
            {
                movie_ctrl->ab_mode = MOVIE_MODE_B;
                robin_set_ab_loop_count_v2(1000);
                robin_set_ab_b_v2();
                robin_enable_ab();
                //movie_ctrl->sset_create_para->attr->childarray[index-1].item_attr.string[0]
                id_string   = STRING_MOVIE_B;
                //movie_ctrl->sset_create_para->attr->childarray[index-1].item_attr.string[1]
                //  = STRING_MOVIE_B;
            }
            else if (MOVIE_MODE_B == movie_ctrl->ab_mode)
            {
                movie_ctrl->ab_mode = MOVIE_MODE_AB;
                robin_cancle_ab_v2();
                //movie_ctrl->sset_create_para->attr->childarray[index-1].item_attr.string[0]
                id_string   = STRING_MOVIE_AB;
                //movie_ctrl->sset_create_para->attr->childarray[index-1].item_attr.string[1]
                //  = STRING_MOVIE_AB;
            }

            //__app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_AB_ID, movie_ctrl->ab_mode);
            {
                /*setmsg.id             =  SSET_IN_INFO_UPDATE;
                setmsg.h_srcwin     =  msg->h_deswin;
                setmsg.h_deswin     =  sset_data.info_fwin;
                setmsg.dwAddData1   =  msg->dwAddData1;
                setmsg.dwAddData2   =  0;
                setmsg.p_arg        =  NULL;

                GUI_SendNotifyMessage(&setmsg); */
                __gui_msg_t my_msg_to_sset;
                eLIBs_memset(&my_msg_to_sset, 0, sizeof(__gui_msg_t));
                my_msg_to_sset.id = SSET_STRING_UPDATE;
                my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
                my_msg_to_sset.h_srcwin = NULL;
                my_msg_to_sset.dwAddData1 = id_string;
                GUI_SendMessage(&my_msg_to_sset);
            }
        }

        default:
        {
            break;
        }
    }

    return EPDK_FAIL;
}

//-1:无  0:左  1:上  2:右  3:下
static __s32 __app_movie_touch_direct_identify(__s32 dx, __s32 dy)
{
    if (abs(abs(dx) - abs(dy)) <= 1 ||
        ((double)abs(dx) / abs(dy) > 0.577 //tan(30)
         && (double)abs(dx) / abs(dy) < 1.732) //ctan(30) //抛弃大于30度角小于90-30度角的曲线
       )
    {
        __wrn("direct_identify fail, abs(abs(dx)-abs(dy)) <= 1");
        __wrn("dx=%d", dx);
        __wrn("dy=%d", dy);
        return -1;
    }

    if (abs(dx) > abs(dy))
    {
        if (dx > 0)
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }
    else if (abs(dx) < abs(dy))
    {
        if (dy > 0)
        {
            return 3;
        }
        else
        {
            return 1;
        }
    }

    __wrn("direct_identify fail, abs(dx) > abs(dy)==abs(dx) < abs(dy)");
    return -1;
}

static __s32 __app_movie_time2int(__awos_time_t *time)
{
    __s32 second;

    if (NULL == time)
    {
        return 0;
    }

    second = 0;
    second += time->hour;
    second *= 60;
    second += time->minute;
    second *= 60;
    second += time->second;
    return second;
}

//-1:无  0:左  1:上  2:右  3:下
static __s32 __app_movie_touch_identify(__gui_msg_t *msg)
{
    static __s32 touch_down_x = -1;
    static __s32 touch_down_y = -1;
    static __s32 is_touch_valid = 0;//代表多次判断过程当前状态是否正常
    static __s32 last_x = -1;
    static __s32 last_y = -1;
    static __s32 last_move_direct = -1;
    __s32 cur_move_direct = -1;
    __s32 cur_x, cur_y;
    __s32 ret;
    static __awos_time_t touch_down_time;
    __awos_time_t touch_up_time;
    __bool touch_timeout = 0;;

    if (1 == is_touch_ff_rr_valid)
    {
        return EPDK_FAIL;
    }

    ret = -1;
    cur_x = LOSWORD(msg->dwAddData2);
    cur_y = HISWORD(msg->dwAddData2);

    if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1
        || GUI_MSG_TOUCH_OVERDOWN == msg->dwAddData1)
    {
        __wrn("__app_movie_touch_identify, touch down...");

        if (msg->h_deswin != GUI_WinGetCaptureWin())
        {
            //  GUI_WinSetCaptureWin(msg->h_deswin);
        }

        touch_down_x = cur_x;
        touch_down_y = cur_y;
        last_x = cur_x;
        last_y = cur_y;
        is_touch_valid = 1;
        eLIBs_memset(&touch_down_time, 0, sizeof(__awos_time_t));
        esTIME_GetTime(&touch_down_time);
    }
    else if (GUI_MSG_TOUCH_UP == msg->dwAddData1
             || GUI_MSG_TOUCH_OVERUP == msg->dwAddData1)
    {
        __wrn("__app_movie_touch_identify, touch up...");

        if (msg->h_deswin == GUI_WinGetCaptureWin())
        {
            // GUI_WinReleaseCapture();
        }

        eLIBs_memset(&touch_up_time, 0, sizeof(__awos_time_t));
        esTIME_GetTime(&touch_up_time);
        touch_timeout = 0;

        if (__app_movie_time2int(&touch_up_time)
            - __app_movie_time2int(&touch_down_time) > 4)
        {
            touch_timeout = 1;
        }

        __wrn("touch_timeout=%d", touch_timeout);
        __wrn("is_touch_valid=%d", is_touch_valid);
        __wrn("touch_down_x=%d", touch_down_x);
        __wrn("touch_down_y=%d", touch_down_y);

        if (0 == touch_timeout
            && 1 == is_touch_valid
            && -1 != touch_down_x
            && -1 != touch_down_y)
        {
            __wrn("test");

            if (abs(cur_x - touch_down_x) > 10
                || abs(cur_y - touch_down_y) > 10)
            {
                __wrn("test");

                if (abs(cur_x - touch_down_x) > abs(cur_y - touch_down_y))
                {
                    __wrn("test");

                    if (abs(cur_x - touch_down_x) > 480 / 5)
                    {
                        if (cur_x > touch_down_x) //-->
                        {
                            ret = 2;
                        }
                        else//<--
                        {
                            ret = 0;
                        }
                    }
                    else
                    {
                        __wrn("identify fail, abs(cur_x-touch_down_x) <= 480/5");
                    }
                }
                else
                {
                    if (abs(cur_y - touch_down_y) > 272 / 5)
                    {
                        if (cur_y > touch_down_y) //down
                        {
                            ret = 3;
                        }
                        else//up
                        {
                            ret = 1;
                        }
                    }
                    else
                    {
                        __wrn("identify fail, abs(cur_y-touch_down_y) <= 272/5");
                    }
                }
            }
            else
            {
                __wrn("identify fail, distance is too small...");
            }
        }

        touch_down_x = -1;
        touch_down_y = -1;
        last_x = -1;
        last_y = -1;
        is_touch_valid = 0;
        last_move_direct = -1;
    }
    else if (GUI_MSG_TOUCH_MOVE == msg->dwAddData1
             || GUI_MSG_TOUCH_OVERMOVE == msg->dwAddData1)
    {
        __wrn("__app_movie_touch_identify, touch move...");

        if (-1 != last_x
            && -1 != last_y)
        {
            __wrn("test");

            if (abs(cur_x - last_x) > 5
                || abs(cur_y - last_y) > 5)
            {
                __wrn("test");

                if (1 == is_touch_valid)
                {
                    __wrn("test");
                    cur_move_direct = __app_movie_touch_direct_identify(cur_x - last_x, cur_y - last_y);
                    __wrn("cur_move_direct=%d", cur_move_direct);

                    if (-1 == cur_move_direct)
                    {
                        __wrn("cur_move_direct invalid...");
                        is_touch_valid = 0;
                    }
                    else
                    {
                        if (-1 != last_move_direct
                            && last_move_direct != cur_move_direct)
                        {
                            __wrn("last_move_direct=%d", last_move_direct);
                            __wrn("cur_move_direct invalid...");
                            is_touch_valid = 0;
                        }

                        last_x = cur_x;
                        last_y = cur_y;
                        last_move_direct = cur_move_direct;
                    }
                }
            }
        }
        else
        {
            last_x = cur_x;
            last_y = cur_y;
        }
    }
    else
    {
        __wrn("__app_movie_touch_identify, other touch msg...");
    }

    return ret;
}

void BSP_disp_print_regs(__u32 base, __u32 size)
{
    __u32 i = 0;
    unsigned char str[20];
#define sys_get_wvalue(n)   (*((volatile __u32 *)(unsigned long)(n)))          /* word input */

    for (i = 0; i < size; i += 16)
    {
        __u32 reg[4];
        reg[0] = sys_get_wvalue(base + i);
        reg[1] = sys_get_wvalue(base + i + 4);
        reg[2] = sys_get_wvalue(base + i + 8);
        reg[3] = sys_get_wvalue(base + i + 12);

        if (((base + i) & 0xff) == 0)
        {
            __msg("");
        }

        __msg("\t%08x :  %08x | %08x | %08x | %08x", (__u32)((base + i) & 0xfff), reg[0], reg[1], reg[2], reg[3]);
    }
}

//是否有除字幕以外的子场景
static __bool __app_has_visuable_sub_scene_except_x(movie_ctrl_t *movie_ctrl, __u32 sub_scene)
{
    __s32 ret;
    __wrn("__app_has_visuable_sub_scene_except_subtitle...");
    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~sub_scene) & (~MOVIE_SSET_ID));
    __wrn("ret=%d", ret);

    if (EPDK_TRUE == ret)
    {
        //__here__;
        return EPDK_TRUE;
    }

    //__here__;
    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);
    __wrn("ret=%d", ret);

    if (EPDK_TRUE == ret
        && SSET_court_is_tbar() == EPDK_TRUE
        && SSET_tbar_is_show() == EPDK_TRUE)
    {
        __cedar_status_t fsm_sta;
        /*fsm_sta = robin_get_fsm_status();
        if (CEDAR_STAT_PLAY == fsm_sta)
        {
            movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL );
            //movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
            //  STRING_MOVIE_PLAY, MOVIE_SSET_NC_PLAY, 1 );
        }
        else if (CEDAR_STAT_PAUSE == fsm_sta)
        {
            movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                ID_MOVIE_SSET_PAUSE_UF_BMP, MOVIE_SSET_NC_PLAY,FOCUSE_NORMAL);
        //  movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
        //      STRING_MOVIE_STOP, MOVIE_SSET_NC_PLAY, 1 );
        } */
        return EPDK_TRUE;
    }
    else if (EPDK_TRUE == ret                       //使其tlist状态也不行操作其他动作
             && SSET_court_is_tbar() != EPDK_TRUE
             && SSET_tbar_is_show() != EPDK_TRUE)
    {
        //__here__;
        return EPDK_TRUE;
    }

    //__here__;
    return EPDK_FALSE;
}

//=====================================30234384

static __s32 movie_digit_layer_draw(__gui_msg_t *msg)
{
    __s32    i;
    __s32    j;
    RECT     rect;
    char     text[256];
    GUI_RECT gui_rect;
    __s32    total_movie_num;
    movie_ctrl_t *movie_ctrl;
    __wrn("&&&&&&&& movie_digit_layer_draw ");
    movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (!movie_ctrl)
    {
        __err("movie_ctrl is null...");
        return EPDK_OK;
    }

    if (NULL == movie_ctrl->hdigit_layer)
    {
        __wrn("digit layer is null...");
        return EPDK_FAIL;
    }

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(movie_ctrl->hdigit_layer))
    {
        __wrn("digit layer is not on...");
        return EPDK_FAIL;
    }

    total_movie_num = robin_npl_get_total_count();

    if (movie_ctrl->ndigit_movie_num > total_movie_num)
    {
        dsk_langres_get_menu_text(STRING_NO_THIS_MOVIE, text, sizeof(text));
    }
    else
    {
        eLIBs_memset(text, 0, sizeof(text));
        eLIBs_int2str_dec(movie_ctrl->ndigit_movie_num, text);
    }

    GUI_LyrWinSel(movie_ctrl->hdigit_layer);
    GUI_SetFont(SWFFont);
    GUI_SetColor(GUI_RED);
    GUI_CharSetToEncode(dsk_get_langres_charset());
    GUI_SetBkColor(0x0);
    GUI_Clear();
    GUI_LyrWinGetScnWindow(movie_ctrl->hdigit_layer, &rect);
    __wrn("======text=%s======", text);
    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + rect.width - 1;
    gui_rect.y1 = gui_rect.y0 + rect.height - 1;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_LyrWinSetTop(movie_ctrl->hdigit_layer);
    return EPDK_OK;
}


static __s32 __movie_play_by_index(movie_ctrl_t *movie_ctrl, __u32 movie_index)
{
    __s32 ret;
    __s32 index;
    __s32 total;

    if (MOVIE_MODE_AB != movie_ctrl->ab_mode)
    {
        __gui_msg_t my_msg_to_sset;
        movie_ctrl->ab_mode = MOVIE_MODE_AB;
        robin_cancle_ab_v2();
        eLIBs_memset(&my_msg_to_sset, 0, sizeof(__gui_msg_t));
        my_msg_to_sset.id = SSET_STRING_UPDATE;
        my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
        my_msg_to_sset.h_srcwin = NULL;
        my_msg_to_sset.dwAddData1 = STRING_MOVIE_AB;
        GUI_SendMessage(&my_msg_to_sset);
    }

    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    __movie_delete_breakpoint_info(index);
    total = robin_npl_get_total_count();

    if (-1 == total)
    {
        __msg("robin_npl_get_total_count fail...");
        return EPDK_FAIL;
    }

    //index++;
    index = movie_index;

    if (index >= total)
    {
        //index = 0;
        return EPDK_FAIL;
    }
    else if (0 == index)
    {
        return EPDK_FAIL; //如果输入第0  个,直接返回
    }
    else
    {
        index --;
    }

    robin_npl_set_cur(index);
    ret = __movie_play_file(movie_ctrl, index, EPDK_FALSE);
    return ret;
}


__s32 movie_delete_digit_layer(__gui_msg_t *msg)
{
    movie_ctrl_t *movie_ctrl;
    __wrn("&&&&&&&& movie_delete_digit_layer");
    movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (!movie_ctrl)
    {
        __err("movie_ctrl is null...");
        return EPDK_OK;
    }

    if (movie_ctrl->hdigit_layer)
    {
        GUI_LyrWinDelete(movie_ctrl->hdigit_layer);
        movie_ctrl->hdigit_layer = NULL;
    }

    if (movie_ctrl->digit_timmer_id)
    {
        if (GUI_IsTimerInstalled(msg->h_deswin, movie_ctrl->digit_timmer_id))
        {
            GUI_KillTimer(msg->h_deswin, movie_ctrl->digit_timmer_id);
        }
    }

    __wrn("movie_delete_digit_layer end...");
    return EPDK_OK;
}

static __s32 __app_movie_show_dialog(H_WIN movie_manager, __s32 win_id, __s32  item_id)
{
    H_WIN h_dialog = NULL;
    __s32 lang_id[] = {STRING_MOVIE_TIPS, 0};

    switch (win_id)
    {
        case MOVIE_CHANNEL_ID:
        {
            switch (item_id)
            {
                case AUDIO_DEV_CHANNEL_STEREO:
                    lang_id[1] = STRING_MOVIE_CHANNEL_STEREO;
                    break;

                case AUDIO_DEV_CHANNEL_LEFT:
                    lang_id[1] = STRING_MOVIE_CHANNEL_LEFT;
                    break;

                case AUDIO_DEV_CHANNEL_RIGHT:
                    lang_id[1] = STRING_MOVIE_CHANNEL_RIGHT;
                    break;

                default:
                    lang_id[1] = 0;
                    break;
            }
        }
        break;

        case MOVIE_TRACK_ID:
        {
            switch (item_id)
            {
                case 0:
                    lang_id[1] = STRING_MOVIE_TRACK0;
                    break;

                case 1:
                    lang_id[1] = STRING_MOVIE_TRACK1;
                    break;

                case 2:
                    lang_id[1] = STRING_MOVIE_TRACK2;
                    break;

                default:
                    lang_id[1] = 0;
                    break;
            }
        }
        break;

        case MOVIE_PLAY_MODE_ID:
        {
            switch (item_id)
            {
                case RAT_PLAY_MODE_ONLY_ONCE:
                    lang_id[1] = STRING_MOVIE_ONLY_ONE;
                    break;

                case RAT_PLAY_MODE_ROTATE_ONE:
                    lang_id[1] = STRING_MOVIE_ROTATE_ONE;
                    break;

                case RAT_PLAY_MODE_ROTATE_ALL:
                    lang_id[1] = STRING_MOVIE_ROTATE_ALL;
                    break;

                case RAT_PLAY_MODE_SEQUENCE:
                    lang_id[1] = STRING_MOVIE_SEQUENCE;
                    break;

                case RAT_PLAY_MODE_RANDOM:
                    lang_id[1] = STRING_MOVIE_RANDOM;
                    break;

                default:
                    lang_id[1] = 0;
                    break;
            }
        }
        break;

        case MOVIE_ZOOM_ID:
        {
            switch (item_id)
            {
                case ROBIN_ZOOM_FULL_SCREEN_VIDEO_RATIO:
                    lang_id[1] = STRING_MOVIE_VIDEO_RATIO;
                    break;

                case ROBIN_ZOOM_FULL_SCREEN_SCREEN_RATIO:
                    lang_id[1] = STRING_MOVIE_SCREEN_RATIO;
                    break;

                case ROBIN_ZOOM_ORIGIN_SIZE:
                    lang_id[1] = STRING_MOVIE_ORGIN_SIZE;
                    break;

                case ROBIN_ZOOM_4R3:
                    lang_id[1] = STRING_MOVIE_4R3;
                    break;

                case ROBIN_ZOOM_16R9:
                    lang_id[1] = STRING_MOVIE_16R9;
                    break;

                default:
                    lang_id[1] = 0;
                    break;
            }
        }
        break;

        case MOVIE_SUBTITLE_ID:
        {
            switch (item_id)
            {
                case 0:
                    lang_id[1] = STRING_MOVIE_SUB_BOTTOM;
                    break;

                case 1:
                    lang_id[1] = STRING_MOVIE_SUB_MID;
                    break;

                case 2:
                    lang_id[1] = STRING_MOVIE_SUB_TOP;
                    break;

                default:
                    lang_id[1] = 0;
                    break;
            }
        }
        break;

        case MOVIE_AB_ID:
        {
            switch (item_id)
            {
                case MOVIE_MODE_AB:
                    lang_id[1] = STRING_MOVIE_AB;
                    break;

                case MOVIE_MODE_A :
                    lang_id[1] = STRING_MOVIE_A;
                    break;

                case MOVIE_MODE_B:
                    lang_id[1] = STRING_MOVIE_B;
                    break;

                default:
                    lang_id[1] = 0;
                    break;
            }
        }
        break;

        default:
        {
            lang_id[1] = 0;
        }
        break;
    }

    default_dialog(h_dialog, movie_manager, win_id, ADLG_OK, lang_id);
    esKRNL_TimeDly(50);

    if (h_dialog)
    {
        app_dialog_destroy(h_dialog);
        h_dialog = NULL;
    }

    return EPDK_OK;
}

//1~128
static void __movie_ff_rr_set_speed(__u8 multiple)
{
    __u32 total_time = robin_get_total_time() / 1000;
    total_time = total_time / 60;
    multiple = (multiple > 0) ? multiple : 1;
    __msg("multiple================%d ", multiple);

    if (total_time <= 10)
    {
        robin_set_ff_rr_speed((4 * multiple < 128) ? (4 * multiple) : 128);
    }
    else if (total_time <= 30)
    {
        robin_set_ff_rr_speed((16 * multiple < 128) ? (16 * multiple) : 128);
    }
    else if (total_time <= 90)
    {
        robin_set_ff_rr_speed((32 * multiple < 128) ? (32 * multiple) : 128);
    }
    else if (total_time <= 180)
    {
        robin_set_ff_rr_speed((64 * multiple < 128) ? (64 * multiple) : 128);
    }
    else //if(total_time <= 1)
    {
        robin_set_ff_rr_speed(128);
    }
}
//=====================================

static __s32 __app_movie_proc(__gui_msg_t *msg)
{
    __wrn("msgid=%d, h_deswin=%x, dwAddData1=%d, dwAddData2=%d"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __s32 ret;
            movie_ctrl_t *movie_ctrl = NULL;
            root_para_t *para = NULL;
            g_disable_close_scn();
            __movie_set_title(STRING_MOVIE_TITLE);
            //gscene_hbar_set_state(HBAR_ST_HIDE);
            //gscene_bgd_set_state(BGD_STATUS_HIDE);
            __msg("GUI_MSG_CREATE begin");
            movie_ctrl = esMEMS_Malloc(0, sizeof(movie_ctrl_t));

            if (NULL == movie_ctrl)
            {
                __msg("mem not enough...");
                return EPDK_FAIL;
            }

            eLIBs_memset(movie_ctrl, 0, sizeof(movie_ctrl_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)movie_ctrl);
            g_movie_ctrl = movie_ctrl;
            para = (root_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (NULL != para)
            {
                movie_ctrl->mode = para->tv_mode;
                para->tv_mode = MOVIE_MODE_NORMAL;//置成初始状态
            }
            else
            {
                movie_ctrl->mode = MOVIE_MODE_NORMAL;
            }

            if (para != NULL) //add by chenguofeng 2013.2.23
            {
                if (RAT_TF == para->root_type)
                {
                    movie_ctrl->root_type = RAT_TF;
                }
                else if (RAT_USB == para->root_type)
                {
                    movie_ctrl->root_type  = RAT_USB;
                }
                else
                {
                    movie_ctrl->root_type = RAT_UNKNOWN;
                }
            }

            __movie_robin_sem_init();
            movie_ctrl->sence_flag = 0;
            movie_ctrl->movie_manager = msg->h_deswin;
            __movie_ctrl_para_init(movie_ctrl);
            __app_movie_reg_para_init(movie_ctrl);

            //////com_set_palette_by_id(ID_MOVIE_PAL_BMP);//设置调色板
            {
                //修改调色板
                //背景色，灰白色，字幕边框颜色
                /////   __u32 color[]={BEETLES_BLACK_COLOR, BEETLES_GRAY_COLOR, 0xC0323232};
                /////// com_set_palette_color_by_index(0xF0, color, BEETLES_TBL_SIZE(color));
            }
            ret = __movie_cur_file_has_breakpoint();

            __msg("__movie_cur_file_has_breakpoint=%d", ret);

            if (1 == MOVIE_ENABLE_BREAKPLAY && EPDK_TRUE == ret) //弹框让用户选择是否断点播放
            {
                //弹框
                ret = __movie_breakplay_scene_create(movie_ctrl);

                if (EPDK_FAIL == ret)
                {
                    __msg("__movie_breakplay_scene_create fail, exit movie...");
                    __movie_exit_movie(movie_ctrl, 0);
                    return EPDK_OK;
                }
            }
            else//不弹框，默认有断点则断点播放，无断点则从头播放
            {
                __movie_start_play(movie_ctrl, ret);

                //__movie_process_after_start_play(movie_ctrl);
                if (robin_subtitle_exist() == EPDK_TRUE)
                {
                    if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) != EPDK_TRUE)
                    {
                        __movie_sub_show_scene_create(movie_ctrl);
                        //__movie_install_auto_play_timmer(movie_ctrl);
                    }
                }
                else
                {
                    if (__movie_has_sub_scene(movie_ctrl, MOVIE_SUBTITLE_ID) == EPDK_TRUE)
                    {
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUBTITLE_ID);
                        //__movie_uninstall_auto_play_timmer(movie_ctrl);
                    }
                }
            }

            __wrn("********movie_ctrl->mode=%d**********", movie_ctrl->mode);

            if (MOVIE_MODE_TV == movie_ctrl->mode) //切换到tv
            {
                __movie_install_set_cb_timmer(movie_ctrl);
            }

            is_touch_ff_rr_valid = 0;
            //while(1);
            //esKRNL_TimeDly(1000);
            //dsk_amplifier_onoff(1);//打开功放,播放暂停处理较复杂，robin处理
            __msg("GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            __s32 ret;
            static __s32 last_key = -1;
            movie_ctrl_t *movie_ctrl;
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __err("movie_ctrl is null...");
                return EPDK_OK;
            }

            //__msg("msg->dwAddData1=====================%d ",msg->dwAddData1);
            if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
            {
                if (!((msg->dwAddData1 == GUI_MSG_KEY_ESCAPE) || (msg->dwAddData1 == GUI_MSG_KEY_ENTER)))
                {
                    last_key = -1;
                    //__msg("last_key=====================%d ",last_key);
                    return EPDK_OK;
                }
            }

            {
                //如果有断点播放对话框的子场景，则让按键消息全部往下传
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);

                if (EPDK_TRUE == ret)
                {
                    goto key_fail;
                }
            }

#if 0

            if (0)
            {
                //屏蔽spsc场景的按键消息
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                if (EPDK_TRUE == ret)
                {
                    //__here__;
                    return EPDK_OK;
                }
            }

            //__here__;
            if (1)
            {
                //屏蔽ssset场景的多余按键消息
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                if ((
                        EPDK_TRUE == ret
                        && SSET_court_is_tbar() == EPDK_TRUE
                        && SSET_tbar_is_show() == EPDK_TRUE))
                {
                    //__here__;
                    if (GUI_MSG_KEY_MENU != msg->dwAddData1
                        && GUI_MSG_KEY_LONGMENU != msg->dwAddData1
                        && GUI_MSG_KEY_ESCAPE != msg->dwAddData1
                        && GUI_MSG_KEY_LONGESCAPE != msg->dwAddData1)
                    {
                        //__here__;
                        return EPDK_OK;
                    }

                    //__here__;
                }
            }

            //__here__;

            if (0 &&
                (KEY_UP_ACTION == msg->dwAddData2
                 || KEY_REPEAT_ACTION == msg->dwAddData2))
            {
                static __s32 xval = 0;

                if (GUI_MSG_KEY_VADD == last_key
                    || GUI_MSG_KEY_LONGVADD == last_key)
                {
                    __s32           ret;
                    __s32           args[3] = {0};
                    __disp_rect_t   disp_rect;
                    __disp_layer_info_t disp_lyr_info;
                    args[0] = movie_ctrl->hvedio_lyr;
                    args[1] = (__s32)&disp_rect;
                    args[2] = 0;
                    ret = esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_LAYER_GET_SCN_WINDOW, 0, (void *)args);
                    __wrn("disp_rect.x=%d, disp_rect.y=%d,disp_rect.width=%d,disp_rect.height=%d",
                          disp_rect.x, disp_rect.y, disp_rect.width, disp_rect.height);
                    disp_rect.x++;
                    ret = esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_LAYER_GET_SCN_WINDOW, 0, (void *)args);
                    __wrn("disp_rect.x=%d, disp_rect.y=%d,disp_rect.width=%d,disp_rect.height=%d",
                          disp_rect.x, disp_rect.y, disp_rect.width, disp_rect.height);
                    //__wrn("DE_FE:");
                    //BSP_disp_print_regs(0xf1e00000, 0xf1e0077c-0xf1e00000);
                    //__wrn("DE_BE:");
                    //BSP_disp_print_regs(0xf1e60800, 0xf1e60970-0xf1e60800);
                    return EPDK_OK;
                }
                else if (GUI_MSG_KEY_VDEC == last_key
                         || GUI_MSG_KEY_LONGVDEC == last_key)
                {
                    __s32           ret;
                    __s32           args[3] = {0};
                    __disp_rect_t   disp_rect;
                    args[0] = movie_ctrl->hvedio_lyr;
                    args[1] = (__s32)&disp_rect;
                    args[2] = 0;
                    ret = esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_LAYER_GET_SCN_WINDOW, 0, (void *)args);
                    __wrn("disp_rect.x=%d, disp_rect.y=%d,disp_rect.width=%d,disp_rect.height=%d",
                          disp_rect.x, disp_rect.y, disp_rect.width, disp_rect.height);
                    disp_rect.x--;
                    ret = esMODS_MIoctrl(movie_ctrl->hdisp, DISP_CMD_LAYER_SET_SCN_WINDOW, 0, (void *)args);
                    __wrn("disp_rect.x=%d, disp_rect.y=%d,disp_rect.width=%d,disp_rect.height=%d",
                          disp_rect.x, disp_rect.y, disp_rect.width, disp_rect.height);
                    //__wrn("DE_FE:");
                    //
                    //BSP_disp_print_regs(0xf1e00000, 0xf1e0077c-0xf1e00000);
                    //__wrn("DE_BE:");
                    //BSP_disp_print_regs(0xf1e60800, 0xf1e60970-0xf1e60800);
                    return EPDK_OK;
                }
            }

#endif

            //            __wrn("GUI_MSG_KEY, msg->dwAddData1=0x%x, msg->dwAddData2=0x%x", msg->dwAddData1, msg->dwAddData2);

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                /*else if (GUI_MSG_KEY_RR == last_key
                    ||GUI_MSG_KEY_LONGRR == last_key)
                {
                    __cedar_status_t sta;
                    sta = robin_get_fsm_status();
                    if(CEDAR_STAT_PLAY == sta)
                    {
                        robin_set_cmd_play();
                    }
                    goto key_ok;
                }
                else if (GUI_MSG_KEY_FF== last_key
                    ||GUI_MSG_KEY_LONGFF == last_key)
                {
                    __cedar_status_t sta;
                    sta = robin_get_fsm_status();
                    if(CEDAR_STAT_PLAY == sta)
                    {
                        robin_set_cmd_play();
                    }
                    goto key_ok;
                }*/
                __wrn("&&&&&&& last kay = %d ", last_key);

                if (GUI_MSG_KEY_LONGPREV == last_key || GUI_MSG_KEY_LONGNEXT == last_key
                    || GUI_MSG_KEY_LONGFF == last_key || GUI_MSG_KEY_LONGRR == last_key
                    || GUI_MSG_KEY_FF == last_key || GUI_MSG_KEY_RR == last_key
                    || GUI_MSG_KEY_LONGDOWN == last_key
                    || GUI_MSG_KEY_LONGUP == last_key
                    || GUI_MSG_KEY_LONGLEFT == last_key
                    || GUI_MSG_KEY_LONGRIGHT == last_key)
                {
#if 0
                    movie_ctrl->rr_flag = 0;
                    movie_ctrl->ff_rr_cnt = 0;
                    __movie_prog_scene_enable_auto_hide(movie_ctrl, EPDK_TRUE);
                    ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PROG_ID | MOVIE_PLAYSTA_ID);

                    if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，进度条，则恢复快退快进后的正常播放
                    {
                        __cedar_status_t fsm_sta;
                        fsm_sta = robin_get_fsm_status();

                        if (CEDAR_STAT_RR == fsm_sta
                            || CEDAR_STAT_FF == fsm_sta)
                        {
                            robin_set_cmd_play();
                            __msg("after robin_set_cmd_play");
                        }
                        else
                        {
                            __msg("fsm not in ff/rr status, do not set play cmd");
                        }

                        goto key_ok;
                    }

#else
                    ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                    if (EPDK_FALSE == ret)
                    {
                        if (!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
                        {
                            GUI_SetTimer(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id, 100, NULL);
                        }
                    }

#endif
                }
                else if (GUI_MSG_KEY_NEXT == last_key)
                    //case GUI_MSG_KEY_LONGNEXT:
                {
                    __movie_play_next(movie_ctrl);
                    goto key_ok;
                    //break;
                }
                else if (GUI_MSG_KEY_PREV == last_key)
                    //case GUI_MSG_KEY_LONGPREV:
                {
                    __movie_play_prev(movie_ctrl);
                    goto key_ok;
                    //break;
                }
                else if (GUI_MSG_KEY_ACCOMP_ONOFF == last_key)  //切换音轨//add by chenguofeng 2011.2.23
                {
                    __s32 i;
                    __s32 track_index;
                    __u8  track_num;
                    __audstream_profile_t   track_para;
                    robin_get_track_info(&track_para);
                    track_num = track_para.nAudStrmNum;
                    __wrn("@@@@@@@ track_para.nAudStrmNum = %d", track_para.nAudStrmNum);

                    if (0 == track_para.nAudStrmNum)
                    {
                        __wrn("********** nAudStrmNum == 0 ");
                        //return EPDK_OK;
                        goto key_ok;
                    }

                    if (/*track_num < 0 || */track_num > 3) //限定3  个音轨
                    {
                        __wrn("********* the track num > 3");
                        track_num = 3;
                    }

                    track_index = robin_get_track_index();
                    __wrn("@@@@@@@@ get track index = %d", track_index);
                    track_index++;

                    if (track_index < 0 || track_index >= track_num)
                    {
                        __wrn("********* invalid track index ");
                        track_index = 0;
                    }

                    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                    __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_TRACK_ID, track_index);
                    __wrn("@@@@@@@ robin_select_track index = %d", track_index);
                    robin_select_track(track_index);
                    __movie_save_track(track_index);
                    goto key_ok;
                }
                else if (GUI_MSG_KEY_LEFT ==  last_key
                         || GUI_MSG_KEY_UP ==  last_key)
                {
                    __wrn("+++++GUI_MSG_KEY_LEFT ++++++");
                    ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PLAYSTA_ID | MOVIE_PROG_ID);

                    if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，状态图标、进度条、则处理上一个下一个
                    {
                        __cedar_status_t fsm_sta;
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                        fsm_sta = robin_get_fsm_status();

                        if (CEDAR_STAT_PLAY == fsm_sta
                            || CEDAR_STAT_PAUSE == fsm_sta)
                        {
                            __movie_play_prev(movie_ctrl);
                            __msg("after __movie_play_prev");
                        }
                        else
                        {
                            __msg("fsm not in play/pause status, do not play next...");
                        }

                        goto key_ok;
                    }
                }
                else if (GUI_MSG_KEY_RIGHT ==  last_key
                         || GUI_MSG_KEY_DOWN ==  last_key)
                {
                    __wrn("+++++GUI_MSG_KEY_RIGHT ++++++");
                    ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PLAYSTA_ID | MOVIE_PROG_ID);

                    if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，状态图标、进度条、则处理上一个下一个
                    {
                        __cedar_status_t fsm_sta;
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                        fsm_sta = robin_get_fsm_status();

                        if (CEDAR_STAT_PLAY == fsm_sta
                            || CEDAR_STAT_PAUSE == fsm_sta)
                        {
                            __msg("before __movie_play_prev");
                            __movie_play_next(movie_ctrl);
                            __msg("after __movie_play_prev");
                        }
                        else
                        {
                            __msg("fsm not in play/pause status, do not play prev...");
                        }

                        goto key_ok;
                    }
                }

#if KEY_UP_DOWN_POPUP_MENU
                else if (GUI_MSG_KEY_UP ==  last_key) //出现上面的spsc菜单
                {
                    __s32 ret = 0;
                    __sset_nodelist_sta_e nodelist_sta = NODELIST_DEL;
                    __wrn("+++++GUI_MSG_KEY_UP ++++++");
                    ret = SSET_Get_Nodelist("movie", &nodelist_sta);
                    __log("UP ret = %d, nodelist_sta = %d", ret, nodelist_sta);

                    if (EPDK_FALSE == SSET_court_is_tbar() && EPDK_OK == ret && NODELIST_OFF == nodelist_sta)
                    {
                        goto key_ok;
                    }

                    if (0 == msg->dwReserved)
                    {
                        __movie_play_prev(movie_ctrl);
                        goto key_ok;
                    }
                    else
                    {
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                        if (EPDK_TRUE == ret)
                        {
                            //__here__;
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SPSC_ID);
                            goto key_ok;
                        }
                        else
                        {
                            //__here__;
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                            __movie_spsc_scene_create(movie_ctrl);
                            goto key_ok;
                        }
                    }
                }
                else if (GUI_MSG_KEY_DOWN ==  last_key) //出现下面的菜单
                {
                    __s32 ret = 0;
                    __sset_nodelist_sta_e nodelist_sta = NODELIST_DEL;
                    __wrn("@@@@@@@@ GUI_MSG_KEY_DOWN ");
                    ret = SSET_Get_Nodelist("movie", &nodelist_sta);
                    __log("DOWN ret = %d, nodelist_sta = %d", ret, nodelist_sta);

                    if (EPDK_FALSE == SSET_court_is_tbar() && EPDK_OK == ret && NODELIST_OFF == nodelist_sta)
                    {
                        goto key_ok;
                    }

                    if (0 == msg->dwReserved)
                    {
                        __movie_play_next(movie_ctrl);
                        goto key_ok;
                    }
                    else
                    {
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                        if (EPDK_TRUE == ret)
                        {
                            //__here__;
                            if (SSET_court_is_tbar() == EPDK_TRUE)
                            {
                                if (SSET_tbar_is_show() == EPDK_TRUE)
                                {
                                    //__here__;
                                    SSET_hide_tbar();
                                    goto key_ok;
                                }
                                else
                                {
                                    //__here__;
                                    SSET_show_tbar();
                                    goto key_ok;
                                }
                            }

                            //发消息给SSET  让他删除除sset tbar外的所有子菜单
                            {
                                __gui_msg_t my_msg_to_sset;
                                my_msg_to_sset.id = SSET_APP2SSET_PREV;
                                my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
                                my_msg_to_sset.h_srcwin = NULL;
                                GUI_SendMessage(&my_msg_to_sset);
                            }
                            goto key_ok;
                        }
                        else
                        {
                            //__here__;
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                            __movie_sset_scene_create(movie_ctrl);
                            goto key_ok;
                        }
                    }
                }

#endif
            }
            else if (KEY_DOWN_ACTION == msg->dwAddData2 || KEY_REPEAT_ACTION == msg->dwAddData2)
            {
                switch (msg->dwAddData1)
                {
                    case GUI_MSG_KEY_VADD:
                    case GUI_MSG_KEY_LONGVADD:
                    {
                        __wrn("@@@@@@ movie volume inc @@@@@@");
                        __movie_install_auto_del_habr_timmer(movie_ctrl);
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                        goto key_ok;
                    }

                    case GUI_MSG_KEY_VDEC:
                    {
                       // __log("[YU][] jump play");
                      //  robin_set_cmd_jump(2 * 1000 * 60);
                        goto key_ok;
                    }
                    case GUI_MSG_KEY_LONGVDEC:
                    {
                        __wrn("@@@@@@ movie volume dec @@@@@@");
                        __movie_install_auto_del_habr_timmer(movie_ctrl);
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                        goto key_ok;
                    }

                    case GUI_MSG_KEY_ENTER:
                    {
                        __s32 ret;
#if 1

                        if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
                        {
                            GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id);
                            movie_ctrl->rr_flag = 0;
                            movie_ctrl->ff_rr_cnt = 0;
                            __movie_prog_scene_enable_auto_hide(movie_ctrl, EPDK_TRUE);
                            ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PROG_ID | MOVIE_PLAYSTA_ID);

                            if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，进度条，则恢复快退快进后的正常播放
                            {
                                __cedar_status_t fsm_sta;
                                fsm_sta = robin_get_fsm_status();

                                if (CEDAR_STAT_RR == fsm_sta
                                    || CEDAR_STAT_FF == fsm_sta)
                                {
                                    robin_set_cmd_play();
                                    __msg("after robin_set_cmd_play");
                                }
                                else
                                {
                                    __msg("fsm not in ff/rr status, do not set play cmd");
                                }
                            }

                            return EPDK_OK;
                        }

#endif
                        //如果没有弹出菜单，则按enter暂停或开始播放
                        // __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
                        ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_SUB_SCENE_NULL);
                        __msg("ret = %d ", ret);

                        if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，则处理暂停、继续播放
                        {
                            __cedar_status_t fsm_sta;
                            fsm_sta = robin_get_fsm_status();

                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                robin_set_cmd_pause();
                                __msg("after robin_set_cmd_pause");
                                __movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
                                goto key_ok;
                            }
                            else if (CEDAR_STAT_PAUSE == fsm_sta)
                            {
                                __msg("before robin_set_cmd_play");
                                robin_set_cmd_play();
                                __msg("after robin_set_cmd_play");
                                goto key_ok;
                            }
                        }
                        else
                        {
                            //__here__;
                            if (movie_ctrl->sset_sence && movie_ctrl->playsta_scene)
                            {
                                // //__here__;
                                //movie_playsta_scene_set_focus(movie_ctrl->playsta_scene);
                                __cedar_status_t fsm_sta;
                                GUI_WinSetFocusChild(movie_ctrl->sset_sence);
                                movie_playsta_scene_change_sta(movie_ctrl->playsta_scene);
                            }
                        }

                        break;
                    }

                    case GUI_MSG_KEY_PLAY_PAUSE:
                    {
                        __cedar_status_t sta;
                        sta = robin_get_fsm_status();

                        if (CEDAR_STAT_PLAY == sta)
                        {
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                            robin_set_cmd_pause();
                            __movie_playsta_scene_create(movie_ctrl, movie_playsta_pause);
                        }
                        else
                        {
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                            robin_set_cmd_play();
                            __movie_playsta_scene_create(movie_ctrl, movie_playsta_play);
                        }

                        goto key_ok;
                        break;
                    }

                    //case GUI_MSG_KEY_EQ:  //切换声道 //add by chenguofeng 2011.2.23
                    case GUI_MSG_KEY_TACK:
                    {
                        __s32 channel;
                        channel = robin_get_channel();
                        //__wrn("@@@@@@@ get movie channel = %d", channel);
                        //channel = (channel + 1) % 3;                          //this is abandoned practice
                        //__wrn("@@@@@@@ cur movie channel = %d", channel);
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                        __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_CHANNEL_ID, channel);
                        robin_set_channel(channel);
                        __movie_save_channel(channel);
                        break;
                    }

#if 0

                    case GUI_MSG_KEY_ACCOMP_ONOFF: //切换音轨//add by chenguofeng 2011.2.23
                    {
                        __s32 i;
                        __s32 track_index;
                        __u8  track_num;
                        __audstream_profile_t   track_para;
                        robin_get_track_info(&track_para);
                        track_num = track_para.nAudStrmNum;
                        __wrn("@@@@@@@ track_para.nAudStrmNum = %d", track_para.nAudStrmNum);

                        if (0 == track_para.nAudStrmNum)
                        {
                            __wrn("********** nAudStrmNum == 0 ");
                            //return EPDK_OK;
                            goto key_ok;
                        }

                        if (/*track_num < 0 || */track_num > 3) //限定3  个音轨
                        {
                            __wrn("********* the track num > 3");
                            track_num = 3;
                        }

                        track_index = robin_get_track_index();
                        __wrn("@@@@@@@@ get track index = %d", track_index);
                        track_index++;

                        if (track_index < 0 || track_index >= track_num)
                        {
                            __wrn("********* invalid track index ");
                            track_index = 0;
                        }

                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                        __app_movie_show_dialog(movie_ctrl->movie_manager, MOVIE_TRACK_ID, track_index);
                        __wrn("@@@@@@@ robin_select_track index = %d", track_index);
                        robin_select_track(track_index);
                        __movie_save_track(track_index);
                        break;
                    }

#endif

                    case GUI_MSG_KEY_ESCAPE:
                    {
                        __s32 ret;
                        __cedar_status_t fsm_sta;
                        //__here__;
                        ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_SUB_SCENE_NULL);
                        __msg("ret==================%d ", ret);

                        if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景
                        {
                            SIZE screen_size;
                            __s32 screen_type;
                            //__here__;
                            dsk_display_get_size(&screen_size.width, &screen_size.height);
                            __inf("screen_size.width:%d screen_size.height:%d", screen_size.width, screen_size.height);
#if 0

                            if ((screen_size.width == 800) && (screen_size.height == 480))
#endif
                            {
                                //__here__;
                                if (is_on_tv()) //退出tvout
                                {
                                    //__here__;
                                    __movie_switch_to_tv_or_lcd(movie_ctrl);
                                    goto key_ok;
                                }
                                else//退出movie
                                {
                                    //__here__;
                                    __movie_exit_movie(movie_ctrl, 0);
                                    goto key_ok;
                                }
                            }

#if 0
                            else
                            {
                                //__here__;
                                __movie_exit_movie(movie_ctrl, 0);
                                goto key_ok;
                            }

#endif
                        }
                        else
                        {
                            //__here__;
                            __msg("robin_set_cmd_play==================%d ", __LINE__);
                            __msg("robin_set_cmd_play==================%d ", __LINE__);

                            if (GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
                            {
                                GUI_KillTimer(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id);
                                movie_ctrl->rr_flag = 0;
                                movie_ctrl->ff_rr_cnt = 0;
                                __msg("robin_set_cmd_play==================%d ", __LINE__);
                                __movie_prog_scene_enable_auto_hide(movie_ctrl, EPDK_TRUE);
                                robin_set_cmd_play();
                                __msg("robin_set_cmd_play==================%d ", ret);
                                //return EPDK_OK;
                            }

                            __msg("robin_set_cmd_play==================%d ", __LINE__);
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                            //__here__;
                            goto key_ok;
                        }

                        break;
                    }

                    case GUI_MSG_KEY_MENU:
                    case GUI_MSG_KEY_LONGMENU:   //menu 循环出现上下的菜单
                    {
                        __msg("++++++++++++++++++++++++++++++++");
                        __msg("movie_ctrl->sence_flag = %d", movie_ctrl->sence_flag);

                        if (movie_ctrl->sence_flag == 0)
                        {
                            movie_ctrl->sence_flag = 1;
                            //__here__;
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                            if (EPDK_FALSE == ret)
                            {
                                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                                if (EPDK_TRUE == ret)
                                {
                                    __movie_spsc_hide_layer_win(movie_ctrl->spsc_scene);
                                }

                                //__here__;
                                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                                //__here__;
                                __movie_sset_scene_create(movie_ctrl);
                                //__here__;
                                return EPDK_OK;
                            }
                            else
                            {
                                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));

                                //是否是sset 中是tbar 的情况
                                if (SSET_court_is_tbar() == EPDK_TRUE)
                                {
                                    //隐藏状态下show 它出来
                                    if (SSET_tbar_is_show() == EPDK_FALSE)
                                    {
                                        //__here__;
                                        SSET_show_tbar();
                                        return EPDK_OK;
                                    }
                                }
                                else
                                {
                                    __gui_msg_t my_msg_to_sset;
                                    my_msg_to_sset.id = SSET_APP2SSET_PREV;
                                    my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
                                    my_msg_to_sset.h_srcwin = NULL;
                                    //__here__;
                                    GUI_SendMessage(&my_msg_to_sset);
                                    return  EPDK_OK;
                                }
                            }
                        }
                        else if (movie_ctrl->sence_flag == 1)
                        {
                            movie_ctrl->sence_flag = 0;
                            //__here__;
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                            //__here__;
                            if (EPDK_FALSE == ret)
                            {
                                //__here__;
                                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                                if (ret == EPDK_TRUE)
                                {
                                    if (SSET_court_is_tbar() == EPDK_TRUE)
                                    {
                                        if (SSET_tbar_is_show() == EPDK_TRUE)
                                        {
                                            //__here__;
                                            SSET_hide_tbar();
                                        }
                                    }
                                    else
                                    {
                                        __gui_msg_t my_msg_to_sset;
                                        my_msg_to_sset.id = SSET_APP2SSET_PREV;
                                        my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
                                        my_msg_to_sset.h_srcwin = NULL;
                                        GUI_SendMessage(&my_msg_to_sset);
                                        return  EPDK_OK;
                                    }
                                }

                                //__here__;
                                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                                //__here__;
                                __movie_spsc_scene_create(movie_ctrl);
                                GUI_LyrWinSetTop(GUI_WinGetLyrWin((H_WIN)movie_ctrl->spsc_scene));
                                //__here__;
                            }
                        }
                        else
                        {
                            __msg("+++++++++++++++++++++++++++++++++");
                        }

                        return EPDK_OK;
                    }

                    case GUI_MSG_KEY_RR:
                    case GUI_MSG_KEY_LONGLEFT:
                    case GUI_MSG_KEY_LONGUP:
                    case GUI_MSG_KEY_LONGPREV:
                    case GUI_MSG_KEY_LONGRR:
                    {
                        ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PROG_ID | MOVIE_PLAYSTA_ID);
                        __msg("ret=========%d ", ret);

                        if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，进度条，状态图标，则快退快进
                        {
                            __cedar_status_t fsm_sta;
                            //__here__;
                            fsm_sta = robin_get_fsm_status();

                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                __msg("before robin_set_cmd_rr");
                                movie_ctrl->rr_flag = 1;
                                __msg("after robin_set_cmd_rr");
                                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

                                if (EPDK_FALSE == ret)
                                {
                                    //__here__;
                                    __movie_prog_scene_create(movie_ctrl);
                                    __movie_prog_scene_set_focus(movie_ctrl);
                                }
                                else
                                {
                                    __movie_prog_scene_update_ui(movie_ctrl);
                                }

                                __movie_prog_scene_enable_auto_hide(movie_ctrl, EPDK_FALSE);
                                movie_ctrl->ff_rr_cnt++;
                                __movie_ff_rr_set_speed(movie_ctrl->ff_rr_cnt / 5);
                                robin_set_cmd_rr();
                            }
                            else
                            {
                                __msg("fsm not in play status, do not rr");

                                if (CEDAR_STAT_RR == fsm_sta)
                                {
                                    movie_ctrl->ff_rr_cnt++;
                                    __movie_ff_rr_set_speed(movie_ctrl->ff_rr_cnt / 5);
                                    robin_set_cmd_rr();
                                }
                            }

                            goto key_fail;//消息往下传给prog scene
                        }

                        goto key_fail;
                    }

                    case GUI_MSG_KEY_FF:
                    case GUI_MSG_KEY_LONGRIGHT:
                    case GUI_MSG_KEY_LONGDOWN:
                    case GUI_MSG_KEY_LONGNEXT:
                    case GUI_MSG_KEY_LONGFF:
                    {
                        ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PROG_ID | MOVIE_PLAYSTA_ID);
                        __msg("ret=========%d ", ret);

                        if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，进度条，状态图标，则快退快进
                        {
                            __cedar_status_t fsm_sta;
                            fsm_sta = robin_get_fsm_status();

                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                __msg("before robin_set_cmd_ff");
                                movie_ctrl->rr_flag = 0;
                                __msg("after robin_set_cmd_ff");
                                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PROG_ID);

                                if (EPDK_FALSE == ret)
                                {
                                    __movie_prog_scene_create(movie_ctrl);
                                    __movie_prog_scene_set_focus(movie_ctrl);
                                }
                                else
                                {
                                    __movie_prog_scene_update_ui(movie_ctrl);
                                }

                                __movie_prog_scene_enable_auto_hide(movie_ctrl, EPDK_FALSE);
                                movie_ctrl->ff_rr_cnt++;
                                __movie_ff_rr_set_speed(movie_ctrl->ff_rr_cnt / 5);
                                robin_set_cmd_ff();
                                __log("%s %d ", __FILE__, __LINE__);
                            }
                            else
                            {
                                __msg("fsm not in play status, do not ff");

                                if (CEDAR_STAT_FF == fsm_sta)
                                {
                                    movie_ctrl->ff_rr_cnt++;
                                    __movie_ff_rr_set_speed(movie_ctrl->ff_rr_cnt / 5);
                                    robin_set_cmd_ff();
                                    __log("%s %d ", __FILE__, __LINE__);
                                }
                            }

                            goto key_fail;
                        }

                        goto key_fail;
                    }

                    //===========================================================
                    //处理数字选取
                    case GUI_MSG_KEY_NUM0:
                    case GUI_MSG_KEY_NUM1:
                    case GUI_MSG_KEY_NUM2:
                    case GUI_MSG_KEY_NUM3:
                    case GUI_MSG_KEY_NUM4:
                    case GUI_MSG_KEY_NUM5:
                    case GUI_MSG_KEY_NUM6:
                    case GUI_MSG_KEY_NUM7:
                    case GUI_MSG_KEY_NUM8:
                    case GUI_MSG_KEY_NUM9:
                    {
                        __s32 cur_val;
                        __s32 total_movie_num;
                        __log("&&&&&&&&&&&&&&&&& movie sel key:%d &&&&&&&&&&&", msg->dwAddData1);

                        if (NULL == movie_ctrl->hdigit_layer)
                        {
                            RECT  rect;
                            __s32 width, height;
                            __s32 scn_width, scn_height;
                            __wrn("&&&&&&&&& create hdigit layer &&&&&&&&&");
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));
                            dsk_display_get_size(&scn_width, &scn_height);
                            width       = 300;//20*5;
                            height      = 24;
                            rect.x      = (scn_width - width) / 2;
                            rect.y      =  scn_height - height - 20;
                            rect.width  = width;
                            rect.height = height;
                            movie_ctrl->hdigit_layer = com_layer_create(&rect, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_ON, "");

                            if (NULL == movie_ctrl->hdigit_layer)
                            {
                                __wrn("**********digit layer create fail...");
                            }
                            else
                            {
                                __wrn("digit layer create ok...");
                            }
                        }

                        __wrn("movie_ctrl->digit_timmer_id=%d", movie_ctrl->digit_timmer_id);

                        //__wrn("msg->h_deswin=0x%x", msg->h_deswin);
                        if (!GUI_IsTimerInstalled(msg->h_deswin, movie_ctrl->digit_timmer_id))
                        {
                            __wrn("===========create timmer...");
                            movie_ctrl->ndigit_movie_num = 0;
                            GUI_SetTimer(msg->h_deswin, movie_ctrl->digit_timmer_id, 300, NULL);
                        }
                        else
                        {
                            __wrn("=============reset timmer...");
                            GUI_ResetTimer(msg->h_deswin, movie_ctrl->digit_timmer_id, 300, NULL);
                        }

                        cur_val = msg->dwAddData1 - GUI_MSG_KEY_NUM0;
                        __wrn("&&&&&&&&&&& cur_val=%d ", cur_val);
                        total_movie_num = robin_npl_get_total_count();
                        __wrn("&&&&&&&&&&  total_movie_num=%d", total_movie_num);

                        if (movie_ctrl->ndigit_movie_num < total_movie_num)
                        {
                            movie_ctrl->ndigit_movie_num *= 10;
                            movie_ctrl->ndigit_movie_num += cur_val;
                        }
                        else
                        {
                            __wrn("=============movie num exceed...");
                        }

#if HT_1628_LED_EX
                        SetMovieNum(APP_MOVIE_ID, ((movie_ctrl->ndigit_movie_num) & 0x0fff) | 0x2000);
#endif
                        __wrn("movie_ctrl->ndigit_movie_num=%d", movie_ctrl->ndigit_movie_num);
                        movie_digit_layer_draw(msg);
                        goto key_fail;//break;
                    }

                    //===========================================================

                    default:
                    {
                        __wrn("********* default key set *********");
                        goto key_fail;
                    }
                }
            }

key_fail:
            ;

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                last_key = -1;
            }
            else
            {
                last_key = msg->dwAddData1;
                __wrn("&&&&&&& last kay = %d ", last_key);
            }

            break;
key_ok:
            ;

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                last_key = -1;
            }
            else
            {
                last_key = msg->dwAddData1;
            }

            __msg("GUI_MSG_KEY end");
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
        {
            if (1 == msg->dwAddData1) //退出到home
            {
                movie_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);//退出movie
            }
            else//退出到explorer
            {
                movie_ctrl_t *movie_ctrl = NULL;
                movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

                if (movie_ctrl == NULL)
                {
                    __err("movie_ctrl is null...");
                    return EPDK_OK;
                }

                __wrn("shiql root_type:%d", movie_ctrl->root_type);
                movie_cmd2parent(msg->h_deswin, SWITCH_TO_EXPLORER, movie_ctrl->root_type, 0);//退出movie
            }

            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            __s32 ret;
            movie_ctrl_t *movie_ctrl;
            __s32 x, y;
            __wrn("GUI_MSG_TOUCH begin");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __err("movie_ctrl is null...");
                return EPDK_OK;
            }

            do //处理上一个下一个的触摸消息
            {
                __s32 ret;
                __s32 direction;
                direction = __app_movie_touch_identify(msg);

                if (-1 == direction)
                {
                    goto touch_next_fail;
                }

                __wrn("identify ok, direction=%d", direction);
                ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PROG_ID | MOVIE_PLAYSTA_ID);

                if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，进度条，状态图标，则快退快进
                {
                    __wrn("no eny visable scene...");
                }
                else
                {
                    __wrn("there is some visable scene...");
                    goto touch_next_fail;
                }

                if (0 == direction
                    || 2 == direction)
                {
                }
                else if (1 == direction
                         || 3 == direction)
                {
                    __wrn("direction invalid...");
                    goto touch_next_fail;
                }
                else
                {
                    __wrn("direction invalid...");
                    goto touch_next_fail;
                }

                {
                    __cedar_status_t cedar_sta;
                    cedar_sta = robin_get_fsm_status();
                    __wrn("cedar_sta =%d", cedar_sta);

                    if (CEDAR_STAT_PLAY == cedar_sta
                        || CEDAR_STAT_PAUSE == cedar_sta)
                    {
                        if (CEDAR_STAT_PLAY == cedar_sta)
                        {
                            __wrn("cedar is playing now...");
                            ret = robin_set_cmd_pause();

                            if (EPDK_FAIL == ret)
                            {
                                __wrn("robin_set_cmd_pause fail...");
                                goto touch_next_fail;
                            }

                            __wrn("robin_set_cmd_pause ok...");
                        }
                    }
                    else
                    {
                        __wrn("cedar not in play and pause sta, process fail...");
                        goto touch_next_fail;
                    }
                }

                __wrn("after set pause...");

                if (1) //move video layer
                {
                    __s32           x;
                    __s32           screen_width;
                    __s32           screen_height;
                    signed long     args[3] = {0};
                    __disp_rect_t   disp_rect;
                    __bool          quit_flag = 0;
                    __s32           step;
                    __s32           org_x;
                    __wrn("begin move video layer...");

                    if (NULL == movie_ctrl->hvedio_lyr
                        || NULL == movie_ctrl->hdisp)
                    {
                        __wrn("invalid para...");
                        goto touch_next_fail;
                    }

                    if (0 == direction)
                    {
                        step = -16;
                    }
                    else if (2 == direction)
                    {
                        step = 16;
                    }
                    else
                    {
                        step = 0;
                        __wrn("direction is invalid...");
                        goto touch_next_fail;
                    }

                    //get lcd size
                    dsk_display_get_size(&screen_width, &screen_height);
                    quit_flag = 0;
                    __wrn("move layer begin...");
                    args[0] = (signed long)movie_ctrl->hvedio_lyr;
                    args[1] = (signed long)&disp_rect;
                    args[2] = 0;
                    ret = esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_LAYER_GET_SCN_WINDOW, 0, (void *)args);
                    __wrn("disp_rect.x=%d, disp_rect.y=%d,disp_rect.width=%d,disp_rect.height=%d",
                          disp_rect.x, disp_rect.y, disp_rect.width, disp_rect.height);
                    org_x = disp_rect.x;

                    while (1)
                    {
                        disp_rect.x += step;

                        if (disp_rect.x + (__s32)(disp_rect.width) <= 0
                            || disp_rect.x >= screen_width)
                        {
                            quit_flag = 1;
                        }

                        if (disp_rect.x <= -screen_width)
                        {
                            disp_rect.x = -screen_width + 1;
                        }

                        if (disp_rect.x >= screen_width)
                        {
                            disp_rect.x = screen_width - 1;
                        }

                        __wrn("disp_rect.x=%d, disp_rect.y=%d,disp_rect.width=%d,disp_rect.height=%d",
                              disp_rect.x, disp_rect.y, disp_rect.width, disp_rect.height);
                        ret = esMODS_MIoctrl(movie_ctrl->hdisp, MOD_DISP_CMD_LAYER_GET_SCN_WINDOW, 0, (void *)args);
                        esKRNL_TimeDly(1);

                        if (1 == quit_flag)
                        {
                            break;
                        }
                    }

                    __wrn("begin move layer end...");
                    ret = robin_set_cmd_stop();

                    if (EPDK_FAIL == ret)
                    {
                        __wrn("robin_set_cmd_stop fail...");
                        goto touch_next_fail;
                    }

                    __wrn("robin_set_cmd_stop ok...");
                    //move video layer to normal pos
                    __wrn("move layer back begin...");
                    disp_rect.x = org_x;
                    //ret = esMODS_MIoctrl(movie_ctrl->hdisp, DISP_CMD_LAYER_SET_SCN_WINDOW, 0, (void*)args);//cedar会自动移动，无需处理
                    __wrn("move layer back end...");
                }

                {
                    //
                    __wrn("play prevext begin...");

                    if (2 == direction)
                    {
                        __movie_play_prev(movie_ctrl);
                    }
                    else if (0 == direction)
                    {
                        __movie_play_next(movie_ctrl);
                    }
                    else
                    {
                        __wrn("direction invalid...");
                        goto touch_next_fail;
                    }

                    __wrn("play prevext end...");
                }

                {
                    //播放起来后删除多余的子场景
                    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                }

                //touch_next_ok:
                __wrn("touch_next_ok");
                return EPDK_OK;
touch_next_fail:
                __wrn("touch_next_fail");
                break;
            } while (0);

            if (1) // 滑动快进快退
            {
                __s32 ret = -1;
                movie_ff_rr_show_info_t ff_rr_show_info;
                eLIBs_memset(&ff_rr_show_info, 0, sizeof(movie_ff_rr_show_info_t));
                ret = __app_movie_touch_ff_rr_identify(msg, &ff_rr_show_info);

                if (EPDK_OK == ret)
                {
                    movie_ff_rr_show_scene_update_string(movie_ctrl->ff_rr_show_scene, &ff_rr_show_info);
                    return EPDK_OK;
                }
            }

            ret = -1;
            x = LOSWORD(msg->dwAddData2);
            y = HISWORD(msg->dwAddData2);
            __wrn("***************x=%d, y=%d************", x, y);
            {
                //如果有对话框的子场景，则让触摸消息全部往对话框传(避免下面的对话框没有set capture)
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_BREAKPLAY_DIALOG_ID);

                if (EPDK_TRUE == ret)
                {
                    if (movie_ctrl->breakplay_scene)
                    {
                        __gui_msg_t setmsg;
                        eLIBs_memcpy(&setmsg, msg, sizeof(__gui_msg_t));
                        setmsg.h_deswin = movie_ctrl->breakplay_scene;
                        GUI_SendMessage(&setmsg);
                        return EPDK_OK;
                    }
                }
            }
            //__here__;
            {
                //如果有对话框的子场景，则让触摸消息全部往对话框传(避免下面的对话框没有set capture)
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYERROR_DIALOG_ID);

                if (EPDK_TRUE == ret)
                {
                    if (movie_ctrl->playerror_scene)
                    {
                        __gui_msg_t setmsg;
                        eLIBs_memcpy(&setmsg, msg, sizeof(__gui_msg_t));
                        setmsg.h_deswin = movie_ctrl->playerror_scene;
                        GUI_SendMessage(&setmsg);
                        return EPDK_OK;
                    }
                }
            }
            //__here__;
            {
                //如果有对话框的子场景，则让触摸消息全部往对话框传(避免下面的对话框没有set capture)
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYEND_DIALOG_ID);

                if (EPDK_TRUE == ret)
                {
                    if (movie_ctrl->playend_scene)
                    {
                        __gui_msg_t setmsg;
                        eLIBs_memcpy(&setmsg, msg, sizeof(__gui_msg_t));
                        setmsg.h_deswin = movie_ctrl->playend_scene;
                        GUI_SendMessage(&setmsg);
                        return EPDK_OK;
                    }
                }
            }

            //__here__;

            //如果有play_sta子场景，点击了其附近，则把消息发给它
            if (GUI_MSG_TOUCH_UP == msg->dwAddData1
                || GUI_MSG_TOUCH_OVERUP == msg->dwAddData1)
            {
                __s32 ret;
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_PLAYSTA_ID);

                if (EPDK_TRUE == ret)
                {
                    RECT rect;
                    ret = movie_playsta_scene_get_lyr_scn(&rect);

                    if (EPDK_OK == ret)
                    {
                        ret = __movie_point_in_rect(x, y, &rect, 5, 5, 10, 10);

                        if (EPDK_TRUE == ret)
                        {
                            __gui_msg_t setmsg;
                            eLIBs_memcpy(&setmsg, msg, sizeof(__gui_msg_t));
                            setmsg.h_deswin = (H_WIN)movie_playsta_scene_get_hwnd(movie_ctrl->playsta_scene);

                            if (setmsg.h_deswin)
                            {
                                GUI_SendMessage(&setmsg);
                                return EPDK_OK;
                            }
                        }
                    }
                }
            }

            //__here__;
            if (GUI_MSG_TOUCH_UP == msg->dwAddData1
                || GUI_MSG_TOUCH_OVERUP == msg->dwAddData1)
            {
                {
                    //上、下菜单不同时出现
                    __s32 ret;
                    RECT rect_spsc;
                    __sset_uipara_t *sset_uipara;
                    ret = movie_spsc_scene_get_lyr_scn(&rect_spsc);

                    if (EPDK_FAIL == ret)
                    {
                        __msg("movie_spsc_scene_get_lyr_scn fail...");
                        return EPDK_OK;
                    }

                    sset_uipara = SSET_Get_Uipara();

                    if (!sset_uipara)
                    {
                        __err("sset_uipara is null..");
                        return EPDK_OK;
                    }

                    __msg("rect_spsc.y=%d", rect_spsc.y);
                    __msg("rect_spsc.height=%d", rect_spsc.height);

                    //点击了中间区域，删除所有子场景
                    if (y > rect_spsc.y + rect_spsc.height + 20 && y < 272 - 70 - 20)
                    {
                        //__here__;
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                        //__here__;
                        if (ret == EPDK_TRUE)
                        {
                            //__here__;
                            //是否是sset 中是tbar 的情况
                            if (SSET_court_is_tbar() == EPDK_TRUE)
                            {
                                //__here__;
                                //SHOW状态下隐藏
                                if (SSET_tbar_is_show() == EPDK_TRUE)
                                {
                                    //__here__;
                                    SSET_hide_tbar();
                                    //__here__;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }

                        //__here__;
                        //删除场景
                        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                        //__here__;
                        return EPDK_OK;
                    }
                    //点击了屏幕上方，如果没有创建上方的菜单，则创建
                    else if (y <= rect_spsc.y + rect_spsc.height + 20)
                    {
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                        if (EPDK_FALSE == ret)
                        {
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                            if (ret == EPDK_TRUE)
                            {
                                //是否是sset 中是tbar 的情况
                                if (SSET_court_is_tbar() == EPDK_TRUE)
                                {
                                    //SHOW状态下隐藏
                                    if (SSET_tbar_is_show() == EPDK_TRUE)
                                    {
                                        SSET_hide_tbar();
                                    }
                                }
                                else
                                {
                                    break;
                                }
                            }

                            //__here__;
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                            //__here__;
                            __movie_spsc_scene_create(movie_ctrl);
                            GUI_LyrWinSetTop(GUI_WinGetLyrWin((H_WIN)movie_ctrl->spsc_scene));
                            //__here__;
                            return EPDK_OK;
                        }
                    }
                    //点击了屏幕下方，如果没有创建下方的菜单，则创建
                    else  if (y >= 272 - 70 - 20)
                    {
                        ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID);

                        if (EPDK_FALSE == ret)
                        {
                            ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SPSC_ID);

                            if (EPDK_TRUE == ret)
                            {
                                __movie_spsc_hide_layer_win(movie_ctrl->spsc_scene);
                            }

                            //__here__;
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
                            //__here__;
                            __movie_sset_scene_create(movie_ctrl);
                            //__here__;
                            return EPDK_OK;
                        }
                        else
                        {
                            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));

                            //是否是sset 中是tbar 的情况
                            if (SSET_court_is_tbar() == EPDK_TRUE)
                            {
                                //隐藏状态下show 它出来
                                if (SSET_tbar_is_show() == EPDK_FALSE)
                                {
                                    SSET_show_tbar();
                                    return EPDK_OK;
                                }
                            }
                            else
                            {
                                __gui_msg_t my_msg_to_sset;
                                my_msg_to_sset.id = SSET_APP2SSET_PREV;
                                my_msg_to_sset.h_deswin = movie_ctrl->sset_sence;
                                my_msg_to_sset.h_srcwin = NULL;
                                GUI_SendMessage(&my_msg_to_sset);
                                return  EPDK_OK;
                            }
                        }
                    }
                }
            }//if(GUI_MSG_TOUCH_UP == msg->dwAddData1

            if (0) //112350
            {
                ret = __movie_has_sub_scene(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID));

                if (EPDK_FALSE == ret ||
                    (EPDK_TRUE == __movie_has_sub_scene(movie_ctrl, MOVIE_SSET_ID)
                     && SSET_court_is_tbar() == EPDK_TRUE
                     && SSET_tbar_is_show() == EPDK_FALSE)
                   )//如果没有除字幕和进度\播放状态以外的可见子场景，则触摸消息不往下传
                {
                    __wrn("touch msg ret ok...");
                    return EPDK_OK;
                }
            }

            __wrn("GUI_MSG_TOUCH end");
            break;
        }

        case GUI_MSG_TIMER:
        {
            movie_ctrl_t *movie_ctrl;
            __s32 ret;
            ret = -1;
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (movie_ctrl)
            {
                if (LOWORD(msg->dwAddData1) == movie_ctrl->auto_play_timmer_id)
                {
                    ret =  __movie_check_for_play_err(movie_ctrl);
                    __msg("before __movie_check_for_auto_play ret%d", ret);

                    if (ret != EPDK_FALSE)
                    {
                        __movie_check_for_auto_play(movie_ctrl);
                    }

                    //__msg("aftr __movie_check_for_auto_play");
                    __movie_install_auto_play_timmer(movie_ctrl);
                    ret = EPDK_OK;
                }
                else if (LOWORD(msg->dwAddData1) == movie_ctrl->set_cb_timmer_id)
                {
                    __movie_uninstall_set_cb_timmer(movie_ctrl);
                    __wrn("before msg_emit_register_tvout_tp_hook");
                    msg_emit_register_tvout_tp_hook((cb_tvout_tp_hook)esKRNL_GetCallBack((__pCBK_t)__movie_tvout_tp_hook));
                }
                //==========================================
                else if (LOWORD(msg->dwAddData1) == movie_ctrl->digit_timmer_id) // 30234384
                    //else if(msg->dwAddData1 == this->digit_timmer_id )
                {
                    __wrn("timmer has come...");
                    __wrn("this->ndigit_movie_num=%d", movie_ctrl->ndigit_movie_num);
                    __movie_play_by_index(movie_ctrl, movie_ctrl->ndigit_movie_num);
                    movie_delete_digit_layer(msg);
                }
                else if (LOWORD(msg->dwAddData1) == movie_ctrl->auto_del_hbar_timmer_id)
                {
                    HBarState hbar_sta;
                    gscene_hbar_get_state(&hbar_sta);
                    __msg("================!!!!!!!!!!!!!hbar_sta = %d ", hbar_sta);

                    if (HBAR_ST_HIDE != hbar_sta)
                    {
                        //__here__;
                        gscene_hbar_set_state(HBAR_ST_HIDE);
                    }

                    //__movie_uninstall_auto_del_habr_timmer(movie_ctrl);
                }
                else if (LOWORD(msg->dwAddData1) == movie_ctrl->auto_ffrr_timmer_id)
                {
                    ret = __app_has_visuable_sub_scene_except_x(movie_ctrl, MOVIE_PROG_ID | MOVIE_PLAYSTA_ID);

                    if (EPDK_FALSE == ret) //如果没有除字幕以外的可见子场景，进度条，状态图标，则快退快进
                    {
                        __cedar_status_t fsm_sta;
                        fsm_sta = robin_get_fsm_status();

                        if (CEDAR_STAT_PLAY == fsm_sta)
                        {
                            {
                                __movie_prog_scene_update_ui(movie_ctrl);
                            }
                            __movie_prog_scene_enable_auto_hide(movie_ctrl, EPDK_FALSE);
                            movie_ctrl->ff_rr_cnt++;
                            __movie_ff_rr_set_speed(movie_ctrl->ff_rr_cnt / 5);

                            if (movie_ctrl->rr_flag == 0)
                            {
                                robin_set_cmd_ff();
                                __log("%s %d ", __FILE__, __LINE__);
                            }
                            else
                            {
                                robin_set_cmd_rr();
                            }
                        }
                        else
                        {
                            __msg("fsm not in play status, do not ff");

                            if (CEDAR_STAT_RR == fsm_sta || CEDAR_STAT_FF == fsm_sta)
                            {
                                movie_ctrl->ff_rr_cnt++;
                                __movie_ff_rr_set_speed(movie_ctrl->ff_rr_cnt / 5);

                                if (movie_ctrl->rr_flag == 0)
                                {
                                    robin_set_cmd_ff();
                                    __log("%s %d ", __FILE__, __LINE__);
                                }
                                else
                                {
                                    robin_set_cmd_rr();
                                }

                                if (robin_get_total_time() <= robin_get_cur_time())
                                {
                                    robin_set_cmd_stop();
                                    __movie_check_for_auto_play(movie_ctrl);
                                }
                            }
                        }

                        return EPDK_OK;
                    }
                }

                //==========================================
            }
            else
            {
                __err("movie_ctrl is null...");
                return EPDK_OK;
            }

            if (-1 == ret)//未处理，交给子场景处理
            {
                break;
            }
            else//已处理
            {
                return EPDK_OK;
            }
        }

        case GUI_MSG_DESTROY:
        {
            movie_ctrl_t *movie_ctrl;
            __msg("GUI_MSG_DESTROY begin");
            dsk_amplifier_onoff(0);//关闭功放,播放暂停处理较复杂，robin处理
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (movie_ctrl)
            {
                msg_emit_register_tvout_tp_hook(NULL);
                __movie_uninstall_set_cb_timmer(movie_ctrl);

                if (is_on_tv()) //退出tvout
                {
                    //__here__;
                    __movie_switch_to_tv_or_lcd(movie_ctrl);
                    //goto key_ok;
                }

                {
                    __gui_msg_t msg = {0};
                    __s32 args[3] = { -1};
                    msg.p_arg = args;
                    __movie_tvout_tp_hook(&msg);
                }

                //__here__;
                if (movie_ctrl->h_frm_alarm_coming)
                {
                    //__here__;
                    app_alarm_frm_destory(movie_ctrl->h_frm_alarm_coming);
                    movie_ctrl->h_frm_alarm_coming = NULL ;
                }

                __movie_uninstall_auto_play_timmer(movie_ctrl);
                __movie_uninstall_auto_del_habr_timmer(movie_ctrl); //
                {
                    //save breakpoint
                    __s32 index;
                    index = robin_npl_get_cur();

                    //__here__;
                    if (-1 != index)
                    {
                        //__here__;
                        __movie_save_breakpoint_info(index);
                    }
                }
                {
                    //save last playing file
                    __s32 ret;
                    ret = __movie_save_last_playing_file(msg);

                    if (EPDK_OK == ret)
                    {
                        __msg("__movie_save_last_playing_file success...");
                    }
                    else
                    {
                        __msg("__movie_save_last_playing_file fail...");
                    }
                }
                //__here__;
                __movie_stop_play(movie_ctrl);
                //__here__;
                __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL);
                movie_delete_digit_layer(msg);// 30234383
                //__here__;
                __movie_ctrl_para_deinit(movie_ctrl);
                g_movie_ctrl = NULL;
                eLIBs_memset(movie_ctrl, 0, sizeof(movie_ctrl_t));
                esMEMS_Mfree(0, movie_ctrl);
                //__here__;
            }

            //112350 当前SDK不需要，所有界面都已经实现TVOUT
            //g_display_switch_output(LION_DISP_LCD);//movie 退出时恢复到小机显示
            g_enable_close_scn();
            robin_close();
            __movie_robin_sem_deinit();
            //dsk_reg_flush();
            //gscene_bgd_set_state(BGD_STATUS_SHOW);
            {
                reg_root_para_t *root_reg_para = NULL;
                root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

                if (root_reg_para)
                {
                    root_reg_para->cur_play_storage = -1;
                }
                else
                {
                    __wrn("para is null...");
                }
            }
            GUI_Memdev_FrameBuffer_Release(1);//必须
            __msg("GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            __s32 ret;
            movie_ctrl_t *movie_ctrl;
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (movie_ctrl)
            {
                switch (LOWORD(msg->dwAddData1))
                {
                    case MOVIE_SPSC_ID:
                    {
                        ret = __spsc_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                        break;
                    }

                    case MOVIE_PLAYSTA_ID:
                    {
                        ret = __playsta_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                        break;
                    }

                    case MOVIE_SUB_SHOW_ID:
                    {
                        break;
                    }

                    case MOVIE_PROG_ID:
                    {
                        ret = __prog_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                        break;
                    }

                    case MOVIE_BREAKPLAY_DIALOG_ID:
                    {
                        ret = __breakplay_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                        break;
                    }

                    case MOVIE_SSET_ID:
                    {
                        //__msg("++++++MOVIE_SSET_ID+++++");
                        ret = __sset_scene_cmd_proc(movie_ctrl, HIWORD(msg->dwAddData1), msg->dwAddData2);
                        break;
                    }

                    case MOVIE_SPSC_INFOR_ID:
                    {
                        __gui_msg_t msg_info;
                        H_WIN hparent;
                        __msg("++++++MOVIE_SPSC_INFOR_ID+++++");
                        hparent = GUI_WinGetParent((H_WIN)movie_ctrl->movie_manager);

                        if (!hparent)
                        {
                            __err("hparent is null...");
                            return EPDK_FAIL;
                        }

                        __msg("++++++++++++++++++++++++++++++");
                        __msg("msg.p_arg = %s", (char *)msg->p_arg);
                        msg_info.h_deswin = h_infor;
                        msg_info.h_srcwin = NULL;
                        msg_info.id = GUI_MSG_COMMAND;
                        msg_info.dwAddData1 = MAKELONG(GUI_WinGetItemId(h_infor), 0);;
                        msg_info.dwAddData2 = 0;
                        msg_info.dwReserved = 0;
                        msg_info.p_arg     = msg->p_arg;
                        GUI_SendMessage(&msg_info);
                        return EPDK_OK;
                    }

                    case SSET_MENU_ID:
                    {
                        __cedar_status_t fsm_sta;
                        __u32  index = msg->dwAddData2 + 1;
                        //__u32 index = msg->dwReserved+1;
                        fsm_sta = robin_get_fsm_status();
                        __msg("fsm_sta = %d", fsm_sta);
                        __msg("msg->dwAddData2 = %d", msg->dwAddData2);

                        if (index ==  MOVIE_SSET_NC_NEXT || index == MOVIE_SSET_NC_PRE)
                        {
                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                movie_cmd2sset_syn(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                                                   ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
                            }
                            else if (CEDAR_STAT_PAUSE == fsm_sta)
                            {
                                movie_cmd2sset_syn(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                                                   ID_MOVIE_SSET_PAUSE_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
                            }
                        }
                        else if (index ==  MOVIE_SSET_NC_PLAY)
                        {
                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                movie_cmd2sset_syn(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                                                   ID_MOVIE_SSET_PLAY_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS);
                            }
                            else if (CEDAR_STAT_PAUSE == fsm_sta)
                            {
                                movie_cmd2sset_syn(movie_ctrl->sset_sence, SSET_ITEM_UPDATE,
                                                   ID_MOVIE_SSET_PAUSE_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS);
                            }
                        }

                        //if()
                        /*if (index  == MOVIE_SSET_NC_RETURN)
                        {
                        __msg("MOVIE_SSET_NC_RETURN");
                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                    ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL);
                            }
                            else if (CEDAR_STAT_PAUSE == fsm_sta)
                            {
                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                    ID_MOVIE_SSET_PAUSE_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL );
                            }
                        }
                        else if (index  == MOVIE_SSET_NC_PRE)
                        {
                        __msg("MOVIE_SSET_NC_PRE");
                                if (CEDAR_STAT_PLAY == fsm_sta)
                                {
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                        ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL );
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
                                        STRING_MOVIE_PLAY, MOVIE_SSET_NC_PRE, 1 );
                                }
                                else if (CEDAR_STAT_PAUSE == fsm_sta)
                                {
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                        ID_MOVIE_SSET_PAUSE_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL );
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
                                        STRING_MOVIE_STOP, MOVIE_SSET_NC_PRE, 1 );
                                }

                        }
                        else if (index  == MOVIE_SSET_NC_PLAY)
                        {
                            __msg("MOVIE_SSET_NC_PLAY");

                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                        ID_MOVIE_SSET_PLAY_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS);

                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
                                    STRING_MOVIE_STOP, 0, 0 );
                            }
                            else if(CEDAR_STAT_PAUSE == fsm_sta)
                            {
                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                    ID_MOVIE_SSET_PAUSE_F_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_PRESS );

                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
                                    STRING_MOVIE_PLAY, 0, 0 );
                            }
                        }
                        else if (index  == MOVIE_SSET_NC_NEXT)
                        {
                            __msg("MOVIE_SSET_NC_NEXT");
                                if (CEDAR_STAT_PLAY == fsm_sta)
                                {
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                        ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL );
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
                                        STRING_MOVIE_PLAY, MOVIE_SSET_NC_NEXT, 1 );
                                }
                                else if (CEDAR_STAT_PAUSE == fsm_sta)
                                {
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                        ID_MOVIE_SSET_PAUSE_UF_BMP, MOVIE_SSET_NC_PLAY,FOCUSE_NORMAL);
                                    movie_cmd2sset(movie_ctrl->sset_sence,SSET_STRING_UPDATE,
                                        STRING_MOVIE_STOP, MOVIE_SSET_NC_NEXT, 1 );
                                }

                        }
                        else if (index  == MOVIE_SSET_NC_ZOOM || MOVIE_SSET_NC_SUBTITLE || MOVIE_SSET_NC_PLAYMODE  /
                            MOVIE_SSET_NC_CHANNEL || MOVIE_SSET_NC_TRACK || MOVIE_SSET_NC_AB)
                        {
                            __msg("MOVIE_SSET_NC_other");
                            if (CEDAR_STAT_PLAY == fsm_sta)
                            {
                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                    ID_MOVIE_SSET_PLAY_UF_BMP, MOVIE_SSET_NC_PLAY, FOCUSE_NORMAL );
                            }
                            else if (CEDAR_STAT_PAUSE == fsm_sta)
                            {
                                movie_cmd2sset(movie_ctrl->sset_sence,SSET_ITEM_UPDATE,
                                    ID_MOVIE_SSET_PAUSE_UF_BMP, MOVIE_SSET_NC_PLAY,FOCUSE_NORMAL);
                            }
                        }
                        */
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }
            else
            {
                __msg("movie_ctrl is null...");
            }

            return EPDK_OK;
        }

        case DSK_MSG_TVDAC_PLUGOUT:
        case DSK_MSG_HDMI_PLUGOUT:
        {
            movie_ctrl_t *movie_ctrl;
            __msg("************DSK_MSG_TVDAC_PLUGOUT/DSK_MSG_HDMI_PLUGOUT*******");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __err("movie_ctrl is null...");
                return EPDK_OK;
            }

            //__msg("before __movie_switch_to_tv_or_lcd");
            //__movie_switch_to_tv_or_lcd(movie_ctrl);
            //__msg("after __movie_switch_to_tv_or_lcd");
            if (is_on_tv()) //退出tvout
            {
                //__here__;
                __movie_switch_to_tv_or_lcd(movie_ctrl);
                //goto key_ok;
            }

            return EPDK_OK;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __bool to_exist;//用于记录是否退出
            __s32 ret;
            char diskname[2] = {0};
            char cur_play_file[RAT_MAX_FULL_PATH_LEN] = {0};
            movie_ctrl_t *movie_ctrl;
            __msg("************DSK_MSG_FS_PART_PLUGOUT*******");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __msg("movie_ctrl is null...");
                return EPDK_OK;
            }

            diskname[0] = (char)(msg->dwAddData2);
            diskname[1] = 0;
            to_exist = 1;//默认需要退出
            ret = robin_npl_get_cur();

            if (-1 != ret)
            {
                ret = robin_npl_index2file(ret, cur_play_file);

                if (EPDK_OK == ret)
                {
                    __msg("%s %c", cur_play_file, diskname[0]);

                    if (0 != eLIBs_strnicmp(cur_play_file, diskname, 1))
                    {
                        to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
                    }
                }
            }

            __msg("to_exist=========%d ", to_exist);

            if (1 == to_exist) //需要退出
            {
                // __movie_exit_movie(movie_ctrl, 1);
                {
                    __gui_msg_t dmsg = {0};
                    eLIBs_memset(&dmsg, 0, sizeof(__gui_msg_t));
                    dmsg.id = GUI_MSG_CLOSE;
                    dmsg.dwAddData1 = 1;
                    dmsg.h_deswin = msg->h_deswin;
                    GUI_SendNotifyMessage(&dmsg);
                }
            }

            return EPDK_OK;
        }

        case DSK_MSG_HOME:
        {
            movie_ctrl_t *movie_ctrl;
            __msg("************DSK_MSG_HOME*******");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __msg("movie_ctrl is null...");
                return EPDK_OK;
            }

            __movie_exit_movie(movie_ctrl, 1);
            return EPDK_OK;
        }

        case DSK_MSG_ALARM:
        {
            movie_ctrl_t *movie_ctrl;
            __msg("************DSK_MSG_ALARM*******");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __msg("movie_ctrl is null...");
                return EPDK_OK;
            }

            robin_cancle_ab_v2();
            //__movie_uninstall_auto_play_timmer(movie_ctrl);//禁止自动播放
            __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID));
            movie_ctrl->current_npl_index = robin_npl_get_cur();    //保存当前播放下标
            __movie_save_breakpoint_info(movie_ctrl->current_npl_index);    // 保存当前播放信息
            __movie_stop_play(movie_ctrl);
            movie_ctrl->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
            return EPDK_OK;
        }

        case ALARM_MSG_CLOSE:
        {
            movie_ctrl_t *movie_ctrl;
            __msg("************ALARM_MSG_CLOSE*******");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __msg("movie_ctrl is null...");
                return EPDK_OK;
            }

            if (movie_ctrl->h_frm_alarm_coming)
            {
                movie_ctrl->h_frm_alarm_coming = NULL ;
            }

            robin_npl_set_cur(movie_ctrl->current_npl_index);
            __movie_start_play(movie_ctrl, EPDK_TRUE);
            __movie_install_auto_play_timmer(movie_ctrl);
            robin_enable_ab();
            GUI_WinSetFocusChild(msg->h_deswin);
            return EPDK_OK;
        }

        case GUI_MSG_SET_TVOUT_TP_HOOK_MSG:
        {
            movie_ctrl_t *movie_ctrl;
            __msg("************GUI_MSG_SET_TVOUT_TP_HOOK_MSG*******");
            movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!movie_ctrl)
            {
                __msg("movie_ctrl is null...");
                return EPDK_OK;
            }

            __movie_uninstall_set_cb_timmer(movie_ctrl);
            __wrn("before msg_emit_register_tvout_tp_hook");
            msg_emit_register_tvout_tp_hook((cb_tvout_tp_hook)esKRNL_GetCallBack((__pCBK_t)__movie_tvout_tp_hook));
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN app_movie_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    H_WIN hManWin;
    __inf("****************************************************************************************");
    __inf("********  enter movie app  **************");
    __inf("****************************************************************************************");
    //esKSRV_SysInfo();
    gscene_hbar_set_state(HBAR_ST_HIDE);
    gscene_bgd_set_state(BGD_STATUS_HIDE);
    GUI_Memdev_FrameBuffer_Release(0);//必须
    //SetCurrentDevTypeEx(para->root_type);
#if HT_1628_LED_EX
    SetFoucsWindowEx(APP_MOVIE_ID);
#endif
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = APP_MOVIE;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__app_movie_proc);
    create_info.attr            = (void *)para;
    create_info.id              = APP_MOVIE_ID;
    create_info.hHosting        = NULL;
    hManWin = GUI_ManWinCreate(&create_info);
    __msg("app_movie_create, hManWin=%x", hManWin);

    if (hManWin)
    {
        __s32 reg_storage_type = 0;
        reg_root_para_t *root_reg_para = NULL;

        if (para)
        {
            if (RAT_TF == para->root_type)
            {
                SetCurrentDevTypeEx(RAT_TF);
                reg_storage_type = 0;
            }
            else if (RAT_USB == para->root_type)
            {
                SetCurrentDevTypeEx(RAT_USB);
                reg_storage_type = 1;
            }
            else
            {
                __wrn("para->root_para->root_type invalid...");
            }
        }

        root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

        if (root_reg_para)
        {
            __s32 reg_app_type = 0;
            reg_app_type = 1;
            root_reg_para->cur_play_storage = reg_storage_type;
            root_reg_para->last_play_app = reg_app_type;
            root_reg_para->last_app_play_storage[reg_app_type] = reg_storage_type;
            root_reg_para->last_storage_play_app[reg_storage_type] = reg_app_type;
        }
        else
        {
            __wrn("para is null...");
        }
    }

    return hManWin;
}

__s32 app_movie_notify_delete_sub_scene(H_WIN hmanwin)
{
    movie_ctrl_t *movie_ctrl = NULL;

    if (!hmanwin)
    {
        __err("invalid para...");
    }

    movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(hmanwin);

    if (NULL == movie_ctrl)
    {
        __wrn("movie_ctrl is null...");
        return EPDK_FAIL;
    }

    if (movie_ctrl->h_frm_alarm_coming)
    {
        app_alarm_frm_destory(movie_ctrl->h_frm_alarm_coming);
        movie_ctrl->h_frm_alarm_coming = NULL ;
    }

    //__movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL&(~MOVIE_SUB_SHOW_ID)&(~MOVIE_SSET_ID));
    __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL & (~MOVIE_SUB_SHOW_ID)); // 30234384
    //__here__;
    gscene_hbar_set_state(HBAR_ST_HIDE);
    return EPDK_OK;
}

__s32 app_movie_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
    movie_ctrl_t *movie_ctrl;

    if (!hmanwin)
    {
        __err("invalid para...");
    }

    movie_ctrl = (movie_ctrl_t *)GUI_WinGetAddData(hmanwin);

    if (!movie_ctrl)
    {
        __wrn("movie_ctrl is null...");
        return EPDK_FAIL;
    }

    if (movie_ctrl->h_frm_alarm_coming)
    {
        app_alarm_frm_destory(movie_ctrl->h_frm_alarm_coming);
        movie_ctrl->h_frm_alarm_coming = NULL ;
    }

    if (!GUI_IsTimerInstalled(movie_ctrl->movie_manager, movie_ctrl->auto_ffrr_timmer_id))
    {
        __movie_delete_subscene_by_id(movie_ctrl, MOVIE_SUB_SCENE_TYPE_ALL &
                                      (~MOVIE_SUB_SHOW_ID) & (~MOVIE_SSET_ID) & (~MOVIE_SPSC_ID));
    }

    __movie_install_auto_del_habr_timmer(movie_ctrl);
    return EPDK_OK;
}

