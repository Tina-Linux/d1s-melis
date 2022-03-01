/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, SoftWinners Microelectronic Co., Ltd.
*                                           All Rights Reserved
*
* File    : dtv.c
* By      : lihaoyi
* Version : V1.00
*********************************************************************************************************
*/

/*
******************************************************************************************************
*                                       app_dtv_create  播放器入口
*
*Description: dtv entry functoin.
*
*Arguments  : *arg - user define
*
*Return     : =
*             =
*******************************************************************************************************
*/

/*
//界面,除信号状态外，其创建都在GUI_MSG_KEY中处理
*1、频道预览
*2、节目预告
*3、设置菜单
*4、搜索进度
*5、信号状态

* 1~4界面由同一图层进行管理，方便各界面之间的切换

//使用此播放器，需要配置如下信息:
*1、MAPLE_TV_STANDAR = MAPLE_TV_DTMB or MAPLE_TV_ISDB_T ..... //播放CMMB或ISDBT
*2、IS_CA_EXIST  = 1 or 0 是否存在加密CA模块,测试天线灵敏度时必须是1
*3、G_DTV_current_area ＝ 0 or 1 ?  0:只搜索深圳频点1:全国频点搜索,0只做为开发人员测试用
*4、ISDBT字幕必须配置g_type, 现默认巴西
*/

//注意:比较老的SDK用内存设备会有问题,导致重启，请屏蔽内存设备
//eg:com_memdev_create \ GUI_MEMDEV_Create ......
//如果CA加密模块不存在，设置中CA的序列号请改为其它信息

//节目预告信息从频道开始播放到解释出ESG信息需要3秒钟，再加上
//定时器的1秒，大概要4秒左右才出显示出来，所以如果觉得慢，
//可以把节目预告放到设置中，这样从播放成功到进到设置，到选择查看节目预告
//基本上就4秒了(用户可自己修改，公版不做此改动，请悉知!!!)


//如果电影音乐有断点记忆，在保存前，需要把CMMB存台的文件读出重新保存
//因为注册表在保存时会把NOR FLASH扇区清除


//重要: 进入CMMB/ISDBT之前请务必把主界面删除，释放内存
//因为CMMB/ISDBT解码需要用到大块连续内存，如果内存不足
//可能会莫名死机!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


#include <log.h>
#include "dtv_i.h"
#include "beetles_app.h"
#include <emodules/mod_ca.h>
#include "dtv_setting_ca.h"
#include "dtv_func_interface.h"
#include "dtv_mid.h"

#include <emodules/mod_fm.h>
#define TIME_NUM            (5)

extern void  main_cmd2parent(H_WIN hwin, __s32 id, __u32 data1, __u32 data2);



__s32 open_back_boor = EPDK_FALSE;
static __s32 key_code = -1;

static __u32 id_battery_bmp[MAX_CHARGE_LEVEL] =
{
    ID_INIT_ENERGY_0_BMP,
    ID_INIT_ENERGY_1_BMP,
    ID_INIT_ENERGY_2_BMP,
    ID_INIT_ENERGY_3_BMP,
    ID_INIT_ENERGY_4_BMP,
    ID_INIT_ENERGY_5_BMP
};


static __mp  *fm_drv = NULL;
extern maple_serv_list_t *search_service_list;
extern __mp *maple;

__s32 dsk_radio_open_i(void)
{
    /*加载FM驱动*/
    esDEV_Plugin("\\drv\\fm.drv", 0, 0, 1);
    //__here__;
    /*打开FM驱动*/
#if 0/*langaojie temp test*/
    fm_drv = eLIBs_fopen("b:\\USER\\FM", "r+");
#else
    fm_drv = NULL;
#endif

    //__here__;
    if (fm_drv == NULL)
    {
        __wrn(" Error: open fm driver failed! ");
        return EPDK_FAIL;
    }

    esMODS_MIoctrl(fm_drv, DRV_FM_CMD_INIT, 0, 0);
    return EPDK_OK;
}


