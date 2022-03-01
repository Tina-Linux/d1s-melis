/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : spsc_ctrl.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include <log.h>
#include "spsc_ctrl.h"
#include "spsc_ctrl_ui.h"
#include "common/common.h"
typedef struct
{
    // 输入参数
    H_LYR hlyr;
    H_WIN hparent;
    __s32 scene_id;
    __s32 sub_state;

    //内部参数
    H_WIN hfrm;
    __hdle long_str_handle;
    __s32 prog_timmer_id;//进度和时间重绘定时器分开，进度不需要这么实时
    char long_string[RAT_MAX_FULL_PATH_LEN];
    __s32 time_timmer_id;
    __s32 hide_timmer_id;
    __s32 playsta;

} movie_spsc_ctrl_scene_t;

//定义prog子场景的ui类型
#define MOVIE_SPSC_PROG_UI_TYPE_PROG            0x01
#define MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME        0x02
#define MOVIE_SPSC_PROG_UI_TYPE_TOTAL_TIME  0x04
#define MOVIE_SPSC_PROG_UI_TYPE_BG          0x08

//prog的所有ui类型
#define MOVIE_SPSC_PROG_UI_TYPE_ALL (MOVIE_SPSC_PROG_UI_TYPE_PROG\
                                     |MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME\
                                     |MOVIE_SPSC_PROG_UI_TYPE_TOTAL_TIME\
                                     |MOVIE_SPSC_PROG_UI_TYPE_BG)

typedef union
{
    struct
    {
        __s32    hour;
        __s32    minute;
        __s32    second;
    } data1;

    __s32 data2[3];
} spsc_prog_cuckoo_time_t;


//除背景外的其他ui类型
#define MOVIE_SPSC_PROG_UI_TYPE_OTHER    (MOVIE_SPSC_PROG_UI_TYPE_ALL&(~MOVIE_SPSC_PROG_UI_TYPE_BG))

static __s32 __movie_spsc_ctrl_icon_draw_item(movie_spsc_ctrl_scene_t *scene_para, __s32 index, __s32 focus);

static void __time2time(__u32 ms, spsc_prog_cuckoo_time_t *format_time)
{
    __u32  total_minute;
    __u32 second_time;
    second_time = ms / 1000;
    total_minute = second_time / 60;
    format_time->data1.second = second_time % 60;
    format_time->data1.minute = total_minute % 60;
    format_time->data1.hour   = total_minute / 60;
}

static __s32 __movie_spsc_ctrl_init_ui(void)
{
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FAIL;
    }

    {
        int j;

        for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
        {
            if (ui_para->uipara_bg.res_id[j])
            {
                ui_para->uipara_bg.res_hdl[j] = dsk_theme_open(ui_para->uipara_bg.res_id[j]);

                if (!ui_para->uipara_bg.res_hdl[j])
                {
                    __msg("dsk_theme_open fail...");
                }
                else//先占住内存
                {
                    //  dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[j]);
                }
            }
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_ctrl_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_id[j])
                {
                    ui_para->uipara_icon[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_icon[i].res_id[j]);

                    if (!ui_para->uipara_icon[i].res_hdl[j])
                    {
                        __msg("dsk_theme_open fail...");
                    }
                    else//先占住内存
                    {
                        //  dsk_theme_hdl2buf(ui_para->uipara_icon[i].res_hdl[j]);
                    }
                }
            }
        }
    }

#if 0
    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_ctrl_static_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_static_icon[i].res_id[j])
                {
                    ui_para->uipara_static_icon[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_static_icon[i].res_id[j]);

                    if (!ui_para->uipara_static_icon[i].res_hdl[j])
                    {
                        __msg("dsk_theme_open fail...");
                    }
                    else//先占住内存
                    {
                        //  dsk_theme_hdl2buf(ui_para->uipara_static_icon[i].res_hdl[j]);
                    }
                }
            }
        }
    }
    {
        int i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_playmode_res) ; i++)
        {
            if (ui_para->uipara_playmode_res[i].res_id)
            {
                ui_para->uipara_playmode_res[i].res_hdl = dsk_theme_open(ui_para->uipara_playmode_res[i].res_id);

                if (!ui_para->uipara_playmode_res[i].res_hdl)
                {
                    __msg("dsk_theme_open fail...");
                }
                else//先占住内存
                {
                    // dsk_theme_hdl2buf(ui_para->uipara_playmode_res[i].res_hdl);
                }
            }
        }
    }
    {
        int i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_screenmode_res) ; i++)
        {
            if (ui_para->uipara_screenmode_res[i].res_id)
            {
                ui_para->uipara_screenmode_res[i].res_hdl = dsk_theme_open(ui_para->uipara_screenmode_res[i].res_id);

                if (!ui_para->uipara_screenmode_res[i].res_hdl)
                {
                    __msg("dsk_theme_open fail...");
                }
                else//先占住内存
                {
                    //  dsk_theme_hdl2buf(ui_para->uipara_screenmode_res[i].res_hdl);
                }
            }
        }
    }
#endif
    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_prog_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_spsc_prog[i].res_id[j])
                {
                    ui_para->uipara_spsc_prog[i].res_hdl[j] = dsk_theme_open(ui_para->uipara_spsc_prog[i].res_id[j]);

                    if (!ui_para->uipara_spsc_prog[i].res_hdl[j])
                    {
                        __msg("dsk_theme_open fail...");
                    }
                }
            }
        }
    }
    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_deinit_ui(void)
{
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FAIL;
    }

    {
        int j;

        for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
        {
            if (ui_para->uipara_bg.res_hdl[j])
            {
                dsk_theme_close(ui_para->uipara_bg.res_hdl[j]);
                ui_para->uipara_bg.res_hdl[j] = NULL;
            }
        }
    }

    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_ctrl_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_icon[i].res_hdl[j])
                {
                    dsk_theme_close(ui_para->uipara_icon[i].res_hdl[j]);
                    ui_para->uipara_icon[i].res_hdl[j] = NULL;
                }
            }
        }
    }

	{
		int i,j;
		for (i = 0 ; i < movie_spsc_prog_icon_num ; i++)
		{
			for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
			{
				if (ui_para->uipara_spsc_prog[i].res_hdl[j])
				{
					dsk_theme_close(ui_para->uipara_spsc_prog[i].res_hdl[j]);
					ui_para->uipara_spsc_prog[i].res_hdl[j] = NULL;
				}
			}
		}
	}

#if 0
    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_ctrl_static_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_static_icon[i].res_hdl[j])
                {
                    dsk_theme_close(ui_para->uipara_static_icon[i].res_hdl[j]);
                    ui_para->uipara_static_icon[i].res_hdl[j] = NULL;
                }
            }
        }
    }
    {
        int i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_playmode_res) ; i++)
        {
            if (ui_para->uipara_playmode_res[i].res_hdl)
            {
                dsk_theme_close(ui_para->uipara_playmode_res[i].res_hdl);
                ui_para->uipara_playmode_res[i].res_hdl = NULL;
            }
        }
    }
    {
        int i;

        for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_screenmode_res) ; i++)
        {
            if (ui_para->uipara_screenmode_res[i].res_hdl)
            {
                if (ui_para->uipara_screenmode_res[i].res_hdl)
                {
                    dsk_theme_close(ui_para->uipara_screenmode_res[i].res_hdl);
                    ui_para->uipara_screenmode_res[i].res_hdl = NULL;
                }
            }
        }
    }
    {
        int i;
        int j;

        for (i = 0 ; i < movie_spsc_prog_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
            {
                if (ui_para->uipara_spsc_prog[i].res_id[j])
                {
                    if (ui_para->uipara_spsc_prog[i].res_hdl[j])
                    {
                        dsk_theme_close(ui_para->uipara_spsc_prog[i].res_hdl[j]);
                        ui_para->uipara_spsc_prog[i].res_hdl[j] = NULL;
                        //__msg("dsk_theme_close ok...");
                    }
                }
            }
        }
    }
