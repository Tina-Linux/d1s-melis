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
#ifndef __DTV_MID_H__
#define __DTV_MID_H__

//#include "epdk.h"
#include <emodules/mod_defs.h>
#include <emodules/mod_maple.h>
#include "apps.h"

//以下为CMMB/ISDBT需要配置的信息
/**************************************************************/
//可选项 // MAPLE_TV_DVB_T//MAPLE_TV_ISDB_T//MAPLE_TV_DTMB//MAPLE_TV_ATSC
#define  MAPLE_TV_STANDAR   MAPLE_TV_DTMB


//ISDBT字幕需要设置的参数，见dtv_mid.c文件
//#if (MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)
//static __epdk_charset_enm_e g_type = EPDK_CHARSET_ENM_ISO_8859_15; //默认巴西,需要根据出往哪个国家进行设置
//#endif
//static __u32 G_DTV_current_area = 1;   //0:只用于深圳频率，1:全频段搜索

/**************************************************************/

#if((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB) \
        || (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)|| (MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
#define     SUBTITLE_HIGHT   (0)  //ISDBT等字幕预留，不遮挡视频播放，不影响观看效果

#endif

#define  DTV_DATABASE_NAME   "Z:\\program.db"
#define  DTV_BASEINFO_NAME   "Z:\\baseinfo.bin"

#define TRUE 1
#define FALSE 0

#define MAX_EPG_DAY   7
#define MAX_SEPG_STRING 128
#define MAX_TMP_STRING 256

#define MAX_VOLUME 30
#define MAX_LIGHT 4


typedef struct
{
    __u8   flag; //有效标志
    __s16   index;//第几个台
    __s32   total;//台总数
    maple_vd_win_ratio_mode_e mode;
    //maple_serv_list_t service_list;            //这两个参数由中间件保存，
    //maple_serv_item_t service_item[128];//最多128个台
    __u32   reserverd[8]; //保留参数设置
    __u32   time_format;//时制，0代表24小时制，1代表12小时制

    __bool  b_show_subtitle;//是否显示字幕,isdbt才有用

    __u8    default_antenna;
} reg_cmmb_para_t;

typedef enum tag_scan_s
{
    SLIDE_UPDATE_FREQ = 1,//更新频点的同时更新进度条
    SLIDE_UPDATE_CH,

} tag_scan_e;


typedef enum tag_submenu_man_s
{
    DELETE_SEARCH_MENU = 1,


} tag_submenu_man_e;



typedef enum tag_areaset
{
    SSET_NC_AREASET_ALLFREQ,
    SSET_NC_AREASET_SHENZHEN,
    SSET_NC_AREASET_AREAMAX,
} areaset_e;

typedef enum tag_programtype
{
    SSET_NC_PROGRAMTYPE_TV,
    SSET_NC_PROGRAMTYPE_RADIO,
    SSET_NC_PROGRAMTYPE_ALL,
    SSET_NC_PROGRAMTYPE_MAX,
} programtype_e;

typedef enum        /* user define msg id */
{
    DVBSCAN_ID_SET_SLIDE = GUI_MSG_,
    DVBSCAN_ID_SET_FREQ,
    DVBSCAN_ID_SET_PROG_LIST,
    DVBSCAN_ID_SET_BW,
} __dvbscan_cmdid_t;

typedef enum        /* user define msg id */
{
    DVBEPG_ID_SET_SLIDE = GUI_MSG_,
    DVBEPG_ID_SET_FREQ,
    DVBEPG_ID_SET_PROG_LIST,
    DVBEPG_ID_SET_BW,
    DVBEPG_ID_SET_EPG_LIST,
    DVBEPG_ID_CREATE_EPG_LIST,
} __dvbepg_cmdid_t;

typedef enum        /* user define msg id */
{
    DVBADJUST_ID_SET_ADD = GUI_MSG_,
    DVBADJUST_ID_SET_DEC,

} __dvbadjust_cmdid_t;


typedef enum
{
    DTV_ANTENNA_LA = 0, //拉杆天线
    DTV_ANTENNA_OUT,//外接天线
} __dtv_antenna_t;

typedef struct __maple_sl_cb_param_t_pOpaque
{
    void        *pOpaque;
    __gui_msg_t *msg;


} maple_sl_cb_param_t_pOpaque;

typedef struct __maple_schdl_list_day
{
    __u32                 eventNum;       //* how many events totally in the list
    __u32                 MJD;
    __u32                 serviceid;
    maple_schdl_event_t  pSchdlEvents[512];   //* schedule event list

} maple_schdl_list_day;

typedef struct DTVMID_TIME
{
    __u32   year;
    __u32   month;
    __u32    day;
    __u32    weekDay;
    __u32    hour;
    __u32    min;
    __u32    sec;
    __u32   MJD;
} dtvmid_time_t;


//extern   void sprintf(char *buffer, const char *str, ...);
#define sprintf eLIBs_sprintf //modify by Bayden, 140223, use melis interface.


#define MALLOC(x)                       esMEMS_Malloc(0, x)
#define malloc(x)                       esMEMS_Malloc(0, x)
#define FREE(x)                         esMEMS_Mfree(0, x)
#define free(x)                         esMEMS_Mfree(0, x)
#define palloc(x, y)                    esMEMS_Palloc(x, y)
#define pfree(x, y)                     esMEMS_Pfree(x, y)

#define MEMSET(buf, val, size)          eLIBs_memset(buf, val, size)
#define memset(buf, val, size)          eLIBs_memset(buf, val, size)
#define MEMCPY(dst, src, size)          eLIBs_memcpy(dst, src, size)
#define memcpy(dst, src, size)          eLIBs_memcpy(dst, src, size)

#define TOUPPER(n)                      eLIBs_toupper(n)
#define toupper(n)                      eLIBs_toupper(n)
#define STR_NCMP(dst, src, size)        eLIBs_strncmp(dst, src, size)
#define strncmp(dst, src, size)         eLIBs_strncmp(dst, src, size)
#define STR_CMP(s1, s2)                 eLIBs_strcmp(s1, s2)
#define strcmp(s1, s2)                  eLIBs_strcmp(s1, s2)
#define STR_LEN(s)                      eLIBs_strlen(s)
#define strlen(s)                       eLIBs_strlen(s)
#define STR_NCPY(s1, s2, n)             eLIBs_strncpy(s1, s2, n)
#define strncpy(s1, s2, n)              eLIBs_strncpy(s1, s2, n)
#define STR_CPY(s1, s2)                 eLIBs_strcpy(s1, s2)
#define strcpy(s1, s2)                  eLIBs_strcpy(s1, s2)
#define STR_CHR(s, ch)                  eLIBs_strchr(s,ch)
#define strchr(s, ch)                   eLIBs_strchr(s,ch)
#define STR_STR(s1, s2)                 eLIBs_strstr(s1, s2)
#define strstr(s1, s2)                  eLIBs_strstr(s1, s2)
#define strcat(s1, s2)                    eLIBs_strcat(s1, s2)


typedef struct Region_TPSects_s
{
    __u32 StartFreq;            /*Start Transponder Frequency*/
    __u32 BandWidth;      /*Transponder Bandwidth*/
} Region_TPSects_t;

typedef struct tag_scan_para
{
    __u32 TPIndex;
    __u8 scan_type;
    __u8 scan_major;
    const Region_TPSects_t *pTPRegion_Area;
} scan_para_t;

typedef struct DVBT_AreaTable_s
{
    __u32 CountryIndex;
    const Region_TPSects_t *pTPRegion_Area;
    const char *const *pChannelName;
    __u32 TPIndex;
    char CountryCode[3];
} DVBT_AreaTable_t;

__s32 maple_open(void);

__s32 maple_close(void);

__s32 maple_get_antenna_mode(void);
__s32 maple_set_antenna_mode(__u8 mode);


__s32 maple_connect_signal(maple_demod_info_t *tp);
__s32 maple_register_search_callback(__pCBK_t  cb_search_list_event_callback, __pCBK_t cb_search_list_finish_callback);
__s32 maple_search(maple_demod_info_t *tp);
__s32 maple_search_stop(void);

__s32 maple_get_play_status(void);

__s32 maple_program_load(char *database_file_name, __u8 first_load);

__s32 maple_program_add_srvlist(void);
__s32 maple_cur_flash_save_program_index(void);
__s32 maple_set_cur_flash_save_program_index(__s32 index);

__s32 maple_cmmb_reg_flush(void);

__s32 maple_program_save(char *database_file_name);
void dtv_loading_destory_timer(void);
void dtv_loading_show(void);
void dtv_loading_hide(void);
void check_auth_timer(void);
void check_auth_timer_destroy(void);
void check_dtv_have_auth(void);
void check_dtv_or_radio(void);
__s32 maple_play_program(void);
__s32 maple_play_program_ex(RECT *video_window, __s32 index);

__s32 maple_get_freq_from_index(__s32 index);
__s32 maple_get_current_program_freq(void);

__s32 maple_stop_program(void);

__s32 maple_register_epg_callback(__pCBK_t cb_search_schedule_event_callback, __pCBK_t cb_search_schedule_finish_callback);
__s32 maple_epg_search(void);
__s32 maple_epg_search_stop(void);
void maple_epg_reset_schdl_list_day(__u32 serviceid);
__s32 maple_epg_getdata(void);
__s32 maple_epg_get_eventnum(__s32 day_index);
maple_schdl_event_t *maple_epg_get_schdl_event(__s32 day_index, __s32 event_index);
__s32 maple_get_cur_schedule_index(__s32 day_index);


__u32 UI_epg_info_delete(char *item_path);
__u32 UI_epg_info_add(char *item_path);



__u32 UI_item_delete(char *item_path);
__u32 UI_manual_scan_info_add(char *item_path, DVBT_AreaTable_t *areatable);
__s32 maple_thread_delete(__u32 thread_id);
__s32 maple_area_search(scan_para_t *scan_para);
void maple_time_monitor_start(void);

void maple_time_monitor_stop(void);
__s32 maple_time_monitor_get(maple_time_t *tv_time);

__s32 maple_register_sepg_callback(__pCBK_t search_sepg_finish_callback);
__s32 maple_sepg_clean(void);
__s32 maple_sepg_search(void);
//__s32 maple_sepg_search_stop(void);

//__s32 maple_get_ss_status(int *strength,int*quality);

__s32   maple_get_ss(maple_demod_ss_t *ss);


__u8  maple_control_get_lock_status(void);
maple_ve_status_e *maple_get_decode_status(void);

__s32 maple_set_tv_mode(__u8 flag);



__s32 maple_save_reg_current_program(void);
__s32 maple_load_reg_current_program(void);

__s32 maple_save_reg_video_window_ratio_mode(void);
__s32 maple_load_reg_video_window_ratio_mode(void);

__s32 maple_inc_volume(void);
__s32 maple_dec_volume(void);
__s32 maple_get_volume(void);

__s32 maple_inc_light(void);
__s32 maple_dec_light(void);
__s32 maple_get_light(void);

__s32 maple_set_screensize(maple_vd_win_ratio_mode_e mode);
maple_vd_win_ratio_mode_e maple_get_screensize(void);

__s32 maple_set_init_screensize(void);

__s32 maple_set_area(__u32 area);
__s32 maple_get_area(void);

__s32 maple_clear_database(char *database_file_name);

__u32 ymd2mjd(__u32 year, __u32 month, __u32 date);
void mjd2ymd(__u32 MJD, __u32 *pYear, __u32 *pMonth, __u32 *pDate);
void getweekday(__u32 year, __u32 month, __u32 day, __u32 *pWeekDay);

#if ((MAPLE_TV_STANDAR == MAPLE_TV_ISDB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_DTMB)\
            ||(MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)||(MAPLE_TV_STANDAR == MAPLE_TV_ATSC))
