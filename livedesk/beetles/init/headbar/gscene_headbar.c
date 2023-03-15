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
#include "mod_init_i.h"
#include "headbar_uipara.h"
#include "headbar_com.h"
#include "prog_bar/prog_bar.h"

/**********************************************************************************************************************/
#define ID_SHBAR        4000

int32_t gscene_hbar_set_screen_rect(RECT *rect);


/**********************************************************************************************************************/
H_WIN headbar_win   = NULL;
static int32_t      g_music_state = 0;

typedef struct
{
    H_LYR           hbar_lyr;
    H_WIN           hbar_frm;
    void            *hbar_progbar;
    int32_t         kill_hbar_on_volume_timmout;//hbar音量条timeout的时候是否隐藏hbar
} __shbar_ctrl_t;

static H_LYR __hbar_layer_create(__hbar_format_t format)
{
    int32_t     width, height;
    H_LYR       layer = NULL;
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
        11,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "headbar layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    dsk_display_get_size(&width, &height);

    if (format == HBAR_FOARMAT_8BPP && width == 1920 && height == 1080)
    {
        //  fb.fmt.fmt.rgb.pixelfmt = PIXEL_MONO_8BPP;
        //  lstlyr.mode             = DISP_LAYER_WORK_MODE_PALETTE;
    }

    fb.size.width           = headbar_uipara.hbar_lyr.fb_w;
    fb.size.height          = headbar_uipara.hbar_lyr.fb_h;
    lstlyr.src_win.x        = 0;//headbar_uipara.hbar_lyr.src_x;
    lstlyr.src_win.y        = 0;//headbar_uipara.hbar_lyr.src_y;
    lstlyr.src_win.width    = headbar_uipara.hbar_lyr.src_w;
    lstlyr.src_win.height   = headbar_uipara.hbar_lyr.src_h;
    lstlyr.scn_win.x        = headbar_uipara.hbar_lyr.scn_x;
    lstlyr.scn_win.y        = headbar_uipara.hbar_lyr.scn_y;
    lstlyr.scn_win.width    = headbar_uipara.hbar_lyr.scn_w;
    lstlyr.scn_win.height   = headbar_uipara.hbar_lyr.scn_h;
    lstlyr.pipe             = headbar_uipara.hbar_lyr.pipe;
    lstlyr.fb               = &fb;
    lyrcreate_info.lyrpara  = &lstlyr;
	__log("hbar fb %d %d gui_dir=%d",fb.size.width,fb.size.height, GUI_GetScnDir());
	__log("hbar src %d %d %d %d",lstlyr.src_win.x,lstlyr.src_win.y,lstlyr.src_win.width,lstlyr.src_win.height);
	__log("hbar scn %d %d %d %d",lstlyr.scn_win.x,lstlyr.scn_win.y,lstlyr.scn_win.width,lstlyr.scn_win.height);
    layer   = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("headbar layer create error !");
    }

    return layer;
}

static int32_t __hbar_prog_scene_create(H_WIN hManwin, int32_t id, int32_t min, int32_t cur, int32_t max)
{
    __shbar_ctrl_t  *hbar_ctrl;
    int32_t         ret;

    hbar_ctrl = (__shbar_ctrl_t *)GUI_WinGetAddData(hManwin);
__log("__hbar_prog_scene_create");
    if (NULL == hbar_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (NULL == hbar_ctrl->hbar_progbar)
    {
        init_prog_bar_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(init_prog_bar_create_para_t));
        create_para.hlyr = hbar_ctrl->hbar_lyr;
        create_para.hparent = hManwin;
        create_para.scene_id = id;
        create_para.max_val = max;
        create_para.min_val = min;
        create_para.cur_val = cur;
        hbar_ctrl->hbar_progbar = init_prog_bar_scene_create(&create_para);

        if (NULL == hbar_ctrl->hbar_progbar)
        {
            __msg("init_prog_bar_scene_create fail...");
            return EPDK_FAIL;
        }
    }

    return EPDK_OK;
}

