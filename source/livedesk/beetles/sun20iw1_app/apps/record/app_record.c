/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*
* File      : record.c
* By        : Kingvan
* Func      : record main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  Kingvan  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "app_record.h"

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

#define CONTROL_TEST    0

#if CONTROL_TEST
H_WIN g_controlWin = NULL;
#endif

static __s32 get_auto_close_scn_time_fm(void);


__u32 app_record_check_disk(void)
{
    __s32 i, ret;
    __u32 disk;
    char diskname[RAT_MAX_PARTITION][4];
    __msg("**********check_disk**********\n");
    disk = RAT_UNKNOWN;
    ret = rat_get_partition_name(RAT_USB_DISK, diskname, 0);

    if (EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("USB : diskname[%d]=%s\n", i, diskname[i]);
                disk |= RAT_USB;
            }

            i++;
        }
    }

    ret = rat_get_partition_name(RAT_SD_CARD, diskname, 0);

    if (EPDK_OK == ret)
    {
        i = 0;
        //while (i < RAT_MAX_PARTITION)
        {
            if (diskname[i][0])
            {
                __msg("SD : diskname[%d]=%s\n", i, diskname[i]);
                disk |= (RAT_TF << 8);
            }

            i++;
        }
    }

    return disk;
}

static void app_record_path(precord_ctrl_t precord_ctrl)
{
    char root_type[1024];
    char disk_name[RAT_MAX_PARTITION][4];
    eLIBs_memset(root_type, 0, sizeof(root_type));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));

    switch (precord_ctrl->root_type)
    {
        case RAT_LOCAL:
            eLIBs_strcpy(root_type, RAT_LOCAL_DISK);
            break;

        case RAT_TF:
            eLIBs_strcpy(root_type, RAT_SD_CARD);
            break;

        case RAT_USB:
            eLIBs_strcpy(root_type, RAT_USB_DISK);
            break;

        default:
            break;
    }

    if (rat_get_partition_name(root_type, disk_name, 0) == EPDK_OK)
    {
        precord_ctrl->root_type_ch = disk_name[0][0];

        if (precord_ctrl->root_type_ch > 'a' && precord_ctrl->root_type_ch < 'z')
        {
            precord_ctrl->root_type_ch -= 32;
        }
    }
    else
    {
        precord_ctrl->root_type_ch = '\0';
    }
}


static void app_record_sendMsg(H_WIN hWin, __u32 id)
{
    __gui_msg_t sendMsg;
    sendMsg.h_deswin = hWin;
    sendMsg.h_srcwin = NULL;
    sendMsg.id = id;
    sendMsg.dwAddData1 = 0;
    sendMsg.dwAddData2 = 0;
    sendMsg.dwReserved = 0;
    GUI_SendNotifyMessage(&sendMsg);
}
void  app_record_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    msg.h_deswin = GUI_WinGetParent(hwin);
    __msg("msg.h_deswin=%x\n", msg.h_deswin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    __msg("msg.dwAddData1=%x\n", msg.dwAddData1);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_SendNotifyMessage(&msg);
}
#if 0
static H_LYR record_8bpp_layer_create(void)
{
    H_LYR layer = NULL;
    RECT LayerRect = {0, 0, 800, 480};
    FB  fb =
    {
        {0, 0},                                                     /* size      */
        {0, 0, 0},                                                  /* buffer    */
        {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},   /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        DISP_LAYER_WORK_MODE_PALETTE,
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
        "app_record",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width           = LayerRect.width;
    fb.size.height          = LayerRect.height;
    lstlyr.src_win.x        = LayerRect.x;
    lstlyr.src_win.y        = LayerRect.y;
    lstlyr.src_win.width    = LayerRect.width;
    lstlyr.src_win.height   = LayerRect.height;
    lstlyr.scn_win.x        = LayerRect.x;
    lstlyr.scn_win.y        = LayerRect.y;
    lstlyr.scn_win.width    = LayerRect.width;
    lstlyr.scn_win.height   = LayerRect.height;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !\n");
    }

    return layer;
}
#endif
static H_LYR record_8bpp_layer_create(void)
{
    H_LYR layer = NULL;
    RECT LayerRect = {0, 0, 800, 480};
    FB  fb =
    {
        {0, 0},                                                         /* size      */
        {0, 0, 0},                                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},     /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
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
        "app_record",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = LayerRect.width;
    fb.size.height      = LayerRect.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = LayerRect.width;
    lstlyr.src_win.height   = LayerRect.height;
    lstlyr.scn_win.x        = LayerRect.x;
    lstlyr.scn_win.y        = LayerRect.y;
    lstlyr.scn_win.width  = LayerRect.width;
    lstlyr.scn_win.height = LayerRect.height;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app record list layer create error !\n");
    }

    return layer;
}