#endif
    return EPDK_OK;
}

static __s32 __spsc_ctrl_install_prog_timmer(movie_spsc_ctrl_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    scene_para->prog_timmer_id = 0x14;
    scene_para->time_timmer_id = 0x15;

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->prog_timmer_id
                     , 400 * 1, NULL); //4秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->time_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->time_timmer_id
                     , 400 * 0.25, NULL); //1秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __spsc_ctrl_uninstall_prog_timmer(movie_spsc_ctrl_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->prog_timmer_id);
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->time_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->time_timmer_id);
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_install_hide_timmer(movie_spsc_ctrl_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    scene_para->hide_timmer_id = 0x13;

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                     , 400 * 3, NULL); //12秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_reset_hide_timmer(movie_spsc_ctrl_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->hide_timmer_id))
    {
        GUI_ResetTimer(scene_para->hfrm, scene_para->hide_timmer_id
                       , 400 * 3, NULL); //5秒
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_uninstall_hide_timmer(movie_spsc_ctrl_scene_t *scene_para)
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

static __s32 __movie_spsc_prog_draw_progress(movie_spsc_ctrl_rect_t *prog_bg, movie_spsc_ctrl_rect_t *prog_left,
        movie_spsc_ctrl_rect_t *prog_mid, movie_spsc_ctrl_rect_t *prog_cursor,
        __s32 min, __s32 max, __s32 cur)
{
    __msg("++++++++__movie_spsc_prog_draw_progress++++++++");
    //画进度条背景
    {
        void *pbmp;

        if (!prog_bg->res_hdl[0])
        {
            __msg("prog_bg->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_bg->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_bg->x, prog_bg->y);
    }
    //画进度条左边的图标
    {
        void *pbmp;
        __s32 focus;
        focus = 0;

        if (!prog_left->res_hdl[focus])
        {
            __msg("prog_left->res_hdl[focus] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_left->res_hdl[focus]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, prog_left->x, prog_left->y);
    }

    //画进度条和cursor
    if (cur > max) //保护一下
    {
        cur = max;
    }

    if (cur < min) //保护一下
    {
        cur = min;
    }

    if (cur >= min
        && cur <= max)
    {
        void *pbmp;

        if (!prog_mid->res_hdl[0])
        {
            __msg("prog_mid->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(prog_mid->res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
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

            if (cur == min)
            {
                n = 0;
            }
            else
            {
                n = (prog_bg->w - prog_cursor->w) * (cur - min)
                    / prog_mid->w / (max - min);
            }

            mid_w = prog_mid->w;
            max_mid_w = prog_bg->w - prog_cursor->w;
            cursor_w = prog_cursor->w;
            bg_pos = prog_bg->x;
            bg_w = prog_bg->w;

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

            for (i = 1 ; i < n ; i++)
            {
                GUI_BMP_Draw(pbmp, prog_mid->x + i * prog_mid->w
                             , prog_mid->y);
            }

            //画cursor
            if (!prog_cursor->res_hdl[0])
            {
                __msg("prog_cursor->res_hdl[0] is null...");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(prog_cursor->res_hdl[0]);

            if (!pbmp)
            {
                __msg("pbmp is null...");
                return EPDK_FAIL;
            }

            GUI_BMP_Draw(pbmp, cursor_pos, prog_cursor->y);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_prog_update_prog_ui(movie_spsc_ctrl_scene_t *scene_para
        , __u32 ui_type)
{
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_prog_get_uipara fail...");
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
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(scene_para->hlyr);

    //GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_SLEEP);
    com_memdev_create(scene_para->hlyr);

    //背景
    if (ui_type & MOVIE_SPSC_PROG_UI_TYPE_BG)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]), ui_para->uipara_bg.x, ui_para->uipara_bg.y);
    }

    //当前时间
    //  //__here__;
    if (ui_type & MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME)
    {
        char str_val[32] = {0};
        char str_val_tmp[32] = {0};
        GUI_RECT gui_rect;
        GUI_RECT text_rect;
        spsc_prog_cuckoo_time_t format_time;
        char time_text[20];
        __u32 t;
        __u32 cur_time = 0;
        __s32 screen_width, screen_height;
        t = robin_get_total_time();
        cur_time = robin_get_cur_time();
        __time2time(t, &format_time);

        if (SWFFont)
        {
            GUI_SetFont(SWFFont);
        }
        else
        {
            GUI_SetFont(GUI_GetDefaultFont());
        }

        //GUI_SetColor(APP_COLOR_WHITE);
        GUI_SetColor(GUI_WHITE);
        GUI_SetBkColor(0xa6000000);
        {
            __s32 i;

            //the snippet below will cause total time display error
            //maybe it's obsolete changes, so discard it now.
            #if 0
            if (format_time.data2[0] > 0) //如果小时大于0，则分钟显示99
            {
                format_time.data2[1] = 99;
            }
            #endif

            //          //__here__;
            eLIBs_memset(str_val, 0, sizeof(str_val));

            //accordingly, the initial value of i should be 0
            for (i = 0 ; i < BEETLES_TBL_SIZE(format_time.data2) ; i++)
            {
                eLIBs_sprintf(str_val_tmp, "%.2d", format_time.data2[i]);
                eLIBs_strcat(str_val, str_val_tmp);

                if (i < BEETLES_TBL_SIZE(format_time.data2) - 1)
                {
                    eLIBs_strcat(str_val, ":");
                }
            }

            dsk_display_get_size(&screen_width, &screen_height);

            if ((screen_width == 800) && (screen_height == 480))
            {
                gui_rect.x0 = ui_para->uipara_time.x;
                gui_rect.y0 = ui_para->uipara_time.y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_time.w - 1;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_time.h - 1;
            }
            else
            {
                gui_rect.x0 = ui_para->uipara_time.x - 10;
                gui_rect.y0 = ui_para->uipara_time.y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_time.w - 1;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_time.h - 1;
            }

            //GUI_ClearRectEx(&gui_rect);
            //GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1 , gui_rect.y1);
            //clear total time background
#if 0
            APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
                           gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
                           gui_rect.x1, gui_rect.y1);
#else
            GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
#endif
            text_rect.x0 = ui_para->cur_time.x;
            text_rect.y0 = ui_para->cur_time.y;
            text_rect.x1 = text_rect.x0 + ui_para->cur_time.w;
            text_rect.y1 = text_rect.y0 + ui_para->cur_time.h;
            eLIBs_memset(time_text, 0, sizeof(time_text));
            time2str_by_format(cur_time, time_text, TIME_AUTO_HMS);
            //clear current time background
            GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
            //draw  current time
            GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
            //draw total time
            GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
    }

    //  //__here__;
    if (ui_type & MOVIE_SPSC_PROG_UI_TYPE_PROG)
    {
        __s32 ret;
        ret = __movie_spsc_prog_draw_progress(&ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_bg],
                                              &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_left],
                                              &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_mid],
                                              &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_cursor],
                                              0,
                                              robin_get_total_time(),
                                              robin_get_cur_time());

        if (EPDK_OK != ret)
        {
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            com_memdev_delete();
            //GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
            return EPDK_FAIL;
        }
    }

    //  //__here__;
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
    com_memdev_delete();
    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_play_set_state(movie_spsc_ctrl_scene_t *scene_para, __s32 index)
{
    __s32 i;
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FAIL;
    }

    if (index < 0 || index > 1)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

#if 0

    for (i = 0 ; i < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; i++)
    {
        if (ui_para->uipara_icon[movie_spsc_ctrl_icon_playpause].res_hdl[i])
        {
            dsk_theme_close(ui_para->uipara_icon[movie_spsc_ctrl_icon_playpause].res_hdl[i]);
            ui_para->uipara_icon[movie_spsc_ctrl_icon_playpause].res_hdl[i] = NULL;
        }
    }

    for (i = 0 ; i < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; i++)
    {
        ui_para->uipara_icon[movie_spsc_ctrl_icon_playpause].res_hdl[i] =
            dsk_theme_open(ui_para->uipara_playpause_res[index][i].res_id);
    }

    __movie_spsc_ctrl_icon_draw_item(scene_para, movie_spsc_ctrl_icon_playpause, 0);
    return EPDK_OK;
#endif
    return EPDK_FAIL;

}

static __s32 __movie_spsc_ctrl_icon_draw_item(movie_spsc_ctrl_scene_t *scene_para, __s32 index, __s32 focus)
{
    //draw the pre next pause start button
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FAIL;
    }

    if (index < 0 || index >= movie_spsc_ctrl_icon_num)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (focus < 0 || focus > 1)
    {
        __err("invalid para...");
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
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
        return EPDK_FAIL;
    }

    {
        void *pbmp;

        if (!ui_para->uipara_icon[index].res_hdl[focus])
        {
            __msg("res_hdl is null...");
            return EPDK_FAIL;
        }

        __msg("ui_para->uipara_icon[index].res_hdl[focus] = %x", ui_para->uipara_icon[index].res_hdl[focus]);
        pbmp = dsk_theme_hdl2buf(ui_para->uipara_icon[index].res_hdl[focus]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        __msg("pbmp = %x", pbmp);
        //draw icon
        {
            __s32 ret;
            //__msg("ui_para->uipara_icon[index].x = %d", ui_para->uipara_icon[index].x);
            //__msg(" ui_para->uipara_icon[index].y =%d", ui_para->uipara_icon[index].y);
            GUI_LyrWinSel(scene_para->hlyr);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_SetBkColor(GUI_WHITE);
            __msg("+++++++++++++++++++++++++++++++++++++");
            ret = GUI_BMP_Draw(pbmp, ui_para->uipara_icon[index].x, ui_para->uipara_icon[index].y);

            if (0 != ret)
            {
                __msg("GUI_BMP_Draw fail...");
                return EPDK_FAIL;
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_bg_draw(movie_spsc_ctrl_scene_t *scene_para)
{
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
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
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
        return EPDK_FAIL;
    }

    {
        __s32 j;
        void *pbmp;

        for (j = 0 ; j < MOVIE_SPSC_CTRL_MAX_ICON_RES_NUM ; j++)
        {
            if (ui_para->uipara_bg.res_hdl[j])
            {
                pbmp = dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[j]);

                if (pbmp == NULL)
                {
                    __log("dsk_theme_hdl2buf fail...");
                    continue;
                }

                GUI_BMP_Draw(pbmp, ui_para->uipara_bg.x, ui_para->uipara_bg.y);
            }
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_static_icon_draw(movie_spsc_ctrl_scene_t *scene_para, movie_spsc_ctrl_static_icon_t type)
{
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FAIL;
    }

#if 0

    if (type < 0 || type >= movie_spsc_ctrl_static_icon_num)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

#endif

    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    //__here__;

    if (!scene_para->hlyr)
    {
        __msg("scene_para->hlyr is null..., do not paint");
        return EPDK_FAIL;
    }

    //__here__;
    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr))
    {
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
        return EPDK_FAIL;
    }

    switch (type)
    {
        /*case movie_spsc_ctrl_static_icon_playmode:
        {
            void* pbmp;
            orc_play_mode_e playmode;
            //__here__;

            playmode = robin_get_play_mode();

            if (playmode >= BEETLES_TBL_SIZE(ui_para->uipara_playmode_res))
            {
                playmode = 0;
            }

            if (!ui_para->uipara_playmode_res[playmode].res_hdl)
            {
                __msg("ui_para->uipara_playmode_res[playmode].res_hdl is null...");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(ui_para->uipara_playmode_res[playmode].res_hdl);
            if (!pbmp)
            {
                __msg("dsk_theme_hdl2buf fail...");
                return EPDK_OK;
            }

            GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_playmode].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_playmode].y);

            return EPDK_OK;
        }*/
        /*case movie_spsc_ctrl_static_icon_channel:
        {
            void* pbmp;

            if (!ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel].res_hdl[0])
            {
                __msg("ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel].res_hdl[0]...");
                return EPDK_FAIL;
            }
            //__here__;

            pbmp = dsk_theme_hdl2buf(ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel].res_hdl[0]);
            if (!pbmp)
            {
                __msg("dsk_theme_hdl2buf fail...");
                return EPDK_OK;
            }

            GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel].y);

            return EPDK_OK;
        }
        case movie_spsc_ctrl_static_icon_channel_text:
        {
            char str[32]={0};
            GUI_RECT gui_rect;
            __s32 channel;

            //__here__;

            channel = robin_get_channel();
            if (channel > 2 || channel < 0)
            {
                channel = 0;
            }

            eLIBs_sprintf(str, "%d/3", channel+1);

            if (SWFFont)
            {
                GUI_SetFont(SWFFont);
            }
            else
            {
                GUI_SetFont(GUI_GetDefaultFont());
            }

            GUI_SetColor(APP_COLOR_WHITE);
            GUI_SetBkColor(0xF0);
            GUI_UC_SetEncodeUTF8();

            gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel_text].x;
            gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel_text].y;
            gui_rect.x1 = gui_rect.x0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel_text].w-1;
            gui_rect.y1 = gui_rect.y0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_channel_text].h-1;
            APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
                    gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
                    gui_rect.x1, gui_rect.y1);
            //GUI_ClearRectEx(&gui_rect);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER|GUI_TA_HCENTER);

            return EPDK_OK;
        }*/
        /*case movie_spsc_ctrl_static_icon_screen_mode:
        {
            void* pbmp;
            robin_zoom_e zoom;

            //__here__;

            zoom = robin_get_zoom();

            if (zoom >= BEETLES_TBL_SIZE(ui_para->uipara_screenmode_res))
            {
                zoom = 0;
            }

            if (!ui_para->uipara_screenmode_res[zoom].res_hdl)
            {
                __msg("ui_para->uipara_screenmode_res[zoom].res_hdl is null...");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(ui_para->uipara_screenmode_res[zoom].res_hdl);
            if (!pbmp)
            {
                __msg("dsk_theme_hdl2buf fail...");
                return EPDK_OK;
            }

            GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_screen_mode].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_screen_mode].y);

            return EPDK_OK;
        }*/
        /*case movie_spsc_ctrl_static_icon_file_info:
        {
            __s32 ret;
            __s32 index;
            char file[RAT_MAX_FULL_PATH_LEN]={0};
            char str[256]={0};
            char strtmp[32];

            //__here__;

            index = robin_npl_get_cur();
            if(-1 == index)
            {
                __msg("robin_npl_get_cur fail...");
                return EPDK_FAIL;
            }

            ret = robin_npl_index2file(index, file);
            if(EPDK_FAIL == ret)
            {
                __msg("robin_npl_index2file fail...");
                return EPDK_FAIL;
            }

            //__here__;

            ret = eLIBs_GetFileSize(file);
            //__here__;
            filesize2str(ret, strtmp);
            //__here__;
            dsk_langres_get_menu_text(STRING_MOVIE_FILE_INFO, str, sizeof(str));
            eLIBs_strcat(str, strtmp);
            __msg("str=%s", str);

            {
                GUI_RECT gui_rect;

                if (SWFFont)
                {
                    //__here__;
                    GUI_SetFont(SWFFont);
                    //__here__;
                }
                else
                {
                    //__here__;
                    GUI_SetFont(GUI_GetDefaultFont());
                    //__here__;
                }
                //__here__;
                GUI_SetColor(APP_COLOR_WHITE);
                //__here__;
                GUI_SetBkColor(0xF0);
                //__here__;
                GUI_UC_SetEncodeUTF8();
                //__here__;
                gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].x;
                gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].y;
                gui_rect.x1 = gui_rect.x0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].w-1;
                gui_rect.y1 = gui_rect.y0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_file_info].h-1;
                //__here__;
                //GUI_ClearRectEx(&gui_rect);
                APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
                    gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
                    gui_rect.x1, gui_rect.y1);
                //__here__;
                GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER|GUI_TA_LEFT);
                //__here__;
            }

            return EPDK_OK;
        }*/
        /*case movie_spsc_ctrl_static_icon_video_info:
        {
            __s32 ret;
            char str[256]={0};
            char strtmp[32];

            //return EPDK_OK;//112350

            __msg("***********before robin_get_video_bps*****");
            ret = robin_get_video_bps();
            __msg("************robin_get_video_bps=%d*******", ret);

            if(0 == ret)
            {
                eLIBs_strcpy(strtmp, "");
            }
            else
            {
                eLIBs_sprintf(strtmp, "%dkbps", (ret+1023)/1024);
            }

            //__here__;
            dsk_langres_get_menu_text(STRING_MOVIE_VIDEO_INFO, str, sizeof(str));
            eLIBs_strcat(str, strtmp);
            __msg("str=%s", str);

            {
                GUI_RECT gui_rect;

                if (SWFFont)
                {
                    //__here__;
                    GUI_SetFont(SWFFont);
                    //__here__;
                }
                else
                {
                    //__here__;
                    GUI_SetFont(GUI_GetDefaultFont());
                    //__here__;
                }
                //__here__;
                GUI_SetColor(APP_COLOR_WHITE);
                GUI_SetBkColor(0xF0);
                GUI_UC_SetEncodeUTF8();
                //__here__;
                gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].x;
                gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].y;
                gui_rect.x1 = gui_rect.x0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].w-1;
                gui_rect.y1 = gui_rect.y0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_video_info].h-1;
                //GUI_ClearRectEx(&gui_rect);
                APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
                    gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
                    gui_rect.x1, gui_rect.y1);
                //__here__;
                GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER|GUI_TA_LEFT);
                //__here__;
            }

            return EPDK_OK;
        }*/
        /*case movie_spsc_ctrl_static_icon_subtitle:
        {
            void* pbmp;

            if (!ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle].res_hdl[0])
            {
                __msg("ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle].res_hdl[0]...");
                return EPDK_FAIL;
            }

            pbmp = dsk_theme_hdl2buf(ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle].res_hdl[0]);
            if (!pbmp)
            {
                __msg("dsk_theme_hdl2buf fail...");
                return EPDK_OK;
            }

            GUI_BMP_Draw(pbmp, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle].x, ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle].y);

            return EPDK_OK;
        }*/
        /*case movie_spsc_ctrl_static_icon_subtitle_text:
        {
            char str[32]={0};
            GUI_RECT gui_rect;
            __s32 channel;

            if(scene_para->sub_state)
            {
                eLIBs_strcpy(str, "ON");
            }
            else
            {
                eLIBs_strcpy(str, "OFF");
            }

            if (SWFFont)
            {
                GUI_SetFont(SWFFont);
            }
            else
            {
                GUI_SetFont(GUI_GetDefaultFont());
            }

            GUI_SetColor(APP_COLOR_WHITE);
            GUI_SetBkColor(0xF0);
            GUI_UC_SetEncodeUTF8();

            gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle_text].x;
            gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle_text].y;
            gui_rect.x1 = gui_rect.x0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle_text].w-1;
            gui_rect.y1 = gui_rect.y0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_subtitle_text].h-1;
            //GUI_ClearRectEx(&gui_rect);
            APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
                    gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
                    gui_rect.x1, gui_rect.y1);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_VCENTER|GUI_TA_HCENTER);

            return EPDK_OK;
        }*/
        /* case movie_spsc_ctrl_static_icon_filename:
         {
             __s32 ret;
             __s32 index;
             char src_string[RAT_MAX_FULL_PATH_LEN] = {0};
             char dst_string[RAT_MAX_FULL_PATH_LEN] = {0};
             char* pstr;
             movie_spsc_ctrl_uipara_t* ui_para;
             GUI_RECT gui_rect;

             //return EPDK_OK;//长字符串滚动已经画了。

             index = robin_npl_get_cur();
             if (-1 == index)
             {
                 __msg("robin_npl_get_cur fail...");
                 return EPDK_FAIL;
             }

             ret = robin_npl_index2file(index, src_string);
             if (-1 == ret)
             {
                 __msg("robin_npl_index2file fail, index=%d...", index);
                 return EPDK_FAIL;
             }

             pstr = eLIBs_strchrlast(src_string, '\\');
             if(!pstr)
             {
                 __msg("eLIBs_strchrlast fail...");
                 return EPDK_FAIL;
             }
             pstr++;

             ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());
             if(!ui_para)
             {
                 __err("invalid para...");
                 return EPDK_FAIL;
             }

             gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].x;
             gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].y;
             gui_rect.x1 = gui_rect.x0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].w-1;
             gui_rect.y1 = gui_rect.y0+ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].h-1;
             GUI_SetFont(SWFFont);
             GUI_SetColor(APP_COLOR_WHITE);
             GUI_SetBkColor(0xF0);
             GUI_UC_SetEncodeUTF8();
             GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
             //GUI_ClearRectEx(&gui_rect);
             GetClippedString(&gui_rect, pstr, dst_string, "  ");
             APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
                     gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
                     gui_rect.x1, gui_rect.y1);
             GUI_DispStringInRect(dst_string, &gui_rect, GUI_TA_LEFT| GUI_TA_BOTTOM);
             //__here__;
             return EPDK_OK;
         }*/
        default :
            break;
    }

    return EPDK_FAIL;
}

static __s32 __movie_spsc_ctrl_static_icon_draw_all(movie_spsc_ctrl_scene_t *scene_para)
{
    __s32 ret;

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
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
        return EPDK_FAIL;
    }

    ret = EPDK_OK;
#if 0
    {
        __s32 i;
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

        for (i = 0 ; i < movie_spsc_ctrl_static_icon_num ; i++)
        {
            if (EPDK_FAIL == __movie_spsc_ctrl_static_icon_draw(scene_para, i))
            {
                __msg("__movie_spsc_ctrl_static_icon_draw fail, i= %d", i);
                ret = EPDK_FAIL;
            }
        }
    }
#endif
    return ret;
}

static __s32 __movie_spsc_ctrl_icon_draw_all(movie_spsc_ctrl_scene_t *scene_para)
{
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
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
        __msg("GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
        return EPDK_FAIL;
    }

    {
        int i;

        for (i = 0; i < movie_spsc_ctrl_icon_num ; i++)
        {
            /*if (ui_para->focus_icon_index == i)
            {
                __movie_spsc_ctrl_icon_draw_item(scene_para, i, 1);
            }
            else
            {
                __movie_spsc_ctrl_icon_draw_item(scene_para, i, 0);
            }*/
            __msg("+++++++++++++++++++++++++++++++");
            __movie_spsc_ctrl_icon_draw_item(scene_para, i, 0);
        }
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_init(movie_spsc_ctrl_scene_t *scene_para)
{
    if (scene_para->long_str_handle == NULL)
    {
        __show_info_move_t show_info;
        movie_spsc_ctrl_uipara_t *ui_para;
        GUI_RECT gui_rect;
        ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

        if (!ui_para)
        {
            __err("invalid para...");
            return EPDK_FAIL;
        }

        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = scene_para->hlyr;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = SWFFont;
        show_info.string = "";
        show_info.bkColor = 0xF0;
        //show_info.fontColor = APP_COLOR_WHITE;
        show_info.fontColor = GUI_WHITE;
        show_info.encode_id = EPDK_CHARSET_ENM_UTF8;
        show_info.align = GUI_TA_HCENTER | GUI_TA_VCENTER;
        show_info.bmp = NULL;
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;
        scene_para->long_str_handle = GUI_LongStringCreate(&show_info);

        if (scene_para->long_str_handle == NULL)
        {
            __wrn("create long string failed!");
        }
    }
    else
    {
        __msg("long string scroll has already started!");
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_uninit(movie_spsc_ctrl_scene_t *scene_para)
{
    if (scene_para->long_str_handle != NULL)
    {
        GUI_LongStringDelete(scene_para->long_str_handle);
        scene_para->long_str_handle = NULL;
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_start_roll(movie_spsc_ctrl_scene_t *scene_para)
{
    __show_info_move_t show_info;
    __s32 ret;
    __s32 index;
    char *pstr;
    movie_spsc_ctrl_uipara_t *ui_para;
    GUI_RECT gui_rect;

    if (scene_para->long_str_handle != NULL)
    {
        GUI_LongStringStop(scene_para->long_str_handle);
        eLIBs_memset(scene_para->long_string, 0, sizeof(scene_para->long_string));
        index = robin_npl_get_cur();

        if (-1 == index)
        {
            __msg("robin_npl_get_cur fail...");
            return EPDK_FAIL;
        }

        ret = robin_npl_index2file(index, scene_para->long_string);

        if (-1 == ret)
        {
            __msg("robin_npl_index2file fail, index=%d...", index);
            return EPDK_FAIL;
        }

        pstr = eLIBs_strchrlast(scene_para->long_string, '\\');

        if (!pstr)
        {
            __msg("eLIBs_strchrlast fail...");
            return EPDK_FAIL;
        }

        pstr++;
        ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

        if (!ui_para)
        {
            __err("invalid para...");
            return EPDK_FAIL;
        }

        gui_rect.x0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].x;
        gui_rect.y0 = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].y;
        gui_rect.x1 = gui_rect.x0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].w;
        gui_rect.y1 = gui_rect.y0 + ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].h;
        GUI_SetFont(SWFFont);
        //GUI_SetColor(APP_COLOR_WHITE);
        GUI_SetColor(GUI_WHITE);
        GUI_SetBkColor(0xF0);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        //__here__;
        //GUI_ClearRectEx(&gui_rect);
        // APP_BMP_DrawEx(scene_para->hlyr, dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]),
        //            gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0,
        //             gui_rect.x1, gui_rect.y1);
        //__here__;
        GUI_LyrWinSel(scene_para->hlyr);
        __msg("pstr=%s", pstr);
        __movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_filename);
        //__here__;
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = scene_para->hlyr;
        show_info.region.x = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].x;
        show_info.region.y = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].y;
        show_info.region.width = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].w;
        show_info.region.height = ui_para->uipara_static_icon[movie_spsc_ctrl_static_icon_filename].h;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = SWFFont;
        show_info.string = pstr;
        show_info.bkColor = 0xF0;
        //show_info.fontColor = APP_COLOR_WHITE;
        show_info.fontColor = GUI_WHITE;
        show_info.encode_id = EPDK_CHARSET_ENM_UTF8;
        show_info.align = GUI_TA_LEFT | GUI_TA_BOTTOM;
        show_info.bmp = NULL;
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;
        __msg("show_info.region.x=%d", show_info.region.x);
        __msg("show_info.region.y=%d", show_info.region.y);
        __msg("show_info.region.width=%d", show_info.region.width);
        __msg("show_info.region.height=%d", show_info.region.height);
        __msg("show_info.pFont=%x", show_info.pFont);
        __msg("show_info.string=%s", show_info.string);
        GUI_LongStringReset(scene_para->long_str_handle, &show_info);
        //__here__;
        GUI_LongStringStart(scene_para->long_str_handle);
        //__here__;
    }

    return EPDK_OK;
}

static __s32 __movie_spsc_ctrl_long_string_stop_roll(movie_spsc_ctrl_scene_t *scene_para)
{
    if (scene_para->long_str_handle != NULL)
    {
        return GUI_LongStringStop(scene_para->long_str_handle);
    }

    return EPDK_FAIL;
}

static __s32 __movie_spsc_ctrl_notify_parent(movie_spsc_ctrl_scene_t *scene_para, __gui_msg_t *msg, movie_spsc_ctrl_icon_t icon_id,
        __s32 dwAddData2)
{
    switch (icon_id)
    {
#if 0

        case movie_spsc_ctrl_icon_prev:
        {
            movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_prev, dwAddData2, 0);
            return EPDK_OK;
        }

        case movie_spsc_ctrl_icon_next:
        {
            movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_next, dwAddData2, 0);
            return EPDK_OK;
        }

        case movie_spsc_ctrl_icon_playpause:
        {
            if (0 == dwAddData2) //如果是短按消息
            {
                __cedar_status_t fsm_sta;
                fsm_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == fsm_sta)
                {
                    scene_para->playsta = 0;
                    __movie_spsc_ctrl_play_set_state(scene_para, scene_para->playsta);
                    movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_playpause, 0, 0);
                }
                else if (CEDAR_STAT_PAUSE == fsm_sta
                         || CEDAR_STAT_FF == fsm_sta
                         || CEDAR_STAT_RR == fsm_sta)
                {
                    scene_para->playsta = 1;
                    __movie_spsc_ctrl_play_set_state(scene_para, scene_para->playsta);
                    movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_playpause, 1, 0);
                }
                else
                {
                    __msg("cedar not in stop/jump status, not need process...");
                }
            }

            return EPDK_OK;
        }

