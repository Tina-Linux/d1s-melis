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
#include <log.h>
#include "beetles_app.h"
#include "common/common.h"
#include "prog.h"

//定义prog子场景的ui类型
#define MOVIE_PROG_UI_TYPE_PROG         0x01
#define MOVIE_PROG_UI_TYPE_CUR_TIME     0x02
#define MOVIE_PROG_UI_TYPE_TOTAL_TIME   0x04
#define MOVIE_PROG_UI_TYPE_BG           0x08


//prog的所有ui类型
#define MOVIE_PROG_UI_TYPE_ALL  (MOVIE_PROG_UI_TYPE_PROG\
                                 |MOVIE_PROG_UI_TYPE_CUR_TIME\
                                 |MOVIE_PROG_UI_TYPE_TOTAL_TIME\
                                 |MOVIE_PROG_UI_TYPE_BG)

//除背景外的其他ui类型
#define MOVIE_PROG_UI_TYPE_OTHER    (MOVIE_PROG_UI_TYPE_ALL&(~MOVIE_PROG_UI_TYPE_BG))

typedef union
{
    struct
    {
        __s32    hour;
        __s32    minute;
        __s32    second;
    } data1;

    __s32 data2[3];
} prog_cuckoo_time_t;

static void __time2time(__u32 ms, prog_cuckoo_time_t *format_time)
{
    __u32  total_minute;
    __u32 second_time;
    second_time = ms / 1000;
    total_minute = second_time / 60;
    format_time->data1.second = second_time % 60;
    format_time->data1.minute = total_minute % 60;
    format_time->data1.hour   = total_minute / 60;
}

typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;


    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
    __s32 cur_time_index;//当画当前时间时亮选的位置（时、分、秒）
    prog_cuckoo_time_t cur_time;//输入的设置时间
    __u8 prog_timmer_id;
    __u8 prog_hide_timmer_id;
} movie_prog_scene_t;

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR __prog_32bpp_layer_create(RECT *rect, __s32 pipe)
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
    __log("__prog_32bpp_layer_create");
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}

