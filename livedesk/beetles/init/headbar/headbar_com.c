/*
**************************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                               desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                             All Rights Reserved
*
* File      : headbar_com.c
* By        : Andy.zhang
* Func      : app bar view
* Version   : v1.0
* ============================================================================================================
* 2009-9-24 15:12:06  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "mod_init_i.h"
#include "headbar_uipara.h"
#include "headbar_com.h"
#include "live_init.h"
/**********************************************************************************************************************/

//#define HB_BACKCOLOR              0xBF171717
#define HB_BACKCOLOR_LCD            0x0//0xB23F4C60
#define HB_BACKCOLOR_TV             0xB206396A

#define MAX_VOL_LEVEL               6
#define TBAR_VOL_TIMER_ID           1000

#define ValueBetweenRangeEx(_min, _v1, _v2, _max) (((_min) <= (_v1)) && ((_v1) < (_v2)) && ((_v2) <= (_max)))

static __bool g_enable_paint_time = 1;
static __bool g_enable_paint_signal = 0;


typedef enum
{
    HB_PAINT_BG                         = 0,
    HB_PAINT_SIG_STATUS,//1
    HB_PAINT_TIME,//2
    HB_PAINT_DATA,//3
    HB_PAINT_VOL,//4
    HB_PAINT_BRIGHT,//5
    HB_PAINT_VOLUME,//6
    HB_PAINT_TITLE,//7
    HB_PAINT_HOME,//8
    HB_PAINT_MUSCI_BG,//9

    HB_PAINT_NUM,//10
    HB_PAINT_INIT,//11

} headbar_paint_e;

typedef struct
{
    int32_t         vol_level;          // 电压级别，供电池显示
    __bool          charge_sta;         //充电状态
    __awos_date_t   date;               // 日期
    __awos_time_t   time;               // 时间
    int32_t         music_bg;           // 标识音乐是否后台

    int32_t         volume;             // 标识当前音量大小
    int32_t         bright;             // 标识当前亮度

} headbar_data_t;

typedef struct
{
    H_WIN   hbar;                       // bar窗口

    RECT    music_rect;                 // 音乐后台坐标
    GUI_RECT    date_rect;                  // 日期坐标
    GUI_RECT    time_rect;                  // 时间坐标
    RECT    volume_rect;                // 音量坐标
    RECT    volume_data_rect;           // 音量数据坐标
    RECT    bright_rect;                // 亮度坐标
    RECT    bright_data_rect;           // 亮度数据坐标
    RECT    vol_rect;                   // 电池坐标
    RECT    home_rect;                  // home坐标
    RECT    title_rect;                 // title坐标

    RECT    sig_strength_rect;          //信号强度
    RECT    sig_qa_rect;                // 信号质量

    HTHEME  vol_htheme[MAX_VOL_LEVEL];
    HTHEME  vol_htheme_charge[MAX_VOL_LEVEL];
    HTHEME  music_htheme;
    HTHEME  aux_htheme;
    HTHEME  pause_htheme;
    HTHEME  bright_htheme;
    HTHEME  volume_htheme;
    HTHEME  home_htheme;
    HTHEME  homefcs_htheme;
    HTHEME  bg_htheme;

    HTHEME  bg_hsig_st[MAX_SIGNAL_LEVEL];

    __u32   vol_id[MAX_VOL_LEVEL];          // 电池资源
    __u32   vol_id_charge[MAX_VOL_LEVEL];   // 充电电池资源

    void   *vol_bmp[MAX_VOL_LEVEL];         // 电池图片
    void   *vol_bmp_charge[MAX_VOL_LEVEL];  //充电电池图片
    void   *music_bmp;                      // 音乐后台播放图片
    void   *aux_bmp;                        // aux后台播放图片
    void   *pause_bmp;                  // 音乐\ aux后台暂停图片
    void   *bright_bmp;                 // 背光强度图片
    void   *volume_bmp;                 // 音量图片
    void   *home_bmp;                   // 音量图片
    void   *home_fcs_bmp;
    void   *bg_bmp;                     // 背景图片
    void   *sig_st[6];

    char    date_buf[32];               // 日期
    char    time_buf[32];               // 时间
    char    volume_buf[32];             // 音量
    char    bright_buf[32];             // 亮度
    char    title_buf[32];              // 应用程序title

    headbar_data_t  data;               // 状态数据
    GUI_FONT        *font;
    __bool          flag;               //用于记录是否强制刷新电池
} headbar_para_t;

typedef struct
{
    headbar_para_t  para;
    __u32           refresh_tid;
} headbar_ctl_t;
/**********************************************************************************************************************/
static int32_t  cb_headbar_framewin(__gui_msg_t    *p_msg);
static void     init_headbar_para(__gui_msg_t *p_msg, headbar_para_t   *para);
static void     deinit_headbar_para(headbar_para_t *para);

static int32_t  power_level_2_display(power_level_e level);
static void     time_to_char(char *buff, __awos_time_t time);
static void     date_to_char(char *buff, __awos_date_t date);
static int32_t  bright_level_2_display(__lion_bright_t bright);
/**********************************************************************************************************************/
static __bg_audio_t is_music_background(void)
{
    return gscene_hbar_get_music_state();
}

#define DATA_OF_ADRR(t, p, x, y, w) (*( (t*)(p)+(y)*(w)+(x) ))

static int32_t copy_rect(uint32_t *pDst, int32_t dst_x, int32_t dst_y, int32_t dst_line_w, int32_t dst_line_h, uint32_t *pSrc,
                       int32_t src_x, int32_t src_y, int32_t src_line_w, int32_t w, int32_t h)
{
    __s32 i, j;
    int dir = GUI_GetScnDir();

    if(GUI_SCNDIR_NORMAL == dir)
    {
        for (i = 0 ; i < w ; i++)
        {
            for (j = 0 ; j < h ; j++)
            {
                DATA_OF_ADRR(__u32, pDst, dst_x + i, dst_y + j, dst_line_w) = DATA_OF_ADRR(__u32, pSrc, src_x + i, src_y + j, src_line_w);
            }
        }
    }
    else if(GUI_SCNDIR_ROTATE90 == dir)
    {
        for (i = 0 ; i < w ; i++)
        {
            for (j = 0 ; j < h ; j++)
            {
                *(pDst + (dst_line_w - (dst_x + i) -1)*dst_line_h + (dst_y + j))  = DATA_OF_ADRR(__u32, pSrc, src_x + i, src_y + j, src_line_w);
            }
        }
    }
    else if(GUI_SCNDIR_ROTATE180 == dir)
    {
        for (i = 0 ; i < w ; i++)
        {
            for (j = 0 ; j < h ; j++)
            {
                DATA_OF_ADRR(__u32, pDst, dst_y + i, dst_x + j, dst_line_w)  = DATA_OF_ADRR(__u32, pSrc, src_x + i, src_y + j, src_line_w);
            }
        }
    }
    else if(GUI_SCNDIR_ROTATE270 == dir)
    {
        for (i = 0 ; i < w ; i++)
        {
            for (j = 0 ; j < h ; j++)
            {
                *(pDst + (dst_line_w - (dst_y + i) -1)*dst_line_h + (dst_x + j))  = DATA_OF_ADRR(__u32, pSrc, src_x + i, src_y + j, src_line_w);
            }
        }
    }    
    return EPDK_OK;
}

