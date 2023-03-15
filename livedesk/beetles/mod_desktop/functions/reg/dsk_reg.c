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
#include "functions_i.h"
#include <kconfig.h>

#include "app_version.h"

#define REG_DEFAULT_BGD_PATH            "/data/apps/"
typedef enum
{
    ID_HOME_DTV = 0,
    ID_HOME_MOVIE,
    ID_HOME_MUSIC,
    ID_HOME_PICTURE,
    ID_HOME_EBOOK,
    ID_HOME_FM,
    ID_HOME_RECORD,
    ID_HOME_SETTING,
    ID_HOME_OTHERS
} root_home_id_t;


typedef struct
{
    char                *name;
    root_home_id_t      type;
} root_home_icon_name2enm_t;

static root_home_icon_name2enm_t g_home_icon_name2enm[] =
{
    {"Dtv",             ID_HOME_DTV},
    {"Movie",           ID_HOME_MOVIE},
    {"Music",           ID_HOME_MUSIC},
    {"Picture",         ID_HOME_PICTURE},
    {"Ebook",           ID_HOME_EBOOK},
    {"Fm",              ID_HOME_FM},
    {"Record",          ID_HOME_RECORD},
    {"setting",         ID_HOME_SETTING},
};

typedef struct
{
    reg_system_para_t   system_para;
    reg_init_para_t     init_para;
    reg_root_para_t     root_para;
    reg_movie_para_t    movie_para;
    reg_music_para_t    music_para;
    reg_photo_para_t    photo_para;
    reg_fm_para_t       fm_para;
    reg_ebook_para_t    ebook_para;
    reg_record_para_t   record_para;
    reg_calendar_para_t calendar_para;
    reg_game_para_t     game_para;
    reg_dict_para_t     dict_para;
} reg_app_para_t;

typedef struct
{
    __u32 restore_flag[REG_APP_MAX];
    reg_app_para_t para_current;//当前值
    reg_app_para_t para_default;//出厂默认值
    __u32 reserverd[3]; //保留参数
	__u32 check_sum;
} reg_all_para_t;

static reg_all_para_t   *all_app_para       = NULL;
static reg_all_para_t   *all_app_para_bak   = NULL;//add by lihaoyi, compare to check if need to flush flash

static void multi_screen_unit_init(pmulti_screen_t multi_screen_para)
{
    int16_t i   = 0;

    GUI_RECT desktop_iconRect[DESKTOP_UNIT_MAX] =
    {
        //固定应用
        REG_HOME_ICON_POS(0, 0, 1),
        REG_HOME_ICON_POS(0, 1, 1),
        REG_HOME_ICON_POS(0, 2, 1),
        REG_HOME_ICON_POS(0, 3, 1),

        REG_HOME_ICON_POS(1, 0, 0),
        REG_HOME_ICON_POS(1, 1, 0),
        REG_HOME_ICON_POS(1, 2, 0),
        REG_HOME_ICON_POS(1, 3, 0),

        REG_HOME_ICON_POS(1, 0, 1),
        REG_HOME_ICON_POS(1, 1, 1),
        { -1, -1,  -1, -1},
        { -1, -1,  -1, -1},

        //快捷键
        {0,   0,   99,  100},
        {0,   0,   57,  160},
        {0,   0,   64,  93 },
        {0,   0,   184, 156},

        {REG_HOME_ICON_POS_X0(0, 0, 0) + 30,            REG_HOME_ICON_POS_Y0(0, 0, 0), REG_HOME_ICON_POS_X1(0, 0, 0) + 30 + 5,      REG_HOME_ICON_POS_Y1(0, 0, 0)},
        {REG_HOME_ICON_POS_X0(0, 1, 0) + 30 * 2 + 5,    REG_HOME_ICON_POS_Y0(0, 1, 0), REG_HOME_ICON_POS_X1(0, 1, 0) + 30 * 2 + 5,  REG_HOME_ICON_POS_Y1(0, 1, 0)},
        {REG_HOME_ICON_POS_X0(0, 2, 0) + 30 * 2 + 5,    REG_HOME_ICON_POS_Y0(0, 2, 0), REG_HOME_ICON_POS_X1(0, 2, 0) + 30 * 2 + 5,  REG_HOME_ICON_POS_Y1(0, 2, 0)},
    };
    GUI_RECT    headbar_iconRect[HEADBAR_UNIT_MAX] =
    {
        {0,   30,  66,  96},
        {76,  30,  142, 96},
        {152, 30,  218, 96},
        {228, 30,  294, 96},

        {304, 30,  370, 96},
        {380, 30,  446, 96},
        {456, 30,  522, 96},
    };

    multi_screen_para->desktop_screen_index = 0;
    multi_screen_para->desktop_unit_num     = DESKTOP_UNIT_MAX;
    multi_screen_para->headbar_unit_num     = HEADBAR_UNIT_MAX;

    for (i = 0; i < DESKTOP_UNIT_MAX; i++)
    {
        multi_screen_para->desktop_unit[i].rect.x0 = desktop_iconRect[i].x0;
        multi_screen_para->desktop_unit[i].rect.y0 = desktop_iconRect[i].y0;
        multi_screen_para->desktop_unit[i].rect.x1 = desktop_iconRect[i].x1;
        multi_screen_para->desktop_unit[i].rect.y1 = desktop_iconRect[i].y1;

        if (i < DESKTOP_UNIT_AP_MAX || i >= DESKTOP_UNIT_AP_MAX + HEADBAR_INITIAL_MAX)
        {
            multi_screen_para->desktop_unit[i].isExist = EPDK_TRUE;
        }
        else
        {
            multi_screen_para->desktop_unit[i].isExist = EPDK_FALSE;
        }
    }

    for (i = 0; i < HEADBAR_UNIT_MAX; i++)
    {
        multi_screen_para->headbar_unit[i].rect.x0 = headbar_iconRect[i].x0;
        multi_screen_para->headbar_unit[i].rect.y0 = headbar_iconRect[i].y0;
        multi_screen_para->headbar_unit[i].rect.x1 = headbar_iconRect[i].x1;
        multi_screen_para->headbar_unit[i].rect.y1 = headbar_iconRect[i].y1;

        if (i < HEADBAR_INITIAL_MAX)
        {
            multi_screen_para->headbar_unit[i].isExist = EPDK_TRUE;
        }
        else
        {
            multi_screen_para->headbar_unit[i].isExist = EPDK_FALSE;
        }
    }
}

