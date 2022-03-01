/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : app_settings.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "app_setting.h"
#include "app_setting_i.h"
#include "setting_uipara.h"
#include "setting_general.h"
#include "setting_power.h"
#include "setting_entrance.h"
#include "setting_tips.h"
#include "setting_alarm.h"
#include "setting_alarm_clock.h"
#include "alarm_select_song_frm.h"
#include "setting_handle.h"


#define     UDISK_NAME          "f:\\"

Setting_focus_t SETTING_DISPLAY_FOCUS = SETTING_DISPLAY_FOCUS_NONE; //0 :LEFT    1:RIGHT
__s32 key_focous_left_right = 1; //0 :LEFT    1:RIGHT

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR setting_layer_32bpp_create(void)
{
    setting_uipara_t   *uipara = NULL;
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        //.. {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        //    DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        8,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "setting layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    uipara = (setting_uipara_t *)setting_get_uipara();
    fb.size.width       = uipara->lyr_win.w;
    fb.size.height      = uipara->lyr_win.h;
    lstlyr.src_win.x        = uipara->lyr_win.x;
    lstlyr.src_win.y        = uipara->lyr_win.y;
    lstlyr.src_win.width        = uipara->lyr_win.w;
    lstlyr.src_win.height   = uipara->lyr_win.h;
    lstlyr.scn_win.x        = uipara->lyr_scn.x;
    lstlyr.scn_win.y        = uipara->lyr_scn.y;
    lstlyr.scn_win.width    = uipara->lyr_scn.w;
    lstlyr.scn_win.height   = uipara->lyr_scn.h;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}


static void setting_get_volume_size(__gui_msg_t *msg)
{
    char        str[128];
    char        temp_str[32]  = { 0 };
    char        numstr[32];
    __u64       size;
    __u32       gnum;
    __u32        snum;
    setting_ctrl_t  *setting_ctrl;
    setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    eLIBs_memset(str, 0, sizeof(str));
    eLIBs_memset(&(setting_ctrl->volume_para),  0,  sizeof(setting_ctrl->volume_para)) ;
    get_menu_text(STRING_SET_TIPS_TSIZE, temp_str, sizeof(temp_str));
    eLIBs_strcat(str, temp_str);
    size = eLIBs_GetVolTSpace(UDISK_NAME);
    gnum = (size * 100) >> 30;//先乘100，用于取.GB 后面两位小数
    snum = (gnum % 100);        //去小数点后面的数值
    gnum = (gnum / 100);        //取小数点前面的数值
    eLIBs_uint2str_dec(gnum, numstr);
    eLIBs_strcat(str, numstr);
    eLIBs_strcat(str, ".");
    eLIBs_uint2str_dec(snum, numstr);

    if (snum >= 10)
    {
        eLIBs_strcat(str, numstr);
    }
    else
    {
        eLIBs_strcat(str, "0");
        eLIBs_strcat(str, numstr);
    }

    eLIBs_strcat(str, "GB");
    eLIBs_strcpy(setting_ctrl->volume_para.total_space_txt, (const char *)str) ;
    eLIBs_memset(str, 0, sizeof(str));
    eLIBs_memset(temp_str, 0, sizeof(temp_str));
    get_menu_text(STRING_SET_TIPS_FSIZE, temp_str, sizeof(temp_str));
    size = eLIBs_GetVolFSpace(UDISK_NAME);
    gnum = (size * 100) >> 30;
    snum = (gnum % 100);
    gnum = (gnum / 100);
    eLIBs_uint2str_dec(gnum, numstr);
    eLIBs_strcat(str, numstr);
    eLIBs_strcat(str, ".");
    eLIBs_uint2str_dec(snum, numstr);

    if (snum >= 10)
    {
        eLIBs_strcat(str, numstr);
    }
    else
    {
        eLIBs_strcat(str, "0");
        eLIBs_strcat(str, numstr);
    }

    eLIBs_strcat(str, "GB");
    eLIBs_strcpy(setting_ctrl->volume_para.free_space_txt, (const char *)str);
}

__s32 _setting_set_hbar_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}

void  main_cmd2product_info(setting_ctrl_t *setting_ctrl, __s32 id)
{
    __gui_msg_t msg;
    eLIBs_memset(&msg, 0x00, sizeof(msg));
    msg.h_deswin = setting_ctrl->h_frm_dafault_dialog_win;
    msg.h_srcwin = NULL;
    msg.id = id;
    GUI_SendMessage(&msg);
}
void  main_cmd2dafault_dialog(setting_ctrl_t *setting_ctrl, __s32 id)
{
    __gui_msg_t msg;
    eLIBs_memset(&msg, 0x00, sizeof(msg));
    msg.h_deswin = setting_ctrl->h_frm_dafault_dialog_win;
    msg.h_srcwin = NULL;
    msg.id = id;
    GUI_SendMessage(&msg);
}