#endif

        case movie_spsc_ctrl_icon_infor:
        {
            DE_INFO;
            __msg("++++++movie_spsc_ctrl_icon_infor++++++++");
            movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_info, 0, 0);
            return EPDK_OK;
        }

        case movie_spsc_ctrl_icon_tvout:
        {
            __msg("++++++movie_spsc_ctrl_icon_tvout++++++++");
            movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_tvout, 1, 0);
            return EPDK_OK;
        }

        default:
            break;
    }

    __msg("unknown message...");
    return EPDK_FAIL;
}

static __s32 __movie_spsc_ctrl_find_touch_icon_index(__s32 x, __s32 y,
        movie_spsc_ctrl_rect_t *icon_tbl)
{
    __s32 i;
    __s32 ret;

    for (i = 0 ; i < movie_spsc_ctrl_icon_num ; i++)
    {
        ret = __movie_point_in_rect(x, y, (RECT *) & (icon_tbl[i]),
                                    2, 5, 2, 5);

        if (EPDK_TRUE == ret)
        {
            return i;
        }
    }

    return -1;
}

static __s32 __movie_spsc_ctrl_touch_in_slidebar(__s32 x, __s32 y)
{
    __s32 i;
    __s32 ret;
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FALSE;
    }

    ret = __movie_point_in_rect(x, y, (RECT *)&ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_bg],
                                2, 10, 2, 10);

    if (EPDK_TRUE == ret)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