static H_LYR record_32bpp_layer_create(void)
{
    H_LYR layer = NULL;
    RECT LayerRect = {0, 0, 800, 480};
    FB  fb =
    {
        {0, 0},                                                         /* size      */
        {0, 0, 0},                                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},     /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        10,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "app_record_list",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = LayerRect.width;
    fb.size.height      = LayerRect.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = LayerRect.width;
    lstlyr.src_win.height   = LayerRect.height;
    lstlyr.scn_win.x        = LayerRect.x;
    lstlyr.scn_win.y        = LayerRect.y;
    lstlyr.scn_win.width  = LayerRect.width;
    lstlyr.scn_win.height = LayerRect.height;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app record list layer create error !\n");
    }

    return layer;
}

static void record_release_layer(precord_ctrl_t precord_para)
{
    if (NULL == precord_para)
    {
        __wrn("%s precord_para null \r\n", __FUNCTION__);
        return ;
    }

    if (precord_para->layer != NULL)
    {
        GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
        GUI_LyrWinDelete(precord_para->layer);
        precord_para->layer = NULL;
    }

    if (precord_para->record_list_layer != NULL)
    {
        GUI_LyrWinSetSta(precord_para->record_list_layer, GUI_LYRWIN_STA_SUSPEND);
        GUI_LyrWinDelete(precord_para->record_list_layer);
        precord_para->record_list_layer = NULL;
    }
}

static H_WIN record_rec_disk_view_frm_create(__gui_msg_t *msg)
{
    H_WIN hWin;
    __u32 disk;
    precord_ctrl_t precord_para;
    precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
    record_release_layer(precord_para);

    if (precord_para->layer == NULL)
    {
        precord_para->layer = record_8bpp_layer_create();
    }

    return rec_disk_view_frm_create(msg->h_deswin, precord_para);
}
static H_WIN record_rec_view_frm_create(__gui_msg_t *msg)
{
    H_WIN hWin;
    precord_ctrl_t precord_para = NULL;
    precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
    record_release_layer(precord_para);

    if (precord_para->layer == NULL)
    {
        precord_para->layer = record_8bpp_layer_create();
    }

    app_record_path(precord_para);
    return rec_view_frm_create(msg->h_deswin, precord_para);
}
static H_WIN record_rec_list_view_frm_create(__gui_msg_t *msg)
{
    precord_ctrl_t precord_para;
    precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
    record_release_layer(precord_para);

    if (precord_para->record_list_layer == NULL)
    {
        precord_para->record_list_layer = record_32bpp_layer_create();
    }

    return rec_list_view_frm_create(msg->h_deswin, precord_para);
}

