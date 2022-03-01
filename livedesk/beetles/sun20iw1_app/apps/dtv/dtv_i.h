/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              willow app sample
*
*                               (c) Copyright 2006-2007, JOHN, China
*                                           All Rights Reserved
*
* File    : movie_i.h
* By      : john.fu
* Version : V1.00
*********************************************************************************************************
*/
#ifndef _DVBPLAY_I_H_
#define _DVBPLAY_I_H_


//#include "epdk.h"
#include <emodules/mod_defs.h>
#include "beetles_app.h"
#include "dtv.h"
#include "dtv_mid.h"
#include "program_preview.h"
#include "schedule.h"
#include "scan_proc.h"
#include "dtv_setting.h"
#include "dtv_subtitle_ui.h"
#include "app_loading.h"
#include "dtv_setting_ca.h"
#include <emodules/mod_ca.h>
//#include "dtv_ui.h"

extern dtv_ctr_t  dtv_ctr;
extern H_WIN g_dtv_mainwin;
extern H_WIN g_dvbscan_win;
extern __u32 dtv_mute_status;
extern __s32 g_is_searching;
extern __u32 sl_task_id;
extern __bool b_full_screen;
extern maple_serv_list_t *service_list;
extern maple_schdl_list_t      *schdl_list;
extern dtvmid_time_t *tv_time;
extern char *sepg_string_c;
extern char *sepg_string_n;
extern __u32  sl_task_id;
extern __s32 g_weak_singal;
extern __s32 g_mute_flag;
extern __s32 g_record_flag;
extern maple_ve_status_e *g_maple_ve_status;
extern maple_sl_cb_param_t *sl_event_cbp;

extern __s32 g_is_searching;

//extern __u32 G_DTV_current_area;

//extern __bool     bpreview_submenu_exist;

extern __s32 cur_playing_index;

extern __s32 g_is_programme_list_selected;

#define  KEY_NUM_DELAY    (1000)

#define  DTV_FLUSH_DELAY    (1000)





#include "dtv_uipara.h"

#define SUBMENU_DVB_SCAN            "SCAN SUB MENU"        //搜索

#define SUBMENU_DVB_SCHUDULE        "SCHUDULE  MENU"    // 节目安排
#define SUBMENU_DVB_SCHUDULE_LST        "SCHUDULE SUB MENU LIST"    // 

#define SUBMENU_DVB_PREVIEW        "PREVIEW  MENU"    // 预览
#define SUBMENU_DVB_PREVIEW_LST        "PREVIEW SUB MENU LIST"    //


#define SUBMENU_DVB_SETTING        "SETTING MENU"    //设置菜单
#define SUBMENU_DVB_SETTING_LST        "SETTING SUB MENU LIST"
#define SUBMENU_DVB_SETTING_DISP        "SETTING SUB MENU DISP"
#define SUBMENU_DVB_SETTING_LCD        "SETTING SUB MENU LCD"
#define SUBMENU_DVB_SETTING_CA        "SETTING SUB MENU CA"
#define SUBMENU_DVB_SETTING_CA_SUB  "SETTING MENU CA SUB"

#define SUBMENU_DVB_SETTING_ANT        "SETTING ANTENNA"
#define SUBMENU_DVB_SETTING_SEARCH        "SETTING SEARCH"


#define SUBMENU_DTV_SG_STRENGTH        "SINGAL STRENGTH"

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))

#define DTV_SUBTITLE                   "DTV SUBTITLE"
#define SUBTITLE_FRMWIN                "SUBTITLE FRMWIN"
#endif



#define SUBMENU_DVB_SCAN_ID     (APP_DTV_ID + 0x10)

#define SUBMENU_DVB_SCHUDULE_ID     (APP_DTV_ID + 0x20)
#define SUBMENU_DVB_SCHUDULE_LIST_ID     (SUBMENU_DVB_SCHUDULE_ID + 0x01)
#define SUBMENU_DVB_SCHUDULE_TIMER_ID     (SUBMENU_DVB_SCHUDULE_ID + 0x02)
#define SUBMENU_DVB_SCHUDULE_DESTROY_TIMERID     (SUBMENU_DVB_SCHUDULE_ID + 0x03)


#define SUBMENU_DVB_PREVIEW_ID          (APP_DTV_ID + 0x30)
#define SUBMENU_DVB_PREVIEW_LIST_ID     (SUBMENU_DVB_PREVIEW_ID + 0x01)
#define SUBMENU_DVB_PREVIEW_TIMERID     (SUBMENU_DVB_PREVIEW_ID + 0x02)
#define SUBMENU_DVB_PREVIEW_UPDATEID     (SUBMENU_DVB_PREVIEW_ID + 0x03)

#define SUBMENU_DVB_SETTING_ID     (APP_DTV_ID + 0x40)
#define SUBMENU_DVB_SETTING_LIST_ID     (SUBMENU_DVB_SETTING_ID + 0x01)
#define SUBMENU_DVB_SETTING_DISP_ID     (SUBMENU_DVB_SETTING_ID + 0x02)
#define SUBMENU_DVB_SETTING_LCD_ID     (SUBMENU_DVB_SETTING_ID + 0x03)
#define SUBMENU_DVB_SETTING_TIMERID     (SUBMENU_DVB_SETTING_ID + 0x04)
#define SUBMENU_DVB_SETTING_DLG_ID     (SUBMENU_DVB_SETTING_ID + 0x05)