static __s32 __prog_init_ui(void)
{
    movie_prog_uipara_t *ui_para;
    ui_para = movie_prog_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_prog_get_uipara fail...");
        return EPDK_FAIL;
    }

    ui_para->uipara_bg.res_hdl[0] = dsk_theme_open(ui_para->uipara_bg.res_id[0]);
    {
        int i;
        int j;

        for (i = 0 ; i < movie_prog_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_PROG_MAX_ICON_RES_NUM ; j++)
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

static __s32 __prog_deinit_ui(void)
{
    movie_prog_uipara_t *ui_para;
    ui_para = movie_prog_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        __msg("movie_prog_get_uipara fail...");
        return EPDK_FAIL;
    }

    dsk_theme_close(ui_para->uipara_bg.res_hdl[0]);
    ui_para->uipara_bg.res_hdl[0] = NULL;
    {
        int i;
        int j;

        for (i = 0 ; i < movie_prog_icon_num ; i++)
        {
            for (j = 0 ; j < MOVIE_PROG_MAX_ICON_RES_NUM ; j++)
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

static __s32 __prog_init_internal_para(movie_prog_scene_t *scene_para)
{
    if (NULL == scene_para)
    {
        __msg("invalid para...");
        return EPDK_FAIL;
    }

    scene_para->cur_time_index = -1;
    scene_para->prog_timmer_id = 0x18;
    scene_para->prog_hide_timmer_id = 0x19;
    eLIBs_memset(&scene_para->cur_time, 0, sizeof(prog_cuckoo_time_t));
    return EPDK_OK;
}

static __s32 __prog_draw_progress(movie_prog_rect_t *prog_bg, movie_prog_rect_t *prog_left,
                                  movie_prog_rect_t *prog_mid, movie_prog_rect_t *prog_cursor,
                                  __s32 min, __s32 max, __s32 cur)
{
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
            __msg("prog_mid->.res_hdl[0] is null...");
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
                //  n = ((prog_bg->w - prog_cursor->w) * (cur - min))/(prog_mid->w * (max - min));//会造成溢出，反转为负值
                n = ((prog_bg->w - prog_cursor->w) / prog_mid->w) * (cur - min) / (max - min);
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

static __s32 __prog_update_prog_ui(movie_prog_scene_t *scene_para
                                   , __u32 ui_type)
{
    movie_prog_uipara_t *ui_para;
    ui_para = movie_prog_get_uipara(GUI_GetScnDir());

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
    if (ui_type & MOVIE_PROG_UI_TYPE_BG)
    {
        void *pbmp;

        if (!ui_para->uipara_bg.res_hdl[0])
        {
            __msg("prog_bg->res_hdl[0] is null...");
            return EPDK_FAIL;
        }

        pbmp = dsk_theme_hdl2buf(ui_para->uipara_bg.res_hdl[0]);

        if (!pbmp)
        {
            __msg("pbmp is null...");
            return EPDK_FAIL;
        }

        GUI_BMP_Draw(pbmp, ui_para->uipara_bg.x, ui_para->uipara_bg.y);
    }

    //当前时间
    //__here__;
    if (ui_type & MOVIE_PROG_UI_TYPE_CUR_TIME)
    {
        char str_val[32] = {0};
        GUI_RECT gui_rect;
        prog_cuckoo_time_t format_time;
        __u32 cur = 0;

        //__here__;
        if (-1 == scene_para->cur_time_index)//普通更新，从robin获取时间
        {
            cur = robin_get_cur_time();
            __time2time(cur, &format_time);
        }
        else if (scene_para->cur_time_index >= 0 && scene_para->cur_time_index < 3) //设置时间，从外部获取时间
        {
            eLIBs_memcpy(&format_time, &scene_para->cur_time, sizeof(prog_cuckoo_time_t));
        }
        else
        {
            __wrn("invalid para...");
            GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
            com_memdev_delete();
            return EPDK_FAIL;
        }

        if (SWFFont)
        {
            GUI_SetFont(SWFFont);
        }
        else
        {
            GUI_SetFont(GUI_GetDefaultFont());
        }

        GUI_SetColor(GUI_WHITE);
        {
            __s32 i, k = 0;

            //__here__;
            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_cur_time) ; i++)
            {
                if (i == scene_para->cur_time_index)
                {
                    //GUI_SetBkColor(GUI_BLACK);//亮选该时间位置
                }
                else
                {
                    //GUI_SetBkColor(GUI_BLACK);
                }

                gui_rect.x0 = ui_para->uipara_prog_cur_time[i].x;
                gui_rect.y0 = ui_para->uipara_prog_cur_time[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_cur_time[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_cur_time[i].h;

                if (0 == k)
                {
                    GUI_RECT rect1;
                    rect1.x0 = gui_rect.x0;
                    rect1.y0 = gui_rect.y0 ;
                    rect1.x1 = gui_rect.x1 + 100 ;
                    rect1.y1 = gui_rect.y1 ;
                    GUI_ClearRectEx(&rect1);
                    k++;
                }

                eLIBs_sprintf(str_val, "%.2d", format_time.data2[i]);
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
        {
            __s32 i;

            //__here__;
            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_cur_time_dot) ; i++)
            {
                gui_rect.x0 = ui_para->uipara_prog_cur_time_dot[i].x;
                gui_rect.y0 = ui_para->uipara_prog_cur_time_dot[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_cur_time_dot[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_cur_time_dot[i].h;
                eLIBs_sprintf(str_val, ":");
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
    }

    //总时间
    //__here__;
    if (ui_type & MOVIE_PROG_UI_TYPE_TOTAL_TIME)
    {
        __u32 total = 0;
        char str_val[32] = {0};
        GUI_RECT gui_rect = {0};
        prog_cuckoo_time_t format_time = {0};
        //__here__;
        total = robin_get_total_time();
        __time2time(total, &format_time);

        if (SWFFont)
        {
            GUI_SetFont(SWFFont);
        }
        else
        {
            GUI_SetFont(GUI_GetDefaultFont());
        }

        GUI_SetColor(GUI_WHITE);
        //GUI_SetBkColor(GUI_BLACK);
        {
            __s32 i, k = 0;
            //__here__;

            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_total_time) ; i++)
            {
                gui_rect.x0 = ui_para->uipara_prog_total_time[i].x;
                gui_rect.y0 = ui_para->uipara_prog_total_time[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_total_time[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_total_time[i].h;

                //GUI_ClearRectEx(&gui_rect);
                if (0 == k)
                {
                    GUI_RECT rect1;
                    rect1.x0 = gui_rect.x0;
                    rect1.y0 = gui_rect.y0 ;
                    rect1.x1 = gui_rect.x1 + 100 ;
                    rect1.y1 = gui_rect.y1 ;
                    GUI_ClearRectEx(&rect1);
                    k++;
                }

                eLIBs_sprintf(str_val, "%.2d", format_time.data2[i]);
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
        {
            __s32 i;

            //__here__;
            for (i = 0 ; i < BEETLES_TBL_SIZE(ui_para->uipara_prog_total_time_dot) ; i++)
            {
                gui_rect.x0 = ui_para->uipara_prog_total_time_dot[i].x;
                gui_rect.y0 = ui_para->uipara_prog_total_time_dot[i].y;
                gui_rect.x1 = gui_rect.x0 + ui_para->uipara_prog_total_time_dot[i].w;
                gui_rect.y1 = gui_rect.y0 + ui_para->uipara_prog_total_time_dot[i].h;
                eLIBs_sprintf(str_val, ":");
                GUI_DispStringInRect(str_val, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }
        }
    }

    //__here__;
    if (ui_type & MOVIE_PROG_UI_TYPE_PROG)
    {
        __s32 ret;
        ret = __prog_draw_progress(&ui_para->uipara_icon[movie_prog_icon_prog_bg],
                                   &ui_para->uipara_icon[movie_prog_icon_prog_left],
                                   &ui_para->uipara_icon[movie_prog_icon_prog_mid],
                                   &ui_para->uipara_icon[movie_prog_icon_prog_cursor],
                                   0,
                                   robin_get_total_time(),
                                   robin_get_cur_time());

        if (EPDK_OK != ret)
        {
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            com_memdev_delete();
            GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
            return EPDK_FAIL;
        }
    }

    //__here__;
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    com_memdev_delete();
    //GUI_LyrWinSetSta(scene_para->hlyr, GUI_LYRWIN_STA_ON);
    return EPDK_OK;
}

static __s32 __prog_install_prog_timmer(movie_prog_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_timmer_id))
    {
        __wrn("prog hide timmer installed，scene_para->prog_timmer_id=0x%x...", scene_para->prog_timmer_id);
        GUI_SetTimer(scene_para->hfrm, scene_para->prog_timmer_id,
                     100, NULL);
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __prog_uninstall_prog_timmer(movie_prog_scene_t *scene_para)
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

    return EPDK_OK;
}

static __s32 __prog_install_hide_timmer(movie_prog_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (!GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_hide_timmer_id))
    {
        GUI_SetTimer(scene_para->hfrm, scene_para->prog_hide_timmer_id
                     , 400 * 1, NULL);
    }
    else
    {
        __msg("timmer already install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __prog_reset_hide_timmer(movie_prog_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_hide_timmer_id))
    {
        GUI_ResetTimer(scene_para->hfrm, scene_para->prog_hide_timmer_id
                       , 400 * 1.0, NULL);
    }
    else
    {
        __msg("timmer not install...");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32 __prog_uninstall_hide_timmer(movie_prog_scene_t *scene_para)
{
    if (!scene_para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(scene_para->hfrm, scene_para->prog_hide_timmer_id))
    {
        GUI_KillTimer(scene_para->hfrm, scene_para->prog_hide_timmer_id);
    }

    return EPDK_OK;
}


static __s32 __prog_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            movie_prog_scene_t *scene_para;
            __msg("__prog_proc GUI_MSG_CREATE begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
            __prog_init_internal_para(scene_para);
            __prog_init_ui();
            __prog_update_prog_ui(scene_para, MOVIE_PROG_UI_TYPE_ALL);
            __prog_install_prog_timmer(scene_para);
            __msg("__prog_proc GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            movie_prog_scene_t *scene_para;
            __msg("__prog_proc GUI_MSG_DESTROY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            __msg("before __prog_uninstall_prog_timmer");
            __prog_uninstall_prog_timmer(scene_para);
            __msg("after __prog_uninstall_prog_timmer");
            __msg("before __prog_uninstall_hide_timmer");
            __prog_uninstall_hide_timmer(scene_para);
            __msg("after __prog_uninstall_hide_timmer");
            __msg("before __prog_deinit_ui");
            __prog_deinit_ui();
            __msg("after __prog_deinit_ui");
            __msg("__prog_proc GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            static __s32 last_key = -1;
            movie_prog_scene_t *scene_para;
            __msg("__prog_proc GUI_MSG_KEY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            if (KEY_UP_ACTION == msg->dwAddData2)
            {
                last_key = -1;
            }
            else
            {
                last_key = msg->dwAddData1;
            }

            __msg("__prog_proc GUI_MSG_KEY end");
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            __msg("__prog_proc GUI_MSG_TOUCH begin");
            __msg("__prog_proc GUI_MSG_TOUCH end");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            movie_prog_scene_t *scene_para;
            __msg("__prog_proc GUI_MSG_PAINT begin");
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

            __prog_update_prog_ui(scene_para, MOVIE_PROG_UI_TYPE_ALL);
            __msg("__prog_proc GUI_MSG_PAINT begin");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __s32 ret;
            movie_prog_scene_t *scene_para;
            __wrn("__prog_proc GUI_MSG_TIMER begin，msg->dwAddData1=0x%x", msg->dwAddData1);
            scene_para = GUI_WinGetAttr(msg->h_deswin);
            ret = -1;

            if (scene_para)
            {
                if (LOWORD(msg->dwAddData1) == scene_para->prog_timmer_id)
                {
                    __msg("before __prog_update_prog_ui");
                    __prog_update_prog_ui(scene_para, MOVIE_PROG_UI_TYPE_OTHER);
                    //__prog_update_prog_ui(scene_para, MOVIE_PROG_UI_TYPE_ALL);
                    __msg("after __prog_update_prog_ui");
                    ret = EPDK_OK;
                }
                else if (LOWORD(msg->dwAddData1) == scene_para->prog_hide_timmer_id)
                {
                    __wrn("prog hide timmer come...");
                    movie_cmd2parent(msg->h_deswin, movie_prog_scene_msg_timeout, 0, 0);
                    ret = EPDK_OK;
                }
            }
            else
            {
                __err("scene_para is null...");
            }

            __msg("__prog_proc GUI_MSG_TIMER end");

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
            __msg("__prog_proc GUI_MSG_CLOSE begin");
            __msg("__prog_proc GUI_MSG_CLOSE end");
            break;
        }

        default:
        {
            break;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_prog_scene_create(movie_prog_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_prog_scene_t *sence_para;
    movie_prog_uipara_t *ui_para;
    RECT lyr_rect;
    sence_para = (movie_prog_scene_t *)esMEMS_Malloc(0, sizeof(movie_prog_scene_t));

    if (!sence_para)
    {
        __msg("mem not enough...");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_prog_scene_t));
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    ui_para = movie_prog_get_uipara(GUI_GetScnDir());

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
    __msg(" lyr_rect.x = %d,  lyr_rect.y = %d,  lyr_rect.width= %d,  lyr_rect.height= %d",
          lyr_rect.x,  lyr_rect.y,  lyr_rect.width,  lyr_rect.height);
    sence_para->hlyr = __prog_32bpp_layer_create(&lyr_rect, ui_para->pipe);

    if (NULL == sence_para->hlyr)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __msg("sence_para->hlyr is null...");
        return NULL;
    }

    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          =   "movie_prog_win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE | WS_DISABLED;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = sence_para->scene_id;
    framewin_para.hHosting      =  sence_para->hparent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__prog_proc);
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

__s32 movie_prog_scene_delete(void *handle)
{
    movie_prog_scene_t *scene_para;
    scene_para = (movie_prog_scene_t *)handle;

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
    eLIBs_memset(scene_para, 0, sizeof(movie_prog_scene_t));//防止重复释放
    esMEMS_Mfree(0, scene_para);
    return EPDK_OK;
}

__s32 movie_prog_scene_set_focus(void *handle)
{
    movie_prog_scene_t *scene_para;
    scene_para = (movie_prog_scene_t *)handle;

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

H_WIN movie_prog_scene_get_hwnd(void *handle)
{
    movie_prog_scene_t *scene_para;
    scene_para = (movie_prog_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return (H_WIN)EPDK_FAIL;
    }

    return scene_para->hfrm;
}

__s32 movie_prog_scene_enable_auto_hide(void *handle, __bool enable_hide)
{
    movie_prog_scene_t *scene_para;
    scene_para = (movie_prog_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    __wrn("enable_hide=%d", enable_hide);

    if (EPDK_TRUE == enable_hide)
    {
        __prog_install_hide_timmer(scene_para);
    }
    else
    {
        __prog_uninstall_hide_timmer(scene_para);
    }

    return EPDK_OK;
}

__s32 movie_prog_scene_update_ui(void *handle)
{
    movie_prog_scene_t *scene_para = NULL;
    __msg("[movie_prog_scene_update_ui] called!");
    scene_para = (movie_prog_scene_t *)handle;

    if (NULL == scene_para)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    return __prog_update_prog_ui(scene_para, MOVIE_PROG_UI_TYPE_ALL);
}