__s32 dsk_radio_close_i(void)
{
    __s32 result = 0;

    if (fm_drv != NULL)
    {
        esMODS_MIoctrl(fm_drv, DRV_FM_CMD_SEND_EXIT, 0, 0);
        esMODS_MIoctrl(fm_drv, DRV_FM_CMD_RECVE_EXIT, 0, 0);
        /*退出FM驱动*/
        esMODS_MIoctrl(fm_drv, DRV_FM_CMD_EXIT, 0, 0);
        /*关闭FM驱动*/
#if 0/*langaojie temp test*/
        //eLIBs_fclose(fm_drv);
#endif
    }

    /*卸载FM驱动*/
    esDEV_Plugout("\\drv\\fm.drv", 0);
    return result;
}

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
static H_LYR h_preAuth_lyr = NULL;
void dtv_show_preAuth_dialog(void)
{
    GUI_RECT gui_rect;
    char text[128];
    RECT rect;
    __wrn("dtv_show_preAuth_dialog : %08x", g_dtv_mainwin);

    if (g_dtv_mainwin)
    {
        if (h_preAuth_lyr == NULL)
        {
            rect.x = 20;
            rect.y = SCREEN_HEIGHT - 62;
            rect.width = 60;
            rect.height = 30;
            h_preAuth_lyr = dtv_com_layer_create(&rect, "preAhtu tips");
            __wrn("h_preAuth_lyr : %08x", h_preAuth_lyr);
            GUI_LyrWinSetSta(h_preAuth_lyr, GUI_LYRWIN_STA_OFF);
            GUI_LyrWinSetTop(h_preAuth_lyr);
            GUI_LyrWinSel(h_preAuth_lyr);
            GUI_UC_SetEncodeUTF8();
            GUI_SetFont(SWFFont);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            gui_rect.x0 = 0;
            gui_rect.y0 = 0;
            gui_rect.x1 = 59;
            gui_rect.y1 = 29;
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_SetColor(GUI_WHITE);
            dsk_langres_get_menu_text(STRING_DTV_CA_PRE_AUTH, text, sizeof(text));
            GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            GUI_LyrWinSetSta(h_preAuth_lyr, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(h_preAuth_lyr);
        }

        if (!GUI_IsTimerInstalled(g_dtv_mainwin, DTMB_CA_PREAUTH_TIMER_ID))
        {
            GUI_SetTimer(g_dtv_mainwin, DTMB_CA_PREAUTH_TIMER_ID, 500, NULL);
        }
        else
        {
            GUI_ResetTimer(g_dtv_mainwin, DTMB_CA_PREAUTH_TIMER_ID, 500, NULL);
        }
    }
}

static void dtv_show_preAuth_dialog_destroy(void)
{
    __wrn("dtv_show_preAuth_dialog_destroy : %08x", h_preAuth_lyr);

    if (h_preAuth_lyr)
    {
        GUI_LyrWinDelete(h_preAuth_lyr);
        h_preAuth_lyr = NULL;
    }
}

static H_LYR h_cardId_lyr = NULL;
extern __u8 dtvCardIdTxt[];
void dtv_show_cardId_dialog(void)
{
    GUI_RECT gui_rect;
    RECT rect;
    __wrn("dtv_show_cardId_dialog : %08x", g_dtv_mainwin);

    if (g_dtv_mainwin)
    {
        if (h_cardId_lyr == NULL)
        {
            rect.x = SCREEN_WIDTH - 150;
            rect.y = SCREEN_HEIGHT - 62;
            rect.width = 150;
            rect.height = 30;
            h_cardId_lyr = dtv_com_layer_create(&rect, "card id");
            __wrn("h_cardId_lyr : %08x", h_cardId_lyr);
            GUI_LyrWinSetSta(h_cardId_lyr, GUI_LYRWIN_STA_OFF);
            GUI_LyrWinSetTop(h_cardId_lyr);
            GUI_LyrWinSel(h_cardId_lyr);
            GUI_UC_SetEncodeUTF8();
            GUI_SetFont(SWFFont);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            gui_rect.x0 = 0;
            gui_rect.y0 = 0;
            gui_rect.x1 = rect.width - 1;
            gui_rect.y1 = rect.height - 1;
            GUI_SetColor(GUI_BLACK);
            GUI_FillRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_SetColor(GUI_WHITE);
            GUI_DispStringInRect(dtvCardIdTxt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            GUI_LyrWinSetSta(h_cardId_lyr, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(h_cardId_lyr);
        }

        if (!GUI_IsTimerInstalled(g_dtv_mainwin, DTMB_CA_CARDID_TIMER_ID))
        {
            GUI_SetTimer(g_dtv_mainwin, DTMB_CA_CARDID_TIMER_ID, 18000, NULL);
        }
        else
        {
            GUI_ResetTimer(g_dtv_mainwin, DTMB_CA_CARDID_TIMER_ID, 18000, NULL);
        }
    }
}

static void dtv_show_cardId_dialog_destroy(void)
{
    __wrn("dtv_show_preAuth_dialog_destroy : %08x", h_cardId_lyr);

    if (h_cardId_lyr)
    {
        GUI_LyrWinDelete(h_cardId_lyr);
        h_cardId_lyr = NULL;
    }
}

#endif  // #if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
////dtv_key_proc
//如果当前处于搜索界面，除返回键外不响应任何其它按键
static __s32 dtv_key_proc(__gui_msg_t *msg)
{
    static __s8 last_key = -1;
    //__log("%s %d msg->dwAddData1 = %d, msg->dwAddData2 = %d", __FILE__, __LINE__, msg->dwAddData1, msg->dwAddData2);
    //__log("%s %d ---key action: last_key=%d msg->dwAddData2=%d ",  __FILE__, __LINE__, last_key, msg->dwAddData2);

    if (last_key == -1 && KEY_DOWN_ACTION == msg->dwAddData2)
    {
        last_key = msg->dwAddData1;

        //只在播放的时候响应数字键
        if (((((!dtv_ctr.h_scan)    && (!dtv_has_sub_scene())) || dtv_ctr.sel_sta))
            && (msg->dwAddData1 >= GUI_MSG_KEY_NUM0)
            && (msg->dwAddData1 <= GUI_MSG_KEY_NUM9) && 0 < maple_get_cur_service_list_num())
        {
            if (dtv_ctr.sel_sta == 0)
            {
                dtv_ctr.sel_num = 0;
                dtv_ctr.sel_cnt = 0;
                dtv_ctr.sel_sta = 1;
            }

            if (dtv_ctr.h_no_signal)
            {
                app_dialog_destroy(dtv_ctr.h_no_signal);
                dtv_ctr.h_no_signal = NULL;
            }

            if (dtv_ctr.h_no_auth)
            {
                app_dialog_destroy(dtv_ctr.h_no_auth);
                dtv_ctr.h_no_auth = NULL;
            }

            if (dtv_ctr.h_radio_tip)
            {
                app_dialog_destroy(dtv_ctr.h_radio_tip);
                dtv_ctr.h_radio_tip = NULL;
            }

            if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_LOADING_TIMER_ID))
            {
                dtv_loading_hide();
            }

            //数字选台时，重置定时器
            {
                __msg("input number => %d", (msg->dwAddData1 - GUI_MSG_KEY_NUM0));
                dtv_ctr.sel_num = dtv_ctr.sel_num % 1000;
                dtv_ctr.sel_num = dtv_ctr.sel_num * 10 + (msg->dwAddData1 - GUI_MSG_KEY_NUM0); //台号

                if ((++dtv_ctr.sel_cnt) > 3)
                {
                    dtv_ctr.sel_cnt = 3;
                }

                __msg("sel_cnt => %d", dtv_ctr.sel_cnt);
                dtv_show_select_number(msg);

                if (!GUI_IsTimerInstalled(msg->h_deswin, DVB_SPIC_CHANNEL_ID))
                {
                    GUI_SetTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID, KEY_NUM_DELAY, NULL);
                }
                else
                {
                    GUI_ResetTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID, KEY_NUM_DELAY, NULL);
                }
            }
            return EPDK_OK;
        }

        key_code = msg->dwAddData1;

        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_SCAN://搜台
                //case GUI_MSG_KEY_UP:
            {
                if (!dtv_ctr.h_scan)
                {
                    //maple_epg_search_stop();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)\
||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                    //dtv_set_cur_user_index(0);//isdbt默认播放第一个频点的第一个台
                    dtv_subtitle_proc();
#endif
                    dtv_delete_all_menu(0);
                    maple_epg_search_stop();
                    maple_search_stop();
                    maple_stop_program();
#ifdef DTV_AUTO_SEARCH_ENABLE
                    dtv_search_program_start(msg, AUTO_SERACH_TYPE, 0);// 自动搜索
#else
                    dtv_search_program_start(msg, ALL_FREQ_SERACH_TYPE, 0);
#endif
                }
                else
                {
                    __msg("searching .....");
                }

                return EPDK_OK;
                //break; //unreachable
            }

            case GUI_MSG_KEY_EPG:
                /*********************************************************************************
                                //epg的信息获取大概需要4秒，不宜单独做快捷键
                                //最好放到设置菜单中，做为设置菜单的一个选项
                                //放在这里是后期才发现不太合适，
                                //因为节目播放后如果马上按EPG查看节目预告，
                                //将不会马上显示，从感觉上给的人感觉慢了
                *********************************************************************************/
            {
                //搜台时没法显示EPG，已经显示EPG时没必要再重新显示
                if ((!dtv_ctr.h_scan) /* && service_list->servNum > 0 */
                    && !dtv_ctr.h_epg && 0 < maple_get_cur_service_list_num())
                {
                    maple_epg_getdata();
                    clear_dtv_select_number(msg);
                    dtv_delete_all_menu(0);
                    dtv_schedule_list_menu_create(msg);
                    return EPDK_OK;
                }
                else
                {
                    __msg("...............");
                }

                return EPDK_FAIL;
            }

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_PREV:
            {
                g_is_programme_list_selected = 1;

                if ((!dtv_ctr.h_scan) && !dtv_has_sub_scene() && 0 < maple_get_cur_service_list_num()) //不存在才响应上下换台键
                {
                    __s32 index ;
                    clear_dtv_select_number(msg);
                    index = dtv_get_cur_user_index();
                    index--;

                    if (index < 0)
                    {
                        index = maple_get_cur_service_list_num() - 1;

                        if (index < 0)
                        {
                            index = 0;
                        }
                    }

                    dtv_set_cur_user_index(index);
                    __wrn("maple_stop_program ....");

                    if (dtv_ca_msg_is_exist())
                    {
                        dtv_ca_msg_off();
                        dtv_ca_msg_clear();
                    }

                    if (dtv_ctr.h_no_signal)
                    {
                        app_dialog_destroy(dtv_ctr.h_no_signal);
                        dtv_ctr.h_no_signal = NULL;
                    }

                    if (dtv_ctr.h_no_auth)
                    {
                        app_dialog_destroy(dtv_ctr.h_no_auth);
                        dtv_ctr.h_no_auth = NULL;
                    }

                    if (dtv_ctr.h_radio_tip)
                    {
                        app_dialog_destroy(dtv_ctr.h_radio_tip);
                        dtv_ctr.h_radio_tip = NULL;
                    }

                    maple_stop_program();
                    maple_play_program();
                    check_dtv_or_radio();
                    return EPDK_OK;
                }

                if (dtv_ctr.h_no_channel != NULL)
                {
                    return EPDK_OK;
                }

                break;
            }

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_NEXT:
            {
                g_is_programme_list_selected = 1;

                if ((!dtv_ctr.h_scan) && !dtv_has_sub_scene() && 0 < maple_get_cur_service_list_num()) //不存在才响应上下换台键
                {
                    //搜台时不切换台，有字场景时消息往子场景传，由字场景处理
                    __s32 index ;
                    index = dtv_get_cur_user_index();
                    index++;

                    if (index > (maple_get_cur_service_list_num() - 1))
                    {
                        index = 0;
                    }

                    dtv_set_cur_user_index(index);

                    ///maple_epg_getdata();
                    //maple_epg_search_stop();
                    if (dtv_ca_msg_is_exist())
                    {
                        dtv_ca_msg_off();
                        dtv_ca_msg_clear();
                    }

                    if (dtv_ctr.h_no_signal)
                    {
                        app_dialog_destroy(dtv_ctr.h_no_signal);
                        dtv_ctr.h_no_signal = NULL;
                    }

                    if (dtv_ctr.h_no_auth)
                    {
                        app_dialog_destroy(dtv_ctr.h_no_auth);
                        dtv_ctr.h_no_auth = NULL;
                    }

                    if (dtv_ctr.h_radio_tip)
                    {
                        app_dialog_destroy(dtv_ctr.h_radio_tip);
                        dtv_ctr.h_radio_tip = NULL;
                    }

                    maple_stop_program();
                    maple_play_program();
                    check_dtv_or_radio();
                    return EPDK_OK;
                }

                if (dtv_ctr.h_no_channel != NULL)
                {
                    return EPDK_OK;
                }

                break;
            }

            case GUI_MSG_KEY_VADD:
            case GUI_MSG_KEY_VDEC:
            case GUI_MSG_KEY_LONGVDEC:
            case GUI_MSG_KEY_LONGVADD:
            {
                //dtv_delete_all_menu(DELETE_SCENE_EXP_NO_CHANNEL);
                dtv_ctr.hide_vol_cnt = 0;
                dtv_show_signal_status(EPDK_TRUE);

                if (EPDK_TRUE == GUI_IsTimerInstalled(msg->h_deswin, DVB_VOL_TIMER_ID))
                {
                    GUI_ResetTimer(msg->h_deswin, DVB_VOL_TIMER_ID, 100, NULL);
                }

#ifdef APP_ADD_RECORD_VEDIO

                if (dsk_volume_get() == 0)
                {
                    dtv_ctr.record_ts_remark_num++;

                    if (dtv_ctr.record_ts_remark_num >= DVB_RECORD_TS_TRRIGER_NUM)
                    {
                        dtv_ctr.record_ts_remark_num = 0;

                        if (maple_get_cur_service_list_num() > 0)
                        {
                            main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, maple_get_current_program_freq(), 0);
                        }
                        else
                        {
                            main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, 0, 0);
                        }
                    }
                }
                else
                {
                    dtv_ctr.record_ts_remark_num = 0;
                }