void  main_cmd2power(setting_ctrl_t *setting_ctrl, __s32 id)
{
    __gui_msg_t msg;
    eLIBs_memset(&msg, 0x00, sizeof(msg));
    msg.h_deswin = setting_ctrl->h_frm_power;
    msg.h_srcwin = NULL;
    msg.id = id;
    GUI_SendMessage(&msg);
}

void  main_cmd2parent(H_WIN hwin, __s32 id, unsigned long data1, unsigned long data2)
{
    __gui_msg_t msg;
    eLIBs_memset(&msg, 0x00, sizeof(msg));
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_SendNotifyMessage(&msg);
}

static __s32 _app_setting_Proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            setting_ctrl_t  *setting_ctrl = NULL;
            setting_ctrl = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (setting_ctrl == NULL)
            {
                __err("_app_setting_Proc: %d err", __LINE__);
                return EPDK_FAIL;
            }

#if 1

            if (SETTING_ALARM == setting_ctrl->setting_type)
            {
                setting_alarm_attr_t alarm_para;
                setting_ctrl->lyr_setting = setting_layer_32bpp_create();
                eLIBs_memset(&alarm_para, 0x00, sizeof(alarm_para));
                alarm_para.layer = setting_ctrl->lyr_setting;
                alarm_para.font = setting_ctrl->setting_font;
                setting_ctrl->h_frm_alarm = setting_alarm_win_create(msg->h_deswin, &alarm_para);
                GUI_WinSetFocusChild(setting_ctrl->h_frm_alarm);
            }
            else
            {
                setting_general_para_t general_para;//在通用设置中被 调用
                setting_ctrl->lyr_setting = setting_layer_32bpp_create();
                eLIBs_memset(&general_para, 0x00, sizeof(general_para));
                general_para.focus_id = 0;
                general_para.old_focus = 0;
                general_para.layer = setting_ctrl->lyr_setting;
                general_para.font = setting_ctrl->setting_font;
                setting_ctrl->h_frm_general = setting_general_win_create(msg->h_deswin, &general_para);
                __inf("------setting_ctrl->h_frm_general=%08x", setting_ctrl->h_frm_general);
                GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
            }

#else
            setting_ctrl->h_frm_entrance = setting_entrance_win_create(msg->h_deswin);//byf
            GUI_WinSetFocusChild(setting_ctrl->h_frm_entrance);//byf

            if (SETTING_ALARM == setting_ctrl->setting_type
                && NULL != setting_ctrl->h_frm_entrance)
            {
                __gui_msg_t mymsg;
                eLIBs_memset(&mymsg, 0, sizeof(mymsg));
                mymsg.h_deswin = setting_ctrl->h_frm_entrance;
                mymsg.id = GUI_MSG_COMMAND;
                mymsg.dwAddData1 = SETTING_ENTRANCE_CREATE_ALARM;
                GUI_SendMessage(&mymsg);
            }
            else if (NULL != setting_ctrl->h_frm_entrance)
            {
                __gui_msg_t mymsg;
                eLIBs_memset(&mymsg, 0, sizeof(mymsg));
                mymsg.h_deswin = setting_ctrl->h_frm_entrance;
                mymsg.id = GUI_MSG_COMMAND;
                mymsg.dwAddData1 = SETTING_ENTRANCE_PAINT;
                GUI_SendMessage(&mymsg);
            }