//读取配置文件默认配置
static __s32 __reg_paramter_config_init(reg_app_para_t *para)
{
    //init key val by app_config.fex
    char        *path      = "d:\\app_config.bin";
    void        *parser = NULL;
    int32_t     ret;
    int32_t     i;
    char        keyname[64];
    int         val;
    char        str_val[32];

    if (!para)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

    do
    {
        parser = 0;
        parser = esCFG_Init_Ex(path);

        if (0 == parser)
        {
            __wrn("esCFG_Init_Ex fail...");
            break;
        }

        eLIBs_memset(keyname, 0, sizeof(keyname));
        eLIBs_strcpy(keyname, "prog_ver");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "ver");
        eLIBs_memset(str_val, 0, sizeof(str_val));
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)str_val, sizeof(str_val) / sizeof(int));

        if (EPDK_OK != ret)
        {
            eLIBs_strcpy(para->system_para.ver_inf, "version unknown");
        }
        else
        {
            eLIBs_strcpy(para->system_para.ver_inf, str_val);
        }

        __wrn("before esCFG_GetKeyValue_Ex");
        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "volume");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);
        __wrn("after esCFG_GetKeyValue_Ex, ret=%d", ret);

        if (EPDK_OK == ret)
        {
            if (val < AUDIO_DEVICE_VOLUME_MIN || val > AUDIO_DEVICE_VOLUME_MAX)
            {
                val = 20;
            }

            para->system_para.volume = val;
            __wrn("default volume=%d", para->system_para.volume);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "lang");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (EPDK_LANGUAGE_ENM_ENGLISH != val
                && EPDK_LANGUAGE_ENM_CHINESES != val
                && EPDK_LANGUAGE_ENM_CHINESET != val)
            {
                val = EPDK_LANGUAGE_ENM_CHINESES;
            }

            para->system_para.language = val;
            __wrn("default lang=0x%x", para->system_para.language);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "style");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (val < 0)
            {
                val = 0;
            }

            para->init_para.default_bgd_index = val;
            __wrn("default style=0x%d", para->system_para.style);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "light");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (val < LION_BRIGHT_LEVEL1 || val > LION_BRIGHT_LEVEL16)
            {
                val = LION_BRIGHT_LEVEL12;
            }

            para->system_para.backlight = val;
            __wrn("default light=%d", para->system_para.backlight);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "power_off");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (0 * 60 != val
                && 15 * 60 != val
                && 30 * 60 != val
                && 45 * 60 != val
                && 60 * 60 != val)
            {
                val = 0 * 60;
            }

            para->system_para.poweroff = val;
            __wrn("default poweroff=%d", para->system_para.poweroff);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "keytone");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (val < 0 || val > 1)
            {
                val = 0;
            }

            para->system_para.keytone = val;
            __wrn("default keytone=%d", para->system_para.keytone);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "light_off");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (0 != val && 15 != val && 30 != val && 45 != val && 60 != val && 90 != val)
            {
                val = 0;
            }

            para->system_para.backlightoff = val;
            __wrn("default backlightoff=%d", para->system_para.backlightoff);
        }

        eLIBs_strcpy(keyname, "system");
        eLIBs_strcat(keyname, "/");
        eLIBs_strcat(keyname, "cvbs");
        ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&val, 1);

        if (EPDK_OK == ret)
        {
            if (val < 0 || val > 1)
            {
                val = 0;
            }

            para->system_para.pal_ntsc = val;
            __wrn("default cvbs=%d", para->system_para.pal_ntsc);
        }

        {
            __s32 i;
            __s32 size;
            __s32 ret;
            __s32 scn_no;
            __s32 x_no;
            __s32 y_no;
            size = sizeof(g_home_icon_name2enm) / sizeof(g_home_icon_name2enm[0]);

            for (i = 0 ; i < size ; i++)
            {
                eLIBs_strcpy(keyname, g_home_icon_name2enm[i].name);
                eLIBs_strcat(keyname, "/");
                eLIBs_strcat(keyname, "scn_no");
                ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&scn_no, 1);

                if (EPDK_OK != ret)
                {
                    continue;
                }

                if (scn_no < 0 || scn_no > 2)
                {
                    scn_no = 0;
                }

                eLIBs_strcpy(keyname, g_home_icon_name2enm[i].name);
                eLIBs_strcat(keyname, "/");
                eLIBs_strcat(keyname, "x_no");
                ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&x_no, 1);

                if (EPDK_OK != ret)
                {
                    continue;
                }

                if (x_no < 0 || x_no > 3)
                {
                    x_no = 0;
                }

                eLIBs_strcpy(keyname, g_home_icon_name2enm[i].name);
                eLIBs_strcat(keyname, "/");
                eLIBs_strcat(keyname, "y_no");
                ret = esCFG_GetKeyValue_Ex(parser, keyname, (__s32 *)&y_no, 1);

                if (EPDK_OK != ret)
                {
                    continue;
                }

                if (y_no < 0 || y_no > 1)
                {
                    y_no = 0;
                }

                {
                    GUI_RECT gui_rect;
                    gui_rect.x0 = REG_HOME_ICON_POS_X0(scn_no, x_no, y_no);
                    gui_rect.y0 = REG_HOME_ICON_POS_Y0(scn_no, x_no, y_no);
                    gui_rect.x1 = REG_HOME_ICON_POS_X1(scn_no, x_no, y_no);
                    gui_rect.y1 = REG_HOME_ICON_POS_Y1(scn_no, x_no, y_no);
                    para->system_para.multi_screen_para.desktop_unit[g_home_icon_name2enm[i].type].rect = gui_rect;
                }
            }
        }
    } while (0);

    if (parser)
    {
        esCFG_Exit_Ex(parser);
        parser = 0;
    }

    return EPDK_OK;
}