static __s32 __movie_spsc_ctrl_draw_progress_by_touch(movie_spsc_ctrl_scene_t *scene_para,
        __s32 x, __s32 y)
{
    __s32 i;
    __s32 ret;
    __s32 min, max, cur;
    movie_spsc_ctrl_rect_t *prect;
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return EPDK_FALSE;
    }

    prect = &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_bg];

    if (!prect)
    {
        return EPDK_FAIL;
    }

    min = 0;
    max = prect->w - 1;
    cur = x - prect->x;

    if (cur < min)
    {
        cur = min;
    }

    if (cur >= max)
    {
        cur = max;
    }

    GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_SLEEP);
    /*
     *ret = com_memdev_create(scene_para->hlyr);
     *if(EPDK_FAIL == ret)
     *{
     *    __msg("com_memdev_create fail...");
     *    return EPDK_FAIL;
     *}
     */
    ret = __movie_spsc_prog_draw_progress(&ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_bg],
                                          &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_left],
                                          &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_mid],
                                          &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_cursor],
                                          min,
                                          max,
                                          cur);
    //com_memdev_delete();
    GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
    return ret;
}

static float __movie_spsc_ctrl_calc_ratio(movie_spsc_ctrl_scene_t *scene_para,
        __s32 x, __s32 y)
{
    __s32 i;
    __s32 ret;
    __s32 min, max, cur;
    movie_spsc_ctrl_rect_t *prect;
    movie_spsc_ctrl_uipara_t *ui_para;
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_spsc_get_uipara fail...");
        return 0;
    }

    prect = &ui_para->uipara_spsc_prog[movie_spsc_prog_icon_prog_bg];

    if (!prect)
    {
        return 0;
    }

    min = 0;
    max = prect->w - 1;
    cur = x - prect->x;

    if (cur < min)
    {
        cur = min;
    }

    if (cur >= max)
    {
        cur = max;
    }

    return (float)cur / max;
}