maple_cas_param_t *maple_ca_get_sn(void);
#endif


__s32 maple_get_cur_search_freq(__s32 index);//add by hot.lee
__s32 maple_get_cur_search_freq_tatol(void);
__s32 maple_get_cur_foucs_index_from_freq(__u32 freq);

__s32 maple_get_search_freq(__u32 index);
__s32 maple_get_search_bandwidth(__u32 index);


//获取当前台数
__s32 maple_get_cur_service_list_num(void);
__s32 maple_get_cur_program_preview_index(void);
// 获取频道名称
char  *maple_get_name_of_channel(__s32 index);
__u8 maple_get_cafree_status_of_channel(__s32 index);
char *maple_get_channel_of_curprogramname(void);
char *maple_get_channel_of_nextprogramname(void);


__s32 maple_service_list_get(maple_serv_group_type_e type);

void maple_set_video_window_ratio_mode(maple_vd_win_ratio_mode_e radio_mode);
__s32  maple_get_video_window_ratio_mode(void);

__s32  maple_set_cur_channel(__s32 index);
__s32 maple_set_rotate(__s32  ratate);

void maple_break_search_task(void);

__s32 maple_set_default_char_coding_type(void);

__s32 maple_get_current_av_info(void);

__s32 change_window_ratio(void);

__s32 app_misc_audio_mute(__s32 flag);
__u16 dtv_get_cur_service_id(void);
__s32  maple_check_serviceid_exist(__s32 svcId);
__s32  maple_get_cur_channel_Idx(void);

__s32 maple_reg_flush(void);
__u8 maple_get_first_picture_status(void);

__u8 maple_get_server_type_of_channel(__s32 index);
void maple_all_freq_table(scan_para_t *scan_para);
void maple_auto_search_major(scan_para_t *scan_para);





#endif  /*  __LION_H__  */