#endif
                return EPDK_OK;
            }

#if 0////字幕开关
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                //case GUI_MSG_KEY_SUBTITLE: //字幕开关
                //按键不够，公版先这么处理
            {
                /*if(NULL == dtv_ctr.h_subtitle)
                {
                    dtv_ctr.h_subtitle = dtv_subtitle_create(msg->h_deswin);
                }*/
                if (!dtv_ctr.h_scan)
                {
                    //如果有子界面存在，只记录，不显示字幕
                    //从子界面退出时，会根据dtv_ctr.b_has_subtitle统一处理
                    if (dtv_ctr.h_lyr)
                    {
                        dtv_ctr.b_has_subtitle = !dtv_ctr.b_has_subtitle;
                    }
                    else
                    {
                        dtv_subtitle_switch_onoff();
                    }
                }

                return EPDK_OK;
            }
#endif
#endif

            default:
                break;
        }
    }
    else if (last_key != -1 && KEY_REPEAT_ACTION == msg->dwAddData2)
    {
        if (last_key != msg->dwAddData1)
        {
            last_key = msg->dwAddData1;
        }
        else
        {
            if (dtv_has_sub_scene() == EPDK_TRUE)
            {
                return EPDK_FAIL;
            }

            return EPDK_OK;
        }

        __msg("KEY_REPEAT_ACTION==========:%d ", msg->dwAddData1);

        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LONGENTER:
            {
                if ((!dtv_ctr.h_scan))
                {
                    __msg(".... begin scan ....");
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                    dtv_subtitle_proc();
#endif
                    dtv_delete_all_menu(0);
                    maple_epg_search_stop();
                    maple_search_stop();
                    app_misc_audio_mute(0); // mute
                    maple_stop_program();
#ifdef DTV_AUTO_SEARCH_ENABLE
                    dtv_search_program_start(msg, AUTO_SERACH_TYPE, 0);// 自动搜索
#else
                    dtv_search_program_start(msg, ALL_FREQ_SERACH_TYPE, 0);// 全频点搜索
#endif
                    //_AutoCheckThreadCreate();
                }

                return EPDK_OK;
            }

            case GUI_MSG_KEY_LONGMENU:
                //case GUI_MSG_KEY_ESCAPE:
            {
                {
                    if ((NULL != dtv_ctr.h_scan))
                    {
                        maple_break_search_task();
                    }

                    //长按退出应用
                    {
                        __gui_msg_t dmsg;
                        //__here__;
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), SWITCH_TO_MMENU) ;
                        GUI_SendMessage(&dmsg);
                        //return EPDK_OK;
                    }
                    //return EPDK_OK;
                }
                return EPDK_OK;
            }
        }
    }
    else if (last_key != -1 && KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;

        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_MENU:  //设置
                //case GUI_MSG_KEY_LONGMENU:    //设置
                __wrn("dtv_ctr.h_setting    : 0x%08x", dtv_ctr.h_setting);

                /* add for test
                {
                    static __u8 test = 0;

                    __wrn("test:%d", test);
                    if (test%2 == 1)
                    {
                        test++;
                        dtv_ca_msg_display(0xE7);
                        return;
                    }
                    test++;
                }
                */
                if (dtv_ctr.h_scan)
                {
                    return EPDK_OK;
                }

                if (dtv_ctr.h_serverlist)
                {
                    break;
                }

                {
                    if (dtv_ctr.sel_sta != 0)
                    {
                        clear_dtv_select_number(msg);
                    }

                    //gscene_hbar_set_state(HBAR_ST_HIDE);
                    __wrn("dtv_ctr.h_setting    : 0x%08x", dtv_ctr.h_setting);

                    if (NULL == dtv_ctr.h_setting)
                    {
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                        dtv_subtitle_proc();
#endif
                        dtv_delete_all_menu(0);
                        dtv_setting_menu_create(msg);
                        return EPDK_OK;
                    }
                }

                break;//return EPDK_FAIL;

            case GUI_MSG_KEY_TVOUT:
            case GUI_MSG_KEY_LONGTVOUT:
                break;

            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_LIST://播放预览
            {
                //  __log("%s %d", __FILE__, __LINE__);
                if (dtv_ca_msg_is_exist())
                {
                    dtv_ca_msg_off();
                    dtv_ca_msg_clear();
                }

                if (dtv_ctr.sel_sta == 3)
                {
                    dtv_ctr.sel_sta = 0;
                    {
                        if (!dtv_ctr.h_scan)
                        {
                            //maple_epg_search_stop();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)\
||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                            //dtv_set_cur_user_index(0);//isdbt默认播放第一个频点的第一个台
                            dtv_subtitle_proc();
#endif
                            dtv_delete_all_menu(0);
                            maple_epg_search_stop();
                            maple_search_stop();
                            maple_stop_program();
                            dtv_search_program_start(msg, MANUAL_SERACH_TYPE, 0);// 手动搜索
                        }
                        else
                        {
                            __msg("searching .....");
                        }

                        return EPDK_OK;
                        //break; //unreachable
                    }
                }
                else if (dtv_ctr.sel_sta == 1)
                {
                    if (dtv_ctr.sel_num != 0 && dtv_ctr.sel_num <= maple_get_cur_service_list_num())
                    {
                        if (dtv_ctr.lyr_sel_num != NULL)
                        {
                            GUI_LyrWinDelete(dtv_ctr.lyr_sel_num);
                            dtv_ctr.lyr_sel_num = NULL;
                            __msg("lyr_sel_num <= 0x%08x", dtv_ctr.lyr_sel_num);
                        }

                        if (GUI_IsTimerInstalled(msg->h_deswin, DVB_SPIC_CHANNEL_ID))
                        {
                            GUI_KillTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID);
                        }

                        dtv_set_cur_user_index(dtv_ctr.sel_num - 1);
                        app_misc_audio_mute(0);     // mute on
                        maple_stop_program();
                        maple_play_program();
                        check_dtv_or_radio();

                        if (maple_get_cur_service_list_num() >= 1)
                        {
                            app_misc_audio_mute((dtv_mute_status == 0) ? 1 : 0);
                        }

                        dtv_ctr.sel_sta = 0;
                    }
                    else
                    {
                        // 提示
                        dtv_ctr.sel_sta = 2;
                        dtv_show_select_number(msg);
#if 0

                        if (!GUI_IsTimerInstalled(msg->h_deswin, DVB_SPIC_CHANNEL_ID))
                        {
                            GUI_SetTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID, KEY_NUM_DELAY, NULL);
                        }
                        else
                        {
                            GUI_ResetTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID, KEY_NUM_DELAY, NULL);
                        }

#endif
                        //clear_dtv_select_number();
                        clear_dtv_select_number(msg);
                    }
                }
                else if (dtv_ctr.sel_sta == 2)  // 提示或其它情况
                {
                    clear_dtv_select_number(msg);
                }
                else if ((!dtv_ctr.h_scan) && /*service_list->servNum > 0 &&*/ !dtv_ctr.h_setting && !dtv_ctr.h_serverlist)
                {
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                    dtv_subtitle_proc();
#endif

                    if (maple_get_cur_service_list_num() > 0)
                    {
                        __msg("maple_stop_program().");
                        //maple_stop_program();
                        clear_dtv_select_number(msg);
                        dtv_delete_all_menu(DTV_NO_AUTH);
                        dtv_preview_menu_create(msg);
                    }

                    return EPDK_OK;
                }
                else
                {
                    //无频道时，可视客户要求在此添加提示或进入预览界面
                }

                return EPDK_FAIL;
            }

            case GUI_MSG_KEY_ESCAPE:
            {
                if (dtv_ca_msg_is_exist())
                {
                    dtv_ca_msg_off();
                    dtv_ca_msg_clear();
                }

                {
                    if ((NULL != dtv_ctr.h_scan))
                    {
                        maple_break_search_task();
                    }

                    //长按退出应用
                    if (dtv_has_sub_scene() == EPDK_FALSE/* || dtv_ctr.h_scan*/)
                    {
                        __gui_msg_t dmsg;
                        //__here__;
                        dmsg.id = GUI_MSG_COMMAND;
                        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        dmsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), SWITCH_TO_MMENU) ;
                        GUI_SendNotifyMessage(&dmsg);
                        //return EPDK_OK;
                    }
                    else
                    {
                        return EPDK_FAIL;//有应用场景，往下传
                    }

                    //return EPDK_OK;
                }

                return EPDK_OK;
            }

            default:
                return EPDK_FAIL;
        }
    }

    return EPDK_FAIL;
}