//////////////////////////////////////////////////////////////////////////

/************************************************************************/
// pFileData --- 资源函数获得的数据(也是32位bmp文件数据)
// x，y      --- 目标坐标
// x0, y0, x1, y1 --- 源区域(即bmp图片某一区域)
/************************************************************************/
static __s32 APP_BMP_Draw(H_LYR hlyr, const void *pFileData, int x, int y, int x0, int y0, int x1, int y1)
{
    int32_t     Ret = EPDK_OK;
    uint32_t    *pSrc;
    int         bmp_w, bmp_h;
    int         w, h;
    GUI_RECT    rt;
    FB          fb;
    uint32_t    *pfb_addr;
    int32_t     i, j;
    if (!hlyr)
    {
        __err("invalid para...\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinGetFB(hlyr, &fb);
    //检查区域范围
    bmp_w = GUI_BMP_GetXSize(pFileData);

    if (! ValueBetweenRangeEx(0, x0, x1, bmp_w))
    {
        return EPDK_FAIL;
    }

    bmp_h = GUI_BMP_GetYSize(pFileData);

    if (! ValueBetweenRangeEx(0, y0, y1, bmp_h))
    {
        return EPDK_FAIL;
    }

    if (x1 - x0 + x >= fb.size.width || y1 - y0 + y >= fb.size.height)
    {
        return EPDK_FAIL;
    }

    pSrc = (__u32 *)((char *)pFileData + 54);
    pfb_addr = (__u32 *)(fb.addr[0]);
    copy_rect(pfb_addr, x, y, fb.size.width, fb.size.height, pSrc, x0, y0, bmp_w, x1 - x0 + 1, y1 - y0 + 1);
    return Ret;
}

static void reset_headbar_data(headbar_para_t *para)
{
    power_level_e       level;
    __bool              charge_state;
    __lion_bright_t     bright;

	//para->data.time.hour = 12;
	//para->data.time.minute = 12;
	//para->data.time.second = 12;
    esTIME_GetTime(&(para->data.time));                     // 时间
    eLIBs_memset(para->time_buf, 0, 32);
    time_to_char(para->time_buf, para->data.time);
	//para->data.date.year = 2020;
	//para->data.date.month = 4;
	//para->data.date.day = 13;
    esTIME_GetDate(&(para->data.date));                     // 日期

    eLIBs_memset(para->date_buf, 0, 32);

    date_to_char(para->date_buf, para->data.date);
    
    dsk_power_get_voltage_level(&level);
    para->data.vol_level = power_level_2_display(level);    //电池级别
    para->data.charge_sta = dsk_get_charge_state();         //充电状态
    
    para->data.volume = dsk_volume_get();                   //音量
    __log("xxxxxx para->data.volume = %d",para->data.volume);
    eLIBs_memset(para->volume_buf, 0, 32);
    eLIBs_int2str_dec(para->data.volume, para->volume_buf);
    bright = dsk_display_get_lcd_brightness();
    __log("xxxxxx bright = %d",bright);

    para->data.bright = bright_level_2_display(bright);     //背光亮度
    //eLIBs_printf("%s %d bright:%d\r\n", __FILE__, __LINE__, bright);
    eLIBs_memset(para->bright_buf, 0, 32);

    eLIBs_int2str_dec(para->data.bright, para->bright_buf);

    para->data.music_bg = is_music_background();            //音乐后台
}

static void init_headbar_para(__gui_msg_t *p_msg, headbar_para_t *para)
{
    int32_t         i;
    para->hbar                      =  p_msg->h_deswin;
    para->music_rect.x              = headbar_uipara.music.x;
    para->music_rect.y              = headbar_uipara.music.y;
    para->music_rect.width          = headbar_uipara.music.w;
    para->music_rect.height         = headbar_uipara.music.h;
    para->date_rect.x0              = headbar_uipara.date.x;
    para->date_rect.y0              = headbar_uipara.date.y;
    para->date_rect.x1              = para->date_rect.x0 + headbar_uipara.date.w - 1;
    para->date_rect.y1              = para->date_rect.y0 + headbar_uipara.date.h - 1;
    para->home_rect.x               = headbar_uipara.home.x;
    para->home_rect.y               = headbar_uipara.home.y;
    para->home_rect.width           = headbar_uipara.home.w;
    para->home_rect.height          = headbar_uipara.home.h;
    para->time_rect.x0              = headbar_uipara.time.x;
    para->time_rect.y0              = headbar_uipara.time.y;
    para->time_rect.x1              = para->time_rect.x0 + headbar_uipara.time.w - 1;
    para->time_rect.y1              = para->time_rect.y0 + headbar_uipara.time.h - 1;
    para->sig_strength_rect.x       = headbar_uipara.singal_icon.x;
    para->sig_strength_rect.y       = headbar_uipara.singal_icon.y;
    para->sig_strength_rect.width   = headbar_uipara.singal_icon.w;
    para->sig_strength_rect.height  = headbar_uipara.singal_icon.h;
    para->sig_qa_rect.x             = headbar_uipara.singal_qa.x;
    para->sig_qa_rect.y             = headbar_uipara.singal_qa.y;
    para->sig_qa_rect.width         = headbar_uipara.singal_qa.w;
    para->sig_qa_rect.height        = headbar_uipara.singal_qa.h;
    para->volume_rect.x             = headbar_uipara.volume_icon.x;
    para->volume_rect.y             = headbar_uipara.volume_icon.y;
    para->volume_rect.width         = headbar_uipara.volume_icon.w;
    para->volume_rect.height        = headbar_uipara.volume_icon.h;
    para->volume_data_rect.x        = headbar_uipara.volume_txt.x;
    para->volume_data_rect.y        = headbar_uipara.volume_txt.y;
    para->volume_data_rect.width    = headbar_uipara.volume_txt.w;
    para->volume_data_rect.height   = headbar_uipara.volume_txt.h;
    para->bright_rect.x             = headbar_uipara.bright_icon.x;
    para->bright_rect.y             = headbar_uipara.bright_icon.y;
    para->bright_rect.width         = headbar_uipara.bright_icon.w;
    para->bright_rect.height        = headbar_uipara.bright_icon.h;
    para->bright_data_rect.x        = headbar_uipara.bright_txt.x;
    para->bright_data_rect.y        = headbar_uipara.bright_txt.y;
    para->bright_data_rect.width    = headbar_uipara.bright_txt.w;
    para->bright_data_rect.height   = headbar_uipara.bright_txt.h;
    para->vol_rect.x                = headbar_uipara.battery.x;
    para->vol_rect.y                = headbar_uipara.battery.y;
    para->vol_rect.width            = headbar_uipara.battery.w;
    para->vol_rect.height           = headbar_uipara.battery.h;
    para->title_rect.x              = headbar_uipara.title.x;
    para->title_rect.y              = headbar_uipara.title.y;
    para->title_rect.width          = headbar_uipara.title.w;
    para->title_rect.height         = headbar_uipara.title.h;
    para->vol_id[0]                 = headbar_uipara.res_id.vol_id0;
    para->vol_id[1]                 = headbar_uipara.res_id.vol_id1;
    para->vol_id[2]                 = headbar_uipara.res_id.vol_id2;
    para->vol_id[3]                 = headbar_uipara.res_id.vol_id3;
    para->vol_id[4]                 = headbar_uipara.res_id.vol_id4;
    para->vol_id[5]                 = headbar_uipara.res_id.vol_id5;
    para->vol_id_charge[0]          = headbar_uipara.res_id.charge_vol_id0;
    para->vol_id_charge[1]          = headbar_uipara.res_id.charge_vol_id1;
    para->vol_id_charge[2]          = headbar_uipara.res_id.charge_vol_id2;
    para->vol_id_charge[3]          = headbar_uipara.res_id.charge_vol_id3;
    para->vol_id_charge[4]          = headbar_uipara.res_id.charge_vol_id4;
    para->vol_id_charge[5]          = headbar_uipara.res_id.charge_vol_id5;

    for (i = 0; i < MAX_VOL_LEVEL; i++)
    {
        para->vol_htheme[i]         = dsk_theme_open(para->vol_id[i]);
        para->vol_bmp[i]            = dsk_theme_hdl2buf(para->vol_htheme[i]);
        para->vol_htheme_charge[i]  = dsk_theme_open(para->vol_id_charge[i]);
        para->vol_bmp_charge[i]     = dsk_theme_hdl2buf(para->vol_htheme_charge[i]);
    }
    
    para->music_htheme      = dsk_theme_open(headbar_uipara.res_id.music_id);
    para->aux_htheme        = dsk_theme_open(headbar_uipara.res_id.aux_id);
    para->pause_htheme      = dsk_theme_open(headbar_uipara.res_id.pause_id);
    para->bright_htheme     = dsk_theme_open(headbar_uipara.res_id.bright_id);
    para->volume_htheme     = dsk_theme_open(headbar_uipara.res_id.volume_id);
    para->home_htheme       = dsk_theme_open(headbar_uipara.res_id.home_id);
    para->homefcs_htheme    = dsk_theme_open(headbar_uipara.res_id.home_fcs_id);
    para->bg_htheme         = dsk_theme_open(headbar_uipara.res_id.bg_id);
    para->music_bmp         = dsk_theme_hdl2buf(para->music_htheme);
    para->aux_bmp           = dsk_theme_hdl2buf(para->aux_htheme);
    para->pause_bmp         = dsk_theme_hdl2buf(para->pause_htheme);
    para->bright_bmp        = dsk_theme_hdl2buf(para->bright_htheme);
    para->volume_bmp        = dsk_theme_hdl2buf(para->volume_htheme);
    para->home_bmp          = dsk_theme_hdl2buf(para->home_htheme);
    para->home_fcs_bmp      = dsk_theme_hdl2buf(para->homefcs_htheme);
    para->bg_bmp            = dsk_theme_hdl2buf(para->bg_htheme);

    for (i = 0; i < MAX_SIGNAL_LEVEL; i++)
    {
        para->bg_hsig_st[i] = dsk_theme_open(headbar_uipara.res_id.sig_st_id[i]);
        para->sig_st[i] = dsk_theme_hdl2buf(para->bg_hsig_st[i]);
    }

    para->font              = headbar_uipara.font;
    reset_headbar_data(para);

}


static void deinit_headbar_para(headbar_para_t *para)
{
    int i;

    for (i = 0; i < MAX_VOL_LEVEL; i++)
    {
        dsk_theme_close(para->vol_htheme[i]);
        dsk_theme_close(para->vol_htheme_charge[i]);
    }

    dsk_theme_close(para->music_htheme);
    dsk_theme_close(para->aux_htheme);
    dsk_theme_close(para->pause_htheme);
    dsk_theme_close(para->bright_htheme);
    dsk_theme_close(para->volume_htheme);
    dsk_theme_close(para->home_htheme);
    dsk_theme_close(para->bg_htheme);
    dsk_theme_close(para->homefcs_htheme);

    for (i = 0; i < MAX_SIGNAL_LEVEL; i++)
    {
        dsk_theme_close(para->bg_hsig_st[i]);
        para->bg_hsig_st[i] = 0;
        para->sig_st[i]     = NULL;
    }
}

static void time_to_char(char *buff, __awos_time_t time)
{
    buff[0] = time.hour / 10 + '0';
    buff[1] = time.hour % 10 + '0';
    buff[2] = ':';
    buff[3] = time.minute / 10 + '0';
    buff[4] = time.minute % 10 + '0';
    buff[5] = '\0';
}

static void date_to_char(char *buff, __awos_date_t date)
{
    buff[0] = date.month / 10 + '0';
    buff[1] = date.month % 10 + '0';
    buff[2] = '-';
    buff[3] = date.day / 10 + '0';
    buff[4] = date.day % 10 + '0';
    buff[5] = '\0';
}

static int32_t bright_level_2_display(__lion_bright_t bright)
{
    int32_t     ret = 0;

    switch (bright)
    {
#if 0

        case LION_BRIGHT_LEVEL1:
            ret = 1;
            break;

        case LION_BRIGHT_LEVEL2:
            ret = 2;
            break;

        case LION_BRIGHT_LEVEL3:
            ret = 3;
            break;

        case LION_BRIGHT_LEVEL4:
            ret = 4;
            break;

        case LION_BRIGHT_LEVEL5:
            ret = 5;
            break;

        case LION_BRIGHT_LEVEL6:
            ret = 6;
            break;

        case LION_BRIGHT_LEVEL7:
            ret = 7;
            break;

        case LION_BRIGHT_LEVEL8:
            ret = 8;
            break;

        case LION_BRIGHT_LEVEL9:
            ret = 9;
            break;

        case LION_BRIGHT_LEVEL10:
            ret = 10;
            break;

        case LION_BRIGHT_LEVEL11:
            ret = 11;
            break;

        case LION_BRIGHT_LEVEL12:
            ret = 12;
            break;

        case LION_BRIGHT_LEVEL13:
            ret = 13;
            break;

        case LION_BRIGHT_LEVEL14:
            ret = 14;
            break;

        case LION_BRIGHT_LEVEL15:
            ret = 15;
            break;

        case LION_BRIGHT_LEVEL16:
            ret = 16;
            break;

        case LION_BRIGHT_:
            ret = 0;
            break;
#else

        case LION_BRIGHT_LEVEL1:
            ret = 1;
            break;

        case LION_BRIGHT_LEVEL2:
            ret = 2;
            break;

        case LION_BRIGHT_LEVEL3:
            ret = 1;
            break;

        case LION_BRIGHT_LEVEL4:
            ret = 1;
            break;

        case LION_BRIGHT_LEVEL5:
            ret = 2;
            break;

        case LION_BRIGHT_LEVEL6:
            ret = 3;
            break;

        case LION_BRIGHT_LEVEL7:
            ret = 4;
            break;

        case LION_BRIGHT_LEVEL8:
            ret = 5;
            break;

        case LION_BRIGHT_LEVEL9:
            ret = 6;
            break;

        case LION_BRIGHT_LEVEL10:
            ret = 7;
            break;

        case LION_BRIGHT_LEVEL11:
            ret = 8;
            break;

        case LION_BRIGHT_LEVEL12:
            ret = 9;
            break;

        case LION_BRIGHT_LEVEL13:
            ret = 10;
            break;

        case LION_BRIGHT_LEVEL14:
            ret = 14;
            break;

        case LION_BRIGHT_LEVEL15:
            ret = 15;
            break;

        case LION_BRIGHT_LEVEL16:
            ret = 16;
            break;

        case LION_BRIGHT_:
            ret = 0;
            break;
#endif
    }

    return ret;
}

static int32_t power_level_2_display(power_level_e level)
{
    int32_t     ret = 0;

    switch (level)
    {
        case DSK_POWER_LEVEL_0:
            ret = 0;
            break;

        case DSK_POWER_LEVEL_1:
            ret = 1;
            break;

        case DSK_POWER_LEVEL_2:
            ret = 2;
            break;

        case DSK_POWER_LEVEL_3:
            ret = 3;
            break;

        case DSK_POWER_LEVEL_4:
            ret = 4;
            break;

        case DSK_POWER_LEVEL_5:
            ret = 5;
            break;

        default:
            ret = 0;
            break;
    }

    return ret;
}

//传入24时制小时值，传出12时制小时值，返回0为am，非0为pm
static int32_t hour24_to_hour12(int32_t *hour)
{
    if (*hour == 0)
    {
        //凌晨12点
        *hour = 12;
    }
    else if (*hour == 12)
    {
        //中午12点
        return 1;
    }
    else if (12 < *hour)
    {
        *hour -= 12;
        return 1;
    }

    return 0;
}

static void headbar_refresh_thread(void *arg)
{
    headbar_para_t      *para = (headbar_para_t *)arg;
    headbar_data_t      new_data;
    __lion_bright_t     bright;
    H_WIN               lyr;
    __gui_msg_t         msg;
    int32_t             hour;

    while (1)
    {
        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            esKRNL_TDel(OS_PRIO_SELF);
            return;
        }

        lyr = GUI_WinGetLyrWin(para->hbar);

        if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(lyr))
        {
            esTIME_GetTime(&new_data.time);                         //时间
            esTIME_GetDate(&new_data.date);
            __inf("headbar time =%d %d %d",new_data.time.hour,new_data.time.minute,new_data.time.second);

            {
                reg_calendar_para_t *para;
                para = (reg_calendar_para_t *)dsk_reg_get_para_by_app(REG_APP_CALENDAR);

                if (para && 0 != para->time_format)
                {
                    __wrn("para->time_format=%d\n", para->time_format);
                    __wrn("before convert t.hour=%d\n", new_data.time.hour);
                    hour = new_data.time.hour;
                    hour24_to_hour12((__s32 *)&hour);
                    new_data.time.hour = hour;
                    __wrn("after convert t.hour=%d\n", new_data.time.hour);
                }
            }

            bright              = dsk_display_get_lcd_brightness();
            new_data.bright     = bright;//bright_level_2_display(bright);      //背光亮度
            new_data.volume     = dsk_volume_get();             //音量
            new_data.music_bg   = is_music_background();                //音乐后台

            if ((new_data.date.year != para->data.date.year) || (new_data.date.month != para->data.date.month)|| (new_data.date.day != para->data.date.day))
            {
                //更新显示日期
                eLIBs_memset(para->date_buf, 0, 32);
                para->data.date = new_data.date;
                date_to_char(para->date_buf, para->data.date);
                msg.id = HBAR_DATA_PAINT;
                msg.h_srcwin = NULL;
                msg.h_deswin = para->hbar;
                msg.dwAddData1 = 0;
                msg.dwAddData2 = 0;
                msg.dwReserved = 0;
                msg.p_arg      = NULL;
                GUI_SendNotifyMessage(&msg);
            }

            if ((new_data.time.hour != para->data.time.hour) || (new_data.time.minute != para->data.time.minute))
            {
                //更新显示时间
                eLIBs_memset(para->time_buf, 0, 32);
                para->data.time = new_data.time;
                time_to_char(para->time_buf, para->data.time);
                msg.id          = HBAR_TIME_PAINT;
                msg.h_srcwin    = NULL;
                msg.h_deswin    = para->hbar;
                msg.dwAddData1  = 0;
                msg.dwAddData2  = 0;
                msg.dwReserved  = 0;
                msg.p_arg       = NULL;
                GUI_SendNotifyMessage(&msg);
            }

            if (new_data.bright != para->data.bright)
            {
                // 更新亮度
                extern __bool init_is_close_screen(void);

                if (init_is_close_screen() == EPDK_TRUE)
                {
                    esKRNL_TimeDly(100);
                    continue;
                }

                msg.id = HBAR_BRIGHT_PAINT;
                msg.h_srcwin = NULL;
                msg.h_deswin = para->hbar;
                msg.dwAddData1 = (__u32)bright;
                msg.dwAddData2 = 0;
                msg.dwReserved = 0;
                msg.p_arg      = NULL;
                GUI_SendNotifyMessage(&msg);
            }

            if (new_data.volume != para->data.volume)
            {
                // 更新音量
                msg.id          = HBAR_VOLUME_PAINT;
                msg.h_srcwin    = NULL;
                msg.h_deswin    = para->hbar;
                msg.dwAddData1  = new_data.volume;
                msg.dwAddData2  = 0;
                msg.dwReserved  = 0;
                msg.p_arg       = NULL;
                GUI_SendNotifyMessage(&msg);
            }

            if (new_data.music_bg != para->data.music_bg)
            {
                para->data.music_bg = new_data.music_bg;
                msg.id = HBAR_MUSIC_BG_PAINT;
                msg.h_srcwin = NULL;
                msg.h_deswin = para->hbar;
                msg.dwAddData1 = 0;
                msg.dwAddData2 = 0;
                msg.dwReserved = 0;
                msg.p_arg      = NULL;
                GUI_SendNotifyMessage(&msg);
            }
        }

        esKRNL_TimeDly(100);
    }
}