static __s32 app_record_on_create(__gui_msg_t *msg)
{
    precord_ctrl_t precord_para;
    __u32 disk;
    __s32 auto_close_scn_time;
    g_disable_standby();
    precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
    init_record_res(&precord_para->rec_ui);
    precord_para->rec_view.pui      = &precord_para->rec_ui;
    precord_para->rec_listView.pui  = &precord_para->rec_ui;
    precord_para->record_list_layer = NULL;
    precord_para->layer             = NULL;
    gscene_bgd_set_state(BGD_STATUS_HIDE);
    precord_para->curOpenFrmType = RECORD_NONE;
    disk = app_record_check_disk();

    if (disk == ((RAT_TF << 8) | RAT_USB))
    {
        //precord_para->frm = record_rec_disk_view_frm_create(msg);
        precord_para->root_type = RAT_TF;
        precord_para->frm = record_rec_view_frm_create(msg);
    }
    else
    {
        if (disk == (RAT_TF << 8))
        {
            precord_para->root_type = RAT_TF;
        }
        else
        {
            precord_para->root_type = RAT_USB;//disk;
        }

        //__here__;
        precord_para->frm = record_rec_view_frm_create(msg);
    }

    if (precord_para->frm == NULL)
    {
        __inf("app_record_on_create() return to home!\n");
        GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
        app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
    }
    else
    {
        GUI_WinSetFocusChild(precord_para->frm);
    }

    auto_close_scn_time = get_auto_close_scn_time_fm();
    __wrn("auto_close_scn_time=%d\n", auto_close_scn_time);

    if (auto_close_scn_time > 0) //设置FM的自动关屏时间
    {
        g_set_close_scn_time(auto_close_scn_time);
    }

    return EPDK_OK;
}
static __s32 app_record_on_close(__gui_msg_t *msg)
{
    GUI_ManWinDelete(msg->h_deswin);
    return EPDK_OK;
}
static __s32 app_record_on_destory(__gui_msg_t *msg)
{
    precord_ctrl_t precord_para;
    precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

    if (precord_para == NULL)
    {
        return EPDK_OK;
    }

    if (EPDK_TRUE == dsk_radio_is_open())
    {
        dsk_radio_close();
    }

    app_alarm_frm_destory(precord_para->alarmfrm);
    g_enable_standby();
    uninit_record_res(&precord_para->rec_ui);
    record_release_layer(precord_para);
#if CONTROL_TEST

    if (g_controlWin != NULL)
    {
        record_destory_side_menu(&g_controlWin);
    }

#endif
    precord_para->layer = 0;
    esMEMS_Mfree(0, precord_para);
    rat_set_modify_flag(RAT_MEDIA_TYPE_AUDIO, 1);
    {
        //恢复系统自动关屏时间
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            g_set_close_scn_time(para->backlightoff);
            __msg("para->backlightoff=%d\n", para->backlightoff);
        }
    }
    gscene_hbar_set_state(HBAR_ST_HIDE);
    return EPDK_OK;
}
static __s32 app_record_on_restore(__gui_msg_t *msg)
{
    precord_ctrl_t precord_para;
    precord_para = (precord_ctrl_t)GUI_WinGetAddData(msg->h_deswin);
    gscene_bgd_set_state(BGD_STATUS_SHOW);
    GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_ON);
    GUI_WinSetFocusChild(precord_para->frm);
    rat_set_modify_flag(RAT_MEDIA_TYPE_AUDIO, 1);
    return EPDK_OK;
}
static __s32 app_record_on_key(__gui_msg_t *msg)
{
#if CONTROL_TEST

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (g_controlWin != NULL)
        {
            record_back_side_menu(&g_controlWin);
        }
        else
        {
            g_controlWin = record_create_side_menu(msg->h_deswin, APP_RECORD_ID + 10);
        }
    }

#endif
    return EPDK_OK;
}
static __s32 app_record_on_command(__gui_msg_t *msg)
{
    __gui_msg_t sendMsg;
    precord_ctrl_t precord_para;
    precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
    sendMsg.h_deswin = precord_para->frm;
    sendMsg.h_srcwin = NULL;
    sendMsg.id = GUI_MSG_COMMAND;
    sendMsg.dwAddData1 = msg->dwAddData1;
    sendMsg.dwAddData2 = msg->dwAddData2;
    sendMsg.dwReserved = 0;
    GUI_PostMessage(&sendMsg);
    return EPDK_OK;
}

static __s32 get_auto_close_scn_time_fm(void)
{
    __s32 bok;
    __s32 close_scn_time = 0;
#if 0
    {
        //init key val by app_config.fex
        char *path = "d:\\app_config.bin";
        H_PARSER parser;
        __s32 ret;
        __s32 i;
        char keyname[64];
        int val;
        bok = 1;

        do
        {
            parser = NULL;
            parser = esCFG_Init_Ex(path);

            if (NULL == parser)
            {
                __wrn("esCFG_Init_Ex fail...\n");
                bok = 0;
                break;
            }

            __wrn("before esCFG_GetKeyValue_Ex\n");
            eLIBs_strcpy(keyname, "fm");
            eLIBs_strcat(keyname, "/");
            eLIBs_strcat(keyname, "auto_close_scn");
            ret = esCFG_GetKeyValue_Ex(parser, keyname, (int *)&val, 1);
            __wrn("after esCFG_GetKeyValue_Ex, ret=%d\n", ret);

            if (EPDK_OK == ret)
            {
                bok = 1;
                close_scn_time = val;
            }
            else
            {
                bok = 0;
            }
        } while (0);

        if (parser)
        {
            esCFG_Exit_Ex(parser);
            parser = NULL;
        }
    }

    if (0 == bok) //default is not auto close scn
    {
        close_scn_time = 0;
    }

    __wrn("close_scn_time=%d\n", close_scn_time);
#endif
    return close_scn_time;
}