static __s32 __reg_paramter_init(reg_app_para_t *para)
{
    __u16 i = 0;

    if (para == NULL)
    {
        __err("invalid para...");
        return EPDK_FAIL;
    }

//    multi_screen_unit_init(&para->system_para.multi_screen_para);

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        eLIBs_strcpy(para->system_para.alarm[i].alarm_music_path, REG_ALARM_MUSIC_DEFAULT_PATH);
        para->system_para.alarm[i].alarm_runtime    = REG_ALARM_50SEC;
        para->system_para.alarm[i].alarm_volume     = 20;
        para->system_para.alarm[i].alarm_time.hour  = 12;
        para->system_para.alarm[i].alarm_time.minute = 0;
        para->system_para.alarm[i].alarm_time.second = 0;
    }

    para->system_para.audio_output = 0;//normal
    para->system_para.channel = 0; //立体声
    para->system_para.charset = 5; //gbk
    para->system_para.detect_gate = 1; //on
    para->system_para.volume =  AUDIO_DEVICE_VOLUME_MAX / 2; //20;
    para->system_para.language = EPDK_LANGUAGE_ENM_CHINESES; //简体中文
    para->system_para.style = 0;
    para->system_para.backlight = 10;
    para->system_para.backlightoff = 0; //不自动关屏
    para->system_para.poweroff = 0; //3*60;//不自动关机
    para->system_para.keytone = 0; //按键音是否打开
    /*LCD Parameter Start*/
    para->system_para.lcd_bright = 5;
    para->system_para.lcd_constract = 5;
    para->system_para.lcd_saturation = 5;
    para->system_para.lcd_hue = 5;
    para->system_para.lcd_detail = 5;
    /*LCD Parameter End*/
    para->system_para.gamma_r_value = 0x0a;
    para->system_para.gamma_g_value = 0x0a;
    para->system_para.gamma_b_value = 0x0a;
    para->system_para.output = LION_DISP_LCD;
    para->system_para.pal_ntsc = 1; //0代表ntsc, 1代表pal
    para->system_para.time_set = 1;
    para->system_para.date.year = 2012;
    para->system_para.date.month = 5;
    para->system_para.date.day = 1;
    para->system_para.time.hour = 12;
    para->system_para.time.minute = 0;
    para->system_para.time.second = 0;
    para->system_para.tp_adjust_finish_flag = 0;
    para->init_para.default_bgd_index = 0;