static int32_t headbar_on_paint_argb(__gui_msg_t *msg, headbar_paint_e mode)
{
    headbar_para_t  *p_ctrl;
    RECT            fbrect;
    H_LYR           layer;
    GUI_FONT        *font;

    p_ctrl  = &(((headbar_ctl_t *)GUI_WinGetAttr(msg->h_deswin))->para);
    GUI_WinGetClientFBRect(msg->h_deswin, &fbrect);
    font    = p_ctrl->font;
    layer   = (H_LYR)GUI_WinGetLyrWin(msg->h_deswin);
    __inf("GUI_LyrWinGetSta(layer)=%d  mode = %d\n", GUI_LyrWinGetSta(layer),mode);

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(layer))
    {
        return EPDK_OK;
    }

    GUI_LyrWinSel(layer);

    switch (mode)
    {
        case HB_PAINT_INIT:         /* 全部更新 */
        {
            int32_t     i;
            for (i = 0 ; i < HB_PAINT_NUM ; i++)
            {
                headbar_on_paint_argb(msg, i);
            }

            return EPDK_OK;
        }

        case HB_PAINT_TIME:     /* 更新当前时间 */
        {
#if BEETLES_RTC_EXIST
            if (g_enable_paint_time)
            {
                GUI_SetFont(font);
                GUI_UC_SetEncodeUTF8();
                APP_BMP_Draw(layer, p_ctrl->bg_bmp, p_ctrl->time_rect.x0, p_ctrl->time_rect.y0
                             , p_ctrl->time_rect.x0, p_ctrl->time_rect.y0
                             , p_ctrl->time_rect.x1, p_ctrl->time_rect.y1);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_SetColor(GUI_WHITE);
                GUI_DispStringInRect(p_ctrl->time_buf, &p_ctrl->time_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
            }

#endif
            return EPDK_OK;
        }

        case HB_PAINT_DATA:     /* 更新当前日期 */
        {
            //不显示日期
            if (0)
            {
#if BEETLES_RTC_EXIST

                if (g_enable_paint_time)
                {
                    GUI_SetFont(font);
                    GUI_UC_SetEncodeUTF8();
                    APP_BMP_Draw(layer, p_ctrl->bg_bmp, p_ctrl->date_rect.x0, p_ctrl->date_rect.y0
                                 , p_ctrl->date_rect.x0, p_ctrl->date_rect.y0
                                 , p_ctrl->date_rect.x1, p_ctrl->date_rect.y1);
                    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                    GUI_SetColor(GUI_WHITE);
                    GUI_DispStringInRect(p_ctrl->date_buf, &p_ctrl->date_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
                    
                }

#endif
            }

            return EPDK_OK;
        }

        case HB_PAINT_BG:           /* 更新背景 */
        {
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_ARGB_Draw(p_ctrl->bg_bmp, 0, 0);
            
            return EPDK_OK;
        }

        case HB_PAINT_VOL:          /* 更新电量 */
        {
            static int32_t  cur_charge_showing_vol = 0;
            int32_t         vol_level;

			__wrn("update power!!!!!!");
            vol_level = p_ctrl->data.vol_level;
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            if (EPDK_TRUE == p_ctrl->data.charge_sta)
            {
                __bool is_charge_full;
                is_charge_full = dsk_power_is_full();

                if (EPDK_FALSE == is_charge_full)
                {
                    vol_level = cur_charge_showing_vol;
                    cur_charge_showing_vol++;

                    if (cur_charge_showing_vol >= MAX_VOL_LEVEL)
                    {
                        cur_charge_showing_vol = 0;
                    }
                }

                GUI_ARGB_Draw(p_ctrl->vol_bmp_charge[vol_level], p_ctrl->vol_rect.x, p_ctrl->vol_rect.y);
            }
            else
            {
                cur_charge_showing_vol = 0;
                GUI_ARGB_Draw(p_ctrl->vol_bmp[vol_level], p_ctrl->vol_rect.x, p_ctrl->vol_rect.y);
            }

            return EPDK_OK;
        }

        case HB_PAINT_BRIGHT:       /* 更新亮度 */
        {
            GUI_RECT gui_rect;

            gui_rect.x0 = p_ctrl->bright_data_rect.x;
            gui_rect.y0 = p_ctrl->bright_data_rect.y;
            gui_rect.x1 = gui_rect.x0 + p_ctrl->bright_data_rect.width - 1;
            gui_rect.y1 = gui_rect.y0 + p_ctrl->bright_data_rect.height - 1;
            GUI_SetFont(font);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_SetColor(GUI_WHITE);
            APP_BMP_Draw(layer, p_ctrl->bg_bmp, gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_ARGB_Draw(p_ctrl->bright_bmp, p_ctrl->bright_rect.x, p_ctrl->bright_rect.y);
            GUI_DispStringInRect(p_ctrl->bright_buf, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            return EPDK_OK;
        }

        case HB_PAINT_VOLUME:       /*  更新音量 */
        {
            GUI_RECT gui_rect;

            gui_rect.x0 = p_ctrl->volume_data_rect.x;
            gui_rect.y0 = p_ctrl->volume_data_rect.y;
            gui_rect.x1 = gui_rect.x0 + p_ctrl->volume_data_rect.width - 1;
            gui_rect.y1 = gui_rect.y0 + p_ctrl->volume_data_rect.height - 1;
            GUI_SetFont(font);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_SetColor(GUI_WHITE);
            APP_BMP_Draw(layer, p_ctrl->bg_bmp, gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_ARGB_Draw(p_ctrl->volume_bmp, p_ctrl->volume_rect.x, p_ctrl->volume_rect.y);
            GUI_DispStringInRect(p_ctrl->volume_buf, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            
            return EPDK_OK;
        }

        case HB_PAINT_HOME:     /*  更新home图标 */
        {

            if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN)
            {
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_ARGB_Draw(p_ctrl->home_fcs_bmp, p_ctrl->home_rect.x, p_ctrl->home_rect.y);
            }
            else if (msg->dwAddData1 == GUI_MSG_TOUCH_UP)
            {
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_ARGB_Draw(p_ctrl->home_bmp, p_ctrl->home_rect.x, p_ctrl->home_rect.y);
            }
            else
            {
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                GUI_ARGB_Draw(p_ctrl->home_bmp, p_ctrl->home_rect.x, p_ctrl->home_rect.y);
            }

            return EPDK_OK;
        }

        case HB_PAINT_MUSCI_BG:     /* 更新后台播放状态 */
        {

            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            APP_BMP_Draw(layer, p_ctrl->bg_bmp, p_ctrl->music_rect.x, p_ctrl->music_rect.y
                         , p_ctrl->music_rect.x, p_ctrl->music_rect.y
                         , p_ctrl->music_rect.x + p_ctrl->music_rect.width - 1, p_ctrl->music_rect.y + p_ctrl->music_rect.height - 1);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);

            if (BG_MUSIC == p_ctrl->data.music_bg)
            {
                /* 画后台播放图标 */
                GUI_ARGB_Draw(p_ctrl->music_bmp, p_ctrl->music_rect.x, p_ctrl->music_rect.y);
            }
            else if (BG_AUX == p_ctrl->data.music_bg)
            {
                /* 画后台播放图标 */
                GUI_ARGB_Draw(p_ctrl->aux_bmp, p_ctrl->music_rect.x, p_ctrl->music_rect.y);
            }
            else if (BG_PAUSE == p_ctrl->data.music_bg)
            {
                /* 画后台播放图标 */
                GUI_ARGB_Draw(p_ctrl->pause_bmp, p_ctrl->music_rect.x, p_ctrl->music_rect.y);
            }
            else
            {
            }

            return EPDK_OK;
        }

        case HB_PAINT_TITLE:
        {
			
            GUI_RECT    rect = {0};

            GUI_SetFont(font);
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            APP_BMP_Draw(layer, p_ctrl->bg_bmp, p_ctrl->title_rect.x, p_ctrl->title_rect.y
                         , p_ctrl->title_rect.x, p_ctrl->title_rect.y
                         , p_ctrl->title_rect.x + p_ctrl->title_rect.width - 1, p_ctrl->title_rect.y + p_ctrl->title_rect.height - 1);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_SetColor(GUI_WHITE);
            GUI_UC_SetEncodeUTF8();
            //GUI_DispStringAt(p_ctrl->title_buf, p_ctrl->title_rect.x, p_ctrl->title_rect.y);
            rect.x0 = p_ctrl->title_rect.x;
            rect.y0 = p_ctrl->title_rect.y;
            rect.x1 = rect.x0 + p_ctrl->title_rect.width;
            rect.y1 = rect.y1 + p_ctrl->title_rect.height;
            GUI_DispStringInRect(p_ctrl->title_buf, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            return EPDK_OK;
            
        }

        case HB_PAINT_SIG_STATUS:
        {

            if (msg->dwAddData1 >= 100 || msg->dwAddData2 >= 100)
            {
                return EPDK_OK;
            }

            if (g_enable_paint_signal)
            {
                GUI_RECT gui_rect;
                char buff[32] = {0}, text[64] = {0};

                gui_rect.x0     = p_ctrl->sig_strength_rect.x;
                gui_rect.y0     = p_ctrl->sig_strength_rect.y;
                gui_rect.x1     = gui_rect.x0 + p_ctrl->sig_strength_rect.width - 1;
                gui_rect.y1     = gui_rect.y0 + p_ctrl->sig_strength_rect.height - 1;
                GUI_SetFont(font);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetColor(GUI_WHITE);
                APP_BMP_Draw(layer, p_ctrl->bg_bmp, gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                dsk_langres_get_menu_text(STRING_HEADER_SIGNAL_STRENGTH, text, sizeof(text));
                eLIBs_sprintf(buff, "%d", msg->dwAddData1);
                eLIBs_strcat(text, buff);
                GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                eLIBs_memset(text, 0x00, sizeof(text));
                eLIBs_memset(buff, 0x00, sizeof(buff));
                gui_rect.x0     = p_ctrl->sig_qa_rect.x;
                gui_rect.y0     = p_ctrl->sig_qa_rect.y;
                gui_rect.x1     = gui_rect.x0 + p_ctrl->sig_qa_rect.width - 1;
                gui_rect.y1     = gui_rect.y0 + p_ctrl->sig_qa_rect.height - 1;
                GUI_SetFont(font);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetColor(GUI_WHITE);
                APP_BMP_Draw(layer, p_ctrl->bg_bmp, gui_rect.x0, gui_rect.y0, gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
                GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
                dsk_langres_get_menu_text(STRING_HEADER_SIGNAL_QUALITY, text, sizeof(text));
                eLIBs_sprintf(buff, "%d", msg->dwAddData1);
                eLIBs_strcat(text, buff);
                GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }

            return EPDK_OK;
        }

        default:
            break;
    }

    return EPDK_OK;
}

static __s32 headbar_on_paint_8bpp(__gui_msg_t *msg, headbar_paint_e mode)
{
    return EPDK_OK;
}

static int32_t headbar_on_paint(__gui_msg_t *msg, headbar_paint_e mode)
{
    H_LYR          layer;
    FB             fb; 
    //GUI_MEMDEV_Handle   draw_mem = NULL;

    layer   = (H_LYR)GUI_WinGetLyrWin(msg->h_deswin); 
    //GUI_LyrWinSel(layer);   
    //draw_mem = GUI_MEMDEV_Create(0,0,800,46);
    //GUI_MEMDEV_Select(draw_mem);
    headbar_on_paint_argb(msg, mode);
    //GUI_MEMDEV_CopyToLCD(draw_mem);
    //GUI_MEMDEV_Select(NULL);
    //GUI_MEMDEV_Delete(draw_mem);
    //draw_mem = NULL;    
    GUI_LyrWinFlushFB(layer);    
    return EPDK_OK;
}

static __bool pt_in_rect(RECT *rect, int32_t x, int32_t y, int32_t left, int32_t top, int32_t right, int32_t bottom)
{
    if ((x > rect->x - left) && (y > rect->y - top) && (x < (rect->x + rect->width + right)) && (y < (rect->y + rect->height + bottom)))
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

static int32_t cb_headbar_framewin(__gui_msg_t *p_msg)
{
    __inf("++++++++++++++++++++++p_msg->id=%d\n", p_msg->id);

    switch (p_msg->id)
    {
        case GUI_MSG_CREATE:
        {
            headbar_ctl_t   *p_ctrl;

            p_ctrl  = (headbar_ctl_t *)esMEMS_Malloc(0, sizeof(headbar_ctl_t));      // 分配内存单元

            if (!p_ctrl)
            {
                __err("headbar_para_t malloc error \n");
            }

            eLIBs_memset(p_ctrl, 0, sizeof(headbar_ctl_t));

            GUI_WinSetAttr(p_msg->h_deswin, p_ctrl);

            init_headbar_para(p_msg, &(p_ctrl->para));

            headbar_on_paint(p_msg, HB_PAINT_INIT);

            GUI_SetTimer(p_msg->h_deswin, TBAR_VOL_TIMER_ID, 100, NULL);

            p_ctrl->refresh_tid = esKRNL_TCreate(headbar_refresh_thread, &(p_ctrl->para), 0x2000, KRNL_priolevel5);
            esKRNL_TaskNameSet(p_ctrl->refresh_tid, "headbar_fresh");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            headbar_ctl_t   *p_ctrl;
            RECT            fbrect;
            H_LYR           layer;

            p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
            GUI_KillTimer(p_msg->h_deswin, TBAR_VOL_TIMER_ID);

            while (esKRNL_TDelReq(p_ctrl->refresh_tid) != OS_TASK_NOT_EXIST)
            {
                esKRNL_TimeDly(2);
            }

            layer  = (H_LYR)GUI_WinGetLyrWin(p_msg->h_deswin);

            if (GUI_LYRWIN_STA_SUSPEND != GUI_LyrWinGetSta(layer))
            {
                GUI_LyrWinSel(layer);
                GUI_SetBkColor(0);
                GUI_WinGetClientFBRect(p_msg->h_deswin, &fbrect);
                GUI_ClearRect(fbrect.x, fbrect.y, fbrect.x + fbrect.width - 1, fbrect.y + fbrect.height - 1);
            }

            deinit_headbar_para(&(p_ctrl->para));
            esMEMS_Mfree(0, p_ctrl);
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
            GUI_FrmWinDelete(p_msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_TIMER:
        {
            if (p_msg->dwAddData1 == TBAR_VOL_TIMER_ID)
            {
                H_LYR   lyr;

                lyr = GUI_WinGetLyrWin(p_msg->h_deswin);

                if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(lyr))
                {
                    __bool          charge_sta;
                    power_level_e   level;
                    int32_t         vol_level;
                    headbar_ctl_t   *p_ctrl;
                    __gui_msg_t     msgex;

                    p_ctrl      = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
                    dsk_power_get_voltage_level(&level);
                    vol_level   = power_level_2_display(level);   //电量级别
                    charge_sta  = dsk_get_charge_state();

                    if (p_ctrl->para.data.vol_level != vol_level || p_ctrl->para.data.charge_sta != charge_sta || EPDK_TRUE == charge_sta)//充电状态需要不断更新图标
                    {
                        p_ctrl->para.data.vol_level     = vol_level;
                        p_ctrl->para.data.charge_sta    = charge_sta;
                        msgex.id            = HBAR_VOL_PAINT;
                        msgex.h_srcwin      = NULL;
                        msgex.h_deswin      = p_msg->h_deswin;
                        msgex.dwAddData1    = 0;
                        msgex.dwAddData2    = 0;
                        msgex.dwReserved    = 0;
                        msgex.p_arg         = NULL;
                        GUI_SendMessage(&msgex);
                    }
                }
            }

            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            return EPDK_OK;
        }

        case GUI_MSG_WIN_WAKEUP:
        {
            headbar_ctl_t   *p_ctrl = NULL;

            p_ctrl = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
            reset_headbar_data(&(p_ctrl->para));
            headbar_on_paint(p_msg, HB_PAINT_INIT);
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            headbar_ctl_t   *p_ctrl;

            p_ctrl  = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
            __wrn("*********GUI_MSG_TOUCH*******\n");

            switch (p_msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_DOWN:
                {
                    int     x, y;
                    __bool  ret_home;

                    x           = LOSWORD(p_msg->dwAddData2);
                    y           = HISWORD(p_msg->dwAddData2);
                    ret_home    = pt_in_rect(&(p_ctrl->para.home_rect), x, y, 0, 0, 0, 0);
                    __wrn("p_ctrl->para.home_rect.x=%d\n", p_ctrl->para.home_rect.x);
                    __wrn("p_ctrl->para.home_rect.y=%d\n", p_ctrl->para.home_rect.y);
                    __wrn("p_ctrl->para.home_rect.width=%d\n", p_ctrl->para.home_rect.width);
                    __wrn("p_ctrl->para.home_rect.x.height=%d\n", p_ctrl->para.home_rect.height);
                    __wrn("x=%d\n", x);
                    __wrn("y=%d\n", y);

                    if (ret_home == EPDK_TRUE)
                    {
                        headbar_on_paint(p_msg, HB_PAINT_HOME);
                    }

                    return EPDK_OK;
                }

                case GUI_MSG_TOUCH_UP:
                {
                    int     x, y;
                    __bool  ret1;
                    __bool  ret2;
                    __bool  ret3;
                    __bool  ret4;
                    __bool  ret5;
                    __bool  ret6;
                    //__bool  ret_home;

                    x       = LOSWORD(p_msg->dwAddData2);
                    y       = HISWORD(p_msg->dwAddData2);
                    ret1    = pt_in_rect(&(p_ctrl->para.volume_rect), x, y, 10, 0, 0, 0);
                    ret2    = pt_in_rect(&(p_ctrl->para.volume_data_rect), x, y, 0, 0, 0, 0);
                    ret3    = pt_in_rect(&(p_ctrl->para.home_rect), x, y, 0, 0, 0, 0);
                    ret4    = pt_in_rect(&(p_ctrl->para.bright_rect), x, y, 10, 0, 0, 0);
                    ret5    = pt_in_rect(&(p_ctrl->para.bright_data_rect), x, y, 0, 0, 0, 0);
                    ret6    = pt_in_rect(&(p_ctrl->para.music_rect), x, y, 0, 0, 0, 0);

                    //ret_home = pt_in_rect( &(p_ctrl->para.home_rect) , x, y, 0, 0, 0, 0);

                    if ((ret1 == EPDK_TRUE) || (ret2 == EPDK_TRUE))
                    {
                        __gui_notify_msg_t  notify_msg;

                        notify_msg.hWnd         = p_msg->h_deswin;
                        notify_msg.id           = GUI_WinGetItemId(p_msg->h_deswin);
                        notify_msg.msgcode      = HBAR_ADJUST_VOLUME;
                        notify_msg.dwAddData    = 0;
                        GUI_NotifyParent(&notify_msg);
                    }
                    else if (ret3 == EPDK_TRUE)
                    {
                        __gui_notify_msg_t  notify_msg;

                        headbar_on_paint(p_msg, HB_PAINT_HOME);
                        notify_msg.hWnd         = p_msg->h_deswin;
                        notify_msg.id           = GUI_WinGetItemId(p_msg->h_deswin);
                        notify_msg.msgcode      = HBAR_RETURN_HOME;
                        notify_msg.dwAddData    = 0;
                        GUI_NotifyParent(&notify_msg);
                    }
                    else if ((ret4 == EPDK_TRUE) || (ret5 == EPDK_TRUE))
                    {
                        __gui_notify_msg_t  notify_msg;

                        notify_msg.hWnd         = p_msg->h_deswin;
                        notify_msg.id           = GUI_WinGetItemId(p_msg->h_deswin);
                        notify_msg.msgcode      = HBAR_ADJUST_BRIGHT;
                        notify_msg.dwAddData    = 0;
                        GUI_NotifyParent(&notify_msg);
                    }
                    else if (ret6 == EPDK_TRUE)
                    {
                        __gui_notify_msg_t  notify_msg;

                        notify_msg.hWnd         = p_msg->h_deswin;
                        notify_msg.id           = GUI_WinGetItemId(p_msg->h_deswin);
                        notify_msg.msgcode      = HBAR_MUISC_STATE;
                        notify_msg.dwAddData    = 0;
                        GUI_NotifyParent(&notify_msg);
                    }

                    return 0;
                }

                default:
                    break;
            }

            return EPDK_OK;
        }

        case HBAR_TITLE_PAINT:
        {
            headbar_ctl_t   *p_ctrl = NULL;

            p_ctrl  = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
            eLIBs_memset(p_ctrl->para.title_buf, 0, 32);
            eLIBs_strncpy(p_ctrl->para.title_buf, (char *)p_msg->dwAddData1, 32);
            headbar_on_paint(p_msg, HB_PAINT_TITLE);
            return EPDK_OK;
        }

        case HBAR_INIT_PAINT:
        {
            headbar_on_paint(p_msg, HB_PAINT_INIT);
            return EPDK_OK;
        }

        case HBAR_TIME_PAINT:
        {
            headbar_on_paint(p_msg, HB_PAINT_TIME);
            return EPDK_OK;
        }

        case HBAR_DATA_PAINT:
        {
            headbar_on_paint(p_msg, HB_PAINT_DATA);
            return EPDK_OK;
        }

        case HBAR_VOL_PAINT:
        {
            headbar_on_paint(p_msg, HB_PAINT_VOL);
            return EPDK_OK;
        }

        case HBAR_BRIGHT_PAINT:
        {
            headbar_ctl_t   *p_ctrl;
            headbar_para_t  *para;
            __s32           val;

            p_ctrl  = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
            para    = &(p_ctrl->para);
            val     = bright_level_2_display((__lion_bright_t)p_msg->dwAddData1);

            if (val != para->data.bright)
            {
                para->data.bright = val;
                eLIBs_memset(para->bright_buf, 0, 32);
                eLIBs_int2str_dec(para->data.bright, para->bright_buf);
                headbar_on_paint(p_msg, HB_PAINT_BRIGHT);
            }

            return EPDK_OK;
        }

        case HBAR_VOLUME_PAINT:
        {
            headbar_ctl_t   *p_ctrl;
            headbar_para_t  *para;
            __s32           val;

            p_ctrl  = (headbar_ctl_t *)GUI_WinGetAttr(p_msg->h_deswin);
            para    = &(p_ctrl->para);
            val     = p_msg->dwAddData1;

            if (val != para->data.volume)
            {
                para->data.volume = val;
                eLIBs_memset(para->volume_buf, 0, 32);
                eLIBs_int2str_dec(para->data.volume, para->volume_buf);
                headbar_on_paint(p_msg, HB_PAINT_VOLUME);
            }

            return EPDK_OK;
        }

        case HBAR_MUSIC_BG_PAINT:
        {
            headbar_on_paint(p_msg, HB_PAINT_MUSCI_BG);
            return EPDK_OK;
        }

        case HBAR_SIG_STATUS_PAINT:
        {
            headbar_on_paint(p_msg, HB_PAINT_SIG_STATUS);
            return EPDK_OK;
        }

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(p_msg);
}

H_WIN headbar_frm_create(H_WIN parent, H_LYR layer)
{
    H_WIN                           h_win;
    //headbar_ctl_t                     *p_ctrl;
    __gui_framewincreate_para_t     para = {0};

    eLIBs_memset(&para, 0, sizeof(__gui_framewincreate_para_t));
    para.dwStyle          = WS_NONE | WS_VISIBLE;
    para.dwExStyle        = WS_EX_NONE;
    para.name             = NULL;
    para.hOwner           = 0;
    para.hHosting         = parent;
    para.FrameWinProc     = cb_headbar_framewin;
    para.id               = ID_HEADBAR_COM;
    para.hLayer           = layer;
    para.rect.x           = headbar_uipara.hbar_frmwin.x;
    para.rect.y           = headbar_uipara.hbar_frmwin.y;
    para.rect.width       = headbar_uipara.hbar_frmwin.w;
    para.rect.height      = headbar_uipara.hbar_frmwin.h;
	__inf("headbar_frm_create %d  %d  %d  %d ",para.rect.x,para.rect.y, para.rect.width, para.rect.height);

    h_win = GUI_FrmWinCreate(&para);

    if (!h_win)
    {
        __err("headbar frmwin create error ! \n");
    }

    return h_win;
}

int32_t headbar_frm_delete(H_WIN frmwin)
{
    if (frmwin)
    {
        GUI_FrmWinDelete(frmwin);
    }

    return EPDK_FAIL;
}

int32_t headbar_enable_paint_time(__bool enable)
{
    g_enable_paint_time = enable;
    return 0;
}

int32_t headbar_enable_paint_signal(__bool enable)
{
    g_enable_paint_signal   = enable;
    return 0;
}