#endif
        }

        return EPDK_OK;

        case DSK_MSG_ALARM:
        {
            setting_ctrl_t  *setting_ctrl;
            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            //__here__ ;
            if (!setting_ctrl)
            {
                __err("setting_ctrl is null ......");
                return EPDK_FAIL;
            }

            /* 为防止图层不够*/
            if (setting_ctrl->h_frm_product_info_win)
            {
                H_LYR tips_win_layer ;
                tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_product_info_win);
                GUI_LyrWinDelete(tips_win_layer);
                setting_ctrl->h_frm_product_info_win = NULL ;
            }

            //__here__ ;

            if (setting_ctrl->h_frm_dafault_dialog_win)
            {
                app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
                setting_ctrl->h_frm_dafault_dialog_win = NULL;
            }

            //__here__ ;
            if (setting_ctrl->h_frm_alarm_select_song)
            {
                app_alarm_sel_song_delete(setting_ctrl->h_frm_alarm_select_song);
                setting_ctrl->h_frm_alarm_select_song = NULL ;
            }

            //__here__ ;

            if (setting_ctrl->h_frm_alarm_clock)
            {
                setting_alarm_clock_frm_destory(setting_ctrl->h_frm_alarm_clock);
                setting_ctrl->h_frm_alarm_clock = NULL ;
            }

            //__here__ ;
            setting_ctrl->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
            //__here__ ;
            return EPDK_OK ;
        }

        case ALARM_MSG_CLOSE:
        {
            setting_ctrl_t  *setting_ctrl;
            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (!setting_ctrl)
            {
                __err("setting_ctrl is null ......");
                return EPDK_FAIL;
            }

            if (setting_ctrl->h_frm_alarm_coming)   // 闹铃应用本身以删除该窗体，只需将句柄置空即可
            {
                setting_ctrl->h_frm_alarm_coming = NULL ;
            }

            GUI_WinSetFocusChild(msg->h_deswin);
            return EPDK_OK ;
        }

        case DSK_MSG_HOME:
        case GUI_MSG_CLOSE:
        {
            __log("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
            main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, SETTING_SW_TO_MAIN, 0);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            setting_ctrl_t  *setting_ctrl = NULL;
            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (NULL == setting_ctrl)
            {
                __err("setting_ctrl is null ......");
                return EPDK_FAIL;
            }

            dsk_reg_flush(); // 30234384

            //__here__
            if (setting_ctrl->h_frm_product_info_win)
            {
                H_LYR tips_win_layer ;
                tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_product_info_win);
                GUI_LyrWinDelete(tips_win_layer);
                setting_ctrl->h_frm_product_info_win = NULL ;
            }

            //__here__ ;

            if (setting_ctrl->h_frm_dafault_dialog_win) //删除默认出厂对话框
            {
                app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
                setting_ctrl->h_frm_dafault_dialog_win = NULL;
            }

            if (setting_ctrl->h_frm_entrance)
            {
                setting_entrance_win_delete(setting_ctrl->h_frm_entrance);
                setting_ctrl->h_frm_entrance = 0;
            }

            if (setting_ctrl->h_frm_alarm_select_song)
            {
                app_alarm_sel_song_delete(setting_ctrl->h_frm_alarm_select_song);
                setting_ctrl->h_frm_alarm_select_song = NULL ;
            }

            if (setting_ctrl->h_frm_alarm_clock)
            {
                setting_alarm_clock_frm_destory(setting_ctrl->h_frm_alarm_clock);
                setting_ctrl->h_frm_alarm_clock = NULL ;
            }

            if (setting_ctrl->h_frm_alarm_coming)
            {
                app_alarm_frm_destory(setting_ctrl->h_frm_alarm_coming);
                setting_ctrl->h_frm_alarm_coming = NULL ;
            }

            if (setting_ctrl->lyr_setting)
            {
                GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                setting_ctrl->lyr_setting = 0;
            }

            My_Mfree(0, setting_ctrl);
            //esKSRV_SysInfo();
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            setting_ctrl_t  *setting_ctrl = NULL;
            setting_ctrl = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (setting_ctrl == NULL)
            {
                __inf("%s %d setting_ctrl NULL", __FILE__, __LINE__);
                return EPDK_FAIL;
            }

            switch (msg->dwAddData1)
            {
                    __inf("--------escape");

                case GUI_MSG_KEY_MENU:
                {
                    if (setting_ctrl->h_frm_dafault_dialog_win) //删除默认出厂对话框
                    {
                        __log("======switch=======h_frm_dafault_dialog_win");
                        app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
                        setting_ctrl->h_frm_dafault_dialog_win = NULL;
                        return EPDK_OK;
                    }

                    if (setting_ctrl->h_frm_product_info_win)
                    {
                        H_LYR tips_win_layer ;
                        tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_product_info_win);
                        GUI_LyrWinDelete(tips_win_layer);
                        setting_ctrl->h_frm_product_info_win = NULL ;
                        return EPDK_OK;
                    }

                    if (setting_ctrl->h_frm_alarm_clock)
                    {
                        setting_alarm_clock_frm_destory(setting_ctrl->h_frm_alarm_clock);
                        setting_ctrl->h_frm_alarm_clock = NULL ;
                        return EPDK_OK;
                    }

                    {
                        __gui_msg_t my_msg;
                        my_msg.id = GUI_MSG_CLOSE;
                        my_msg.h_srcwin = msg->h_deswin;
                        my_msg.h_deswin = msg->h_deswin;
                        my_msg.dwAddData1 = 0;
                        my_msg.dwAddData2 = 0;
                        GUI_SendNotifyMessage(&my_msg);
                    }

                    return EPDK_OK;
                }

                default:
                    break;
            }
        }
        break;

        case DSK_MSG_FS_PART_PLUGIN:
        case DSK_MSG_FS_PART_PLUGOUT:
        {
            setting_ctrl_t *setting_ctrl;
            setting_ctrl = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            //__here__;
            if (setting_ctrl->h_frm_product_info_win)
            {
                __gui_msg_t set_msg;
                eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
                set_msg.h_deswin = setting_ctrl->h_frm_product_info_win;
                //__here__;
                GUI_SendMessage(&set_msg);
                //__here__;
            }

            if (setting_ctrl->h_frm_dafault_dialog_win)
            {
                app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
                setting_ctrl->h_frm_dafault_dialog_win = NULL;
            }

            if (setting_ctrl->h_frm_entrance || (NULL == setting_ctrl->h_frm_handle))
            {
                __gui_msg_t set_msg;
                eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));
                set_msg.id = GUI_MSG_COMMAND;
                set_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                set_msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), SWITCH_TO_OTHER_APP);
                set_msg.dwAddData2 = SETTING_SW_TO_PROMPT_UPGRADE;
                GUI_SendNotifyMessage(&set_msg);
            }
            else if (setting_ctrl->h_frm_handle)
            {
                break;//让下handle frm win自己处理
            }

            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            setting_ctrl_t *setting_ctrl = NULL;
            setting_ctrl = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (setting_ctrl == NULL)
            {
                __err("%s %d setting_ctrl = NULL", __FILE__, __LINE__);
                return EPDK_FAIL;
            }

            switch (LOWORD(msg->dwAddData1))
            {
                case ENTRANCE_MENU_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case SETTING_ENTER_GENERAL:
                        {
                            setting_general_para_t general_para;
                            setting_ctrl->lyr_setting = setting_layer_32bpp_create();
                            general_para.layer = setting_ctrl->lyr_setting;
                            general_para.font = setting_ctrl->setting_font;
                            general_para.focus_id = 0;
                            general_para.old_focus = 0;
                            setting_ctrl->h_frm_general = setting_general_win_create(msg->h_deswin, &general_para);
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                        }
                        break;

                        case SETTING_ENTER_POWER_MANAGEMENT:
                        {
                            setting_power_para_t power_para;
                            setting_ctrl->lyr_setting = setting_layer_32bpp_create();
                            power_para.layer = setting_ctrl->lyr_setting;
                            power_para.font = setting_ctrl->setting_font;
                            power_para.focus_id = 0;
                            power_para.old_focus = 0;
                            setting_ctrl->h_frm_power = setting_power_win_create(msg->h_deswin, &power_para);
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_power);
                        }
                        break;

                        case SETTING_ENTER_ALARM:
                        {
                            setting_alarm_attr_t alarm_para;
                            setting_ctrl->lyr_setting = setting_layer_32bpp_create();
                            alarm_para.layer = setting_ctrl->lyr_setting;
                            alarm_para.font = setting_ctrl->setting_font;
                            setting_ctrl->h_frm_alarm = setting_alarm_win_create(msg->h_deswin, &alarm_para);
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_alarm);
                        }
                        break;

                        /**case SETTING_ENTER_HANDLE_ADJUST:
                        {
                            setting_handle_para_t handle_para;
                            setting_ctrl->lyr_setting = setting_layer_32bpp_create();

                            handle_para.focus_id = 0;
                            handle_para.old_focus = 0;
                            handle_para.layer = setting_ctrl->lyr_setting;
                            handle_para.font = setting_ctrl->setting_font;
                            handle_para.call_status = 0;

                            setting_ctrl->h_frm_handle = setting_handle_win_create(msg->h_deswin, &handle_para);
                            if(NULL != setting_ctrl->h_frm_handle)
                            {
                                __msg("handle adjust ..................");
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_handle);
                            }
                            else
                            {
                                __msg("no handle...................");
                                if(setting_ctrl->lyr_setting)
                                {
                                    GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                                    setting_ctrl->lyr_setting = 0;
                                }
                                GUI_LyrWinSetSta(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance),GUI_LYR_STA_ON);
                                GUI_LyrWinSetTop(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance));
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_entrance);

                            }

                        }
                            break;  **/
                        default:
                            break;
                    }
                }
                return EPDK_OK;
                case ALARM_CLOCK_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ID_SWITCH_TO_ALARM_VIEW:
                        {
                            setting_ctrl_t  *setting_ctrl = NULL;
                            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                            if (!setting_ctrl)
                            {
                                __msg("..........setting_ctrl is null..........................");
                                return EPDK_FAIL;
                            }

                            {
                                __gui_msg_t sendmsg ;
                                sendmsg.id = ALARM_CLOCK_MSG ;
                                sendmsg.h_deswin = setting_ctrl->h_frm_alarm ;
                                GUI_SendMessage(&sendmsg);
                            }

                            if (setting_ctrl->h_frm_alarm_clock)
                            {
                                setting_alarm_clock_frm_destory(setting_ctrl->h_frm_alarm_clock);
                                setting_ctrl->h_frm_alarm_clock = NULL ;
                            }

                            if (setting_ctrl->lyr_setting)
                            {
                                GUI_LyrWinSetSta(setting_ctrl->lyr_setting, GUI_LYR_STA_ON);
                            }

                            GUI_WinSetFocusChild(setting_ctrl->h_frm_alarm);
                        }
                        break;

                        default:
                            break;
                    }
                }

                return EPDK_OK;

                case ALARM_SELECT_SONG_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ID_SWITCH_TO_ALARM_VIEW:
                        {
                            setting_ctrl_t  *setting_ctrl = NULL;
                            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                            if (setting_ctrl == NULL)
                            {
                                __msg("..........setting_ctrl is null..........................");
                                return EPDK_FAIL;
                            }

                            {
                                char *p = NULL;
                                __gui_msg_t sendmsg ;
                                eLIBs_memset(&sendmsg, 0x00, sizeof(sendmsg));
                                sendmsg.id = ALARM_MUSIC_PATH_MSG ;
                                sendmsg.h_deswin = setting_ctrl->h_frm_alarm ;
                                sendmsg.dwReserved = msg->dwReserved ;
                                p = (char *)msg->dwReserved ;
                                __msg("music name = %s", p);

                                if (msg->dwReserved)
                                {
                                    //__here__ ;
                                    GUI_SendMessage(&sendmsg);
                                }
                            }

                            if (setting_ctrl->h_frm_alarm_select_song)
                            {
                                app_alarm_sel_song_delete(setting_ctrl->h_frm_alarm_select_song);
                                setting_ctrl->h_frm_alarm_select_song = NULL ;
                            }

                            if (setting_ctrl->lyr_setting)
                            {
                                GUI_LyrWinSetSta(setting_ctrl->lyr_setting, GUI_LYR_STA_ON);
                            }

                            GUI_WinSetFocusChild(setting_ctrl->h_frm_alarm);
                        }
                        break;

                        default:
                            break;
                    }
                }

                return EPDK_OK;

                case ALARM_MENU_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ID_SWITCH_TO_ENTRANCE:
                        {
                            setting_ctrl_t  *setting_ctrl;
                            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                            if (!setting_ctrl)
                            {
                                __msg("..........setting_ctrl is null..........................");
                                return EPDK_FAIL;
                            }

                            if (SETTING_ALARM == setting_ctrl->setting_type) //如果是独立的alarm应用，则直接退回到主界面
                            {
                                __gui_msg_t my_msg;
                                my_msg.id = GUI_MSG_CLOSE;
                                my_msg.h_srcwin = msg->h_deswin;
                                my_msg.h_deswin = msg->h_deswin;
                                my_msg.dwAddData1 = 0;
                                my_msg.dwAddData2 = 0;
                                GUI_SendNotifyMessage(&my_msg);
                            }
                            else
                            {
                                if (setting_ctrl->lyr_setting)
                                {
                                    GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                                    setting_ctrl->lyr_setting = 0;
                                }

                                GUI_LyrWinSetSta(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance), GUI_LYR_STA_ON);
                                GUI_LyrWinSetTop(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance));
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_entrance);
                            }
                        }
                        break;

                        case ID_SWITCH_TO_ALARM_CLOCK_WIN:
                        {
                            setting_ctrl_t  *setting_ctrl;
                            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                            if (!setting_ctrl)
                            {
                                __msg("..........setting_ctrl is null..........................");
                                return EPDK_FAIL;
                            }

                            setting_ctrl->h_frm_alarm_clock = setting_alarm_clock_frm_create(msg->h_deswin, ALARM_CLOCK_ID, (reg_alarm_para_t *)msg->dwAddData2);
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_alarm_clock);
                            break ;
                        }
                        break;

                        case ID_SWITCH_TO_ALARM_SELECT_SONG_WIN:
                        {
                            setting_ctrl_t  *setting_ctrl = NULL;
                            __u32   disk_num ;
                            setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                            if (!setting_ctrl)
                            {
                                __msg("..........setting_ctrl is null..........................");
                                return EPDK_FAIL;
                            }

                            app_alarm_sel_song_get_root_type(&disk_num);

                            if (0 == disk_num)
                            {
                                __s32 lang_id[] = { STRING_SET_TIPS, STRING_SET_NULLDISK};
                                default_dialog(setting_ctrl->h_frm_dafault_dialog_win, msg->h_deswin, ALARM_NULL_DISK_TIPS_WIN_ID, ADLG_OK, lang_id);
                                __msg("there is null disk ");
                            }
                            else
                            {
                                setting_ctrl->h_frm_alarm_select_song = app_alarm_sel_song_create(msg->h_deswin, ALARM_SELECT_SONG_ID);
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_alarm_select_song);
                            }

                            break ;
                        }

                        default:
                            break;
                    }

                    return EPDK_OK;
                }

                case GENERAL_MENU_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ID_OP_LEFT:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_LEFT);
                        }
                        break;

                        case ID_OP_RIGHT:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_RIGHT);
                        }
                        break;

                        case ID_OP_UP:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_UP);
                        }
                        break;

                        case ID_OP_DOWN:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_DOWN);
                        }
                        break;

                        case ID_OP_ENTER:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_ENTER);
                        }
                        break;

                        case ID_OP_EXIT:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_EXIT);
                        }
                        break;

                        case ID_SWITCH_TO_TIPS:
                        {
                            if (msg->dwAddData2 == TIPS_FACTORY_DEFAULT)
                            {
                                __s32 lang_id[] = { STRING_SET_TIPS, STRING_SET_COMMON_RESTORE };
                                default_dialog(setting_ctrl->h_frm_dafault_dialog_win, msg->h_deswin, GENERAL_FACTORY_DEFAULT_TIPS_ID, ADLG_YESNO, lang_id) ;
                            }
                            else
                            {
                                setting_tip_para_t setting_para ;
                                eLIBs_memset(&setting_para, 0x00, sizeof(setting_para));
                                setting_para.layer = setting_tips_layer_create();
                                setting_para.font = setting_ctrl->setting_font ;
                                setting_ctrl->h_frm_product_info_win = setting_tips_win_create(msg->h_deswin, &setting_para);
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_product_info_win);
                            }
                        }
                        break;

                        case ID_SWITCH_TO_ADJUST:
                        {
                            H_WIN   scene_adjust = NULL;
                            scene_adjust = scene_adjust_create(msg->h_deswin);
                            scene_adjust_destroy(scene_adjust);
                        }
                        break;

                        case ID_SWITCH_POWER_MANER:
                        {
                            setting_power_para_t power_para;
                            eLIBs_memset(&power_para, 0x00, sizeof(power_para));
                            power_para.layer = setting_ctrl->lyr_setting;
                            power_para.font = setting_ctrl->setting_font;
                            power_para.focus_id = 0;
                            power_para.old_focus = 0;
                            setting_ctrl->h_frm_power = setting_power_win_create(msg->h_deswin, &power_para);
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_power);
                        }
                        break;

                        case ID_SWITCH_TO_ENTRANCE:
                        {
                            if (setting_ctrl->lyr_setting)
                            {
                                GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                                setting_ctrl->lyr_setting = 0;
                            }

                            GUI_LyrWinSetSta(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance), GUI_LYR_STA_ON);
                            GUI_LyrWinSetTop(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance));
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_entrance);
                        }
                        break;