//    eLIBs_strcpy(para->init_para.default_bgd_path, REG_DEFAULT_BGD_PATH);
//    eLIBs_strcpy(para->init_para.setting_bgd_path, "");
    para->init_para.bgd_flag = 1; //默认背景
    para->init_para.bgd_flag_in_main = 1;//默认背景
    para->root_para.cur_play_storage = -1;           //最后一次运行的ap类型，-1:空 0:音乐,1:视频
    para->root_para.last_play_app = 0;          //最后一次运行的ap类型，0:音乐,1:视频
//    eLIBs_memset(para->root_para.last_app_play_storage, 0, sizeof(para->root_para.last_app_play_storage));
//    eLIBs_memset(para->root_para.last_storage_play_app, 0, sizeof(para->root_para.last_storage_play_app));
    //   para->root_para.last_app_name;
    para->root_para.last_movie_index_sd = 0;
    //  para->root_para.last_movie_path_sd;
    para->root_para.last_movie_index_ud = 0;
    //  para->root_para.last_movie_path_ud;
    para->root_para.last_music_index_sd = 0;
    //  para->root_para.last_music_path_sd;
    para->root_para.last_music_index_ud = 0;
    //  para->root_para.last_music_path_ud;
    para->root_para.last_photo_index_sd = 0;
    //  para->root_para.last_photo_path_sd;
    para->root_para.last_photo_index_ud = 0;
    //  para->root_para.last_photo_path_ud;
    para->root_para.last_ebook_index_sd = 0;
    //  para->root_para.last_ebook_path_sd;
    para->root_para.last_ebook_index_ud = 0;
    //  para->root_para.last_ebook_path_ud =0;
    para->movie_para.rotate_mode = 2; //ROTATE_ALL
    para->movie_para.zoom_mode = 0; //VIDEO_RATIO
    para->movie_para.sub_state = 1; //on
    para->movie_para.sub_color = GUI_WHITE;
    para->movie_para.sub_stream = 0;
    para->movie_para.sub_pos = 0; //下面
    para->movie_para.channel = 0; //立体声
    para->movie_para.track = 0;//音轨0  //chengf3  add
    para->music_para.start_item_id = 0;
    para->music_para.focus_item_id = 0;
    para->music_para.play_mode = 2; //ROTATE_ALL
    para->music_para.EQ_mode = 0; //NORMAL
    para->photo_para.bgmusic = 0; //关闭
    para->photo_para.speedflag = 1;
    para->photo_para.interval_time = 6;
    para->photo_para.ratio = 0;
    para->photo_para.effect = 0; //默认无切换效果
    para->ebook_para.text_colour = GUI_WHITE;
    para->ebook_para.switch_time = 10;
    para->ebook_para.switch_time_flag = 0;
    para->ebook_para.bg_music_switch = 0;
    para->ebook_para.tts_switch = 1;            /*默认为关闭状态*/
    para->fm_para.mode = 0;
    para->fm_para.channel_id = 0;
    para->fm_para.FM_channel[0] = 87500;
    para->fm_para.FM_channel[1] = 88500;
    para->fm_para.FM_channel[2] = 89800;
    para->fm_para.FM_channel[3] = 93900;
    para->fm_para.FM_channel[4] = 103000;
    para->fm_para.FM_channel[5] = 104300;
    para->fm_para.FM_channel[6] = 107100;
    para->fm_para.reserverd[0] = 7;
    para->fm_para.reserverd[2] = para->fm_para.FM_channel[para->fm_para.channel_id];

    para->record_para.reserverd[0] = 2;//录音质量 0:差 1:中 2:好
    para->record_para.reserverd[1] = 0;//录音格式 0:mp3 1:wav
    para->calendar_para.time_format = 0;

//    eLIBs_memcpy(para->system_para.ver_inf, APP_SDK_VERSION, sizeof(APP_SDK_VERSION));
    return EPDK_OK;
}
typedef struct tag_CRC32_DATA
{
	__u32 CRC;             
	__u32 CRC_32_Tbl[256];
} CRC32_DATA_t;
static __u32 calc_crc32(const unsigned char *buffer, __u32 length)
{
	__u32 i, j;
	CRC32_DATA_t crc32;
	__u32 CRC32 = 0xffffffff;  /* 设置初始值 */
	crc32.CRC = 0;

	for ( i = 0; i < 256; ++i) /* 用++i以提高效率 */
	{
		crc32.CRC = i;
		for ( j = 0; j < 8 ; ++j)
		{
			if (crc32.CRC & 1)
				crc32.CRC = (crc32.CRC >> 1) ^ 0xEDB88320;
			else
				crc32.CRC >>= 1;
		}
		crc32.CRC_32_Tbl[i] = crc32.CRC;
	}

	CRC32 = 0xffffffff; /* 设置初始值 */
    for ( i = 0; i < length; ++i)
    {
        CRC32 = crc32.CRC_32_Tbl[(CRC32^((unsigned char*)buffer)[i]) & 0xff] ^ (CRC32 >> 8);
    }

	return CRC32^0xffffffff;
}