static __s32 dtv_command_proc(__gui_msg_t *msg)
{
    __msg("msg->dwAddData1=0x%08x, msg->dwAddData2=0x%08x ", msg->dwAddData1, msg->dwAddData2);

    /*common notify msg*/
    switch (msg->dwAddData1)
    {
        //从设置菜单发来的命令不需要处理字幕信息
        case DTV_CA_MSG_DISPLAY:
        {
            dtv_ca_msg_display(msg->dwAddData2);
            return EPDK_OK;
        }

        case DTV_CA_MSG_RESUME:
        {
            dtv_ca_msg_resume();
            return EPDK_OK;
        }

        case DTV_RESEARCH_SERVICE_LIST:
        {
            clear_dtv_select_number(msg);
            dtv_ca_msg_clear();
            dtv_delete_all_menu(0);
            maple_epg_search_stop();
            maple_search_stop();
            app_misc_audio_mute(0);     // mute on
            maple_stop_program();
            dtv_search_program_start(msg, ALL_FREQ_SERACH_TYPE, 0);
            return EPDK_OK;
        }

#ifdef DTV_AUTO_SEARCH_ENABLE

        case DTV_AUTO_RESEARCH_SERVICE_LIST:
        {
            clear_dtv_select_number(msg);
            dtv_ca_msg_clear();
            dtv_delete_all_menu(0);
            maple_epg_search_stop();
            maple_search_stop();
            app_misc_audio_mute(0);     // mute on
            maple_stop_program();
            dtv_search_program_start(msg, AUTO_SERACH_TYPE, 0);
            return EPDK_OK;
        }

#endif

        case DTV_MENU_RESEARCH://手动搜台
        {
            dtv_delete_all_menu(0);
            {
                dtv_ctr.sel_num = 0;
                dtv_ctr.sel_cnt = 0;
                dtv_ctr.sel_sta = 3;
                dtv_show_select_number(msg);
            }
            return EPDK_OK;
        }

        case DTV_PREVIEW_SERVICE_LIST:
        {
            if (service_list->servNum > 0 && !dtv_ctr.h_serverlist)
            {
                clear_dtv_select_number(msg);
                dtv_delete_all_menu(0);
                dtv_preview_menu_create(msg);
            }

            return EPDK_OK;
        }

        //从各子界面退回到正常播放模式时，都是经过这里
        //包括各子界面定时消失的情况
        //所以在ISDBT中在此恢复字幕场景
        case DTV_BACK2PLAY_PROGRAM:
        {
            clear_dtv_select_number(msg);
            dtv_delete_all_menu(DTV_NO_AUTH);
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
            dtv_subtitle_resume();
#endif
            check_dtv_or_radio();
            return EPDK_OK;
        }

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

        case CMD_CREATE_ROLLING_TXT:
        {
            __log("%s %d:", __FILE__, __LINE__);
            dtv_rolling_txt_create(msg);
        }

        return EPDK_OK;

        case CMD_DESTROY_ROLLING_TXT:
        {
            dtv_hide_rollingTxt_dialog();
        }

        return EPDK_OK;
#endif  // #if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

        case DTV_SETTING_2_MANU_SEARCH:
        {
            {
                dtv_ca_msg_clear();
                dtv_delete_all_menu(0);
                maple_epg_search_stop();
                maple_search_stop();
                maple_stop_program();
                dtv_search_program_start(msg, MANUAL_SERACH_TYPE, msg->dwAddData2);
            }
            return EPDK_OK;
        }

        case CMD_TO_UPDATE_SINGAL_STATUS:
        {
            return EPDK_OK;
        }

        //break; //unreachable
        case CMD_TO_UPDATE_VOLUME:
        {
            return EPDK_OK;
        }

        case CMD_TO_DELETE_SEARCH_THREAD:
        {
            maple_search_stop();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
            //dtv_subtitle_resume();
            //搜索结束之后根据注册表的信息决定是否打开字幕
#endif
            return EPDK_OK;
        }

        case DTV_2_EPG:
        {
            //__log("case 2 DTV_2_EPG");
            //maple_epg_getdata();
            clear_dtv_select_number(msg);
            dtv_delete_all_menu(0);
            dtv_schedule_list_menu_create(msg);
            return EPDK_OK;
        }

        case DTV_SETTING_SWITCH_PLAY:
        {
            __wrn("DTV_SETTING_SWITCH_PLAY ....");
            clear_dtv_select_number(msg);
            dtv_set_cur_user_index(msg->dwAddData2);
            dtv_ca_msg_off();
            dtv_ca_msg_clear();
            dtv_delete_all_menu(0);
            maple_stop_program();
            maple_play_program();
            check_dtv_or_radio();
            return EPDK_OK;
        }
    }

    if ((LOSWORD(msg->dwAddData1) == DVB_PROT_DIALOG_ID)
        && (HIWORD(msg->dwAddData1) == ADLG_CMD_EXIT))
    {
        //处理dialog存在时，退出问题
        //__log("%s %d dtv_ctr.h_lyr:0x%x", __FILE__, __LINE__, dtv_ctr.h_lyr);
#if 0
        if (dtv_ctr.h_lyr)
        {
            /*{
                __gui_msg_t dmsg;

                dmsg.id = GUI_MSG_COMMAND;
                dmsg.h_deswin = (msg->h_deswin);
                dmsg.dwAddData1 = DTV_BACK2PLAY_PROGRAM ;

                GUI_SendNotifyMessage(&dmsg);
            }*/
        }
        else
#endif
        {
            {
                __gui_msg_t dmsg;
                dmsg.id = GUI_MSG_COMMAND;
                dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                dmsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), SWITCH_TO_MMENU) ;
                GUI_SendNotifyMessage(&dmsg);
            }
        }

        return EPDK_OK;
    }

    return EPDK_OK;
}

static H_LYR        h_tips_lyr = NULL;
static void dtv_exit_tips_dialog(void)
{
#if 1//
    {
        GUI_RECT gui_rect;
        H_THEME bg_bmp;
        void *bmp_buffer;
        char text[128];

        RECT rect;

        dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);

        rect.x = (rect.width - LISTBAR_WIDTH) / 2;
        rect.y = 47;

        rect.width = LISTBAR_WIDTH;
        rect.height = rect.height - rect.y;


        h_tips_lyr = dtv_com_layer_create(&rect, "exit tips");

        __msg("... show wait ...");
        //  if(GUI_LyrWinGetSta(h_tips_lyr) == GUI_LYRWIN_STA_SUSPEND)
        {
            //      GUI_LyrWinSetSta(h_tips_lyr, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSetSta(h_tips_lyr, GUI_LYRWIN_STA_OFF);
            GUI_LyrWinSetTop(h_tips_lyr);
        }

        GUI_LyrWinSel(h_tips_lyr);
        GUI_UC_SetEncodeUTF8();
        //GUI_Clear();
        GUI_SetFont(SWFFont);

        gui_rect.x0 = (rect.width - 157) / 2;
        gui_rect.y0 = (rect.height - rect.y) / 2;
        gui_rect.x1 = gui_rect.x0 + 157 - 1;
        gui_rect.y1 = gui_rect.y0 + rect.y - 1;

        bg_bmp = dsk_theme_open(ID_DTV_TIPS_BG_BMP);

        if (bg_bmp)
        {
            __msg("... show tips frame ...");
            bmp_buffer = theme_hdl2buf(bg_bmp);

            if (bmp_buffer)
            {
                __msg("... theme_hdl2buf() => 0x%08x ...", bmp_buffer);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_BMP_Draw(bmp_buffer, gui_rect.x0, gui_rect.y0);
            }

            dsk_theme_close(bg_bmp);
        }

        GUI_SetColor(GUI_WHITE);
        dsk_langres_get_menu_text(STRING_DTV_WAIT, text, sizeof(text));
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);

        {
            //      GUI_LyrWinSetSta(h_tips_lyr, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSetSta(h_tips_lyr, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(h_tips_lyr);
        }

        //  GUI_LyrWinSetSta(h_tips_lyr, GUI_LYRWIN_STA_ON);
        //  GUI_LyrWinSetTop(h_tips_lyr);
    }
#endif
}

static void dtv_exit_tips_dialog_destroy(void)
{
    if (h_tips_lyr)
    {
        GUI_LyrWinDelete(h_tips_lyr);
        h_tips_lyr = NULL;
    }
}
static __s32 __dtv_main_destroy(__gui_msg_t *msg)
{
    int i;
    __msg("__dtv_main_destroy()");

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_SPIC_CHANNEL_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID);
        dtv_ctr.ch_timer = 0;
    }

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_SINGAL_TIMER_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_SINGAL_TIMER_ID);
    }

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_VOL_TIMER_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_VOL_TIMER_ID);
    }

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_BATTERY_TIMER_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_BATTERY_TIMER_ID);
    }

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_FLUSH_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_FLUSH_ID);
        __wrn("which key ???");
    }