static __s32 __movie_spsc_ctrl_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            movie_spsc_ctrl_scene_t *scene_para;
            //GUI_MSG_PAINT
            __msg("__movie_spsc_ctrl_proc GUI_MSG_CREATE begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
            __movie_spsc_ctrl_init_ui();
            __spsc_ctrl_install_prog_timmer(scene_para);
            __movie_spsc_ctrl_install_hide_timmer(scene_para);
            __msg("__movie_spsc_ctrl_proc GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            movie_spsc_ctrl_scene_t *scene_para;
            __msg("__movie_spsc_ctrl_proc GUI_MSG_DESTROY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            __spsc_ctrl_uninstall_prog_timmer(scene_para);
            __movie_spsc_ctrl_uninstall_hide_timmer(scene_para);
            //__movie_spsc_ctrl_long_string_stop_roll(scene_para);
            //__movie_spsc_ctrl_long_string_uninit(scene_para);
            __movie_spsc_ctrl_deinit_ui();
            __msg("__movie_spsc_ctrl_proc GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            static __s32 last_key = -1;
            movie_spsc_ctrl_scene_t *scene_para;
            movie_spsc_ctrl_uipara_t *ui_para;
            __msg("__movie_spsc_ctrl_proc GUI_MSG_KEY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

            if (NULL == ui_para)
            {
                __msg("movie_spsc_get_uipara fail...");
                return EPDK_FAIL;
            }

            __movie_spsc_ctrl_reset_hide_timmer(scene_para);

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                if (GUI_MSG_KEY_ENTER == last_key)
                {
                    __msg("GUI_MSG_KEY_ENTER");

                    if (ui_para->focus_icon_index  == movie_spsc_ctrl_icon_infor)
                    {
                        //__here__;
                        __msg("++++++movie_spsc_ctrl_icon_infor++++++++");
                        movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_info, 0, 0);
                        goto end;
                    }
                    else if (ui_para->focus_icon_index  == movie_spsc_ctrl_icon_tvout)
                    {
                        //__here__;
                        __msg("++++++spsc_ctrl_scene_msg_tvout++++++++");
                        movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_tvout, 1, 0);
                        goto end;
                    }
                }
            }
            else
            {
                switch (msg->dwAddData1)
                {
                    case GUI_MSG_KEY_LEFT:
                    case GUI_MSG_KEY_LONGLEFT:
                    case GUI_MSG_KEY_UP://复用
                    {
                        __msg("GUI_MSG_KEY_LEFT");

                        /*if(ui_para->focus_icon_index < 0)
                        {
                            ui_para->focus_icon_index = movie_spsc_ctrl_icon_num-1;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                        }

                        else*/ if (ui_para->focus_icon_index == 0)
                        {
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
                            ui_para->focus_icon_index = movie_spsc_ctrl_icon_num - 1;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                        }
                        else
                        {
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
                            ui_para->focus_icon_index--;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                        }
                    }
                    break;

                    case GUI_MSG_KEY_RIGHT:
                    case GUI_MSG_KEY_LONGRIGHT:
                    case GUI_MSG_KEY_DOWN://复用
                    {
                        __msg("GUI_MSG_KEY_RIGHT");

                        //__msg("ui_para->focus_icon_index= %d", ui_para->focus_icon_index);
                        //__msg("ui_para->uipara_icon[0].x= %d", ui_para->uipara_icon[0].x);
                        //__msg("ui_para->uipara_icon[0].y= %d", ui_para->uipara_icon[0].y);
                        //__msg("ui_para->uipara_icon[1].x= %d", ui_para->uipara_icon[1].x);
                        //__msg("ui_para->uipara_icon[1].y= %d", ui_para->uipara_icon[1].y);
                        /*if(ui_para->focus_icon_index < 0)
                        {
                            //__here__;
                            ui_para->focus_icon_index++;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                            __msg("ui_para->focus_icon_index= %d", ui_para->focus_icon_index);
                        }
                        else */
                        if (ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num - 1)
                        {
                            //__here__;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
                            ui_para->focus_icon_index = 0;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                            __msg("ui_para->focus_icon_index= %d", ui_para->focus_icon_index);
                        }
                        else
                        {
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
                            ui_para->focus_icon_index++;
                            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                            __msg("ui_para->focus_icon_index= %d", ui_para->focus_icon_index);
                        }
                    }
                    break;

                    case GUI_MSG_KEY_MENU:
                    {
                        __msg("GUI_MSG_KEY_MENU");
                        movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_switch, 0, 0);
                    }
                    break;
                }
            }

end:
            ;

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                last_key = -1;
            }
            else
            {
                last_key = msg->dwAddData1;
            }

            //按键只处理暂停播放
            /*if (KEY_UP_ACTION == msg->dwAddData2)
            {
                if (0)//(GUI_MSG_KEY_LEFT == last_key)
                {
                    __msg("draw unfocus icon..");
                    GUI_LyrWinSel(scene_para->hlyr);
                    com_memdev_create(scene_para->hlyr);
                    if(ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                    {
                   //     ui_para->focus_icon_index = movie_spsc_ctrl_icon_playpause;
                    }
                    __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);

                    ui_para->focus_icon_index--;
                    if (ui_para->focus_icon_index < 0)
                    {
                        ui_para->focus_icon_index = movie_spsc_ctrl_icon_num-1;
                    }
                    __msg("draw focus icon..");
                    __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                    com_memdev_delete();
                }
                else if (0)//(GUI_MSG_KEY_RIGHT == last_key)
                {
                    __msg("draw unfocus icon..");
                    GUI_LyrWinSel(scene_para->hlyr);
                    com_memdev_create(scene_para->hlyr);

                    if(ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                    {
                    //    ui_para->focus_icon_index = movie_spsc_ctrl_icon_playpause;
                    }
                    __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);

                    ui_para->focus_icon_index++;
                    if (ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                    {
                    //  ui_para->focus_icon_index = movie_spsc_ctrl_icon_playpause;
                    }
                    __msg("draw focus icon..");
                    __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                    com_memdev_delete();
                }
                else if (GUI_MSG_KEY_ENTER == last_key)
                {
                    if(ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                    {
                      //  ui_para->focus_icon_index = movie_spsc_ctrl_icon_playpause;
                    }

                    GUI_LyrWinSel(scene_para->hlyr);

                    __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);

                    __movie_spsc_ctrl_notify_parent(scene_para, msg, ui_para->focus_icon_index, 0);
                }
            }
            else
            {

                switch(msg->dwAddData1)
                {
                    case GUI_MSG_KEY_LONGLEFT:
                    {
                        __msg("draw unfocus icon..");
                        GUI_LyrWinSel(scene_para->hlyr);
                        com_memdev_create(scene_para->hlyr);

                        if(ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                        {
                            ui_para->focus_icon_index = 0;
                        }
                        __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);

                        ui_para->focus_icon_index--;
                        if (ui_para->focus_icon_index < 0)
                        {
                            ui_para->focus_icon_index = movie_spsc_ctrl_icon_num-1;
                        }
                        __msg("draw focus icon..");
                        __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                        com_memdev_delete();

                        break;
                    }
                    case GUI_MSG_KEY_LONGRIGHT:
                    {
                        __msg("draw unfocus icon..");
                        GUI_LyrWinSel(scene_para->hlyr);
                        com_memdev_create(scene_para->hlyr);

                        if(ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                        {
                            ui_para->focus_icon_index = 0;
                        }
                        __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);

                        ui_para->focus_icon_index++;
                        if (ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                        {
                            ui_para->focus_icon_index = 0;
                        }
                        __msg("draw focus icon..");
                        __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
                        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                        com_memdev_delete();

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
            */
            //__msg("__movie_spsc_ctrl_proc GUI_MSG_KEY end");
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            static __s32 last_touch_icon_index = -1;//上次down按下的图标索引
            static __s32 slidebar_touch_down = 0; //进度条是否被点击
            static __s32 touch_long_down = 0;//long down是否到达
            static __s32 touch_long_down_cnt = 0;//long down计数
            __s32 x, y;
            __s32 ret;
            movie_spsc_ctrl_uipara_t *ui_para;
            movie_spsc_ctrl_scene_t *sence_para;
            //          __msg("__movie_spsc_ctrl_proc GUI_MSG_TOUCH begin");
            sence_para = GUI_WinGetAttr(msg->h_deswin);

            if (!sence_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            x = LOSWORD(msg->dwAddData2);
            y = HISWORD(msg->dwAddData2);
            ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

            if (NULL == ui_para)
            {
                __msg("movie_spsc_get_uipara fail...");
                return EPDK_FAIL;
            }

            __movie_spsc_ctrl_reset_hide_timmer(sence_para);

            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_DOWN:
                {
                    __s32 touch_down_index;
                    GUI_WinSetCaptureWin(msg->h_deswin);
                    touch_down_index = __movie_spsc_ctrl_find_touch_icon_index(x, y,
                                       ui_para->uipara_icon);
                    __msg("touch_down_index = %d", touch_down_index);

                    if (-1 != touch_down_index)
                    {
                        //draw old focus icon
                        if (ui_para->focus_icon_index < 0 || ui_para->focus_icon_index >= movie_spsc_ctrl_icon_num)
                        {
                            ui_para->focus_icon_index = 0;
                        }

                        GUI_LyrWinSel(sence_para->hlyr);
                        __movie_spsc_ctrl_icon_draw_item(sence_para, ui_para->focus_icon_index, 0);
                        //draw new focus icon
                        last_touch_icon_index = touch_down_index;
                        ui_para->focus_icon_index = touch_down_index;
                        __movie_spsc_ctrl_icon_draw_item(sence_para, ui_para->focus_icon_index, 1);
                    }
                    else
                    {
                        __s32 is_slide_touch_down;
                        is_slide_touch_down = __movie_spsc_ctrl_touch_in_slidebar(x, y);
                        slidebar_touch_down = is_slide_touch_down;

                        if (EPDK_TRUE == is_slide_touch_down)
                        {
                            __movie_spsc_ctrl_draw_progress_by_touch(sence_para, x, y);
                        }
                    }

                    break;
                }

                case GUI_MSG_TOUCH_MOVE:
                case GUI_MSG_TOUCH_OVERMOVE:
                case GUI_MSG_TOUCH_NCMOVE:
                {
                    if (1 == slidebar_touch_down)
                    {
                        __movie_spsc_ctrl_draw_progress_by_touch(sence_para, x, y);
                    }

                    break;
                }

                case GUI_MSG_TOUCH_LONGDOWN:
                case GUI_MSG_TOUCH_OVERLONGDOWN:
                {
                    touch_long_down_cnt++;

                    if (touch_long_down_cnt > 10) //5次longdown才算longdown
                    {
                        touch_long_down = 1;
                    }
                    else
                    {
                        break;
                    }

                    if (-1 != last_touch_icon_index)
                    {
                        __s32 touch_down_index;
                        touch_down_index = __movie_spsc_ctrl_find_touch_icon_index(x, y,
                                           ui_para->uipara_icon);

                        if (-1 != touch_down_index)
                        {
                            if (touch_down_index == last_touch_icon_index)
                            {
                                __movie_spsc_ctrl_notify_parent(sence_para, msg, touch_down_index, 1);//长按消息
                            }
                            else
                            {
                            }
                        }
                    }

                    break;
                }

                case GUI_MSG_TOUCH_UP:
                case GUI_MSG_TOUCH_OVERUP:
                case GUI_MSG_TOUCH_NCUP:
                {
                    if (GUI_WinGetCaptureWin() == msg->h_deswin)
                    {
                        GUI_WinReleaseCapture();
                    }

                    if (-1 != last_touch_icon_index)
                    {
                        __s32 touch_down_index;
                        //draw old focus
                        GUI_LyrWinSel(sence_para->hlyr);
                        __movie_spsc_ctrl_icon_draw_item(sence_para, last_touch_icon_index, 0);
                        touch_down_index = __movie_spsc_ctrl_find_touch_icon_index(x, y,
                                           ui_para->uipara_icon);
                        __msg("touch_down_index = %d", touch_down_index);

                        if (-1 != touch_down_index)
                        {
                            if (touch_down_index == last_touch_icon_index
                                && 0 == touch_long_down)
                            {
                                __movie_spsc_ctrl_notify_parent(sence_para, msg, touch_down_index, 0);//点击消息
                            }
                            else
                            {
                            }
                        }

                        if (1 == touch_long_down) //长按抬起后恢复正常播放
                        {
                            __cedar_status_t cedar_sta;
                            cedar_sta = robin_get_fsm_status();

                            if (CEDAR_STAT_FF == cedar_sta
                                || CEDAR_STAT_RR == cedar_sta)
                            {
                                robin_set_cmd_play();
                                GUI_LyrWinSel(sence_para->hlyr);
                                //__movie_spsc_ctrl_icon_draw_item(sence_para,movie_spsc_ctrl_icon_prev , 0);
                                //__movie_spsc_ctrl_icon_draw_item(sence_para,movie_spsc_ctrl_icon_next , 0);
                            }
                        }
                    }
                    else
                    {
                        if (1 == slidebar_touch_down)
                        {
                            __cedar_status_t  cedar_sta;
                            cedar_sta = robin_get_fsm_status();

                            if (CEDAR_STAT_PLAY == cedar_sta)
                            {
                                __s32 total_time;
                                float ratio;
                                total_time = robin_get_total_time();
                                ratio = __movie_spsc_ctrl_calc_ratio(sence_para, x, y);
                                __msg("total_time=%d", total_time);
                                __msg("ratio=%d", ratio);
                                robin_set_cmd_jump((__s32)(ratio * total_time));
                            }
                            else//如果处于非播放状态滑动了进度条，则把重新恢复到原来的位置
                            {
                                __movie_spsc_prog_update_prog_ui(sence_para, MOVIE_SPSC_PROG_UI_TYPE_PROG);
                            }
                        }
                    }

                    last_touch_icon_index = -1;
                    slidebar_touch_down = 0;
                    touch_long_down = 0;
                    touch_long_down_cnt = 0;
                    break;
                }
            }

            //          __msg("__movie_spsc_ctrl_proc GUI_MSG_TOUCH end");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __s32 ret;
            movie_spsc_ctrl_scene_t *scene_para;
            movie_spsc_ctrl_uipara_t *ui_para;
            ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());
            //          __msg("__spsc_ctrl_proc GUI_MSG_TIMER begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);
            ret = -1;

            if (scene_para)
            {
                if (LOWORD(msg->dwAddData1) == scene_para->time_timmer_id)
                {
                    //__msg("before __spsc_ctrl_update_spsc_ctrl_ui");
                    __movie_spsc_prog_update_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_CUR_TIME);
                    //__msg("after __spsc_ctrl_update_spsc_ctrl_ui");
                    ret = EPDK_OK;
                }
                else if (LOWORD(msg->dwAddData1) == scene_para->prog_timmer_id)
                {
                    //__msg("before __spsc_ctrl_update_spsc_ctrl_ui");
                    __movie_spsc_prog_update_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_PROG);
                    //__msg("after __spsc_ctrl_update_spsc_ctrl_ui");
                    ret = EPDK_OK;
                }
                else if (LOWORD(msg->dwAddData1) == scene_para->hide_timmer_id)
                {
                    //__msg("before movie_cmd2parent");
                    movie_cmd2parent(msg->h_deswin, spsc_ctrl_scene_msg_timeout, 0, 0);
                    //__msg("after movie_cmd2parent");
                    ret = EPDK_OK;
                }
            }
            else
            {
                __err("scene_para is null...");
            }

            //__msg("__spsc_ctrl_proc GUI_MSG_TIMER end");

            if (-1 == ret)//未处理，交给子场景处理
            {
                //__here__;
                break;
            }
            else//已处理
            {
                ////__here__;
                return EPDK_OK;
            }
        }

        case GUI_MSG_SET_FOCUS:
        {
            __msg("__movie_spsc_ctrl_proc GUI_MSG_SET_FOCUS");
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
        {
            __msg("__movie_spsc_ctrl_proc GUI_MSG_CLOSE begin");
            __msg("__movie_spsc_ctrl_proc GUI_MSG_CLOSE end");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            movie_spsc_ctrl_scene_t *scene_para;
            movie_spsc_ctrl_uipara_t *ui_para;
            ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());
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

            GUI_LyrWinSel(scene_para->hlyr);
            // com_memdev_create(scene_para->hlyr);
            GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_SLEEP);
            __movie_spsc_ctrl_bg_draw(scene_para);
            __msg("after __movie_spsc_ctrl_bg_draw");
            //__msg("after __movie_spsc_ctrl_icon_draw_all");