#if (BEETLES_GAME_FLAG==1)

                        case ID_SWITCH_HANDLE_ADJUST:
                        {
                            setting_handle_para_t handle_para;
                            eLIBs_memset(&handle_para, 0x00, sizeof(handle_para));
                            //setting_ctrl->lyr_setting = setting_layer_32bpp_create();
                            handle_para.focus_id = 0;
                            handle_para.old_focus = 0;
                            handle_para.layer = setting_ctrl->lyr_setting;
                            handle_para.font = setting_ctrl->setting_font;
                            handle_para.call_status = 0;
                            setting_ctrl->h_frm_handle = setting_handle_win_create(msg->h_deswin, &handle_para);

                            if (NULL != setting_ctrl->h_frm_handle)
                            {
                                __msg("handle adjust ..................");
                                //..GUI_FrmWinDelete(setting_ctrl->h_frm_general);//..
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_handle);
                                GUI_FrmWinDelete(setting_ctrl->h_frm_general);
                            }
                            else
                            {
                                /*__msg("no handle...................");
                                if(setting_ctrl->lyr_setting)
                                {
                                    GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                                    setting_ctrl->lyr_setting = 0;
                                }
                                __wrn("------setting_ctrl->h_frm_entrance=%08x",setting_ctrl->h_frm_entrance);
                                GUI_LyrWinSetSta(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance),GUI_LYR_STA_ON);
                                GUI_LyrWinSetTop(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance));
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_entrance);*/
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                            }
                        }
                        break;