#define REG_SETTING_PARA_FILE_PATH        "e:\\setting.bin"
static __s32  dsk_reg_read_config_file(unsigned char *filename, reg_all_para_t *reg_para)
{
    ES_FILE *fp = NULL;
	__s32 read_num = 0;
	
	fp = eLIBs_fopen(filename, "rb");
    if (fp == NULL)
    {
        eLIBs_printf("%s %d %s eLIBs_fopen fail\n", __func__, __LINE__, filename);
		eLIBs_fclose(fp);
        return EPDK_FAIL;
    }

	eLIBs_fseek(fp, 0, SEEK_SET);
	eLIBs_memset(reg_para, 0x00, sizeof(reg_all_para_t));
	read_num = eLIBs_fread(reg_para, 1, sizeof(reg_all_para_t), fp);
	if (sizeof(reg_all_para_t) != read_num)
	{
        eLIBs_printf("%s %d file_len:%d, read_num:%d\n", __func__, __LINE__, filename, sizeof(reg_all_para_t), read_num);
		eLIBs_fclose(fp);
        return EPDK_FAIL;
	}
	if (reg_para->check_sum != calc_crc32((unsigned char*)reg_para, sizeof(reg_all_para_t) - sizeof(reg_para->check_sum)))
	{
        eLIBs_printf("%s %d %s check_sum not match\n", __func__, __LINE__, filename);
        eLIBs_printf("%s %d reg_para->check_sum:0x%x\n", __func__, __LINE__, reg_para->check_sum);
		eLIBs_printf("%s %d check_sum:0x%x\n", __func__, __LINE__, calc_crc32((unsigned char*)reg_para, sizeof(reg_all_para_t) - sizeof(reg_para->check_sum)));
		eLIBs_fclose(fp);
		return EPDK_FAIL;
	}
	eLIBs_fclose(fp);
	return EPDK_OK;
}
static __s32  dsk_reg_write_config_file(unsigned char *filename, reg_all_para_t *reg_para)
{
    ES_FILE *fp = NULL;
	__s32 write_num = 0;
	
	fp = eLIBs_fopen(filename, "wb");
    if (fp == NULL)
    {
        eLIBs_printf("%s %d eLIBs_fopen %s fail\n", __func__, __LINE__, filename);
        return EPDK_FAIL;
    }
	
	eLIBs_fseek(fp, 0, SEEK_SET);
	write_num = eLIBs_fwrite(reg_para, 1, sizeof(reg_all_para_t), fp);		
	if (write_num != sizeof(reg_all_para_t)) 
	{
        eLIBs_printf("%s %d eLIBs_fwrite %s not finish\n", __func__, __LINE__, filename);
		eLIBs_fsync(fp);
		eLIBs_fclose(fp);
        return EPDK_FAIL;
	}
	
	eLIBs_fsync(fp);
	eLIBs_fclose(fp);
	return EPDK_OK;
}
static __s32 dsk_reg_flush_config_file(unsigned char *filename)
{
    ES_FILE *fp = NULL;
	__u32 offset = 0, len = 0;

	fp = eLIBs_fopen(filename, "rb+");
    if (fp == NULL)
    {
        eLIBs_printf("%s %d   eLIBs_fopen %s fail\n", __func__, __LINE__, filename);
        return EPDK_FAIL;
    }
	
	offset = 0;
	len = 512;
	for (offset = 0; (offset + len) < sizeof(reg_all_para_t); offset += len)
	{
		if (eLIBs_memcmp((unsigned char *)all_app_para + offset, (unsigned char *)all_app_para_bak + offset, len) == EPDK_NO)
		{
			eLIBs_fseek(fp, offset, SEEK_SET);
			eLIBs_fwrite((unsigned char *)all_app_para + offset, 1, len, fp);
		}
	}
	len = sizeof(reg_all_para_t) - offset;
	if (eLIBs_memcmp((unsigned char *)all_app_para + offset, (unsigned char *)all_app_para_bak + offset, len) == EPDK_NO)
	{
		eLIBs_fseek(fp, offset, SEEK_SET);
		eLIBs_fwrite((unsigned char *)all_app_para + offset, 1, len, fp);
	}
	eLIBs_fsync(fp);
	eLIBs_fclose(fp);
	return EPDK_OK;
}