#if 0

            if (ui_para->focus_icon_index < 0)
            {
                //__here__;
                ui_para->focus_icon_index++;
                //__here__;
                __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
                //__here__;
                ui_para->focus_icon_index++;
                //__here__;
                __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 0);
                //__here__;
                ui_para->focus_icon_index = 0;
                //__here__;
            }

#endif
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            //com_memdev_delete();
            GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
            __movie_spsc_prog_update_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_ALL);
            __movie_spsc_ctrl_icon_draw_all(scene_para);
            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
            __msg("after __movie_spsc_prog_update_prog_ui");
#if 0
            {
                //更新播放按钮
                __s32 sta;
                __cedar_status_t cedar_sta;
                cedar_sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == cedar_sta)
                {
                    __movie_spsc_ctrl_play_set_state(scene_para, 1);
                }
                else
                {
                    __movie_spsc_ctrl_play_set_state(scene_para, 0);
                }
            }
#endif
            {
                __gui_msg_t notify_msg;
                eLIBs_memset(&notify_msg, 0, sizeof(__gui_msg_t));
                notify_msg.h_deswin = msg->h_deswin;
                notify_msg.id = movie_spsc_ctrl_scene_draw_difuse;
                GUI_SendNotifyMessage(&notify_msg);
            }
            return EPDK_OK;
        }

        /*case movie_spsc_ctrl_scene_update_fileinfo:
        {
            movie_spsc_ctrl_scene_t *scene_para;

            scene_para = GUI_WinGetAttr(msg->h_deswin);
            if (!scene_para || !scene_para->hlyr)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            if (GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
            {
                __msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
                return EPDK_OK;
            }

            __movie_spsc_ctrl_long_string_start_roll(scene_para);
            __msg("after __movie_spsc_ctrl_long_string_start_roll");

            GUI_LyrWinSel(scene_para->hlyr);
            com_memdev_create(scene_para->hlyr);

            __movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_filename);
            //__here__;
            __movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_file_info);
            //__here__;
            __movie_spsc_ctrl_static_icon_draw(scene_para, movie_spsc_ctrl_static_icon_video_info);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            com_memdev_delete();

            return EPDK_OK;
        }*/
        case movie_spsc_ctrl_scene_draw_difuse:
        {
            movie_spsc_ctrl_scene_t *scene_para;
            movie_spsc_ctrl_uipara_t *ui_para;
            ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para || !scene_para->hlyr)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            if (GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr))
            {
                __msg("GUI_LYRWIN_STA_ON  != GUI_LyrWinGetSta(scene_para->hlyr), do not paint...");
                return EPDK_OK;
            }

            GUI_LyrWinSel(scene_para->hlyr);
            // com_memdev_create(scene_para->hlyr);
            //__movie_spsc_ctrl_static_icon_draw_all(scene_para);
            //__msg("after __movie_spsc_ctrl_static_icon_draw_all");
            //GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            //com_memdev_delete();
            // __movie_spsc_ctrl_long_string_init(scene_para);
            __movie_spsc_prog_update_prog_ui(scene_para, MOVIE_SPSC_PROG_UI_TYPE_ALL);
            __movie_spsc_ctrl_icon_draw_all(scene_para);
            __movie_spsc_ctrl_icon_draw_item(scene_para, ui_para->focus_icon_index, 1);
            // __movie_spsc_ctrl_long_string_start_roll(scene_para);
            __msg("after __movie_spsc_ctrl_long_string_start_roll");
            return EPDK_OK;
        }

        default:
        {
            break;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_spsc_ctrl_scene_create(movie_spsc_ctrl_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_spsc_ctrl_scene_t *sence_para;
    movie_spsc_ctrl_uipara_t *ui_para;
    sence_para = (movie_spsc_ctrl_scene_t *)esMEMS_Malloc(0, sizeof(movie_spsc_ctrl_scene_t));

    if (!sence_para)
    {
        __msg("mem not enough...");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_spsc_ctrl_scene_t));
    //设置输入参数
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->hlyr = create_para->hlyr;
    sence_para->playsta = 1;

    if (NULL == sence_para->hlyr)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("sence_para->hlyr is null...");
        return NULL;
    }

    //获得UI坐标
    ui_para = movie_spsc_ctrl_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("movie_spsc_get_uipara fail...");
        return NULL;
    }

    __msg("ui_para->uipara_frm.x = %d", ui_para->uipara_frm.x);
    __msg("ui_para->uipara_frm.y = %d", ui_para->uipara_frm.y);
    __msg("ui_para->uipara_frm.w = %d", ui_para->uipara_frm.w);
    __msg("ui_para->uipara_frm.h = %d", ui_para->uipara_frm.h);
    ui_para->focus_icon_index = movie_spsc_ctrl_icon_infor;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          =   "movie_spsc_win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = sence_para->scene_id;
    framewin_para.hHosting      =  sence_para->hparent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__movie_spsc_ctrl_proc);
    framewin_para.rect.x        = ui_para->uipara_frm.x;
    framewin_para.rect.y        = ui_para->uipara_frm.y;
    framewin_para.rect.width    = ui_para->uipara_frm.w;
    framewin_para.rect.height   = ui_para->uipara_frm.h;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr = (void *)sence_para;
    framewin_para.hLayer = sence_para->hlyr;
    sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);

    if (NULL == sence_para->hfrm)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("GUI_FrmWinCreate fail...");
        return NULL;
    }

    return sence_para;
}

__s32 movie_spsc_ctrl_scene_set_focus(void *handle)
{
    movie_spsc_ctrl_scene_t *scene_para;
    scene_para = (movie_spsc_ctrl_scene_t *)handle;

    //__here__;
    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    //__here__;
    if (!scene_para->hfrm)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    //__here__;
    GUI_WinSetFocusChild(scene_para->hfrm);
    //__here__;
    return EPDK_OK;
}

__s32 movie_spsc_ctrl_scene_delete(void *handle)
{
    movie_spsc_ctrl_scene_t *scene_para;
    scene_para = (movie_spsc_ctrl_scene_t *)handle;
    __u32 state;

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
    eLIBs_memset(scene_para, 0, sizeof(movie_spsc_ctrl_scene_t));
    esMEMS_Mfree(0, scene_para);
	
	gscene_hbar_get_state(&state);
	if (state == HBAR_ST_SHOW)
	{
		gscene_hbar_set_state(HBAR_ST_HIDE);
	}

    return EPDK_OK;
}

H_WIN movie_spsc_ctrl_scene_get_hwnd(void *handle)
{
    movie_spsc_ctrl_scene_t *scene_para;
    scene_para = (movie_spsc_ctrl_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return NULL;
    }

    return scene_para->hfrm;
}
