
#include <log.h>
#include "dtv_i.h"
#include "beetles_app.h"



dtv_ctr_t  dtv_ctr;
H_WIN g_dtv_mainwin = NULL;


__s32 cur_playing_index = -1;
__s32 g_is_programme_list_selected = 0;



H_WIN  g_dvbscan_win = NULL;


__u32 dtv_mute_status = 0;

#define DTV_SRH_MUTE    0x10
#define DTV__MUTE   0x20

__s32  g_wate_cnt = 0;


///extern __u32 program_is_playing_error;

#if ((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))

#define CMMB_PATH "Z:\\CMMB"
#define CMMB_INDEX_PATH "Z:\\CMMB\\index.html"
#endif




#define  SINGAL_Q   20//指示无信号 modify140327 //0    // SHOULD > 7 FOR CMMB
#define  SINGAL_Q_2   60//指示有信号可以播放或者强信号恢复 modify140327
#define  SINGAL_STRENGTH_MAX  96


__s32 dtv_search_program_start(__gui_msg_t  *msg, __u8 scan_type, __u32 index);
__s32 dtv_setting_menu_create(__gui_msg_t  *msg);
__s32 dtv_preview_menu_create(__gui_msg_t  *msg);

__bool is_on_tv(void);
__s32 __root_switch_output(void);
__s32 dtv_switch_tvout(__gui_msg_t *msg);


/**********************************************************************************************************************
* 逻辑频道号
**********************************************************************************************************************/

#if 1 //其实逻辑频道号放在dtv_mid.c中更合适
__s32  dtv_get_cur_user_index(void)
{
    return  dtv_ctr.cur_user_index;
}

__s32 dtv_set_cur_user_index(__s32 index)
{
    if (index < 0)
    {
        index = 0;
    }

    dtv_ctr.cur_user_index = index;
    return maple_set_cur_channel(index);
}
#endif