static __krnl_event_t *spinor_proc_sem;
static __krnl_event_t *spinor_proc_flag_sem;
static __u32 dsk_reg_flush_thread_id = 0;
static void save_flash_task(void *parg)
{
	int fd = 0;
    __u8                        err;

	fd = open("/dev/udisk_flush", O_RDONLY);
//	eLIBs_printf("%s %d /dev/udisk_flush\n", __func__, __LINE__);
	if (fd < 0) {
		eLIBs_printf("can not find device:%s\n", "/dev/udisk_flush");
		esKRNL_TDel(EXEC_prioself);
	}
	while (1)
    {
        //check if any request to delete audio play device main task
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            eLIBs_printf("delete save_flash_tasky!");
            goto _save_flash_task_exit;
        }
//		eLIBs_printf("%s %d wait\n", __func__, __LINE__);
        esKRNL_SemPend(spinor_proc_sem, 0, &err);		
        esKRNL_SemPend(spinor_proc_flag_sem, 0, &err);		
        esKRNL_TimeDly(50);
//		eLIBs_printf("%s %d begin\n", __func__, __LINE__);
		dsk_reg_flush_config_file(REG_SETTING_PARA_FILE_PATH);
		eLIBs_memcpy(all_app_para_bak, all_app_para, sizeof(reg_all_para_t));
		ioctl(fd, DEV_IOC_USR_FLUSH_CACHE, 0);
        esKRNL_TimeDly(1);
        esKRNL_SemPost(spinor_proc_flag_sem);		
//		eLIBs_printf("%s %d end\n", __func__, __LINE__);
    }
_save_flash_task_exit:
	close(fd);
    eLIBs_printf("save_flash_tasky main task end!");
    esKRNL_TDel(EXEC_prioself);
}
#ifdef	CONFIG_COMMAND_PQD_USELESS
#define DEST_CFG_NAME	"e:\\disp_firmware"
#define SOURCE_CFG_NAME	"d:\\res\\disp_firmware"
__s32 dsk_reg_copy_lcd_screen_cfg(void)
{
	char *file_buf = NULL;
    __u32 file_len = 0, count = 0, ret = -1;
    ES_FILE *source_fp = NULL, *dest_fp = NULL;

	source_fp = eLIBs_fopen(SOURCE_CFG_NAME, "rb");
    if (source_fp == NULL)
    {
        eLIBs_printf("%s %d eLIBs_fopen %s fail\n", __func__, __LINE__, SOURCE_CFG_NAME);
        goto err;
    }	
    eLIBs_fseek(source_fp, 0, SEEK_END);
    file_len = eLIBs_ftell(source_fp);
    eLIBs_fseek(source_fp, 0, SEEK_SET);

	dest_fp = eLIBs_fopen(DEST_CFG_NAME, "wb");
    if (dest_fp == NULL)
    {
        eLIBs_printf("%s %d eLIBs_fopen %s fail\n", __func__, __LINE__, DEST_CFG_NAME);
        goto err;
    }	
	eLIBs_fseek(dest_fp, 0, SEEK_SET);
	
    file_buf = (unsigned char *)eLIBs_malloc(sizeof(unsigned char) * file_len);
    if (file_buf == NULL)
    {
        eLIBs_printf("%s %d %s  file_buf malloc null \n", __func__, __LINE__);
        goto err;
    }
	eLIBs_memset(file_buf, 0x00, sizeof(unsigned char) * file_len);
	
    count = eLIBs_fread(file_buf, sizeof(unsigned char), file_len, source_fp);
    if (count != file_len)
    {
        eLIBs_printf("%s %d %s  eLIBs_fread count != file_len\n", __func__, __LINE__);
        goto err;
    }
	count = eLIBs_fwrite(file_buf, sizeof(unsigned char), file_len, dest_fp);		
	if (count != file_len) 
	{
        eLIBs_printf("%s %d %s  eLIBs_fwrite count != file_len\n", __func__, __LINE__);
        goto err;
	}
	ret = 0;
err:
	if (file_buf)
	{
		eLIBs_free(file_buf);
		file_buf = NULL;
	}
	if (source_fp)
	{
		eLIBs_fclose(source_fp);
	}
	if (dest_fp)
	{
		eLIBs_fsync(dest_fp);
		eLIBs_fclose(dest_fp);
	}
	return ret;
}
#endif
__s32 dsk_reg_init_para(void)
{
	__s32 ret = EPDK_OK;
	
    if (all_app_para != NULL)
    {
        __msg("para already init...");
        return EPDK_OK;
    }

    //分配内存
    all_app_para = esMEMS_Malloc(0, sizeof(reg_all_para_t));

    if (all_app_para == NULL)
    {
        __msg("mem not enough...");
        return EPDK_FAIL;
    }
    eLIBs_memset(all_app_para, 0x00, sizeof(reg_all_para_t));
    all_app_para_bak = esMEMS_Malloc(0, sizeof(reg_all_para_t));

    if (all_app_para_bak == NULL)
    {
        __msg("mem not enough...");
        return EPDK_FAIL;
    }
	
	spinor_proc_sem = esKRNL_SemCreate(0);
	spinor_proc_flag_sem = esKRNL_SemCreate(1);
	dsk_reg_flush_thread_id = esKRNL_TCreate(save_flash_task, NULL, 0x4000, KRNL_priolevel7);
    eLIBs_memset(all_app_para_bak, 0x00, sizeof(reg_all_para_t));
    //尝试加载文件
    if (dsk_reg_read_config_file(REG_SETTING_PARA_FILE_PATH, all_app_para) == EPDK_FAIL)
    {
		__s32 sta = 0;
//		__ES_FSSTAT fstat;

		eLIBs_printf("%s %d config not ok, format e:\n", __func__, __LINE__);
//		if (eLIBs_statfs("e:\\", &fstat) != 0)
		eLIBs_format("e:\\", "fat", 0);
#ifdef	CONFIG_COMMAND_PQD_USELESS
		dsk_reg_copy_lcd_screen_cfg();
#endif
		eLIBs_memset(all_app_para, 0, sizeof(reg_all_para_t));
		sta = __reg_paramter_init(&all_app_para->para_current);			
		sta = __reg_paramter_init(&all_app_para->para_default);			
		if (EPDK_FAIL == sta)
		{
			eLIBs_printf("%s %d __reg_paramter_init fail\n", __func__, __LINE__);
			ret = EPDK_FAIL;
		}
		all_app_para->check_sum = calc_crc32((unsigned char*)all_app_para, sizeof(reg_all_para_t) - sizeof(all_app_para->check_sum));
		if (dsk_reg_write_config_file(REG_SETTING_PARA_FILE_PATH, all_app_para) == EPDK_FAIL)
		{
			eLIBs_printf("%s %d rewrite %s fail\n", __func__, __LINE__, REG_SETTING_PARA_FILE_PATH);
			ret = EPDK_FAIL;
		}
		{
			int fd = 0;
		    fd = open("/dev/udisk_flush", O_RDONLY);
			if (fd < 0) {
		        eLIBs_printf("can not find device:%s\n", "/dev/udisk_flush");
		        return EPDK_FAIL;
			}
			ioctl(fd, DEV_IOC_USR_FLUSH_CACHE, 0);
			close(fd);
		}
	} 
	eLIBs_memcpy(all_app_para_bak, all_app_para, sizeof(reg_all_para_t));
	return ret;	
}
__s32 dsk_reg_flush(void)
{
	__s32 ret = EPDK_OK;
	OS_SEM_DATA p_sem_data;

	if (!all_app_para || !all_app_para_bak)
    {
        __wrn("reg para not init...");
        return EPDK_FALSE;
    }

	eLIBs_memset(&p_sem_data, 0x00, sizeof(OS_SEM_DATA));
	esKRNL_SemQuery(spinor_proc_flag_sem, &p_sem_data);
	if (p_sem_data.OSCnt == 1)
	{
		//if (EPDK_YES != eLIBs_memcmp(all_app_para, all_app_para_bak, sizeof(reg_all_para_t))) //不等时才需要保存
		{
			all_app_para->check_sum = calc_crc32((unsigned char*)all_app_para, sizeof(reg_all_para_t) - sizeof(all_app_para->check_sum));
			esKRNL_SemPost(spinor_proc_sem);
		}
	}
	return ret;
}