#ifdef APP_ADD_RECORD_VEDIO

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_RECORD_TS_BACK_DOOR_TIMER))
    {
        GUI_KillTimer(msg->h_deswin, DVB_RECORD_TS_BACK_DOOR_TIMER);
    }

#endif

    if (GUI_IsTimerInstalled(msg->h_deswin, DTMB_CA_PREAUTH_TIMER_ID))
    {
        GUI_KillTimer(msg->h_deswin, DTMB_CA_PREAUTH_TIMER_ID);
        __wrn("which key ???");
    }

    if (GUI_IsTimerInstalled(msg->h_deswin, DTMB_CA_CARDID_TIMER_ID))
    {
        GUI_KillTimer(msg->h_deswin, DTMB_CA_CARDID_TIMER_ID);
        __wrn("which key ???");
    }

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

    if (h_preAuth_lyr)
    {
        GUI_LyrWinDelete(h_preAuth_lyr);
        h_preAuth_lyr = NULL;
    }

    if (h_cardId_lyr)
    {
        GUI_LyrWinDelete(h_cardId_lyr);
        h_cardId_lyr = NULL;
    }

    dtv_hide_rollingTxt_dialog();
#endif  // #if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    dtv_loading_destory_timer();
    check_auth_timer_destroy();
    dtv_show_signal_status(EPDK_FALSE);

    if (dtv_ctr.h_loading)
    {
        app_loading_win_delete(dtv_ctr.h_loading);
        dtv_ctr.h_loading = NULL;
    }

    if (dtv_ctr.h_battery_lyr)
    {
        GUI_LyrWinDelete(dtv_ctr.h_battery_lyr);
        dtv_ctr.h_battery_lyr = NULL;
    }

#if 0

    if (dtv_ctr.lyr_radio_program != NULL)
    {
        GUI_LyrWinDelete(dtv_ctr.lyr_radio_program);
        dtv_ctr.lyr_radio_program = NULL;
    }

#endif

    if (dtv_ca_msg_is_exist())
    {
        dtv_ca_msg_off();
        dtv_ca_msg_clear();
    }

    dtv_delete_all_menu(0);
    dtv_exit_tips_dialog();

    if (0 != sl_task_id)
    {
        //__u8 err;
        //__here__;
        maple_break_search_task();
        //__here__;
        maple_thread_delete(sl_task_id);
        sl_task_id = 0;
    }
    else
    {
        //__here__;
        maple_stop_program();
    }

    //__here__;
    dsk_amplifier_onoff(0);
    app_misc_audio_mute(0);         // mute on
    clear_dtv_select_number(msg);
    dtv_delete_all_menu(0);
    maple_reg_flush();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
    dtv_subtitle_delete();
#endif
    //__here__;
    maple_close();
    dtv_uninit();
    uninit_select_dtv_bmp();

    // 电量图片资源
    for (i = 0; i < MAX_CHARGE_LEVEL; i++)
    {
        if (dtv_ctr.h_battery_bmp[i] != NULL)
        {
            dsk_theme_close(dtv_ctr.h_battery_bmp[i]);
            dtv_ctr.h_battery_bmp[i] = NULL;
            __msg("h_battery_bmp[%d] <= 0x%08x", i, dtv_ctr.h_battery_bmp[i]);
        }
    }

#if 0

    if (dtv_ctr.h_radio_program_bmp != NULL)
    {
        dsk_theme_close(dtv_ctr.h_radio_program_bmp);
        dtv_ctr.h_radio_program_bmp = NULL;
        __msg("h_radio_program_bmp[%d] <= 0x%08x", i, dtv_ctr.h_radio_program_bmp);
    }

    if (dtv_ctr.h_switch_program_bmp != NULL)
    {
        dsk_theme_close(dtv_ctr.h_switch_program_bmp);
        dtv_ctr.h_switch_program_bmp = NULL;
        __msg("h_radio_program_bmp[%d] <= 0x%08x", i, dtv_ctr.h_switch_program_bmp);
    }

    //esKRNL_TimeDly(80);//延时一段时间，怀疑模块掉电，会影响到nor的读。
#endif

    //__here__;
    if (dtv_ctr.h_lyr != NULL)
    {
        GUI_LyrWinSel(dtv_ctr.h_lyr);

        if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(dtv_ctr.h_lyr)
            && GUI_LYRWIN_STA_SLEEP != GUI_LyrWinGetSta(dtv_ctr.h_lyr))
        {
            __wrn("------------------------Err Frame Buf fail.");
        }
        else
        {
            GUI_Clear();
        }

        GUI_LyrWinDelete(dtv_ctr.h_lyr);
        dtv_ctr.h_lyr = NULL;
    }

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    dtv_hide_rollingTxt_dialog();
    dtv_show_preAuth_dialog_destroy();
    dtv_show_cardId_dialog_destroy();
#endif  // #if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    dtv_exit_tips_dialog_destroy();
    g_dtv_mainwin = NULL;
    //因为 tunner mxl603地址和fm rda5807 地址冲突
    // 添加退出DTMB电视后，开关一下fm，保证低功耗   by wanggang for aihua
    dsk_radio_open_i();
    dsk_radio_close_i();
    __msg("esPWM_UnlockCpuFreq().");
    esPWRMAN_UnlockCpuFreq();
    GUI_Memdev_FrameBuffer_Release(1);//必须
    return EPDK_OK;
}

static __s32 __dtv_timer_proc(__gui_msg_t *msg)
{
    __msg("timer comming..., msg->dwAddData1:%d", msg->dwAddData1);
    //if(msg->dwAddData1 == DVB_SPIC_CHANNEL_ID)//检测用户输入的数字键

    if (1 == g_is_searching || NULL != dtv_ctr.h_scan)
    {
        return EPDK_OK;
    }

    __msg("----__dtv_timer_proc----");

    switch (msg->dwAddData1)
    {
#if ((MAPLE_CA_STANDAR == MEDIAGATE_CAS) || (MAPLE_CA_STANDAR == DVN_JET_CAS))

        case DVB_ROLLING_TXT_TIMER_ID:
        {
            dtv_rolling_txt_frame_draw(msg);
            return EPDK_OK;
        };

#endif
        case DVB_SPIC_CHANNEL_ID://按数字键选频道
        {
#if 1    //频点搜索     
            __msg("--- DVB_SPIC_CHANNEL_ID ---");

            //  GUI_KillTimer(msg->h_deswin,DVB_SPIC_CHANNEL_ID);

            if (dtv_ctr.sel_sta == 3)
            {
                {
                    if (!dtv_ctr.h_scan)
                    {
                        //maple_epg_search_stop();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)\
||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)\
|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
                        //dtv_set_cur_user_index(0);//isdbt默认播放第一个频点的第一个台
                        dtv_subtitle_proc();
#endif
                        dtv_delete_all_menu(0);
                        maple_epg_search_stop();
                        maple_search_stop();
                        maple_stop_program();
                        dtv_search_program_start(msg, MANUAL_SERACH_TYPE, 0);
                    }
                    else
                    {
                        __msg("searching .....");
                    }

                    return EPDK_OK;
                    //break; //unreachable
                }
            }
            else if (dtv_ctr.sel_sta == 1)
            {
                if (dtv_ctr.sel_num != 0 && dtv_ctr.sel_num <= maple_get_cur_service_list_num())
                {
                    if (dtv_ctr.lyr_sel_num != NULL)
                    {
                        //  GUI_LyrWinSel(dtv_ctr.lyr_sel_num);
                        //  GUI_Clear();
                        GUI_LyrWinDelete(dtv_ctr.lyr_sel_num);
                        dtv_ctr.lyr_sel_num = NULL;
                        __msg("lyr_sel_num <= 0x%08x", dtv_ctr.lyr_sel_num);
                    }

                    dtv_set_cur_user_index(dtv_ctr.sel_num - 1);
                    app_misc_audio_mute(0);     // mute on
                    maple_stop_program();
                    maple_play_program();
                    check_dtv_or_radio();

                    if (maple_get_cur_service_list_num() >= 1)
                    {
                        app_misc_audio_mute((dtv_mute_status == 0) ? 1 : 0);
                    }

                    GUI_KillTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID);
                    dtv_ctr.sel_sta = 0;
                }
                else
                {
                    // 提示
                    dtv_ctr.sel_sta = 2;
                    dtv_show_select_number(msg);
                    //GUI_ResetTimer(msg->h_deswin,DVB_SPIC_CHANNEL_ID,KEY_NUM_DELAY,NULL);
                }
            }
            else
                //if(dtv_ctr.sel_sta == 2)  // 提示或其它情况
            {
                if (dtv_ctr.lyr_sel_num != NULL)
                {
                    //GUI_LyrWinSel(dtv_ctr.lyr_sel_num);
                    //GUI_Clear();
                    GUI_LyrWinDelete(dtv_ctr.lyr_sel_num);
                    dtv_ctr.lyr_sel_num = NULL;
                    __msg("lyr_sel_num <= 0x%08x", dtv_ctr.lyr_sel_num);
                }

                check_dtv_or_radio();
                GUI_KillTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID);
                dtv_ctr.sel_sta = 0;
            }

