/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : movie_spsc.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include <log.h>
#include "spsc_man.h"
#include "../common/common.h"
#include "spsc_ctrl/spsc_ctrl.h"

typedef enum
{
    movie_spsc_ctrl_scene_id = 1231,
    movie_spsc_scene_id_max
} movie_spsc_scene_id_t;

typedef struct
{
    //input
    H_WIN hparent;//spsc 的parent
    __s32 scene_id;
    __s32 sub_state;

    //internal
    H_LYR hlyr;//各个子场景的图层
    void *spsc_ctrl_scene;//控制子场景

    H_WIN spsc_manager;//spsc manwin
} movie_spsc_scene_t;

static __s32 __movie_spsc_move_layer_win_up(movie_spsc_scene_t *sence_para);


/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR __spsc_32bpp_layer_create(RECT *rect, __s32 pipe)
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
        //DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        9,                                           /* prio      */
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
    }

    return layer;
}

//创建控制子场景
static __s32 __spsc_ctrl_scene_create(movie_spsc_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    {
        movie_spsc_ctrl_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(movie_spsc_ctrl_create_para_t));
        create_para.hparent  = scene_para->spsc_manager;
        create_para.scene_id = movie_spsc_ctrl_scene_id;
        create_para.hlyr      = scene_para->hlyr;
        create_para.sub_state = scene_para->sub_state;
        scene_para->spsc_ctrl_scene = movie_spsc_ctrl_scene_create(&create_para);

        if (!scene_para->spsc_ctrl_scene)
        {
            __msg("movie_spsc_ctrl_scene_create fail...");
            return EPDK_FAIL;
        }

        __msg("movie_spsc_ctrl_scene_create success...");
        movie_spsc_ctrl_scene_set_focus(scene_para->spsc_ctrl_scene);
        __msg("movie_spsc_ctrl_scene_set_focus");
    }

    return EPDK_OK;
}

//删除控制子场景
__s32 __spsc_ctrl_scene_delete(movie_spsc_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (scene_para->spsc_ctrl_scene)
    {
        __msg("before movie_spsc_ctrl_scene_delete");
        movie_spsc_ctrl_scene_delete(scene_para->spsc_ctrl_scene);
        scene_para->spsc_ctrl_scene = NULL;
        __msg("after movie_spsc_ctrl_scene_delete");
    }

    return EPDK_OK;
}

static __s32 __spsc_ctrl_msg_match(__s32 msgid)
{
    __s32 ret_id;
    ret_id = -1;

    switch (msgid)
    {
        case spsc_ctrl_scene_msg_rr:
        {
            ret_id = spsc_scene_msg_rr;
            break;
        }

        case spsc_ctrl_scene_msg_tvout:
        {
            __msg("switch to tvout,,,,,,,");
            ret_id = spsc_scene_msg_tvout;
            break;
        }

        default:
        {
            //__here__;
            break;
        }
    }

    return ret_id;
}

static __s32 __spsc_save_breakpoint_info(__s32 index)
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

    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return -1;
    }

    if (!fifo_db_has_exist(NAME_MOVIE_BKPOINT))
    {
        ret = create_fifo_db(NAME_MOVIE_BKPOINT, 4, sizeof(__cedar_tag_inf_t));

        if (0 != ret)
        {
            __msg("create_fifo_db fail...");
            return -1;
        }
    }

    ret = add_item_to_fifo_db(NAME_MOVIE_BKPOINT, filename, &tag_inf);
    return ret;
}

static __s32 __spsc_delete_breakpoint_info(__s32 index)
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


static __s32 __spsc_play_file(movie_spsc_scene_t *sence_para, __s32 index)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __cedar_tag_inf_t tag_inf;
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return EPDK_FAIL;
    }

    robin_set_cmd_stop();//先停止播放当前首
    __msg("robin_play_file, index=%d, filename=%s", index, filename);
    robin_clear_feedback_msgQ();//清空错误信息消息队列
    ret = robin_play_file(filename, NULL);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    //__here__;
    return ret;
}