static int32_t __hbar_prog_scene_update_data(H_WIN hManwin, __s32 min, __s32 cur, __s32 max)
{
    __shbar_ctrl_t  *hbar_ctrl;
    int32_t         ret;

    hbar_ctrl = (__shbar_ctrl_t *)GUI_WinGetAddData(hManwin);
__log("__hbar_prog_scene_update_data");
    if (NULL == hbar_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (NULL == hbar_ctrl->hbar_progbar)
    {
        return EPDK_FAIL;
    }

    {
        //update prog bar
        int32_t     ret;

        init_prog_bar_create_para_t create_para;
        eLIBs_memset(&create_para, 0, sizeof(init_prog_bar_create_para_t));
        create_para.max_val = max;
        create_para.min_val = min;
        create_para.cur_val = cur;
        ret = init_prog_bar_scene_update_data(hbar_ctrl->hbar_progbar, &create_para);

        if (EPDK_FAIL == ret)
        {
            __msg("init_prog_bar_scene_update_data fail...");
            return EPDK_FAIL;
        }
    }

    //update vol text
    if (hbar_ctrl->hbar_frm)
    {
        __gui_msg_t msgex;
        eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
        msgex.id            = HBAR_VOLUME_PAINT;
        msgex.h_srcwin      = NULL;
        msgex.h_deswin      = hbar_ctrl->hbar_frm;
        msgex.dwAddData1    = dsk_volume_get();
        msgex.dwAddData2    = 0;
        msgex.dwReserved    = 0;
        msgex.p_arg         = NULL;
        __wrn("update vol text");
        GUI_SendMessage(&msgex);
    }

    return EPDK_OK;
}

static int32_t __hbar_prog_scene_delete(H_WIN hManwin)
{
    __shbar_ctrl_t  *hbar_ctrl;
    int32_t         ret;

    hbar_ctrl = (__shbar_ctrl_t *)GUI_WinGetAddData(hManwin);

    if (NULL == hbar_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (hbar_ctrl->hbar_progbar)
    {
        init_prog_bar_scene_delete(hbar_ctrl->hbar_progbar);
        hbar_ctrl->hbar_progbar = NULL;
    }

    return EPDK_OK;
}

//判断hbar manwin是否有指定的子场景之一
static int32_t __hbar_has_sub_scene(H_WIN hManwin, uint32_t sub_id)
{
    H_WIN           child;
    char            winname[256];
    uint32_t        child_id;

    if (NULL == hManwin)
    {
        __err("invalid para...");
        return EPDK_FALSE;
    }

    child = GUI_WinGetFirstChild(hManwin);

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

//删除hbar的子场景
static int32_t __hbar_delete_subscene_by_id(H_WIN hManwin, uint32_t sub_id)
{
    H_WIN       child, next_child;
    char        winname[256];
    uint32_t    child_id;

    if (NULL == hManwin)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    child = GUI_WinGetFirstChild(hManwin);

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);
        next_child = GUI_WinGetNextBro(child);
        child_id = GUI_WinGetItemId(child);

        if ((child_id & sub_id))//需要删除
        {
            __msg("begin delete hbar sub scene, winname = %s ", winname);

            switch (child_id)
            {
                case ID_HEADBAR_COM:
                {
                    break;
                }

                case ID_VOLUME_COM:
                {
                    __hbar_prog_scene_delete(hManwin);
                    break;
                }

                case ID_BRIGHT_COM:
                {
                    __hbar_prog_scene_delete(hManwin);
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

static int32_t cb_shbar_mwin(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __shbar_ctrl_t      *scene;
            __hbar_format_t     format;

            format  = (__hbar_format_t)GUI_WinGetAttr(msg->h_deswin);
            scene   = (__shbar_ctrl_t *)esMEMS_Malloc(0, sizeof(__shbar_ctrl_t));

            if (!scene)
            {
                __err(" __shbar_ctrl_t malloc error ");
                return -1;
            }

            eLIBs_memset(scene, 0, sizeof(__shbar_ctrl_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)scene);

            if (!FONT16)
            {
                FONT16 = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, BEETLES_APP_ROOT"res\\fonts\\font16.sft");
            }

            headbar_init_uipara(format);

            /*  创建 headbar frmwin */
            scene->hbar_lyr     = __hbar_layer_create(format);
            scene->hbar_frm     = headbar_frm_create(msg->h_deswin, scene->hbar_lyr);
        }

        return 0;

        case GUI_MSG_DESTROY:
        {
            __shbar_ctrl_t  *scene;
            GUI_FONT        *DefaultFont;

            scene       = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            DefaultFont = GUI_GetDefaultFont();
            GUI_SetFont(DefaultFont);

            if (FONT16)
            {
                GUI_SFT_ReleaseFont(FONT16);
                FONT16 = NULL;
            }

            __hbar_delete_subscene_by_id(msg->h_deswin, ID_BRIGHT_COM | ID_VOLUME_COM);
            GUI_LyrWinDelete(scene->hbar_lyr);
            esMEMS_Mfree(0, scene);
        }

        return 0;

        case GUI_MSG_CLOSE:
            GUI_ManWinDelete(msg->h_deswin);
            return 0;

        case GUI_MSG_COMMAND:
        {
            __shbar_ctrl_t  *scene;
    
            scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            switch (LOSWORD(msg->dwAddData1))
            {
                case ID_HEADBAR_COM:
                {
                    switch (HISWORD(msg->dwAddData1))
                    {
                        case HBAR_ADJUST_VOLUME:
                        {
                            RECT rect1, rect2;

                            rect2.x         = 0;
                            rect2.y         = 0;
                            rect2.width     = 800;
                            rect2.height    = 114;
                            __hbar_delete_subscene_by_id(msg->h_deswin, ID_BRIGHT_COM | ID_VOLUME_COM);
                            __msg("HBAR_ADJUST_VOLUME");
                            headbar_enable_paint_time(0);
                            //GUI_LyrWinSel(scene->hbar_lyr);
                            //gscene_hbar_get_screen_rect(&rect1);
                            gscene_hbar_set_screen_rect(&rect2);
                            //gscene_hbar_get_screen_rect(&rect1);
                            __hbar_prog_scene_create(msg->h_deswin, ID_VOLUME_COM, AUDIO_DEVICE_VOLUME_MIN, dsk_volume_get(), AUDIO_DEVICE_VOLUME_MAX);
                            break;
                        }

                        case HBAR_ADJUST_BRIGHT:
                        {
                            RECT rect2;
                            rect2.x = 0;
                            rect2.y = 0;
                            rect2.width = 800;
                            rect2.height = 114;
                            __hbar_delete_subscene_by_id(msg->h_deswin, ID_BRIGHT_COM | ID_VOLUME_COM);
                            __msg("HBAR_ADJUST_BRIGHT");
                            headbar_enable_paint_time(0);
                            gscene_hbar_set_screen_rect(&rect2);
                            __hbar_prog_scene_create(msg->h_deswin, ID_BRIGHT_COM, LION_BRIGHT_LEVEL1, dsk_display_get_lcd_brightness(), LION_BRIGHT_LEVEL16);
                            break;
                        }

                        //case HBAR_MUISC_STATE:
                        //{
                        //   __gui_msg_t msg;
                        //  __msg("+++++++++++HBAR_MUISC_STATE+++++++++++++");
                        //  eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
                        //  msg.id          = DSK_MSG_MUISC_ON_OFF;
                        //  msg.h_srcwin    = NULL;
                        //  msg.h_deswin    = GUI_WinGetHandFromName("init");
                        //  msg.dwAddData1  = (__u32)0;
                        //  msg.dwAddData2  = (__u32)0;

                        //    if(msg.h_deswin)
                        //    {
                        //      GUI_SendMessage(&msg);
                        //    }
                        //    break;
                        //}
                        case HBAR_RETURN_HOME:
                        {
                            __gui_msg_t msg;

                            __msg("+++++++++++HBAR_RETURN_HOME+++++++++++++");
                            eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

                            msg.id          = DSK_MSG_HOME;
                            msg.h_srcwin    = NULL;
                            msg.h_deswin    = GUI_WinGetHandFromName("init");
                            msg.dwAddData1  = (__u32)0;
                            msg.dwAddData2  = (__u32)0;

                            if (msg.h_deswin)
                            {
                                GUI_SendMessage(&msg);
                            }

                            break;
                        }

                        default:
                            break;
                    }
                }
                break;

                case ID_VOLUME_COM:
                {
                    switch (HISWORD(msg->dwAddData1))
                    {
                        case init_prog_bar_scene_msg_prog_barset:
                        {
                            __gui_msg_t tmp_msg;

                            tmp_msg.id          = HBAR_VOLUME_PAINT;
                            tmp_msg.h_srcwin    = NULL;
                            tmp_msg.h_deswin    = scene->hbar_frm;
                            tmp_msg.dwAddData1  = msg->dwAddData2;
                            tmp_msg.dwAddData2  = 0;
                            tmp_msg.dwReserved  = 0;
                            tmp_msg.p_arg       = NULL;
                            __msg("dsk_volume_set, msg->dwAddData2 = %d", msg->dwAddData2);
                            dsk_volume_set(msg->dwAddData2);
                            GUI_SendMessage(&tmp_msg);
                            break;
                        }

                        case init_prog_bar_scene_msg_timeout:
                        {
                            __gui_msg_t tmp_msg;

                            headbar_enable_paint_time(1);
                            __hbar_delete_subscene_by_id(msg->h_deswin, ID_VOLUME_COM);
                            __wrn("scene->kill_hbar_on_volume_timmout=%d", scene->kill_hbar_on_volume_timmout);

                            if (EPDK_FALSE == scene->kill_hbar_on_volume_timmout)
                            {
                                tmp_msg.id          = HBAR_TIME_PAINT;
                                tmp_msg.h_srcwin    = NULL;
                                tmp_msg.h_deswin    = scene->hbar_frm;
                                tmp_msg.dwAddData1  = msg->dwAddData2;
                                tmp_msg.dwAddData2  = 0;
                                tmp_msg.dwReserved  = 0;
                                tmp_msg.p_arg       = NULL;
                                GUI_SendMessage(&tmp_msg);
                            }
                            else//音量条timmout时，同时隐藏headbar
                            {
                                gscene_hbar_set_state(HBAR_ST_HIDE);
                            }

                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                case ID_BRIGHT_COM:
                {
                    switch (HISWORD(msg->dwAddData1))
                    {
                        case init_prog_bar_scene_msg_prog_barset:
                        {
                            __gui_msg_t tmp_msg;

                            headbar_enable_paint_time(1);
                            tmp_msg.id          = HBAR_BRIGHT_PAINT;
                            tmp_msg.h_srcwin    = NULL;
                            tmp_msg.h_deswin    = scene->hbar_frm;
                            tmp_msg.dwAddData1  = msg->dwAddData2;
                            tmp_msg.dwAddData2  = 0;
                            tmp_msg.dwReserved  = 0;
                            tmp_msg.p_arg       = NULL;
                            __msg("dsk_display_set_lcd_bright, msg->dwAddData2 = %d", msg->dwAddData2);
                            dsk_display_set_lcd_brightness(msg->dwAddData2);
                            GUI_SendMessage(&tmp_msg);
                            break;
                        }

                        case init_prog_bar_scene_msg_timeout:
                        {
                            __gui_msg_t tmp_msg;

                            __hbar_delete_subscene_by_id(msg->h_deswin, ID_BRIGHT_COM);
                            headbar_enable_paint_time(1);
                            tmp_msg.id          = HBAR_TIME_PAINT;
                            tmp_msg.h_srcwin    = NULL;
                            tmp_msg.h_deswin    = scene->hbar_frm;
                            tmp_msg.dwAddData1  = msg->dwAddData2;
                            tmp_msg.dwAddData2  = 0;
                            tmp_msg.dwReserved  = 0;
                            tmp_msg.p_arg       = NULL;
                            GUI_SendMessage(&tmp_msg);
                            break;
                        }

                        default:
                            break;
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }

            return EPDK_OK;
        }

        case HBAR_TITLE_PAINT:
        {
            __gui_msg_t     msgex;
            __shbar_ctrl_t  *scene;

            scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
            msgex.id            = HBAR_TITLE_PAINT;
            msgex.h_srcwin      = msg->h_deswin;
            msgex.h_deswin      = scene->hbar_frm;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            GUI_SendMessage(&msgex);
            return EPDK_OK;
        }

        case HBAR_SIG_STATUS_PAINT:
        {
            __gui_msg_t     msgex;
            __shbar_ctrl_t  *scene;

            scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
            msgex.id            = HBAR_SIG_STATUS_PAINT;
            msgex.h_srcwin      = msg->h_deswin;
            msgex.h_deswin      = scene->hbar_frm;
            msgex.dwAddData1    = msg->dwAddData1;
            msgex.dwAddData2    = msg->dwAddData2;
            GUI_SendMessage(&msgex);
            return EPDK_OK;
        }

        /*case DSK_MSG_MAIN_CMD:
        {
            __gui_msg_t     msgex;
            __shbar_ctrl_t  *scene;

            scene = (__shbar_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));

            if( msg->dwAddData1 == HEADBAR_CMD_SET_VOLUME )
            {
                if( scene->volume_frm )
                {
                    msgex.id            = DSK_MSG_MAIN_CMD;
                    msgex.h_srcwin      = msg->h_srcwin;
                    msgex.h_deswin      = scene->volume_frm;
                    msgex.dwAddData1    = msg->dwAddData1;
                    msgex.dwAddData2    = msg->dwAddData2;

                    GUI_SendMessage(&msgex);
                }
            }
            return EPDK_OK;
        }*/

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}


signed long gscene_hbar_create(H_WIN parent, __hbar_format_t format)
{
    H_WIN                       hManWin;
    __gui_manwincreate_para_t   create_info;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.id              = ID_SHBAR;
    create_info.attr            = (void *)format;
    create_info.hParent         = parent;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = cb_shbar_mwin;
    create_info.name            = "headbar";
    hManWin                     = GUI_ManWinCreate(&create_info);
    if (hManWin == NULL)
    {
        __wrn("headbar: create main windows failed!");
        return (signed long)NULL;
    }

    headbar_win = hManWin;
	
    return 0;
}

int32_t gscene_hbar_delete(void)
{
    if (headbar_win)
    {
        GUI_ManWinDelete(headbar_win);
        headbar_win = NULL;
    }

    return EPDK_OK;
}

int32_t gscene_hbar_set_state(HBarState state)
{
    __shbar_ctrl_t  *pscene     = (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    H_WIN           hbar_layer  = pscene->hbar_lyr;

    __inf("**********gscene_hbar_set_state: state=%d***********", state);

    switch (state)
    {
        case HBAR_ST_SHOW_WIDTH_VOLUME:
        {
            int32_t         ret;
            HBarState       hbar_sta;

            gscene_hbar_get_state(&hbar_sta);
            __wrn("hbar_sta=%d", hbar_sta);
            GUI_LyrWinSetSta(hbar_layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(hbar_layer);

            if (HBAR_ST_HIDE == hbar_sta) //如果先前hbar是隐藏状态，则音量条timeout后要隐藏hbar
            {
                pscene->kill_hbar_on_volume_timmout = EPDK_TRUE;
            }

            __wrn("pscene->kill_hbar_on_volume_timmout=%d", pscene->kill_hbar_on_volume_timmout);
            ret = __hbar_has_sub_scene(headbar_win, ID_VOLUME_COM);
            __wrn("__hbar_has_sub_scene(headbar_win, ID_VOLUME_COM):ret=%d", ret);

            if (EPDK_FALSE == ret)
            {
                headbar_enable_paint_time(0);
                __hbar_prog_scene_create(headbar_win, ID_VOLUME_COM, AUDIO_DEVICE_VOLUME_MIN, dsk_volume_get(), AUDIO_DEVICE_VOLUME_MAX);
            }

            __hbar_prog_scene_update_data(headbar_win, AUDIO_DEVICE_VOLUME_MIN, dsk_volume_get(), AUDIO_DEVICE_VOLUME_MAX);
            __wrn("pscene->kill_hbar_on_volume_timmout=%d", pscene->kill_hbar_on_volume_timmout);
            break;
        }

        case HBAR_ST_SHOW:
            GUI_LyrWinSetSta(hbar_layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(hbar_layer);
            pscene->kill_hbar_on_volume_timmout = EPDK_FALSE;
            headbar_enable_paint_time(1);
            break;

        case HBAR_ST_HIDE:
            GUI_LyrWinSetSta(hbar_layer, GUI_LYRWIN_STA_SUSPEND);
            pscene->kill_hbar_on_volume_timmout = EPDK_FALSE;
            break;

        default:
            __wrn("HBarState is invalid! ");
            break;
    }

    return 0;
}

int32_t gscene_hbar_get_state(HBarState *p_state)
{
    __gui_lyrwin_sta_t  state;
    __shbar_ctrl_t      *pscene     = (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    H_WIN               hbar_layer  = pscene->hbar_lyr;

    state =  GUI_LyrWinGetSta(hbar_layer);

    if (state == GUI_LYRWIN_STA_ON)
    {
        *p_state = HBAR_ST_SHOW;
    }
    else
    {
        *p_state = HBAR_ST_HIDE;
    }

    return 0;
}

/* title 为utf8编码格式字符串，len<=32 ,设置hbar title 区域字符串*/
int32_t gscene_hbar_set_title(char *title, uint32_t len)
{
    __gui_msg_t     msgex;

    eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
    msgex.id            = HBAR_TITLE_PAINT;
    msgex.h_srcwin      = NULL;
    msgex.h_deswin      = headbar_win;
    msgex.dwAddData1    = (unsigned long)title;
    msgex.dwAddData2    = len;
    GUI_SendMessage(&msgex);
    return 0;
}

int32_t gscene_hbar_set_sig_status(uint8_t st, uint8_t qa, __bool en)
{
    __gui_msg_t     msgex;

    eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
    headbar_enable_paint_signal(en);
    msgex.id            = HBAR_SIG_STATUS_PAINT;
    msgex.h_srcwin      = NULL;
    msgex.h_deswin      = headbar_win;
    msgex.dwAddData1    = (__u32)st;
    msgex.dwAddData2    = (__u32)qa;
    __msg("gscene_hbar_set_sig_status");
    GUI_SendMessage(&msgex);
    return 0;
}


/*__s32 gscene_hbar_set_volume(void)
{
    __gui_msg_t msgex;

    eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));

    msgex.id            = DSK_MSG_MAIN_CMD;
    msgex.h_srcwin      = NULL;
    msgex.h_deswin      = headbar_win;
    msgex.dwAddData1    = HEADBAR_CMD_SET_VOLUME;
    msgex.dwAddData2    = NULL;

    GUI_SendMessage(&msgex);

    return 0;
}

__s32 gscene_hbar_set_format(__hbar_format_t format)
{
    __gui_msg_t msg;
    __s32 ret;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
    msg.id          = HEADBAR_CMD_SET_FORMAT;
    msg.h_srcwin    = NULL;
    msg.h_deswin    = GUI_WinGetHandFromName("init");
    msg.dwAddData1  = (__u32)format;
    msg.dwAddData2  = 0;

    ret = GUI_SendMessage(&msg);
    return ret;
}
*/

int32_t gscene_hbar_get_screen_rect(RECT *rect)
{
    __shbar_ctrl_t  *p_scene;

    p_scene = (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    GUI_LyrWinGetScnWindow(p_scene->hbar_lyr, rect);
    __msg("rect->x=%d,rect->y=%d, rect->width=%d, rect->height =%d", rect->x, rect->y, rect->width, rect->height);
    GUI_LyrWinGetSrcWindow(p_scene->hbar_lyr, rect);
    __msg("rect->x=%d,rect->y=%d, rect->width=%d, rect->height =%d", rect->x, rect->y, rect->width, rect->height);
    return EPDK_OK;
}

int32_t gscene_hbar_set_screen_rect(RECT *rect)
{
    __shbar_ctrl_t  *p_scene;
    int32_t         ret;

    p_scene = (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
    GUI_LyrWinCacheOn();
    ret = GUI_LyrWinSetScnWindow(p_scene->hbar_lyr, rect);

    if (ret != EPDK_OK)
    {
        __wrn("layer set src windows failed,layer handle = %d", p_scene->hbar_lyr);
        return __LINE__;
    }

    ret = GUI_LyrWinSetSrcWindow(p_scene->hbar_lyr, rect);

    if (ret != EPDK_OK)
    {
        __wrn("layer set src windows failed,layer handle = %d", p_scene->hbar_lyr);
        return __LINE__;
    }

    //GUI_LyrWinFlushFB(p_scene->hbar_lyr);
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

void gscene_hbar_set_music_state(__bg_audio_t state)
{
    g_music_state = state;
    return ;
}

__bg_audio_t gscene_hbar_get_music_state(void)
{
    return g_music_state;
}

int32_t gscene_hbar_update_text(int32_t id)
{
    __shbar_ctrl_t  *hbar_ctrl;
    int32_t         ret;
    uint32_t        dwAddData1;

    hbar_ctrl = (__shbar_ctrl_t *)GUI_WinGetAddData(headbar_win);
__log("gscene_hbar_update_text");
    if (NULL == hbar_ctrl)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    if (HBAR_VOLUME_PAINT != id && HBAR_BRIGHT_PAINT != id)
    {
        return EPDK_FAIL;
    }

    if (HBAR_VOLUME_PAINT == id)
    {
        dwAddData1 = dsk_volume_get();
    }
    else if (HBAR_BRIGHT_PAINT == id)
    {
        dwAddData1 = dsk_display_get_lcd_brightness();
    }
    else
    {
        return EPDK_FAIL;
    }

    if (hbar_ctrl->hbar_frm)
    {
        __gui_msg_t msgex;

        eLIBs_memset(&msgex, 0, sizeof(__gui_msg_t));
        msgex.id            = id;
        msgex.h_srcwin      = NULL;
        msgex.h_deswin      = hbar_ctrl->hbar_frm;
        msgex.dwAddData1    = dwAddData1;
        msgex.dwAddData2    = 0;
        msgex.dwReserved    = 0;
        msgex.p_arg         = NULL;
        __wrn("update vol text");
        GUI_SendMessage(&msgex);
    }

    return EPDK_OK;
}