__s32 dsk_reg_deinit_para(void)
{
	__u8 error = 0;
	while (esKRNL_TDelReq(dsk_reg_flush_thread_id) != OS_TASK_NOT_EXIST)
	{
	   esKRNL_SemPost(spinor_proc_sem);
	   esKRNL_SemPost(spinor_proc_flag_sem);
	   esKRNL_TimeDly(1);
	}

    if (all_app_para)
    {
        esMEMS_Mfree(0, all_app_para);
        all_app_para = NULL;
    }

    if (all_app_para_bak)
    {
        esMEMS_Mfree(0, all_app_para_bak);
        all_app_para_bak = NULL;
    }
	esKRNL_SemDel(spinor_proc_sem, OS_DEL_ALWAYS, &error);
	esKRNL_SemDel(spinor_proc_flag_sem, OS_DEL_ALWAYS, &error);
	spinor_proc_sem = NULL;
	spinor_proc_flag_sem = NULL;
    return EPDK_OK;
}

void *dsk_reg_get_para_by_app(reg_app_e eApp)
{
    if (!all_app_para)
    {
        __err("reg para not init...");
        return NULL;
    }

    switch (eApp)
    {
        case REG_APP_SYSTEM:
            return &all_app_para->para_current.system_para;

        case REG_APP_INIT:
            return &all_app_para->para_current.init_para;

        case REG_APP_ROOT:
            return &all_app_para->para_current.root_para;

        case REG_APP_MOVIE:
            return &all_app_para->para_current.movie_para;

        case REG_APP_MUSIC:
            return &all_app_para->para_current.music_para;

        case REG_APP_PHOTO:
            return &all_app_para->para_current.photo_para;

        case REG_APP_EBOOK:
            return &all_app_para->para_current.ebook_para;

        case REG_APP_FM:
            return &all_app_para->para_current.fm_para;

        case REG_APP_RECORD:
            return &all_app_para->para_current.record_para;

        case REG_APP_CALENDAR:
            return &all_app_para->para_current.calendar_para;

        case REG_APP_GAME:
            return &all_app_para->para_current.game_para;

        case REG_APP_DICT:
            return &all_app_para->para_current.dict_para;

        default:
            break;
    }

    __err("invalid para...");
    return NULL;
}