#endif

                        default:
                            break;
                    }
                }

                return EPDK_OK;

                case POWER_MANAGE_MENU_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ID_OP_LEFT:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_LEFT);
                        }
                        break;

                        case ID_OP_RIGHT:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_RIGHT);
                        }
                        break;

                        case ID_OP_UP:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_UP);
                        }
                        break;

                        case ID_OP_DOWN:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_DOWN);
                        }
                        break;

                        case ID_OP_ENTER:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_ENTER);
                        }
                        break;

                        case ID_OP_EXIT:
                        {
                            main_cmd2power(setting_ctrl, MSG_OP_EXIT);
                        }
                        break;

                        case ID_SWITCH_GENERAL_SETTING:
                        {
                            setting_general_para_t general_para;
                            eLIBs_memset(&general_para, 0x00, sizeof(general_para));
                            general_para.layer = setting_ctrl->lyr_setting;
                            general_para.font = setting_ctrl->setting_font;
                            general_para.focus_id = 0;
                            general_para.old_focus = 0;
                            setting_ctrl->h_frm_general = setting_general_win_create(msg->h_deswin, &general_para);
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                        }
                        break;

                        case ID_SWITCH_TO_ENTRANCE:
                        {
                            if (setting_ctrl->lyr_setting)
                            {
                                GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                                setting_ctrl->lyr_setting = 0;
                            }

                            GUI_LyrWinSetSta(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance), GUI_LYR_STA_ON);
                            GUI_LyrWinSetTop(GUI_WinGetLyrWin(setting_ctrl->h_frm_entrance));
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_entrance);
                        }
                        break;

                        default:
                            break;
                    }
                }

                return EPDK_OK;

                case ALARM_NULL_DISK_TIPS_WIN_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ADLG_CMD_EXIT:
                        {
                            if (setting_ctrl->h_frm_dafault_dialog_win)
                            {
                                app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
                                setting_ctrl->h_frm_dafault_dialog_win = NULL;
                            }

                            return EPDK_OK;
                        }

                        default:
                            break;
                    }
                }
                break;

                case GENERAL_FACTORY_DEFAULT_TIPS_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ADLG_CMD_EXIT:
                        {
                            if (ADLG_IDYES == msg->dwAddData2)  //恢复出厂设置
                            {
                                __gui_msg_t mymsg;
                                __msg("***********restore factory setting*******");
                                mymsg.h_deswin = setting_ctrl->h_frm_general;
                                mymsg.id = MSG_RESTORE_FACTORY;
                                GUI_SendMessage(&mymsg);
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                            }
                            else if (ADLG_IDNO == msg->dwAddData2)  // 没有选择恢复出厂设置
                            {
                                GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                            }

                            if (setting_ctrl->h_frm_dafault_dialog_win)
                            {
                                app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
                                setting_ctrl->h_frm_dafault_dialog_win = NULL;
                            }
                        }

                        return EPDK_OK;

                        case ID_OP_LEFT:
                        {
                            main_cmd2dafault_dialog(setting_ctrl, MSG_OP_LEFT);
                        }
                        break;

                        case ID_OP_RIGHT:
                        {
                            main_cmd2dafault_dialog(setting_ctrl, MSG_OP_RIGHT);
                        }
                        break;

                        case ID_OP_UP:
                        {
                            main_cmd2dafault_dialog(setting_ctrl, MSG_OP_UP);
                        }
                        break;

                        case ID_OP_DOWN:
                        {
                            main_cmd2dafault_dialog(setting_ctrl, MSG_OP_DOWN);
                        }
                        break;

                        case ID_OP_ENTER:
                        {
                            main_cmd2dafault_dialog(setting_ctrl, MSG_OP_ENTER);
                        }
                        break;

                        case ID_OP_EXIT:
                        {
                            main_cmd2dafault_dialog(setting_ctrl, MSG_OP_EXIT);
                        }
                        break;

                        default:
                            break;
                    }
                }

                return EPDK_OK;

                case GENERAL_PRODUCT_INFO_TIPS_ID:
                {
                    switch (HIWORD(msg->dwAddData1))
                    {
                        case ID_OP_LEFT:
                        {
                            main_cmd2product_info(setting_ctrl, MSG_OP_LEFT);
                        }
                        break;

                        case ID_OP_RIGHT:
                        {
                            main_cmd2product_info(setting_ctrl, MSG_OP_RIGHT);
                        }
                        break;

                        case ID_OP_UP:
                        {
                            main_cmd2product_info(setting_ctrl, MSG_OP_UP);
                        }
                        break;

                        case ID_OP_DOWN:
                        {
                            main_cmd2product_info(setting_ctrl, MSG_OP_DOWN);
                        }
                        break;

                        case ID_OP_ENTER:
                        {
                            main_cmd2product_info(setting_ctrl, MSG_OP_ENTER);
                        }
                        break;

                        case ID_OP_EXIT:
                        {
                            main_cmd2product_info(setting_ctrl, MSG_OP_EXIT);
                        }
                        break;

                        case ID_SWITCH_TO_GENERAL:
                        {
                            H_LYR tips_win_layer;

                            if (setting_ctrl == NULL)
                            {
                                __msg("setting_ctrl error.........");
                                return EPDK_FAIL;
                            }

                            tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_product_info_win);
                            GUI_LyrWinDelete(tips_win_layer);
                            setting_ctrl->h_frm_product_info_win = NULL ;
                            GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                            break ;
                        }

                        default:
                            break;
                    }
                }

                return EPDK_OK;

                case HANDLE_MENU_ID:
                {
#if 0
                    setting_ctrl_t  *setting_ctrl;
                    setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                    if (!setting_ctrl)
                    {
                        __msg("..........setting_ctrl is null..........................");
                        return EPDK_FAIL;
                    }

                    /*if(setting_ctrl->lyr_setting)
                    {
                    GUI_LyrWinDelete(setting_ctrl->lyr_setting);
                    setting_ctrl->lyr_setting = 0;
                    }*/
                    //GUI_LyrWinSetSta(GUI_WinGetLyrWin(setting_ctrl->h_frm_general),GUI_LYR_STA_ON);
                    GUI_LyrWinSetTop(GUI_WinGetLyrWin(setting_ctrl->h_frm_general));
                    GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                    return EPDK_OK;
#endif
                    setting_ctrl_t    *setting_ctrl = NULL;
                    setting_general_para_t general_para;
                    setting_ctrl     = (setting_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                    general_para.layer = setting_ctrl->lyr_setting;
                    general_para.font = setting_ctrl->setting_font;
                    general_para.focus_id = 0;
                    general_para.old_focus = 0;
                    __wrn(" ------  handle_id  general_win create ");
                    setting_ctrl->h_frm_general = setting_general_win_create(msg->h_deswin, &general_para);
                    GUI_WinSetFocusChild(setting_ctrl->h_frm_general);
                }

                default:
                    break;
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN app_setting_create(root_para_t *para)
{
    __gui_manwincreate_para_t create_info;
    setting_ctrl_t *setting_ctrl = NULL;
    __inf("****************************************************************************************");
    __inf("********  enter setting app  **************");
    __inf("****************************************************************************************");
    __inf("----------setting create----------");
    SETTING_DISPLAY_FOCUS = SETTING_DISPLAY_FOCUS_RIGHT;//焦点在右边
    setting_ctrl = (setting_ctrl_t *)My_Malloc(0, sizeof(setting_ctrl_t));

    if (NULL == setting_ctrl)
    {
        __msg("memory alloc fail.........");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset(setting_ctrl, 0, sizeof(setting_ctrl_t));
    GUI_Memdev_FrameBuffer_Release(1);
    setting_ctrl->setting_font = para->font;
    setting_ctrl->setting_type = para->setting_opt;//__u32 setting_opt;//为0代表通用设置，为1代表电源管理
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.id          = APP_SETTING_ID;
    create_info.name            = APP_SETTING;
    create_info.hParent         = para->h_parent;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_setting_Proc);
    create_info.attr            = (void *)setting_ctrl;
    return (GUI_ManWinCreate(&create_info));
}

__s32 app_setting_notify_delete_sub_dlg_scene(H_WIN hmanwin)
{
    setting_ctrl_t  *setting_ctrl = NULL;
    setting_ctrl = (setting_ctrl_t *)GUI_WinGetAttr(hmanwin);

    //__here__ ;

    if (NULL == setting_ctrl)
    {
        __wrn("setting_ctrl is null ......");
        return EPDK_FAIL;
    }

    /* 为防止图层不够*/
    if (setting_ctrl->h_frm_product_info_win)
    {
        H_LYR tips_win_layer ;
        tips_win_layer = GUI_WinGetLyrWin(setting_ctrl->h_frm_product_info_win);
        GUI_LyrWinDelete(tips_win_layer);
        setting_ctrl->h_frm_product_info_win = NULL ;
    }

    //__here__ ;

    if (setting_ctrl->h_frm_dafault_dialog_win)
    {
        app_dialog_destroy(setting_ctrl->h_frm_dafault_dialog_win);
        setting_ctrl->h_frm_dafault_dialog_win = NULL;
    }

    //__here__ ;
    if (setting_ctrl->h_frm_alarm_select_song)
    {
        app_alarm_sel_song_delete(setting_ctrl->h_frm_alarm_select_song);
        setting_ctrl->h_frm_alarm_select_song = NULL ;
    }

    //__here__ ;

    if (setting_ctrl->h_frm_alarm_clock)
    {
        setting_alarm_clock_frm_destory(setting_ctrl->h_frm_alarm_clock);
        setting_ctrl->h_frm_alarm_clock = NULL ;
    }

    return EPDK_OK;
}