#endif
            return EPDK_OK;
        }

        case DVB_SINGAL_TIMER_ID://信号状态
        {
            HBarState state = 0;
            dtv_singal_strength_detect(msg->h_deswin);

            //提示无频道，提示搜台
            if (maple_get_cur_service_list_num() == 0
                && dtv_has_sub_scene() == EPDK_FALSE
                && dtv_ctr.lyr_sel_num == NULL
                && dtv_ctr.h_no_channel == NULL)
            {
                __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_NO_CHANNEL};
                default_dialog(dtv_ctr.h_no_channel, msg->h_deswin, SUBMENU_DVB_SETTING_DLG_ID, ADLG_OK, lang_id);
                //esKRNL_TimeDly(100);
                //app_dialog_destroy(dlg);
            }

            /*提示无信号*/
            {
                maple_demod_ss_t ss;
                eLIBs_memset(&ss, 0x00, sizeof(ss));
                maple_get_ss(&ss);

                if ((maple_get_cur_service_list_num() > 0)
                    && (ss.quality <= 8)
                    && dtv_ctr.h_no_signal == NULL
                    && dtv_has_sub_scene() == EPDK_FALSE
                    && dtv_ctr.lyr_sel_num == NULL
                    && !GUI_IsTimerInstalled(g_dtv_mainwin, DVB_LOADING_TIMER_ID))//当现实loading时不显示信号差
                {
                    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_NO_SINGALS};

                    if (dtv_ctr.h_no_auth)
                    {
                        app_dialog_destroy(dtv_ctr.h_no_auth);
                        dtv_ctr.h_no_auth = NULL;
                    }

                    if (dtv_ctr.h_radio_tip)
                    {
                        app_dialog_destroy(dtv_ctr.h_radio_tip);
                        dtv_ctr.h_radio_tip = NULL;
                    }

                    if (dtv_ca_msg_is_exist())
                    {
                        dtv_ca_msg_off();
                        dtv_ca_msg_clear();
                    }

                    default_dialog(dtv_ctr.h_no_signal, msg->h_deswin, SUBMENU_DVB_SETTING_DLG_ID, ADLG_NOTHING, lang_id);
                }
                else if ((ss.quality > 8) && (dtv_ctr.h_no_signal != NULL))
                {
                    app_dialog_destroy(dtv_ctr.h_no_signal);
                    dtv_ctr.h_no_signal = NULL;
                    check_dtv_or_radio();
                }
            }

            /*shiql add for D100 test backboor*/
            if ((open_back_boor == EPDK_TRUE) && (maple_get_cur_service_list_num() > 0))
            {
                static __s32 num = 0;

                if (num >= TIME_NUM)
                {
                    num = 0;

                    if ((!dtv_ctr.h_scan) && !dtv_has_sub_scene()) //不存在才响应上下换台键
                    {
                        __s32 index = eLIBs_ramdom(maple_get_cur_service_list_num());
                        __log("%s %d ramdom switch:%d", __FILE__, __LINE__, index);
                        clear_dtv_select_number(msg);
                        dtv_set_cur_user_index(index);
                        __wrn("maple_stop_program ....");

                        if (dtv_ca_msg_is_exist())
                        {
                            dtv_ca_msg_off();
                            dtv_ca_msg_clear();
                        }

                        if (dtv_ctr.h_no_signal)
                        {
                            app_dialog_destroy(dtv_ctr.h_no_signal);
                            dtv_ctr.h_no_signal = NULL;
                        }

                        if (dtv_ctr.h_no_auth)
                        {
                            app_dialog_destroy(dtv_ctr.h_no_auth);
                            dtv_ctr.h_no_auth = NULL;
                        }

                        if (dtv_ctr.h_radio_tip)
                        {
                            app_dialog_destroy(dtv_ctr.h_radio_tip);
                            dtv_ctr.h_radio_tip = NULL;
                        }

                        maple_stop_program();
                        maple_play_program();
                        check_dtv_or_radio();
                        return EPDK_OK;
                    }
                }

                num++;
            }

            break;
        }

        case DVB_AUTH_TIMER_ID:
        {
            //提示未授权
            __u8 First_Picture_out = EPDK_FALSE;
            First_Picture_out = maple_get_first_picture_status();

            if (First_Picture_out == EPDK_FALSE
                && dtv_has_sub_scene() == EPDK_FALSE
                && dtv_ctr.h_no_signal == NULL
                && dtv_ctr.lyr_sel_num == NULL
                && (maple_get_cur_service_list_num() > 0)
               )
            {
                check_dtv_have_auth();
            }
            else if (First_Picture_out == EPDK_TRUE && dtv_ctr.h_no_auth != NULL)
            {
                app_dialog_destroy(dtv_ctr.h_no_auth);
                dtv_ctr.h_no_auth = NULL;
            }

            break;
        }

        case DVB_LOADING_TIMER_ID: // hide loading
        {
            static __u8 wait_num = 0;
            __u8 First_Picture_out = EPDK_FALSE;
            wait_num++;
            First_Picture_out = maple_get_first_picture_status();
            __log("%s %d wait_num:%d First_Picture_out:%d", __FILE__, __LINE__, wait_num, First_Picture_out);

            if (First_Picture_out == EPDK_TRUE || wait_num >= 10)
            {
                wait_num = 0;
                __log("%s %d!", __FILE__, __LINE__);
                dtv_loading_hide();
            }
        }
        break;

        case DVB_VOL_TIMER_ID://音量
        {
            //非弱信号时才隐藏
            dtv_show_signal_status(EPDK_TRUE);
            return EPDK_OK;
        }

        case DVB_BATTERY_TIMER_ID:
            break;

        case DVB_FLUSH_ID:
        {
#if 1//30秒是否要保存
            static  __s32 count = 0;
            count++;

            if (0) //count%10 == 0)
            {
                GUI_KillTimer(msg->h_deswin, DVB_FLUSH_ID);
                __wrn("flush flash .................");

                if (maple_set_cur_flash_save_program_index(dtv_ctr.cur_user_index) == EPDK_OK)
                {
                    maple_reg_flush();
                }

                count = 0;
            }

            //dsk_reg_flush();
#endif
            return EPDK_OK;
        }

#ifdef APP_ADD_RECORD_VEDIO

        case DVB_RECORD_TS_BACK_DOOR_TIMER:
        {
            dtv_ctr.record_ts_remark_num = 0;
        }
        break;
#endif
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

        case DTMB_CA_PREAUTH_TIMER_ID:
        {
            __wrn("DTMB_CA_PREAUTH_TIMER_ID@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@22");
            GUI_KillTimer(msg->h_deswin, DTMB_CA_PREAUTH_TIMER_ID);
            dtv_show_preAuth_dialog_destroy();
            return EPDK_OK;
        }

        case DTMB_CA_CARDID_TIMER_ID:
        {
            __wrn("DTMB_CA_CARDID_TIMER_ID@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@22");
            GUI_KillTimer(msg->h_deswin, DTMB_CA_CARDID_TIMER_ID);
            dtv_show_cardId_dialog_destroy();
            return EPDK_OK;
        }

#endif  // #if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
#if 0

        case DVB_SECURE_TIMER_ID:
        {
            GUI_KillTimer(msg->h_deswin, DVB_SECURE_TIMER_ID);
            maple_check_secure();
            return EPDK_OK;
        }

#endif

        default:
            break;
    }

    return EPDK_FAIL;
}