static __s32 __spsc_play_next(movie_spsc_scene_t *sence_para)
{
    __s32 ret;
    __s32 index;
    __s32 total;
    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    __spsc_delete_breakpoint_info(index);
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
    ret = __spsc_play_file(sence_para, index);
    return ret;
}

static __s32 __spsc_play_prev(movie_spsc_scene_t *sence_para)
{
    __s32 ret;
    __s32 index;
    __s32 total;
    index = robin_npl_get_cur();

    if (-1 == index)
    {
        __msg("robin_npl_get_cur fail...");
        return EPDK_FAIL;
    }

    __spsc_delete_breakpoint_info(index);
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

    robin_npl_set_cur(index);
    ret = __spsc_play_file(sence_para, index);
    return ret;
}

static __s32 __spsc_ctrl_scene_cmd_proc(movie_spsc_scene_t *sence_para, __s32 msg_id, __s32 dwAddData2)
{
    __msg("__spsc_ctrl_scene_cmd_proc msg_id=%d, dwAddData2=%d", msg_id, dwAddData2);

    switch (msg_id)
    {
        case spsc_ctrl_scene_msg_prev:
        {
            __msg("spsc_ctrl_scene_msg_prev");

            if (1 == dwAddData2)
            {
                __cedar_status_t cedar_sta;
                cedar_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == cedar_sta)
                {
                    robin_set_cmd_rr();
                }
            }
            else
            {
                MOVIE_ROBIN_PEND;
                __spsc_play_prev(sence_para);

                if (1) //父窗口通知spsc_ctrl场景更新UI 视频文件名字
                {
                    __gui_msg_t msg;
                    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
                    msg.h_deswin = movie_spsc_ctrl_scene_get_hwnd(sence_para->spsc_ctrl_scene);

                    if (!msg.h_deswin)
                    {
                        MOVIE_ROBIN_POST;
                        __msg("msg.h_deswin is null...");
                        return EPDK_FAIL;
                    }

                    msg.id = movie_spsc_ctrl_scene_update_fileinfo;
                    msg.dwAddData1 = 0;
                    GUI_SendNotifyMessage(&msg);
                }

                MOVIE_ROBIN_POST;
            }

            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_next:
        {
            __msg("spsc_ctrl_scene_msg_next");

            if (1 == dwAddData2)
            {
                __cedar_status_t cedar_sta;
                cedar_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == cedar_sta)
                {
                    robin_set_cmd_ff();
                }
            }
            else
            {
                MOVIE_ROBIN_PEND;
                __spsc_play_next(sence_para);

                if (1) //父窗口通知spsc_ctrl场景更新UI
                {
                    __gui_msg_t msg;
                    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

                    if (sence_para->spsc_ctrl_scene)
                    {
                        msg.h_deswin = movie_spsc_ctrl_scene_get_hwnd(sence_para->spsc_ctrl_scene);

                        if (!msg.h_deswin)
                        {
                            MOVIE_ROBIN_POST;
                            __msg("msg.h_deswin is null...");
                            return EPDK_FAIL;
                        }

                        msg.id = movie_spsc_ctrl_scene_update_fileinfo;
                        msg.dwAddData1 = 0;
                        GUI_SendNotifyMessage(&msg);
                    }
                }

                MOVIE_ROBIN_POST;
            }

            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_playpause:
        {
            __msg("spsc_ctrl_scene_msg_playpause");

            if (dwAddData2)
            {
                __msg("robin_set_cmd_play");
                robin_set_cmd_play();
            }
            else
            {
                __msg("robin_set_cmd_pause");
                robin_set_cmd_pause();
            }

            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_tvout:
        {
            __msg("spsc_ctrl_scene_msg_tvout");
            /*__gui_msg_t msg;
            __msg("++++++movie_spsc_ctrl_icon_tvout++++++++");
                    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
                    if(sence_para->spsc_ctrl_scene)
                    {
                        msg.h_deswin = movie_spsc_ctrl_scene_get_hwnd(sence_para->spsc_ctrl_scene);
                        if (!msg.h_deswin)
                        {
                            ;
                            __msg("msg.h_deswin is null...");
                            return EPDK_FAIL;
                        }
                        msg.id = spsc_scene_msg_tvout;
                        msg.dwAddData1 = 0;
                        GUI_SendNotifyMessage(&msg);
                    }
                //  msg.h_deswin = movie_spsc_ctrl_scene_get_hwnd(sence_para->spsc_ctrl_scene);
            //movie_cmd2parent(msg->h_deswin, spsc_scene_msg_tvout, 1, 0);
            return EPDK_OK;*/
            break;//消息还要交给父窗口处理，通知父窗口进行了rr操作，更新一下状态
        }

        case spsc_ctrl_scene_msg_rr:
        {
            __msg("spsc_ctrl_scene_msg_rr");
            {
                __cedar_status_t fsm_sta;
                fsm_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == fsm_sta)
                {
                    __msg("before robin_set_cmd_rr");
                    robin_set_cmd_rr();
                    __msg("after robin_set_cmd_rr");
                }
                else
                {
                    __msg("fsm not in play status, do not rr");
                }
            }
            break;//消息还要交给父窗口处理，通知父窗口进行了rr操作，更新一下状态
            //return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_ff:
        {
            __msg("spsc_ctrl_scene_msg_ff");
            {
                __cedar_status_t fsm_sta;
                fsm_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == fsm_sta)
                {
                    __msg("before robin_set_cmd_ff");
                    robin_set_cmd_ff();
                    __msg("after robin_set_cmd_ff");
                }
                else
                {
                    __msg("fsm not in play status, do not ff");
                }
            }
            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_timeset:
        {
            __msg("spsc_ctrl_scene_msg_timeset");
            {
                __cedar_status_t fsm_sta;
                fsm_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == fsm_sta)
                {
                    __msg("before robin_set_cmd_pause");
                    robin_set_cmd_jump(dwAddData2);
                    __msg("after robin_set_cmd_pause");
                }
            }
            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_timeout:
        {
            __movie_spsc_move_layer_win_up(sence_para);
            movie_cmd2parent(sence_para->spsc_manager, spsc_scene_msg_timeout, 0, 0);
            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_info:
        {
            __movie_spsc_move_layer_win_up(sence_para);
            movie_cmd2parent(sence_para->spsc_manager, spsc_scene_msg_infor, 0, 0);
            return EPDK_OK;
        }

        case spsc_ctrl_scene_msg_switch:
        {
            __msg("spsc_ctrl_scene_msg_switch");
            __movie_spsc_move_layer_win_up(sence_para);
            movie_cmd2parent(sence_para->spsc_manager, spsc_scene_msg_switch, 0, 0);
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return EPDK_FAIL;
}

static __s32 __movie_spsc_move_layer_win_down(movie_spsc_scene_t *sence_para)
{
    movie_spsc_uipara_t *ui_para;
    ui_para = movie_spsc_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return NULL;
    }

    {
        __s32 i;
        __s32 n;
        RECT lyr_rect;
        lyr_rect.x      = ui_para->spsc_scene_lyr.x;
        lyr_rect.y      = -ui_para->spsc_scene_lyr.h;//ui_para->spsc_scene_lyr.y;
        lyr_rect.width  = ui_para->spsc_scene_lyr.w;
        lyr_rect.height = ui_para->spsc_scene_lyr.h;
        n = ui_para->spsc_scene_lyr.h / 20;

        for (i = 0 ; i < n ; i++)
        {
            lyr_rect.y += 20;
            GUI_LyrWinSetScnWindow(sence_para->hlyr, &lyr_rect);
            esKRNL_TimeDly(2);
        }

        lyr_rect.x      = ui_para->spsc_scene_lyr.x;
        lyr_rect.y      = ui_para->spsc_scene_lyr.y;
        lyr_rect.width  = ui_para->spsc_scene_lyr.w;
        lyr_rect.height = ui_para->spsc_scene_lyr.h;
        GUI_LyrWinSetScnWindow(sence_para->hlyr, &lyr_rect);
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_move_layer_win_up(movie_spsc_scene_t *sence_para)
{
    movie_spsc_uipara_t *ui_para;

    if (NULL == sence_para)
    {
        __err("invalid para..");
        return NULL;
    }

    ui_para = movie_spsc_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return NULL;
    }

    {
        __s32 i;
        __s32 n;
        RECT lyr_rect;
        lyr_rect.x      = ui_para->spsc_scene_lyr.x;
        lyr_rect.y      = ui_para->spsc_scene_lyr.y;
        lyr_rect.width  = ui_para->spsc_scene_lyr.w;
        lyr_rect.height = ui_para->spsc_scene_lyr.h;
        n = ui_para->spsc_scene_lyr.h / 20;

        for (i = 0 ; i < n ; i++)
        {
            lyr_rect.y -= 20;
            GUI_LyrWinSetScnWindow(sence_para->hlyr, &lyr_rect);
            esKRNL_TimeDly(2);
        }

        lyr_rect.x      = ui_para->spsc_scene_lyr.x;
        lyr_rect.y      = -ui_para->spsc_scene_lyr.h;
        lyr_rect.width  = ui_para->spsc_scene_lyr.w;
        lyr_rect.height = ui_para->spsc_scene_lyr.h;
        GUI_LyrWinSetScnWindow(sence_para->hlyr, &lyr_rect);
    }

    return EPDK_OK;
}
__s32 __movie_spsc_hide_layer_win(void *handle)
{
    __s32 ret;
    movie_spsc_scene_t *sence_para;
    sence_para = (movie_spsc_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return NULL;
    }

    ret = __movie_spsc_move_layer_win_up(sence_para);
    return ret;
}

static __s32 __spsc_scene_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __s32 ret;
            movie_spsc_scene_t *sence_para = NULL;
            __msg("__movie_spsc_proc GUI_MSG_CREATE begin");
            //GUI_MSG_PAINT
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            sence_para->spsc_manager = msg->h_deswin;//必须在这里设置，否则子场景得到的父亲仍然是空。
            __movie_set_title(STRING_MOVIE_TITLE);
            //__here__;
            gscene_hbar_set_state(HBAR_ST_SHOW);
            //创建控制子场景
            ret = __spsc_ctrl_scene_create(sence_para);

            if (EPDK_FAIL == ret)
            {
                __msg("__spsc_ctrl_scene_create fail...");
                return EPDK_FAIL;
            }

            //movie_spsc_ctrl_scene_set_focus(sence_para->spsc_ctrl_scene);
            __msg("__movie_spsc_proc GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            movie_spsc_scene_t *sence_para;
            __msg("__movie_spsc_proc GUI_MSG_DESTROY begin");
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            __msg("before __spsc_ctrl_scene_delete...");
            __spsc_ctrl_scene_delete(sence_para);
            __msg("after __spsc_ctrl_scene_delete...");
            //__here__;gscene_hbar_set_state(HBAR_ST_HIDE);
            __msg("__movie_spsc_proc GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            __s32 ret;
            __s32 ret_id;
            movie_spsc_scene_t *sence_para;
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            ret = -1;
            ret_id = -1;

            switch (LOWORD(msg->dwAddData1))
            {
                case movie_spsc_ctrl_scene_id:
                {
                    ret = __spsc_ctrl_scene_cmd_proc(sence_para, HIWORD(msg->dwAddData1), msg->dwAddData2);

                    if (EPDK_FAIL == ret)//未处理的消息交给父窗口处理
                    {
                        ret_id = __spsc_ctrl_msg_match(HIWORD(msg->dwAddData1));
                    }

                    break;
                }

                default:
                {
                    __msg("unknown scene id...");
                    break;
                }
            }

            if (0 == ret)//已处理
            {
                //__here__;
                return EPDK_OK;
            }
            else//未处理，交给父窗口处理，音量和亮度的调整消息交给父窗口处理
            {
                if (-1 == ret_id)
                {
                    __msg("unknown msg id...");
                    return EPDK_OK;
                }

                __msg("spsc manwin not proc msg, send to parent, ret_id = %d", ret_id);
                movie_cmd2parent(msg->h_deswin, ret_id, 0, 0);
            }

            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            movie_spsc_scene_t *sence_para;
            __msg("__movie_spsc_proc GUI_MSG_KEY");//按键消息全部往下传到子场景frmwin
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            break;
        }

        case GUI_MSG_TOUCH:
        {
            movie_spsc_scene_t *sence_para;
            __msg("__movie_spsc_proc GUI_MSG_TOUCH");//触摸消息全部往下传到子场景frmwin
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            break;
        }

        case GUI_MSG_CLOSE:
        {
            __msg("__movie_spsc_proc GUI_MSG_CLOSE begin");
            movie_cmd2parent(msg->h_deswin, spsc_scene_msg_timeout, 0, 0);
            __msg("__movie_spsc_proc GUI_MSG_CLOSE end");
            return EPDK_OK;
        }

        case movie_spsc_scene_update_fileinfo:
        {
            movie_spsc_scene_t *sence_para;
            __s32 ret;
            __msg("movie_spsc_scene_update_fileinfo begin");
            ret = -1;
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            {
                //父窗口通知spsc_ctrl场景更新UI
                __gui_msg_t msg;
                eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
                msg.h_deswin = movie_spsc_ctrl_scene_get_hwnd(sence_para->spsc_ctrl_scene);

                if (!msg.h_deswin)
                {
                    __msg("msg.h_deswin is null...");
                    return EPDK_FAIL;
                }

                msg.id = movie_spsc_ctrl_scene_update_fileinfo;
                msg.dwAddData1 = 0;
                GUI_SendMessage(&msg);
            }

            __msg("movie_spsc_scene_update_fileinfo end");
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_ManWinDefaultProc(msg);
}

void *movie_spsc_scene_create(movie_spsc_create_para_t *create_para)
{
    __gui_manwincreate_para_t create_info;
    H_WIN hManWin;
    movie_spsc_scene_t *sence_para = NULL;
    //创建和初始化spsc manwin 的 sence_para
    {
        movie_spsc_uipara_t *ui_para = NULL;
        RECT lyr_rect;
        sence_para = (movie_spsc_scene_t *)esMEMS_Malloc(0, sizeof(movie_spsc_scene_t));

        if (NULL == sence_para)
        {
            __msg("mem not enough...");
            return NULL;
        }

        eLIBs_memset(sence_para, 0, sizeof(movie_spsc_scene_t));
        //接收外部参数
        sence_para->hparent = create_para->hparent;
        sence_para->scene_id = create_para->scene_id;
        //创建图层
        ui_para = movie_spsc_get_uipara(GUI_GetScnDir());

        if (NULL == ui_para)
        {
            esMEMS_Mfree(0, sence_para);
            sence_para = NULL;
            __msg("movie_spsc_get_uipara fail...");
            return NULL;
        }

        lyr_rect.x      = ui_para->spsc_scene_lyr.x;
        lyr_rect.y      = ui_para->spsc_scene_lyr.y;//ui_para->spsc_scene_lyr.y;
        //lyr_rect.y    = -ui_para->spsc_scene_lyr.h;
        lyr_rect.width  = ui_para->spsc_scene_lyr.w;
        lyr_rect.height = ui_para->spsc_scene_lyr.h;
        __msg("lyr_rect.x = %d", lyr_rect.x);
        __msg("lyr_rect.y = %d", lyr_rect.y);
        __msg("lyr_rect.width = %d", lyr_rect.width);
        __msg("lyr_rect.height = %d", lyr_rect.height);
        sence_para->hlyr = __spsc_32bpp_layer_create(&lyr_rect, ui_para->pipe);

        if (NULL == sence_para->hlyr)
        {
            esMEMS_Mfree(0, sence_para);
            sence_para = NULL;
            __msg("sence_para->hlyr is null...");
            return NULL;
        }
    }
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = "spsc_man_win";
    create_info.hParent         = sence_para->hparent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__spsc_scene_proc);
    create_info.attr            = (void *)sence_para;
    create_info.id              = sence_para->scene_id;
    create_info.hHosting        = NULL;
    hManWin = GUI_ManWinCreate(&create_info);
    __msg("spsc manwin, hManWin=%x", hManWin);

    if (!hManWin)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        return NULL;
    }

    //场景内部参数设置
    sence_para->spsc_manager = hManWin;
    GUI_LyrWinSetSta(sence_para->hlyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(sence_para->hlyr);
    __movie_spsc_move_layer_win_down(sence_para);
    return sence_para;
}

__s32 movie_spsc_scene_delete(void *handle)
{
    movie_spsc_scene_t *scene_para;
    scene_para = (movie_spsc_scene_t *)handle;
    __msg("before movie_spsc_manwin_delete");

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    if (!scene_para->spsc_manager)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    GUI_ManWinDelete(scene_para->spsc_manager);
    scene_para->spsc_manager = NULL;

    if (!scene_para->hlyr)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    GUI_LyrWinDelete(scene_para->hlyr);
    scene_para->hlyr = NULL;
    eLIBs_memset(scene_para, 0, sizeof(movie_spsc_scene_t));//预防再次释放，会报错
    esMEMS_Mfree(0, scene_para);
    __msg("after movie_spsc_manwin_delete");
    return EPDK_OK;
}

__s32 movie_spsc_scene_suspend(void *handle)
{
    //  movie_spsc_scene_t* scene_para;
    //
    //  scene_para = (movie_spsc_scene_t*)handle;
    //
    //  __msg("before movie_spsc_manwin_delete");
    //  if (NULL == handle)
    //  {
    //      __err("invalid para..");
    //      return EPDK_FAIL;
    //  }
    //
    //  if (!scene_para->hlyr)
    //  {
    //      __err("invalid para...");
    //      return EPDK_FAIL;
    //  }
    //
    //  spsc_timeset_scene_suspend(create_para->spsc_timeset_scene);
    //
    //  if (GUI_LYRWIN_STA_ON  == GUI_LryWinGetSta(scene_para->hlyr))
    //  {
    //      GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_SUSPEND);
    //  }
    return EPDK_OK;
}

__s32 movie_spsc_scene_reseum(void *handle)
{
    //  movie_spsc_scene_t* scene_para;
    //
    //  scene_para = (movie_spsc_scene_t*)handle;
    //
    //  __msg("before movie_spsc_manwin_delete");
    //  if (NULL == handle)
    //  {
    //      __err("invalid para..");
    //      return EPDK_FAIL;
    //  }
    //
    //  if (!scene_para->hlyr)
    //  {
    //      __err("invalid para...");
    //      return EPDK_FAIL;
    //  }
    //
    //  spsc_timeset_scene_reseum(create_para->spsc_timeset_scene);
    //
    //  if (GUI_LYRWIN_STA_ON  != GUI_LryWinGetSta(scene_para->hlyr))
    //  {
    //      GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
    //      GUI_LyrWinSetTop(sence_para->hlyr);
    //  }
    return EPDK_OK;
}

H_WIN movie_spsc_scene_get_hwnd(void *handle)
{
    movie_spsc_scene_t *scene_para;
    scene_para = (movie_spsc_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return NULL;
    }

    return scene_para->spsc_manager;
}

__s32 movie_spsc_scene_get_lyr_scn(RECT *rect)
{
    movie_spsc_uipara_t *ui_para;
    ui_para = movie_spsc_get_uipara(GUI_GetScnDir());

    if (!ui_para)
    {
        __err("ui_para is null...");
        return EPDK_FAIL;
    }

    rect->x = ui_para->spsc_scene_lyr.x;
    rect->y = ui_para->spsc_scene_lyr.y;
    rect->width = ui_para->spsc_scene_lyr.w;
    rect->height = ui_para->spsc_scene_lyr.h;
    return EPDK_OK;
}