void *dsk_reg_get_default_para_by_app(reg_app_e eApp)
{
    if (!all_app_para)
    {
        __err("reg para not init...");
        return EPDK_FALSE;
    }

    switch (eApp)
    {
        case REG_APP_SYSTEM:
            return &all_app_para->para_default.system_para;

        case REG_APP_INIT:
            return &all_app_para->para_default.init_para;

        case REG_APP_ROOT:
            return &all_app_para->para_default.root_para;

        case REG_APP_MOVIE:
            return &all_app_para->para_default.movie_para;

        case REG_APP_MUSIC:
            return &all_app_para->para_default.music_para;

        case REG_APP_PHOTO:
            return &all_app_para->para_default.photo_para;

        case REG_APP_EBOOK:
            return &all_app_para->para_default.ebook_para;

        case REG_APP_FM:
            return &all_app_para->para_default.fm_para;

        case REG_APP_RECORD:
            return &all_app_para->para_default.record_para;

        case REG_APP_CALENDAR:
            return &all_app_para->para_default.calendar_para;

        case REG_APP_GAME:
            return &all_app_para->para_default.game_para;

        case REG_APP_DICT:
            return &all_app_para->para_default.dict_para;

        default:
            break;
    }

    __err("invalid para...");
    return NULL;
}

__bool dsk_reg_get_app_restore_flag(reg_app_e eApp)
{
    if (!all_app_para)
    {
        __err("reg para not init...");
        return EPDK_FALSE;
    }

    if (eApp >= REG_APP_MAX)
    {
        __err("invalid para...");
        return 0;
    }

    return (__bool)(all_app_para->restore_flag[eApp]);
}

int32_t dsk_reg_set_app_restore_flag(reg_app_e eApp, __bool flag)
{
    if (!all_app_para)
    {
        __err("reg para not init...");
        return EPDK_FALSE;
    }

    if (eApp >= REG_APP_MAX)
    {
        __err("invalid para...");
        return 0;
    }

    all_app_para->restore_flag[eApp] = flag;
    return EPDK_OK;
}

int32_t dsk_reg_save_cur_play_info(reg_app_e eApp, __s32 index, char *filename, rat_type_t root_type)
{
    if (!all_app_para)
    {
        __err("reg para not init...");
        return EPDK_FAIL;
    }

    switch (eApp)
    {
        case REG_APP_MOVIE:
        {
            if (RAT_TF == root_type)
            {
                all_app_para->para_current.root_para.last_movie_index_sd = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_movie_path_sd, filename);
            }
            else if (RAT_USB == root_type)
            {
                all_app_para->para_current.root_para.last_movie_index_ud = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_movie_path_ud, filename);
            }

            break;
        }

        case REG_APP_MUSIC:
        {
            if (RAT_TF == root_type)
            {
                all_app_para->para_current.root_para.last_music_index_sd = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_music_path_sd, filename);
            }
            else if (RAT_USB == root_type)
            {
                all_app_para->para_current.root_para.last_music_index_ud = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_music_path_ud, filename);
            }

            break;
        }

        case REG_APP_PHOTO:
        {
            if (RAT_TF == root_type)
            {
                all_app_para->para_current.root_para.last_photo_index_sd = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_photo_path_sd, filename);
            }
            else if (RAT_USB == root_type)
            {
                all_app_para->para_current.root_para.last_photo_index_ud = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_photo_path_ud, filename);
            }

            break;
        }

        case REG_APP_EBOOK:
        {
            if (RAT_TF == root_type)
            {
                all_app_para->para_current.root_para.last_ebook_index_sd = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_ebook_path_sd, filename);
            }
            else if (RAT_USB == root_type)
            {
                all_app_para->para_current.root_para.last_ebook_index_ud = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_ebook_path_ud, filename);
            }

            break;
        }

        case REG_APP_GAME:
        {
            if (RAT_TF == root_type)
            {
                all_app_para->para_current.root_para.last_game_index_sd = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_game_path_sd, filename);
            }
            else if (RAT_USB == root_type)
            {
                all_app_para->para_current.root_para.last_game_index_ud = index;
                eLIBs_strcpy(all_app_para->para_current.root_para.last_game_path_ud, filename);
            }

            break;
        }

        default:
            __err("invalid app type...");
            return EPDK_FAIL;
    }

    return EPDK_OK;
}


int32_t dsk_reg_set_app_restore_all(void)
{
    if (!all_app_para)
    {
        __err("reg para not init...");
        return EPDK_FAIL;
    }

    eLIBs_memcpy(&all_app_para->para_current, (const void *)(&all_app_para->para_default), sizeof(reg_app_para_t));
    return EPDK_OK;
}