__s32 search_list_finish_callback(void *arg)
{
    __s32 i = 0;
    maple_sl_cb_param_t *p = NULL;
    __u32 before_filter_program_num = 0, after_filter_program_num = 0;
    p = (maple_sl_cb_param_t *)arg;
#ifdef DTV_AUTO_SEARCH_ENABLE

    if (search_service_list->nit_freq_num >= 0)
    {
        if (p && p->pOpaque)
        {
            esKRNL_SemPost((__krnl_event_t *)p->pOpaque);
            __wrn("search service finish.");
        }

        return EPDK_OK;
    }

#endif
    before_filter_program_num = search_service_list->servNum;
    maple_program_add_srvlist();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
    //ISDBT在搜索某个频率结束时才知道台数
    esMODS_MIoctrl(maple, MAPLE_CMD_SL_GET, 0, (void *)search_service_list);

    for (i = 0; i < search_service_list->servNum; i++)
    {
        //if(search_service_list->pServItems[i].nOneSegFlag == EPDK_TRUE)
        if (
#if (MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)
            search_service_list->pServItems[i].pmtId >= 0x1fc8 &&
#endif
            search_service_list->pServItems[i].pmtId <= 0x1fcf)
        {
            after_filter_program_num++;
        }
    }

    if (after_filter_program_num > 0)
    {
        if (dtv_ctr.h_scan)
        {
            __gui_msg_t    setmsg;
            setmsg.id          =  GUI_MSG_COMMAND;
            setmsg.h_srcwin    =  NULL;
            setmsg.h_deswin    =  dtv_ctr.h_scan;
            setmsg.dwAddData1   = SLIDE_UPDATE_CH;
            setmsg.dwAddData2   = after_filter_program_num;
            setmsg.dwReserved   = before_filter_program_num;
            setmsg.p_arg       =  NULL;
            GUI_SendNotifyMessage(&setmsg);
            esKRNL_TimeDly(50);
        }
    }

#endif

    if (p && p->pOpaque)
    {
        esKRNL_SemPost((__krnl_event_t *)p->pOpaque);
        __wrn("search service finish.");
    }

    return EPDK_OK;
}
//用于更新当前搜台数，每搜到一个台会触发一次
__s32 search_list_event_callback(void *arg)
{
    maple_sl_cb_param_t *p;
    p = (maple_sl_cb_param_t *)arg;

    if (p && (p->serv_item.channelId != 0))
    {
        __msg("search one service item, channelid = %d name is: %s.", p->serv_item.channelId, p->serv_item.servName);
        __msg("channelId = %d pmtId = %d servId = %d tsId = %d networkId = %d vdStreamNum=%d adStreamNum=%d", \
              p->serv_item.channelId, p->serv_item.pmtId, \
              p->serv_item.servId, p->serv_item.tsId, p->serv_item.networkId, p->serv_item.vdStreamNum, p->serv_item.adStreamNum);
        //update current search result
        __msg("p->serv_item.isCAFree = %d ", p->serv_item.isCAFree);

        if (dtv_ctr.h_scan)
        {
            __gui_msg_t    setmsg;
            setmsg.id          =  GUI_MSG_COMMAND;
            setmsg.h_srcwin    =  NULL;
            setmsg.h_deswin    =  dtv_ctr.h_scan;
            setmsg.dwAddData1   = SLIDE_UPDATE_CH;
            setmsg.dwAddData2   = 0;
            setmsg.p_arg       =  NULL;
            GUI_SendNotifyMessage(&setmsg);
            //esKRNL_TimeDly(50);
        }
    }

    return EPDK_OK;
}

__s32 search_sepg_finish_callback(void *arg)
{
    maple_sepg_cb_param_t *p;
    static char old_sepgc[MAX_SEPG_STRING];
    static char old_sepgn[MAX_SEPG_STRING];
    p = (maple_sepg_cb_param_t *)arg;
    eLIBs_memset(sepg_string_c, 0, MAX_SEPG_STRING);
    eLIBs_memset(sepg_string_n, 0, MAX_SEPG_STRING);
    __wrn("search_sepg_finish_callback()...");

    if (p)
    {
        //esKRNL_SemPost((__krnl_event_t*)p->pOpaque);
        //__inf("sepg service finish.");
        if (p->sepg.hasCurEventInfo)
        {
#if 0   //defined(__MSG)
            sprintf(sepg_string_c, "  %d:%d:%d - %d:%d:%d : ",
                    p->sepg.curEventstartTime.hour,
                    p->sepg.curEventstartTime.min,
                    p->sepg.curEventstartTime.sec,
                    p->sepg.curEventEndTime.hour,
                    p->sepg.curEventEndTime.min,
                    p->sepg.curEventEndTime.sec);
            strcat(sepg_string_c, (char *)p->sepg.curEventName);
            __msg("sepg_string_c = %s", sepg_string_c);
#endif
        }
        else
        {
            __inf(" ======== ");
        }

        if (p->sepg.hasNextEventInfo)
        {
#if 0   //defined(__MSG)
            sprintf(sepg_string_n, "  %d:%d:%d - %d:%d:%d :",
                    p->sepg.nextEventstartTime.hour,
                    p->sepg.nextEventstartTime.min,
                    p->sepg.nextEventstartTime.sec,
                    p->sepg.nextEventEndTime.hour,
                    p->sepg.nextEventEndTime.min,
                    p->sepg.nextEventEndTime.sec);
            strcat(sepg_string_n, p->sepg.nextEventName);
            __msg("sepg_string_n = %s", sepg_string_n);
#endif
        }
        else
        {
            __inf(" ======== ");
        }

        if ((!eLIBs_strcmp(sepg_string_c, old_sepgc)) && (!eLIBs_strcmp(sepg_string_n, old_sepgn)))
        {
            return EPDK_OK;
        }
        else
        {
            eLIBs_strcpy(old_sepgc, sepg_string_c);
            eLIBs_strcpy(old_sepgn, sepg_string_n);
        }

        /*{
                 //send message to what you want
               __gui_msg_t         setmsg;
               GUI_SendNotifyMessage(&setmsg);
            }*/
    }

    return EPDK_OK;
}
static __s32 _dtv_main_create(__gui_msg_t *msg)
{
    //__cedar_tag_inf_t tag_inf;
    __s32 ret;
    H_WIN h_loading = NULL;
    int i;
    cur_playing_index = -1;
    g_is_programme_list_selected = 0;
    dtv_mute_status = 0;
    app_misc_audio_mute(0);     // mute on
    // 唤醒CPU
    esPWRMAN_UsrEventNotify();
    esKRNL_TimeDly(5);          // delay...
    __msg("esPWRMAN_LockCpuFreq().");
    esPWRMAN_LockCpuFreq();
    esPWRMAN_UsrEventNotify();
    /*disable auto sleep*/
    g_disable_close_scn();
    g_disable_standby();
    dsk_set_auto_off_time(0);
    //gscene_hbar_set_state(HBAR_ST_SHOW) ;
    //gscene_bgd_set_state(BGD_STATUS_HIDE);
    //////////////////////////////
    //启动加载动画//
    /////////////////////////////
    __wrn("DVB_LOADING_ID:%d", DVB_LOADING_ID);
#ifdef  DTV_LOADING_ENABLE
    dtv_ctr.h_loading = app_loading_win_create(msg->h_deswin, DVB_LOADING_ID);
#endif
    key_code = -1;
    dtv_init();
    init_select_dtv_bmp();

    // 电量图片资源
    for (i = 0; i < MAX_CHARGE_LEVEL; i++)
    {
        dtv_ctr.h_battery_bmp[i] = dsk_theme_open(id_battery_bmp[i]);
        __msg("h_battery_bmp[%d] = 0x%08x", i, dtv_ctr.h_battery_bmp[i]);
    }

#if 0
    dtv_ctr.h_radio_program_bmp = dsk_theme_open(ID_DTV_RADIO_BG_BMP);
    dtv_ctr.h_switch_program_bmp = dsk_theme_open(ID_HOME_NEW_EBOOK_UF_BMP);
#endif
    dtv_ctr.battery_wink = 0;/* 一直显示电池 */
    dtv_ctr.low_battery_cnt = 0;
    dtv_ctr.battery_warning = 0;
    dtv_ctr.battery_icon_on = 0;
#if (AW_SDK_PMU == 0)    //不带PMU
    dsk_power_get_voltage_level(&dtv_ctr.battery_level);
#else //带PMU
    dsk_power_get_battery_level(&dtv_ctr.battery_level);
#endif
    //__here__;
    ////////////////////////
    //可以在此添加提示正在加载的动画
    ret = maple_open();

    if (EPDK_FAIL == ret)
    {
        __wrn("maple open fail...");
        goto err0;
    }

    __wrn("maple open ok...");
    //加载maple模块成功后，需要在此释放动画
#ifdef  DTV_LOADING_ENABLE

    if (dtv_ctr.h_loading)
    {
        app_loading_win_hide(dtv_ctr.h_loading);
    }

#endif
    //reigster call back for ca message
    {
        __pCBK_t            callback;
        callback = esKRNL_GetCallBack(dtv_ca_msg_callback);
        esMODS_MIoctrl(maple, MAPLE_CMD_SET_CA_DRAW_CB, 0, (void *)callback);
    }
    maple_register_search_callback(search_list_event_callback, search_list_finish_callback);
    maple_register_sepg_callback(search_sepg_finish_callback);
    maple_set_init_screensize();
#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
    maple_set_default_char_coding_type();
    {
        __pCBK_t            callback;
        callback = esKRNL_GetCallBack(dtv_subtitle_paint);
        esMODS_MIoctrl(maple, MAPLE_CMD_SET_SPLY_DRAW_CB, NULL, (void *)callback);
    }
#endif
    ret = maple_program_load(DTV_DATABASE_NAME, EPDK_TRUE);//maple_load_reg_current_program();

    if (EPDK_OK !=  ret)
    {
#if 0
        ret = dtv_search_program_start(msg, 0);
#else

        if (dtv_ctr.h_no_channel == NULL)
        {
            H_WIN dlg;
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_NO_CHANNEL};
            default_dialog(dtv_ctr.h_no_channel, msg->h_deswin, SUBMENU_DVB_SETTING_DLG_ID, ADLG_OK, lang_id);
            //esKRNL_TimeDly(100);
            //app_dialog_destroy(dlg);
        }

#endif
    }
    else
    {
        //maple_program_load(DTV_DATABASE_NAME);
        maple_epg_search_stop();
        //maple_epg_search();
        dtv_set_cur_user_index(maple_get_cur_program_preview_index());
        maple_search_stop();
        maple_stop_program();
        maple_play_program();//如果不在此调用这个函数，退出应用再进来时然后搜台可能会搜不到台
        check_dtv_or_radio();

        if (maple_get_cur_service_list_num() >= 1)
        {
            app_misc_audio_mute((dtv_mute_status == 0) ? 1 : 0);
        }

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
        dtv_subtitle_resume();
#endif
    }

    dsk_amplifier_onoff(1);

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_SINGAL_TIMER_ID) == EPDK_FALSE)
    {
        GUI_SetTimer(msg->h_deswin, DVB_SINGAL_TIMER_ID, 500, NULL);
    }

    //隐藏状态栏
    //gscene_hbar_get_state(&state);
    //if(state == HBAR_ST_SHOW)
    {
        if (EPDK_FALSE == GUI_IsTimerInstalled(msg->h_deswin, DVB_VOL_TIMER_ID))
        {
            GUI_SetTimer(msg->h_deswin, DVB_VOL_TIMER_ID, 100, NULL);
        }
    }