static __s32 _record_manager_win_proc(__gui_msg_t *msg)
{
    __msg("_record_manager_win_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            dsk_amplifier_onoff(1);//打开功放
            dsk_set_auto_off_time(0);
            return app_record_on_create(msg);

        case GUI_MSG_CLOSE:
            __msg("----record manager window GUI_MSG_CLOSE begin----\n");
            return app_record_on_close(msg);

        case GUI_MSG_DESTROY:
            dsk_amplifier_onoff(0);//关闭功放
            {
                reg_system_para_t *para = NULL;
                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    dsk_set_auto_off_time(para->poweroff);
                    __wrn("para->poweroff=%d\n", para->poweroff);
                }
            }
            __msg("----record manager window GUI_MSG_DESTROY begin----\n");
            return app_record_on_destory(msg);

        case GUI_MSG_KEY:
            app_record_on_key(msg);
            break;//注意!!!

        case DSK_APP_RESTORE:
            return app_record_on_restore(msg);

        case GUI_MSG_TOUCH:
            break;

        case GUI_MSG_COMMAND:
            return app_record_on_command(msg);

        case GUI_MSG_NOTIFY_CHILD_DELETED:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

            if (precord_para->curOpenFrmType == RECORD_FRM)
            {
                precord_para->curOpenFrmType = RECORD_NONE;

                if (precord_para->bIsFMRecord)
                {
                    dsk_radio_unmute();
                }

                __wrn("normal escape 7\n");
                precord_para->frm = record_rec_view_frm_create(msg);

                if (precord_para->frm == NULL)
                {
                    GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
                    app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
                }
                else
                {
                    GUI_WinSetFocusChild(precord_para->frm);
                }
            }
            else if (precord_para->curOpenFrmType == RECORD_LIST_FRM)
            {
                precord_para->curOpenFrmType = RECORD_NONE;

                if (precord_para->bIsFMRecord)
                {
                    dsk_radio_mute();
                }

                precord_para->frm = record_rec_list_view_frm_create(msg);
                __wrn("normal create 4 precord_para->frm = %08x\n", precord_para->frm);

                if (precord_para->frm == NULL)
                {
                    GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
                    app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
                }
                else
                {
                    GUI_WinSetFocusChild(precord_para->frm);
                }
            }
            else if (precord_para->curOpenFrmType == MAIN_HOME)
            {
                precord_para->curOpenFrmType = RECORD_NONE;
                GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
                app_record_cmd2parent(msg->h_deswin, SWITCH_TO_MMENU, 0, 0);
            }
            else if (precord_para->curOpenFrmType == TO_FM)
            {
                precord_para->curOpenFrmType = RECORD_NONE;

                if (precord_para->bIsFMRecord)
                {
                    dsk_radio_unmute();
                }

                GUI_LyrWinSetSta(precord_para->layer, GUI_LYRWIN_STA_SUSPEND);
                app_record_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, MAKELONG(RECORD_SW_TO_FM, precord_para->root_type), 0);
            }
        }

        return EPDK_OK;

        case RECORD_TO_LIST_VIEW_FRAME:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
            precord_para->curOpenFrmType = RECORD_LIST_FRM;
            app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
        }

        return EPDK_OK;

        case RECORD_TO_VIEW_FRAME:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
            precord_para->curOpenFrmType = RECORD_FRM;
            __wrn("normal escape 4 precord_para->frm = %08x\n", precord_para->frm);
            app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
        }

        return EPDK_OK;

        case RECORD_TO_HOME:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

            if (precord_para->bIsFMRecord)
            {
                precord_para->curOpenFrmType = TO_FM;
            }
            else
            {
                precord_para->curOpenFrmType = MAIN_HOME;
            }

            app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
        }

        return EPDK_OK;

        case ALARM_MSG_CLOSE:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

            if (precord_para == NULL)
            {
                return EPDK_OK;
            }

            precord_para->alarmfrm = NULL;
            GUI_WinSetFocusChild(msg->h_deswin);
        }

        return EPDK_OK;

        case DSK_MSG_ALARM:
        {
            precord_ctrl_t precord_para;
            char name[1024] = {0};
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

            if (precord_para == NULL)
            {
                return EPDK_OK;
            }

            GUI_WinGetName(precord_para->frm, name);

            if (eLIBs_strcmp(name, "record_list_view_frm") == 0)
            {
                prec_list_view_frame_t plistviewFrm_ctl;
                plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(precord_para->frm);

                if (plistviewFrm_ctl == NULL)
                {
                    return EPDK_FAIL;
                }

                if (plistviewFrm_ctl->delDlg != NULL)
                {
                    app_dialog_destroy(plistviewFrm_ctl->delDlg);
                    plistviewFrm_ctl->delDlg = NULL;
                }
            }

            precord_para->alarmfrm = app_alarm_frm_create(msg->h_deswin);
        }

        return EPDK_OK;

        case DSK_MSG_HOME:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);

            if (precord_para->bIsFMRecord)
            {
                precord_para->curOpenFrmType = TO_FM;
            }
            else
            {
                precord_para->curOpenFrmType = MAIN_HOME;
            }

            app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
        }

        return EPDK_OK;

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            precord_ctrl_t precord_para;
            precord_para = (precord_ctrl_t)GUI_WinGetAttr(msg->h_deswin);
            __inf("data2:%c, root_type_ch:%c\n", msg->dwAddData2, precord_para->root_type_ch);

            if (msg->dwAddData2 == precord_para->root_type_ch)
            {
                if (precord_para->bIsFMRecord)
                {
                    precord_para->curOpenFrmType = TO_FM;
                }
                else
                {
                    precord_para->curOpenFrmType = MAIN_HOME;
                }

                app_record_sendMsg(precord_para->frm, GUI_MSG_CLOSE);
            }
        }

        return EPDK_OK;
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN app_record_create(root_para_t  *para)
{
    __gui_manwincreate_para_t   create_info;
    precord_ctrl_t              precord_ctrl = NULL;
    __inf("****************************************************************************************\n");
    __inf("********  enter record application  **************\n");
    __inf("****************************************************************************************\n");
    //esKSRV_SysInfo();
    precord_ctrl = (precord_ctrl_t)esMEMS_Malloc(0, sizeof(record_ctrl_t));

    if (precord_ctrl == NULL)
    {
        __msg("esMEMS_Malloc fail\n");
        return NULL;
    }

    eLIBs_memset(precord_ctrl, 0, sizeof(record_ctrl_t));
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    precord_ctrl->pfont         = para->font;
    precord_ctrl->bIsFMRecord   = (para->record_opt == 1);
    precord_ctrl->root_type     = RAT_UNKNOWN;
    create_info.name            = APP_RECORD;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_record_manager_win_proc);
    create_info.attr            = (void *)precord_ctrl;
    create_info.id              = APP_RECORD_ID;
    create_info.hHosting        = NULL;
    return (GUI_ManWinCreate(&create_info));
}

__s32 app_record_notify_delete_sub_scene(H_WIN hmanwin)
{
    precord_ctrl_t precord_para;

    if (NULL == hmanwin)
    {
        __wrn("invalid para...\n");
        return EPDK_FAIL;
    }

    precord_para = (precord_ctrl_t)GUI_WinGetAttr(hmanwin);

    if (precord_para->frm)
    {
        char name[256] = {0};
        GUI_WinGetName(precord_para->frm, name);
        __wrn("name=%s\n", name);

        if (0 == eLIBs_strcmp("record_list_view_frm", name))
        {
            prec_list_view_frame_t plistviewFrm_ctl;
            plistviewFrm_ctl = (prec_list_view_frame_t)GUI_WinGetAttr(precord_para->frm);

            if (plistviewFrm_ctl == NULL)
            {
                return EPDK_FAIL;
            }

            if (plistviewFrm_ctl->delDlg)
            {
                app_dialog_destroy(plistviewFrm_ctl->delDlg);
                plistviewFrm_ctl->delDlg = NULL;
                GUI_WinSetFocusChild(precord_para->frm);
            }
        }
    }

    return EPDK_OK;
}