/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
H_WIN dtv_com_layer_create(RECT *rect, char *layer_name)
{
    FB  fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t para =
    {
        //DISP_LAYER_WORK_MODE_PALETTE,
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,
        0,
        0x80,

        0,
        10,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };
    __layerwincreate_para_t create_info =
    {
        "dvb_scan_proc layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    eLIBs_strcpy(create_info.name, layer_name);
    fb.size.width  = rect->width;
    fb.size.height = rect->height;
    para.pipe           = 1;
    para.scn_win.x      = rect->x;
    para.scn_win.y      = rect->y;
    para.scn_win.width  = rect->width;
    para.scn_win.height = rect->height;
    para.src_win.x      = 0;
    para.src_win.y      = 0;
    para.src_win.width  = rect->width;
    para.src_win.height = rect->height;
    para.fb = &fb;
    create_info.lyrpara = &para;
    return (GUI_LyrWinCreate(&create_info));
}


//图层由mainwin控制，便于各界面之间切换
//设置、节目预告、节目预览、搜索界面
__s32  dtv_layer_create(RECT *rect)
{
    dtv_ctr.h_lyr = dtv_com_layer_create(rect, "comman layer");

    if (!dtv_ctr.h_lyr)
    {
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/




/*
**********************************************************************************************************************
*                                               _VideoScreenInit
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
#if 1

__s32 dtv_select_num[10] =
{
    ID_DTV_SEL_NUM_0_BMP,
    ID_DTV_SEL_NUM_1_BMP,
    ID_DTV_SEL_NUM_2_BMP,
    ID_DTV_SEL_NUM_3_BMP,
    ID_DTV_SEL_NUM_4_BMP,
    ID_DTV_SEL_NUM_5_BMP,
    ID_DTV_SEL_NUM_6_BMP,
    ID_DTV_SEL_NUM_7_BMP,
    ID_DTV_SEL_NUM_8_BMP,
    ID_DTV_SEL_NUM_9_BMP
};

HTHEME dtv_sel_num_bmp[10] = {0};
HTHEME dtv_sel_bg_bmp = NULL;
HTHEME dtv_sel_no_num_bmp = NULL;

void init_select_dtv_bmp(void)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        dtv_sel_num_bmp[i] = dsk_theme_open(dtv_select_num[i]);
        __msg("dtv_sel_num_bmp[%d] = 0x%08x", i, dtv_sel_num_bmp[i]);
    }

    dtv_sel_bg_bmp = dsk_theme_open(ID_DTV_SEL_NUM_BG_BMP);
    dtv_sel_no_num_bmp = dsk_theme_open(ID_DTV_SEL_NUM_NO_NUM_BMP);
}

#define BG_BMP_WIDTH    240
#define BG_BMP_HEIGHT   136

void uninit_select_dtv_bmp(void)
{
    int i;

    for (i = 0; i < 10; i++)
    {
        if (dtv_sel_num_bmp[i])
        {
            dsk_theme_close(dtv_sel_num_bmp[i]);
            dtv_sel_num_bmp[i] = NULL;
        }
    }

    if (dtv_sel_bg_bmp)
    {
        dsk_theme_close(dtv_sel_bg_bmp);
        dtv_sel_bg_bmp  = NULL;
    }

    if (dtv_sel_no_num_bmp)
    {
        dsk_theme_close(dtv_sel_no_num_bmp);
        dtv_sel_no_num_bmp  = NULL;
    }
}

//释放选台时相关的资源,timer & layer
void clear_dtv_select_number(__gui_msg_t *msg)
{
    if (GUI_IsTimerInstalled(msg->h_deswin, DVB_SPIC_CHANNEL_ID))
    {
        GUI_KillTimer(msg->h_deswin, DVB_SPIC_CHANNEL_ID);
    }

    if (dtv_ctr.lyr_sel_num)
    {
        GUI_LyrWinDelete(dtv_ctr.lyr_sel_num);
        dtv_ctr.lyr_sel_num = NULL;
        __msg("lyr_sel_num <= 0x%08x", dtv_ctr.lyr_sel_num);
    }

    dtv_ctr.sel_sta = 0;
    dtv_ctr.sel_num = 0;
    dtv_ctr.sel_cnt = 0;
}
//数字选台界面显示
__s32 dtv_show_select_number(__gui_msg_t *msg)
{
    __u32 x1 = 20;
    __u32 y1 = 10;
    char item_str[1][128];
    //GUI_MEMDEV_Handle draw_mem;
    GUI_RECT gui_rect1;
    RECT lyr_rect;
    __u32 bmp_x = 0;
    __u32 bmp_y = 0;
    int i;
    int number;

    if (dtv_ctr.lyr_sel_num == NULL)
    {
        RECT rect;
        dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
        lyr_rect.x      = (rect.width - 240) / 2;
        lyr_rect.y      = (rect.height - 136) / 2 - 30;
        lyr_rect.width  = 240;
        lyr_rect.height = 136;
        dtv_ctr.lyr_sel_num = dtv_com_layer_create(&lyr_rect, "digital layer");
        __msg("lyr_sel_num <= 0x%08x", dtv_ctr.lyr_sel_num);
        GUI_LyrWinSetSta(dtv_ctr.lyr_sel_num, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(dtv_ctr.lyr_sel_num);
        GUI_UC_SetEncodeUTF8();
        GUI_SetFont(SWFFont);
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetColor(GUI_WHITE);
        GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_sel_bg_bmp), 0, 0); //显示背景
    }

    //reset:
    GUI_LyrWinSel(dtv_ctr.lyr_sel_num);
    GUI_UC_SetEncodeUTF8();
    GUI_SetFont(SWFFont);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetColor(GUI_WHITE);
    //draw_mem = GUI_MEMDEV_Create(0, 0, BG_BMP_WIDTH, BG_BMP_HEIGHT);
    //  __msg("GUI_MEMDEV_Create() => 0x%08X ", draw_mem);
    //if(draw_mem)
    //{
    //  GUI_MEMDEV_Select(draw_mem);
    //}
    GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_sel_bg_bmp), 0, 0); //显示背景

    if (dtv_ctr.sel_sta == 3)
    {
        {
            gui_rect1.x0 = x1;
            gui_rect1.y0 = y1 + 10;
            gui_rect1.x1 = BG_BMP_WIDTH - x1;
            gui_rect1.y1 = gui_rect1.y0 + 28;
            //GUI_ClearRectEx(&gui_rect2);//清除区域
            dsk_langres_get_menu_text(STRING_DTV_MANU_SEARCH, item_str, GUI_TITLE_MAX); //显示 输入频道
            GUI_DispStringInRect((const char *)item_str, &gui_rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);
            number = dtv_ctr.sel_num;

            for (i = 0; i < 3; i++)
            {
                int num;
                num = number % 10;
                number = number / 10;
                bmp_x = (BG_BMP_WIDTH - 30 * 4) / 2 + 30 * (3 - i);
                bmp_y = y1 + 60;

                if (dtv_ctr.sel_cnt > i)
                {
                    GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_sel_num_bmp[num]), bmp_x, bmp_y);
                }
                else
                {
                    GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_sel_no_num_bmp), bmp_x, bmp_y);
                }
            }
        }
    }
    else if (dtv_ctr.sel_sta == 2)
    {
        gui_rect1.x0 = x1;
        gui_rect1.y0 = y1 + 40;
        gui_rect1.x1 = BG_BMP_WIDTH - x1;
        gui_rect1.y1 = gui_rect1.y0 + 28;
        //GUI_ClearRectEx(&gui_rect2);//清除区域
        dsk_langres_get_menu_text(STRING_DTV_SEL_ERR, item_str, GUI_TITLE_MAX);//显示 找不到指定频道
        GUI_DispStringInRect((const char *)item_str, &gui_rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);
        //  if(draw_mem)
        //  {
        //      GUI_MEMDEV_CopyToLCD(draw_mem);
        //      GUI_MEMDEV_Select( NULL );
        //      GUI_MEMDEV_Delete(draw_mem);
        //        draw_mem = NULL;
        //  }
        //esKRNL_TimeDly(150);
        //dtv_ctr.sel_sta=1;
        //dtv_ctr.sel_num = 0;
        //dtv_ctr.sel_cnt = 0;
        //goto reset;
    }

    if (dtv_ctr.sel_sta == 0 || (dtv_ctr.sel_sta == 1))
    {
        gui_rect1.x0 = x1;
        gui_rect1.y0 = y1 + 10;
        gui_rect1.x1 = BG_BMP_WIDTH - x1;
        gui_rect1.y1 = gui_rect1.y0 + 28;
        //GUI_ClearRectEx(&gui_rect2);//清除区域
        dsk_langres_get_menu_text(STRING_DTV_SEL_CH, item_str, GUI_TITLE_MAX); //显示 输入频道
        GUI_DispStringInRect((const char *)item_str, &gui_rect1, GUI_TA_HCENTER | GUI_TA_VCENTER);
        number = dtv_ctr.sel_num;
        //if(number > 0)
        {
            for (i = 0; i < 3; i++)
            {
                int num;
                num = number % 10;
                number = number / 10;
                bmp_x = (BG_BMP_WIDTH - 30 * 4) / 2 + 30 * (3 - i);
                bmp_y = y1 + 60;

                if (dtv_ctr.sel_cnt > i)
                {
                    GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_sel_num_bmp[num]), bmp_x, bmp_y);
                }
                else
                {
                    GUI_BMP_Draw(dsk_theme_hdl2buf(dtv_sel_no_num_bmp), bmp_x, bmp_y);
                }
            }
        }
        /*shiql add for backboor*/
        {
            extern __s32 open_back_boor;
            __log("sel_num:%d %s %d", dtv_ctr.sel_num, __FILE__, __LINE__);

            if (dtv_ctr.sel_num == 987)
            {
                open_back_boor   = EPDK_TRUE;
            }
            else if (dtv_ctr.sel_num == 789)
            {
                open_back_boor   = EPDK_FALSE;
            }
        }
    }

    //  /if(draw_mem)
    //  {
    //      GUI_MEMDEV_CopyToLCD(draw_mem);
    //      GUI_MEMDEV_Select( NULL );
    //      GUI_MEMDEV_Delete(draw_mem);
    //       draw_mem = NULL;
    //  }
    return EPDK_OK;
}
#endif



void dtv_check_cmmb_disk_format(void)
{
    ES_FILE *fp = NULL;
    //存贮盘符是否存在，不存在则需要格式盘符
    fp = eLIBs_fopen(DTV_BASEINFO_NAME, "rb");

    if (fp == NULL)
    {
#if 0
        fp = eLIBs_fopen(DTV_BASEINFO_NAME, "wb");

        if (fp == NULL) //写不成功时才格式化Z盘
        {
            eLIBs_format("z:\\", "fat", 0);
            esKRNL_TimeDly(10);
        }
        else
        {
            //创建文件只为测试盘符是否已经格式化
            eLIBs_fclose(fp);
            fp = NULL;
            eLIBs_remove(DTV_BASEINFO_NAME);
        }

#endif
    }
    else
    {
        //文件存在，证明盘符已经格式化
        eLIBs_fclose(fp);
        fp = NULL;
    }
}
/*
**********************************************************************************************************************
*                                               dtv_init
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
void dtv_init(void)
{
    __wrn("sizeof(dtv_ctr):%d", sizeof(dtv_ctr));
    /*create semphore*/
    //eLIBs_memset(&dtv_ctr, 0x0, sizeof(dtv_ctr));
    dtv_ctr.sem = esKRNL_SemCreate(1);
    dtv_check_cmmb_disk_format();
    dtv_ctr.sel_sta = 0;        // no select
    //  dtv_ctr.sel_num = 0;
    //  dtv_ctr.sel_cnt = 0;
}