#ifdef APP_ADD_RECORD_VEDIO

    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_RECORD_TS_BACK_DOOR_TIMER) == EPDK_FALSE)
    {
        GUI_SetTimer(msg->h_deswin, DVB_RECORD_TS_BACK_DOOR_TIMER, DVB_RECORD_TS_BACK_DOOR_TIMER_OUT, NULL);
    }

#endif
    return EPDK_OK;
    /*
    err2:
        GUI_LyrWinDelete(dtv_ctr.h_lyr);
        dtv_ctr.h_lyr = NULL;
        //err1:
        maple_close();
        //__here__;
    */
err0:
    dtv_uninit();

    if (dtv_ctr.h_loading)
    {
        app_loading_win_delete(dtv_ctr.h_loading);
        dtv_ctr.h_loading = NULL;
    }

    uninit_select_dtv_bmp();

    // 电量图片资源
    for (i = 0; i < MAX_CHARGE_LEVEL; i++)
    {
        if (dtv_ctr.h_battery_bmp[i] != NULL)
        {
            dsk_theme_close(dtv_ctr.h_battery_bmp[i]);
            dtv_ctr.h_battery_bmp[i] = NULL;
            __msg("h_battery_bmp[%d] <= 0x%08x", i, dtv_ctr.h_battery_bmp[i]);
        }
    }

#if 0

    if (dtv_ctr.h_radio_program_bmp != NULL)
    {
        dsk_theme_close(dtv_ctr.h_radio_program_bmp);
        dtv_ctr.h_radio_program_bmp = NULL;
        __msg("h_radio_program_bmp[%d] <= 0x%08x", i, dtv_ctr.h_radio_program_bmp);
    }

    if (dtv_ctr.h_switch_program_bmp != NULL)
    {
        dsk_theme_close(dtv_ctr.h_switch_program_bmp);
        dtv_ctr.h_radio_program_bmp = NULL;
        __msg("h_radio_program_bmp[%d] <= 0x%08x", i, dtv_ctr.h_switch_program_bmp);
    }

    if (dtv_ctr.lyr_radio_program != NULL)
    {
        GUI_LyrWinDelete(dtv_ctr.lyr_radio_program);
        dtv_ctr.lyr_radio_program = NULL;
    }

#endif
    return EPDK_FAIL;
}


//dtv cmd process function
static __s32 dtv_main_proc(__gui_msg_t  *msg)
{
    __msg("dtv_main_proc()::msg->id=%08x, dwAddData1=%08x, dwAddData2=%08x", msg->id, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            g_dtv_mainwin = msg->h_deswin;
            return _dtv_main_create(msg);
        }

        case GUI_MSG_CLOSE:
        {
            __gui_msg_t dmsg;
            dmsg.id = GUI_MSG_COMMAND;
            dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            dmsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), SWITCH_TO_MMENU) ;
            GUI_SendNotifyMessage(&dmsg);//send to app_root_scene   to process
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            return __dtv_main_destroy(msg);
        }

        case GUI_MSG_KEY:
        {
            if (!GUI_IsTimerInstalled(msg->h_deswin, DVB_FLUSH_ID))
            {
                GUI_SetTimer(msg->h_deswin, DVB_FLUSH_ID, DTV_FLUSH_DELAY, NULL);
            }
            else
            {
                GUI_ResetTimer(msg->h_deswin, DVB_FLUSH_ID, DTV_FLUSH_DELAY, NULL);
            }

            if (dtv_key_proc(msg) != EPDK_OK)
            {
                break;
            }

            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            break;
        }

        case GUI_MSG_COMMAND:
        {
            dtv_command_proc(msg);
            return EPDK_OK;
        }

#if 1

        case GUI_MSG_TIMER:
        {
            __s32 ret;
            ret = __dtv_timer_proc(msg);

            if (EPDK_OK == ret)
            {
                return EPDK_OK;
            }

            break;
        }

#endif

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

/*
**********************************************************************************************************************
*                                               dtv_mainwin_create
*
* Description:
*
* Arguments  :
*
* Returns    :
*
* Notes      :
*
**********************************************************************************************************************
*/
static H_WIN dtv_mainwin_create(root_para_t  *para)
{
    __gui_manwincreate_para_t create_info;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name = APP_DTV;
    create_info.id = APP_DTV_ID;
    create_info.hParent = para->h_parent;
    create_info.ManWindowProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)dtv_main_proc);
    create_info.attr = NULL;
    create_info.hHosting = NULL;
    return (GUI_ManWinCreate(&create_info));
}


/*
******************************************************************************************************
*                                       app_dtv_create 播放器入口
*
*Description: dtv entry functoin.
*
*Arguments  : *arg - user define
*
*Return     : =
*             =
*******************************************************************************************************
*/

/*
//播放器所有,除信号状态外，其创建都在GUI_MSG_KEY中处理
*界面1、频道预览
*界面2、节目预告
*界面3、设置菜单
*界面4、搜索进度
*界面5、信号状态

* 1~4界面由同一图层进行管理，方便各界面之间的切换

//使用此播放器，需要配置如下信息:
*1、MAPLE_TV_STANDAR = MAPLE_TV_DTMB or MAPLE_TV_ISDB_T ..... //播放CMMB或ISDBT
*2、IS_CA_EXIST  = 1 or 0 是否存在加密CA模块,测试天线灵敏度时必须是1
*3、G_DTV_current_area ＝ 0 or 1 ?  0:只搜索深圳频点1:全国频点搜索,0只做为开发人员测试用
*4、ISDBT字幕必须配置g_type, 现默认巴西
*/

//注意:比较老的SDK用内存设备会有问题,导致重启，请屏蔽内存设备
//eg:com_memdev_create \ GUI_MEMDEV_Create ......
//如果CA加密模块不存在，设置中CA的序列号请改为其它信息

//节目预告信息从频道开始播放到解释出ESG信息需要3秒钟，再加上
//定时器的1秒，大概要4秒左右才出显示出来，所以如果觉得慢，
//可以把节目预告放到设置中，这样从播放成功到进到设置，到选择查看节目预告
//基本上就4秒了(用户可自己修改，公版不做此改动，请悉知!!!)


//如果电影音乐有断点记忆，在保存前，需要把CMMB存台的文件读出重新保存
//因为注册表在保存时会把NOR FLASH扇区清除


//重要: 进入CMMB/ISDBT之前请务必把主界面删除，释放内存
//因为CMMB/ISDBT解码需要用到大块连续内存，如果内存不足
//可能会莫名死机!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
signed long  app_dtv_create(root_para_t  *para)
{
    H_WIN dtv_mainwin;
    gscene_hbar_set_state(HBAR_ST_HIDE);
    gscene_bgd_set_state(BGD_STATUS_HIDE);
    //esMEMS_Info();
    GUI_Memdev_FrameBuffer_Release(0);//必须
    //esMEMS_Info();
    //__log("128*sizeof(maple_serv_item_t) = [%d]",128*sizeof(maple_serv_item_t));
    eLIBs_memset(&dtv_ctr, 0, sizeof(dtv_ctr_t));
    __wrn("sizeof(dtv_ctr_t):%d", sizeof(dtv_ctr_t));
    dtv_mainwin = dtv_mainwin_create(para);

    if (dtv_mainwin == NULL)
    {
        __wrn("cann't creat dtv mainwin");
        return NULL;
    }

    GUI_WinSetFocusChild(dtv_mainwin);  //do not set active man win. modified by Bayden 14.02.21 ////12345
    //dtv_show_signal_status(EPDK_TRUE);
    //gscene_hbar_set_state(HBAR_ST_SHOW);
    {
        char title[256] = {0};
        eLIBs_memset(title, 0, sizeof(title));
        dsk_langres_get_menu_text(STRING_HOME_TV, title, sizeof(title));
        gscene_hbar_set_title(title, sizeof(title));
    }
    return (signed long)dtv_mainwin;
}