#define SUBMENU_DVB_SINGAL_ID     (APP_DTV_ID + 0x50)
#define DVB_SINGAL_TIMER_ID     (SUBMENU_DVB_SINGAL_ID + 0x01)//信号状态定时器，控制频道播放、弱信号时的处理
#define DVB_VOL_TIMER_ID        (SUBMENU_DVB_SINGAL_ID + 0x02)
#define DVB_AUTH_TIMER_ID       (SUBMENU_DVB_SINGAL_ID + 0x03)
#define DVB_LOADING_TIMER_ID        (SUBMENU_DVB_SINGAL_ID + 0x04)
#ifdef APP_ADD_RECORD_VEDIO
#define DVB_RECORD_TS_BACK_DOOR_TIMER           (SUBMENU_DVB_SINGAL_ID + 0x05)
#define DVB_RECORD_TS_BACK_DOOR_TIMER_OUT       (500)
#define DVB_RECORD_TS_TRRIGER_NUM               (8)
#endif


#define SUBMENU_DVB_CA_ID     (APP_DTV_ID + 0x60)
#define SUBMENU_DVB_CA_SUB_ID     (APP_DTV_ID + 0x60 + 1)

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))

#define DTV_SUBTITLE_ID                (APP_DTV_ID + 0x70)
#define SUBTITLE_FRMWIN_ID             (DTV_SUBTITLE_ID + 0x01)
#endif


#define DTV_TVOUT_ID                (APP_DTV_ID + 0x80)

#define SUBMENU_DVB_SETTING_antenna_ID  (APP_DTV_ID + 0x90)

#define SUBMENU_DVB_SETTING_SEARCH_ID  (APP_DTV_ID + 0xA0)

#define DTV_ROLLING_TEXT_ID     (APP_DTV_ID + 0xA1)

#define  DVB_LOADING_ID         (DTV_TVOUT_ID+1)      //启动加载动画定时器
#define  DVB_SPIC_CHANNEL_ID    (DTV_TVOUT_ID+2)      //数字键切台定时器，用于定时隐藏框
#define  DVB_PROT_DIALOG_ID     (DTV_TVOUT_ID+3)      //no use any more
#define  DVB_BATTERY_TIMER_ID   (DTV_TVOUT_ID+4)
#define  DVB_TVOUT_TIMER_ID     (DTV_TVOUT_ID+5)
#define  DVB_FLUSH_ID           (DTV_TVOUT_ID+6)
#define  DVB_ROLLING_TXT_TIMER_ID    (DTV_TVOUT_ID+7)
#define  DTMB_CA_PREAUTH_TIMER_ID           (APP_DTV_ID + 0xB0)
#define  DTMB_CA_CARDID_TIMER_ID            (APP_DTV_ID + 0xB1)
#define  DTMB_CA_ROLLTEXT_TIMER_ID      (APP_DTV_ID + 0xB2)
#define DVB_SECURE_TIMER_ID        (SUBMENU_DVB_SINGAL_ID + 0x06)



//#define DTV_AUTO_SEARCH_ENABLE


enum
{
    DTV_RESEARCH_SERVICE_LIST = GUI_MSG_USER_DEF + 0x2000, //重新搜台
#ifdef DTV_AUTO_SEARCH_ENABLE
    DTV_AUTO_RESEARCH_SERVICE_LIST,
#endif
    DTV_PREVIEW_SERVICE_LIST,        //进预览界面
    DTV_BACK2PLAY_PROGRAM,          //回归正常播放状态

    DTV_MENU_RESEARCH,//手动搜台
    DTV_2_EPG,


    CMD_TO_UPDATE_SINGAL_STATUS, //更新信号状态
    CMD_TO_UPDATE_VOLUME, //更新音量进度条
    CMD_TO_UPDATE_SYSTIME, //更新系统时间

    CMD_TO_DELETE_SEARCH_THREAD, //删除搜台线程


    CMD_SWITCH_TVOUT_MODE,//切换TVOUT
    DTV_CA_MSG_DISPLAY,
    DTV_CA_MSG_RESUME,
    CMD_CREATE_ROLLING_TXT,
    CMD_DESTROY_ROLLING_TXT,
};

enum
{
    DTV_SIGNAL_LAYER_OFF = 0,       // 完全关闭信号状态条
    DTV_SIGNAL_BATTERY,             // 仅显示电池状态
    DTV_SIGNAL_BAR,                 // 显信号状态示和电池状态
    DTV_SIGNAL_UPDATE
};

#define     LISTBAR_HIGH         35
#define     LISTBAR_WIDTH       311

#define     SETTING_TOP_HIGH    35

#define     DTV_NO_AUTH (1 << 0)
extern  __s32  dtv_layer_create(RECT *rect);
extern  H_WIN dtv_com_layer_create(RECT *rect, char *layer_name);
extern void dtv_delete_all_menu(__u32 exp);
extern __s32 dtv_show_signal_status(__bool en);
#endif/*_MOVIE_I_H_*/