/*
**********************************************************************************************************************
*                                               dtv_uninit
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

void dtv_uninit(void)
{
    __u8 err;
    //__s32 i;
    esKRNL_SemDel(dtv_ctr.sem, OS_DEL_ALWAYS, &err);
    /*enable auto sleep*/
    g_enable_close_scn();
    g_enable_standby();
    __inf("dtv uninit success!");
}

#if 0
void dtv_set_next_channel2play(void)
{
    __s32 index ;
    index = dtv_get_cur_user_index();
    index++;

    if (index > (maple_get_cur_service_list_num() - 1))
    {
        index = 0;
    }

    dtv_set_cur_user_index(index);
}
#endif
enum
{
    DIALOG_CLOSE_NO_CHANNEL = 1,
    DIALOG_CLOSE_NO_SINGAL,
};
#if 0
__s32 dtv_send_cmd2close_dialog(__s32 dlg_id)
{
    __gui_msg_t dmsg;
    dmsg.id = GUI_MSG_COMMAND;
    dmsg.h_deswin = GUI_WinGetHandFromName(APP_DTV);
    dmsg.dwAddData1 = DVB_PROT_DIALOG_ID ;
    dmsg.dwAddData2 = dlg_id;
    GUI_SendNotifyMessage(&dmsg);
    return EPDK_OK;
}
#endif
//是否存在子场景
__bool dtv_has_sub_scene(void)
{
    if (dtv_ctr.h_lyr)
    {
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}


#if 0
//节目预告中需要用于时间校准,确保获取的是想要的指定的那一天的时间
__s32 dtv_time_check(void)
{
    maple_time_t        current_tv_time;
    __awos_time_t            sys_time;
    __date_t            sys_date;
    maple_time_monitor_get(&current_tv_time);

    if ((current_tv_time.year > 2050) || (current_tv_time.year  < 2000)
        || (current_tv_time.month >  12) || (current_tv_time.month <  1)
        || (current_tv_time.day   >  31) || (current_tv_time.day   <  1))
    {
        esTIME_GetDate(&sys_date);
        esTIME_GetTime(&sys_time);

        if ((sys_date.year > 2050) || (sys_date.year  < 2000)
            || (sys_date.month >  12) || (sys_date.month <  1)
            || (sys_date.day   >  31) || (sys_date.day   <  1))
        {
            sys_date.year   = 2010;
            sys_date.month  = 1;
            sys_date.day    = 1;
            sys_time.hour   = 0;
            sys_time.minute = 0;
            sys_time.second = 0;
            esTIME_SetDate(&sys_date);
            esTIME_SetTime(&sys_time);
        }

        tv_time->MJD = ymd2mjd(sys_date.year, sys_date.month, sys_date.day);
        tv_time->year =  sys_date.year;
        tv_time->month = sys_date.month;
        tv_time->day = sys_date.day;
        tv_time->hour = sys_time.hour;
        tv_time->min = sys_time.minute;
        tv_time->sec = sys_time.second;
        getweekday(tv_time->year, tv_time->month, tv_time->day, &(tv_time->weekDay));
    }
    else
    {
        tv_time->MJD = current_tv_time.MJD;
        tv_time->year =  current_tv_time.year;
        tv_time->month = sys_date.month;
        tv_time->day = current_tv_time.day;
        tv_time->hour = current_tv_time.hour;
        tv_time->min = current_tv_time.min;
        tv_time->sec = current_tv_time.sec;
        tv_time->weekDay = current_tv_time.weekDay;
    }

    __msg("pmtId = %d servId = %d channelId=%d name = %s", service_list->pServItems[dtv_ctr.cur_user_index].pmtId, \
          service_list->pServItems[dtv_ctr.cur_user_index].servId, \
          service_list->pServItems[dtv_ctr.cur_user_index].channelId, \
          service_list->pServItems[dtv_ctr.cur_user_index].servName);
    return EPDK_OK;
}
#endif



//信号的强弱检测
//根据信号强弱给出弱信号提示或频道播放都放在此函数中

//之所以放在GUI TIMER 定时器中检测，因为线程的优先级取太高太低都不合适
//放在定时器中可以确保消息得到及时处理，减小换台间隔
__s32 dtv_singal_strength_detect(H_WIN hand)
{
#if 0
    __s32       ret;
    OS_SEM_DATA p_sem_data;
    __maple_status_t sta;
    maple_demod_ss_t ss = {0};
    {
        esKRNL_SemQuery(dtv_ctr.sem, &p_sem_data);

        if (p_sem_data.OSCnt <= 0)
        {
            return EPDK_FAIL;
        }

        esKRNL_SemPend(dtv_ctr.sem, 0, NULL);
        //maple_get_ss(&ss);
        //gscene_hbar_set_sig_status(ss.strength,ss.quality);
        //想要显示信号状态，在需要的地方可以使能
        esKRNL_SemPost(dtv_ctr.sem);
    }
#endif
    return EPDK_OK;
}


__s32 dtv_show_signal_status(__bool en)
{
    maple_demod_ss_t ss = {0};
    HBarState state;
    maple_get_ss(&ss);

    //  if(ss.strength > 5 || ss.quality>100)// will display number
    if (ss.strength >= 100 || ss.quality >= 100)
    {
        ss.strength = 0;
        ss.quality = 0;
    }

    gscene_hbar_get_state(&state);

    if (state == HBAR_ST_SHOW)
    {
        gscene_hbar_set_sig_status(ss.strength, ss.quality, en);
    }

    if (ss.locked == 0)
    {
        if (++dtv_ctr.hide_vol_cnt >= 5)
        {
            gscene_hbar_set_state(HBAR_ST_SHOW);
            gscene_hbar_set_sig_status(ss.strength, ss.quality, en);
            //dtv_show_signal_status(EPDK_TRUE);
        }
    }
    else if (ss.locked == 1 && (state == HBAR_ST_SHOW))
    {
        if (++dtv_ctr.hide_vol_cnt >= 5)
        {
            //dtv_ctr.show_vol_cnt = 0;
            dtv_ctr.hide_vol_cnt = 0;
            gscene_hbar_set_state(HBAR_ST_HIDE);
            __wrn("maple_get_play_status()=%d ", maple_get_play_status());
        }

        if (MAPLE_STAT_PLAY != maple_get_play_status())
        {
            maple_play_program();
            check_dtv_or_radio();
        }
    }

    return 0;
}

//exp 0:delete all
//删除所有的子场景
//搜台或退出应用时调用
//或需要切换界面时简单粗暴的删除当前所有界面，然后重建
void dtv_delete_all_menu(__u32 exp)
{
    __wrn("dtv_delete_all_menu().");
    __wrn("dtv_ctr.h_lyr        : 0x%08x", dtv_ctr.h_lyr);
    __wrn("dtv_ctr.h_scan       : 0x%08x", dtv_ctr.h_scan);
    __wrn("dtv_ctr.h_setting    : 0x%08x", dtv_ctr.h_setting);
    __wrn("dtv_ctr.h_epg        : 0x%08x", dtv_ctr.h_epg);
    __wrn("dtv_ctr.h_serverlist : 0x%08x", dtv_ctr.h_serverlist);

    if (dtv_ctr.lyr_sel_num)
    {
        GUI_LyrWinDelete(dtv_ctr.lyr_sel_num);
        dtv_ctr.lyr_sel_num = NULL;
    }

    if (GUI_IsTimerInstalled(g_dtv_mainwin, DVB_LOADING_TIMER_ID))
    {
#ifdef DTV_LOADING_ENABLE
        dtv_loading_hide();
#endif
    }

    if (dtv_ctr.h_no_channel /*&& (exp&DELETE_SCENE_EXP_NO_CHANNEL == 0)*/)
    {
        app_dialog_destroy(dtv_ctr.h_no_channel);
        dtv_ctr.h_no_channel = NULL;
    }

    if (dtv_ctr.h_no_signal)
    {
        app_dialog_destroy(dtv_ctr.h_no_signal);
        dtv_ctr.h_no_signal = NULL;
    }

    if ((exp & DTV_NO_AUTH) != 1)
    {
        if (dtv_ctr.h_no_auth)
        {
            __log("%s %d will destroy h_no_auth!", __FILE__, __LINE__);
            app_dialog_destroy(dtv_ctr.h_no_auth);
            dtv_ctr.h_no_auth = NULL;
        }
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

    if (dtv_ctr.h_lyr)
    {
        GUI_LyrWinSetBottom(dtv_ctr.h_lyr);
    }

    if (NULL != dtv_ctr.h_scan) //frm win
    {
        GUI_LyrWinDelete(dtv_ctr.h_lyr);
        dtv_ctr.h_scan = NULL;
        dtv_ctr.h_lyr = NULL;
    }

    if (NULL != dtv_ctr.h_epg) //man win
    {
        GUI_ManWinDelete(dtv_ctr.h_epg);
        dtv_ctr.h_epg = NULL;
    }

    if (NULL != dtv_ctr.h_serverlist) //man win
    {
        GUI_ManWinDelete(dtv_ctr.h_serverlist);
        dtv_ctr.h_serverlist = NULL;
    }

    __wrn("dtv_ctr.h_setting    : 0x%08x", dtv_ctr.h_setting);

    if (NULL != dtv_ctr.h_setting) //man win
    {
        GUI_ManWinDelete(dtv_ctr.h_setting);
        dtv_ctr.h_setting = NULL;
    }

    if (dtv_ctr.h_lyr)
    {
        GUI_LyrWinDelete(dtv_ctr.h_lyr);
        dtv_ctr.h_lyr = NULL;
    }
}

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
void dtv_subtitle_proc(void)
{
    dtv_ctr.b_has_subtitle = dtv_has_subtitle();

    if (dtv_ctr.b_has_subtitle)
    {
        dtv_subtitle_switch_onoff();
    }
}

void dtv_subtitle_resume(void)
{
    if (dtv_ctr.b_has_subtitle)
    {
        dtv_ctr.b_has_subtitle = EPDK_FALSE;
        dtv_subtitle_switch_onoff();
    }
}

#endif
#if 0
void  tv_short_key_cmd2parent(H_WIN hwin, __s32 data1, __s32 data2, __u32 dw)
{
    __gui_msg_t msg;
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = hwin;//NULL;
    msg.id = GUI_MSG_SHORTKEY;
    msg.dwAddData1 = data1;
    msg.dwAddData2 = data2;
    msg.dwReserved = dw;
    GUI_SendNotifyMessage(&msg);
}

//extern void notify_close_screen(void);

__s32 __dtv_switch_output(void)
{
    //切换输出
    {
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            __msg("para->pal_ntsc = %d", para->pal_ntsc);

            if (para->output != LION_DISP_LCD)
            {
                __wrn("g_display_switch_output LCD. ");
                g_display_switch_output(LION_DISP_LCD);
            }
            else if (0 == para->pal_ntsc)
            {
                __wrn("g_display_switch_output TV NTSC. ");
                g_display_switch_output(LION_DISP_TV_NTSC);
            }
            else
            {
                __wrn("g_display_switch_output TV PAL. ");
                g_display_switch_output(LION_DISP_TV_PAL);
            }
        }

        //      else
        //      {
        //          notify_close_screen();
        //      }
    }
    return EPDK_OK;
}
#endif





//搜台
//1、创建layer
//2、创建搜台frmwin
//3、启动搜台线程
__s32 dtv_search_program_start(__gui_msg_t  *msg, __u8 scan_type, __u32 index)
{
    __s32 ret = 0;
    scan_para_t *scan_para = NULL;
    extern  maple_demod_info_t *demodParam;
    extern maple_serv_list_t *search_service_list;
    //esKRNL_SemPend(dtv_ctr.sem,0,NULL);//用于各界面的同步
    g_is_searching = 1;
    __msg("dtv_search_program_start()...");
    //
    app_misc_audio_mute(0);     // mute on
    clear_dtv_select_number(msg);

    if (dtv_ctr.h_lyr == NULL)
    {
        RECT rect;
        dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
        rect.x = 0;
        rect.y = 0;
        ret = dtv_layer_create(&rect);

        if (EPDK_FAIL == ret)
        {
            //esKRNL_SemPost(dtv_ctr.sem);
            return EPDK_FAIL;
        }
    }

    __wrn("....1....");
    dtv_ctr.h_scan = submenu_dvb_scan_proc_create(msg->h_deswin, dtv_ctr.h_lyr);

    if (NULL != dtv_ctr.h_scan)
    {
        GUI_WinSetFocusChild(dtv_ctr.h_scan);
    }
    else
    {
        dtv_delete_all_menu(0);
        //esKRNL_SemPost(dtv_ctr.sem);
        return EPDK_FAIL;
    }

    __wrn("....2....");
    scan_para = (scan_para_t *)eLIBs_malloc(sizeof(scan_para_t));

    if (scan_para == NULL)
    {
        __wrn("scan_para eLIBs_malloc fail");
        return EPDK_FAIL;
    }

    eLIBs_memset(scan_para, 0x00, sizeof(scan_para_t));
    demodParam = (maple_demod_info_t *)eLIBs_malloc(sizeof(maple_demod_info_t));

    if (demodParam == NULL)
    {
        __wrn("demodParam create fail.");
        return EPDK_FAIL;
    }

    eLIBs_memset(demodParam, 0x00, sizeof(maple_demod_info_t));

    if (scan_type == MANUAL_SERACH_TYPE)
    {
        Region_TPSects_t *temp = NULL;
        extern const Region_TPSects_t TPRegion_England[];
        scan_para->scan_type = MANUAL_SERACH_TYPE;
        temp = (Region_TPSects_t *)eLIBs_malloc(sizeof(Region_TPSects_t) * 1);

        if (temp == NULL)
        {
            __wrn("scan_para->pTPRegion_Area eLIBs_malloc fail");
            return EPDK_FAIL;
        }

        eLIBs_memset(temp, 0x00, sizeof(Region_TPSects_t) * 1);
        temp->StartFreq = TPRegion_England[index].StartFreq;
        temp->BandWidth = TPRegion_England[index].BandWidth;
        scan_para->TPIndex = 1;
        scan_para->pTPRegion_Area = temp;
    }
    else if (scan_type == ALL_FREQ_SERACH_TYPE)
    {
        scan_para->scan_type = ALL_FREQ_SERACH_TYPE;
        maple_all_freq_table(scan_para);
    }
    else if (scan_type == AUTO_SERACH_TYPE)
    {
        __wrn("%s %d", __FILE__, __LINE__);
        scan_para->scan_type = AUTO_SERACH_TYPE;
        maple_auto_search_major(scan_para);
        __wrn("%s %d", __FILE__, __LINE__);
    }

    ret = maple_area_search(scan_para);
    //esKRNL_SemPost(dtv_ctr.sem);
    return ret;
}

__s32 dtv_schedule_list_menu_create(__gui_msg_t  *msg)
{
    __s32 ret = 0;
    esKRNL_SemPend(dtv_ctr.sem, 0, NULL); //用于各界面的同步
    __msg("dtv_schedule_list_menu_create()...");
    clear_dtv_select_number(msg);

    if (dtv_ctr.h_lyr == NULL)
    {
        RECT rect;
        dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
        rect.x = (rect.width - LISTBAR_WIDTH) / 2;
        rect.y = 47;
        rect.width = LISTBAR_WIDTH;
        rect.height = rect.height - rect.y;
        ret = dtv_layer_create(&rect);

        if (EPDK_FAIL == ret)
        {
            esKRNL_SemPost(dtv_ctr.sem);
            return EPDK_FAIL;
        }
    }

    dtv_ctr.h_epg = dtv_schedule_create(msg->h_deswin, dtv_ctr.h_lyr);

    if (NULL != dtv_ctr.h_epg)
    {
        GUI_WinSetFocusChild(dtv_ctr.h_epg);
    }
    else
    {
        __msg("epg=NULL");
        dtv_delete_all_menu(0);
        esKRNL_SemPost(dtv_ctr.sem);
        return EPDK_FAIL;
    }

    esKRNL_SemPost(dtv_ctr.sem);
    return ret;
}


__s32 dtv_preview_menu_create(__gui_msg_t  *msg)
{
    __s32 ret = 0;
    esKRNL_SemPend(dtv_ctr.sem, 0, NULL); //用于各界面的同步
    __msg("dtv_preview_menu_create()...");
    clear_dtv_select_number(msg);

    if (dtv_ctr.h_lyr == NULL)
    {
        RECT rect;
        dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
        rect.x = (rect.width - LISTBAR_WIDTH * 2 / 3);
        rect.y = 47;
        rect.width = LISTBAR_WIDTH * 2 / 3;
        rect.height = rect.height - rect.y;
        ret = dtv_layer_create(&rect);

        if (EPDK_FAIL == ret)
        {
            esKRNL_SemPost(dtv_ctr.sem);
            return EPDK_FAIL;
        }
    }

    dtv_ctr.h_serverlist = dtv_program_preview_create(msg->h_deswin, dtv_ctr.h_lyr);

    if (NULL != dtv_ctr.h_serverlist)
    {
        __msg("dtv_ctr.h_serverlist : 0x%08x", dtv_ctr.h_serverlist);
        GUI_WinSetFocusChild(dtv_ctr.h_serverlist);
    }
    else
    {
        dtv_delete_all_menu(0);
        esKRNL_SemPost(dtv_ctr.sem);
        return EPDK_FAIL;
    }

    esKRNL_SemPost(dtv_ctr.sem);
    return ret;
}

__s32 dtv_setting_menu_create(__gui_msg_t  *msg)
{
    __s32 ret = 0;

    if (dtv_ctr.h_setting)
    {
        __wrn("--------------setting alreadly on.");
    }

    __wrn("dtv_setting_menu_create1.");
    //esKRNL_SemPend(dtv_ctr.sem,0,NULL);//用于各界面的同步
    __wrn("dtv_setting_menu_create2.");
    clear_dtv_select_number(msg);

    if (dtv_ctr.h_lyr == NULL)
    {
        RECT rect;
        dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
        rect.x = (rect.width - LISTBAR_WIDTH) / 2;
        rect.y = 47;
        rect.width = LISTBAR_WIDTH;
        rect.height = rect.height - rect.y;
        ret = dtv_layer_create(&rect);

        if (EPDK_FAIL == ret)
        {
            //esKRNL_SemPost(dtv_ctr.sem);
            return EPDK_FAIL;
        }
    }

    __wrn("dtv_setting_menu_create()...");
    dtv_ctr.h_setting = dtv_setting_create(msg->h_deswin, dtv_ctr.h_lyr);
    __wrn("dtv_ctr.h_setting:0x%x", dtv_ctr.h_setting);

    if (NULL != dtv_ctr.h_setting)
    {
        __wrn("Test1.");
        GUI_WinSetFocusChild(dtv_ctr.h_setting);
        __wrn("dtv_ctr.h_setting    : 0x%08x", dtv_ctr.h_setting);
        __wrn("Test2.");
    }
    else
    {
        __wrn("Test1.");
        clear_dtv_select_number(msg);
        __wrn("Test2.");
        dtv_delete_all_menu(0);
        __wrn("Test3.");
        __wrn("Test4.");
        //esKRNL_SemPost(dtv_ctr.sem);
        return EPDK_FAIL;
    }

    //esKRNL_SemPost(dtv_ctr.sem);
    return ret;
}

/*
**********************************************************************************************************************
*                                               dtv_main_proc
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
extern __mp *maple;
extern  __s32 dtv_subtitle_paint(void *arg);


#if 0
__s32 is_tv_out_on(void)
{
    user_gpio_set_t gpio_set = {0};
    __hdle tv_out_hdl;
    __s32 ret;
    //  return EPDK_TRUE;
    ret = esCFG_GetKeyValue("matrixkey_para", "tv_out_det", (int *)&gpio_set, sizeof(user_gpio_set_t) / 4);

    if (!ret)
    {
        __wrn("esCFG_GetKeyValue( cmmb_power_ctrl ) : success ");
        tv_out_hdl = esPINS_PinGrpReq(&gpio_set, 1);

        if (!tv_out_hdl)
        {
            __wrn("request TV power pin failed");
            return EPDK_FALSE;
        }

        esPINS_SetPinIO(tv_out_hdl, 0, NULL);
        //      ret = esPINS_SetPinPull(tv_out_hdl, PIN_PULL_UP, NULL);
        //      if (ret)
        //      {
        //          __wrn("pull input_row1 failed");
        //          return EPDK_FAIL;
        //      }
        ret = esPINS_ReadPinData(tv_out_hdl, 0);
        esPINS_PinGrpRel(tv_out_hdl, 2);
        return (ret ? EPDK_FALSE : EPDK_TRUE);
    }
    else
    {
        __wrn("fetch tv_out_det from script failed");
        //      return EPDK_FALSE;
    }

    return EPDK_FALSE;
}
#endif


//判断当前是否处在TV-OUT模式
__bool is_on_tv(void)
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

//extern __s32 __root_switch_output(void);

//根据注册表信息进行切换NTFS OR PAL
__s32 __root_switch_output(void)
{
    //切换输出
    {
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            __wrn("para->pal_ntsc = %d, para->output = %d", para->pal_ntsc, para->output);

            if (0 == para->pal_ntsc)
            {
                if (para->output == LION_DISP_LCD)
                {
                    __wrn("g_display_switch_output TV NTSC ");
                    g_display_switch_output(LION_DISP_TV_NTSC);
                }
                else
                {
                    __wrn("g_display_switch_output LCD ");
                    g_display_switch_output(LION_DISP_LCD);
                }
            }
            else
            {
                if (para->output == LION_DISP_LCD)
                {
                    __wrn("g_display_switch_output TV PAL ");
                    g_display_switch_output(LION_DISP_TV_PAL);
                }
                else
                {
                    __wrn("g_display_switch_output LCD ");
                    g_display_switch_output(LION_DISP_LCD);
                }
            }
        }
    }
    return EPDK_OK;
}


__s32 __root_switch_output_NTSC_PAL(__gui_msg_t *msg)
{
    H_WIN    h_dialog = NULL;
    //if(EPDK_TRUE == is_on_tv())
    //切换输出
    {
        reg_system_para_t *para;
        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (para)
        {
            __inf("para->pal_ntsc = %d", para->pal_ntsc);

            if (0 == para->pal_ntsc) //NTSC
            {
                if (para->output == LION_DISP_LCD)
                {
                    g_display_switch_output(LION_DISP_TV_NTSC);
                    __wrn("**************** LION_DISP_LCD ");
                    return EPDK_OK;
                }
                else
                {
                    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_SWITCH_TO_PAL};
                    default_dialog(h_dialog, msg->h_deswin, DTV_TVOUT_ID, ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);

                    if (h_dialog)
                    {
                        app_dialog_destroy(h_dialog);
                        h_dialog = NULL;
                    }

                    para->pal_ntsc = 1; //PAL
                    g_display_switch_output(LION_DISP_TV_PAL);
                }
            }
            else  // = 1:PAL
            {
                if (para->output == LION_DISP_LCD)
                {
                    g_display_switch_output(LION_DISP_TV_PAL);
                    __wrn("**************** LION_DISP_LCD ");
                    return EPDK_OK;
                }
                else
                {
                    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_SWITCH_TO_NTSC};
                    default_dialog(h_dialog, msg->h_deswin, DTV_TVOUT_ID, ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);

                    if (h_dialog)
                    {
                        app_dialog_destroy(h_dialog);
                        h_dialog = NULL;
                    }

                    para->pal_ntsc = 0;//NTSC
                    g_display_switch_output(LION_DISP_TV_NTSC);
                }
            }
        }
    }
    return EPDK_OK;
}


__s32 __root_switch_to_tv_or_lcd(__gui_msg_t *msg)
{
    H_WIN    h_dialog = NULL;

    if (EPDK_TRUE == is_on_tv())
    {
        __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_EX_TVOUT };
        //__here__;
        default_dialog(h_dialog, msg->h_deswin, DTV_TVOUT_ID, ADLG_OK, lang_id);
    }
    else
    {
        __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_TVOUT};
        default_dialog(h_dialog, msg->h_deswin, DTV_TVOUT_ID, ADLG_OK, lang_id);
    }

    esKRNL_TimeDly(100);

    if (h_dialog)
    {
        app_dialog_destroy(h_dialog);
        h_dialog = NULL;
    }

    __root_switch_output();
    return EPDK_OK;
}

enum
{
    CUR_MENU_SEARCH = 1,
    CUR_MENU_SETTING,
    CUR_MENU_PREVIEW,
    CUR_MENU_EPG,
};


