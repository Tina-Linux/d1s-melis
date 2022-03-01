
#include <log.h>
#include "record_video.h"
#include "desktop_scene.h"
#include <emodules/tsc_record.h>
#include "dtv_mid.h"

#define     RECORD_VIDEO_ICON_NUM                   (2)

#define     RECORD_VIDEO_SELECT_FREQ_Y          (46)//46 headbar height
#define     RECORD_VIDEO_LISTBAR_HIGH           (35)
#define     RECORD_VIDEO_LISTBAR_WIDTH          (311)
#define     RECORD_VIDEO_LISTBAR_TOP_HIGH       (35 + RECORD_VIDEO_SELECT_FREQ_Y)

#define     RECORD_VIDEO_ICON_HEIGHT                (58)
#define     RECORD_VIDEO_START_STOP_WIDTH       (200)
#define     RECORD_VIDEO_MUSIC_LIST_WIDTH       (104)
#define     REFRESH_TIME_ID                     (0x5005)
#define     REFRESH_TIME_OUT                        (200)
#define     RECORD_TIME_ID                          (0x5006)
#define     RECORD_TIME_OUT                     (100)
typedef enum
{
    PIN_PULL_DEFAULT    =   0xFF,
    PIN_PULL_DISABLE    =   0x00,
    PIN_PULL_UP         =   0x01,
    PIN_PULL_DOWN       =   0x02,
    PIN_PULL_RESERVED   =   0x03
} pin_pull_t;

static __s32 _record_video_init_listbar(__gui_msg_t *msg);

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR record_video_layer_32bpp_create(void)
{
    __s32   width = 0, height = 0;
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
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "record video layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    dsk_display_get_size(&width, &height);
    fb.size.width       = width;
    fb.size.height      = height;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = width;
    lstlyr.src_win.height   = height;
    lstlyr.scn_win.x        = 0;
    lstlyr.scn_win.y        = 0;
    lstlyr.scn_win.width    = width;
    lstlyr.scn_win.height   = height;
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
static __s32 record_video_create_file_name(record_video_para_t *record_video_para)
{
    __awos_date_t date;
    __awos_time_t time;
    unsigned char File_Str[64] = {0};

    if (record_video_para == NULL)
    {
        __wrn("%s %d record_video_para null", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    eLIBs_memset(&date, 0x00, sizeof(__awos_date_t));
    eLIBs_memset(&time, 0x00, sizeof(__awos_time_t));
    eLIBs_memset(record_video_para->File_Path, 0x00, sizeof(record_video_para->File_Path));
    eLIBs_GetDate(&date);
    eLIBs_GetTime(&time);
    eLIBs_sprintf(File_Str, "\\%04d%02d%02d%02d%02d%02d.ts", date.year, date.month, date.day, time.hour, time.minute, time.second);
    //__wrn("File_name:%s", File_Str);
    //__wrn("%s %d record_video_para->File_Path:%s", __FILE__, __LINE__, record_video_para->disk);
    eLIBs_strcat(record_video_para->File_Path, record_video_para->disk);
    //__wrn("%s %d record_video_para->File_Path:%s", __FILE__, __LINE__, record_video_para->File_Path);
    eLIBs_strcat(record_video_para->File_Path, File_Str);
    //__wrn("%s %d record_video_para->File_Path:%s", __FILE__, __LINE__, record_video_para->File_Path);
    return EPDK_OK;
}
static __hdle record_video_antenna_switch_hdl = NULL;
static __s32 record_video_antenna_switch_ctrl_init(void)
{
    __s32            ret;
    user_gpio_set_t  gpio_set[1];
    eLIBs_memset(gpio_set, 0, sizeof(user_gpio_set_t));
    ret = esCFG_GetKeyValue("dtmb_para", "antenna_switch", (__s32 *)gpio_set, sizeof(user_gpio_set_t) / 4);

    if (!ret)
    {
        record_video_antenna_switch_hdl = esPINS_PinGrpReq(gpio_set, 1);

        if (!record_video_antenna_switch_hdl)
        {
            __err("request antenna_switch pin failed");
            return EPDK_FAIL;
        }
    }
    else
    {
        __msg("fetch para from script failed");
        return EPDK_FAIL;
    }

    __msg("maple_antenna_switch_ctrl_init ok");
    esPINS_SetPinPull(record_video_antenna_switch_hdl, PIN_PULL_UP, NULL);
    //  esPINS_PinGrpRel(hdl, 0);
    return EPDK_OK;
}
static __s32 record_video_set_antenna_mode(__u8 mode)
{
    if (mode == 0) //拉杆天线
    {
        esPINS_WritePinData(record_video_antenna_switch_hdl, 1, NULL); //pg0 控制天线切换的GPIO口,内置
    }
    else
    {
        esPINS_WritePinData(record_video_antenna_switch_hdl, 0, NULL); //pg0 控制天线切换的GPIO口,外
    }

    return 0;
}

static __s32 record_video_antenna_switch_ctrl_uninit(void)
{
    if (record_video_antenna_switch_hdl)
    {
        esPINS_PinGrpRel(record_video_antenna_switch_hdl, 0);
    }

    return EPDK_OK;
}

static __s32 record_video_check_disk_exist(record_video_para_t *record_video_para)
{
    __s32   len = 0;
    __s32   ret = EPDK_FAIL;
    char    disk_name[RAT_MAX_PARTITION][4] = {0};

    if (record_video_para == NULL)
    {
        //__wrn("%s %d record_video_para null", __FILE__, __LINE__);
        return EPDK_FALSE;
    }

    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
    ret  = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
    len = eLIBs_strlen(disk_name[0]);

    if ((EPDK_OK == ret) && (len != 0))
    {
        //__wrn("%s %d disk_name[0]:%s", __FILE__, __LINE__, disk_name[0]);
        eLIBs_memcpy(record_video_para->disk, disk_name[0], len);
        return RAT_TF;
    }

    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
    ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
    len = eLIBs_strlen(disk_name[0]);

    if ((EPDK_OK == ret) && (len != 0))
    {
        //__wrn("%s %d disk_name[0]:%s", __FILE__, __LINE__, disk_name[0]);
        eLIBs_memcpy(record_video_para->disk, disk_name[0], len);
        return RAT_USB;
    }

    return EPDK_FALSE;
}

static void  main_cmd2parent(H_WIN hwin, __s32 id, __u32 data1, __u32 data2)
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
static unsigned char *record_video_uint2time(__u32 time, unsigned char *time_str)//time per 10ms
{
    __u32 second = 0, minute = 0, hour = 0;

    if (time_str == NULL)
    {
        return NULL;
    }

    hour = time / (60 * 60);
    minute = time % (60 * 60) / 60;
    second = time % 60;
    eLIBs_sprintf(time_str, "%02d:%02d:%02d", hour, minute, second);
    //__wrn("time:%2d:%2d:%2d", hour, minute, second);
    return time_str;
}
static __u8 *record_video_str2lower(__u8 *input)
{
    __u8 *temp_input = input;

    if (input == NULL)
    {
        __log("%s %d param error", __FILE__, __LINE__);
        return NULL;
    }

    while (*temp_input != '\0')
    {
        if ((*temp_input >= 'A') && (*temp_input <= 'Z'))
        {
            *temp_input = *temp_input + ('a' - 'A');
        }

        temp_input++;
    }

    return input;
}
static __s32 record_video_match_freq(record_video_para_t *record_video_para)
{
    __u8 Index = 0;

    if (record_video_para->record_frequency == 0)
    {
        record_video_para->listbar_focus_id = Index;
        record_video_para->record_frequency = maple_get_search_freq(Index);
        record_video_para->record_bandwidth = maple_get_search_bandwidth(Index);
        return EPDK_OK;
    }

    for (Index = 0; Index < record_video_para->total_freq; Index++)
    {
        __wrn("%s %d freq:%d", __FILE__, __LINE__, maple_get_search_freq(Index));

        if (record_video_para->record_frequency == maple_get_search_freq(Index))
        {
            record_video_para->listbar_focus_id = Index;
            record_video_para->record_bandwidth = maple_get_search_bandwidth(Index);
            __wrn("%s %d Index:%d", __FILE__, __LINE__, Index);
            return EPDK_OK;
        }
    }

    if (Index == record_video_para->total_freq)
    {
        record_video_para->listbar_focus_id = 0;
        record_video_para->record_frequency = maple_get_search_freq(0);
        record_video_para->record_bandwidth = maple_get_search_bandwidth(0);
        __wrn("%s %d Index:%d", __FILE__, __LINE__, Index);
        return EPDK_FAIL;
    }

    return EPDK_FAIL;
}
static __s32 record_video_search_item_paint_ex(__lbar_draw_para_t *draw_param)
{
    __s32 i = 0;
    RECT rect = {0};
    GUI_RECT gui_rect = {0};
    record_video_para_t *gpara = NULL;
    maple_schdl_event_t *shdl_event = NULL;
    char time[128] = {0};
    char tmp[128] = {0};
    char shdl_name[128] = {0};
    gpara = (record_video_para_t *)draw_param->attr;

    if (gpara == NULL)
    {
        return EPDK_FAIL;
    }

    if (draw_param->mode != LBAR_MODE_NORMAL && draw_param->mode != LBAR_MODE_FOCUS)
    {
        return EPDK_FAIL;
    }

    //__wrn("%s %d draw_param->mode:%d", __FILE__, __LINE__,draw_param->mode);
    GUI_SetFont(gpara->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            //  __wrn("LBAR_MODE_NORMAL draw_param->index=%d",draw_param->index);
            GUI_BMP_Draw(theme_hdl2buf(gpara->h_item_unfocus_bmp), draw_param->rect.x, draw_param->rect.y);
            //  __wrn("draw_param->rect.x:%d draw_param->rect.y:%d", draw_param->rect.x, draw_param->rect.y);
            GUI_SetColor(gpara->unfocus_txt_color);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_LISTBAR_WIDTH;
            gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;
            eLIBs_sprintf(shdl_name, "%d KHz", maple_get_search_freq(draw_param->index) / 1000);
            //  __wrn("%s %d %d MHz", __FILE__, __LINE__, maple_get_search_freq(draw_param->index)/1000/1000);
            GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            //  __wrn("LBAR_MODE_FOCUS draw_param->index=%d",draw_param->index );
            GUI_BMP_Draw(theme_hdl2buf(gpara->h_item_focus_bmp), draw_param->rect.x, draw_param->rect.y);
            //  __wrn("draw_param->rect.x:%d draw_param->rect.y:%d", draw_param->rect.x, draw_param->rect.y);
            GUI_SetColor(gpara->focus_txt_color);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_LISTBAR_WIDTH;
            gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;
            eLIBs_sprintf(shdl_name, "%d KHz", maple_get_search_freq(draw_param->index) / 1000);
            //  __wrn("%s %d %d MHz", __FILE__, __LINE__, maple_get_search_freq(draw_param->index)/1000/1000);
            GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}
static void record_video_get_free_volume_size(unsigned char *size_str, record_video_para_t *para, __u8 *low_volume)
{
    __u64       size = 0;
    __u32       gnum = 0;
    __u32       snum = 0;
    char        numstr[32] = {0};
    char        temp_str[32] = {0};

    if (size_str == NULL)
    {
        __wrn("%s %d size_str NULL", __FILE__, __LINE__);
        return;
    }

    size = eLIBs_GetVolFSpace(para->disk);
    __wrn("%s %d disk_name:%s", __FILE__, __LINE__, para->disk);
    __wrn("%s %d size:%llu", __FILE__, __LINE__, size);
    gnum = (size * 100) >> 30;
    snum = (gnum % 100);
    gnum = (gnum / 100);
    eLIBs_memset(numstr, 0x00, sizeof(numstr));
    eLIBs_uint2str_dec(gnum, numstr);
    eLIBs_strcat(size_str, numstr);
    eLIBs_strcat(size_str, ".");
    eLIBs_memset(numstr, 0x00, sizeof(numstr));
    eLIBs_uint2str_dec(snum, numstr);

    if (snum >= 10)
    {
        eLIBs_strcat(size_str, numstr);
    }
    else
    {
        eLIBs_strcat(size_str, "0");
        eLIBs_strcat(size_str, numstr);
    }

    __wrn("gnum:%d", gnum);
    __wrn("snum:%d", snum);

    if (gnum <= 0 && snum < 10)
    {
        *low_volume = EPDK_TRUE;
    }
    else
    {
        *low_volume = EPDK_FALSE;
    }

    eLIBs_strcat(size_str, "GB");
    return ;
}
static void record_video_paint_info(__gui_msg_t *msg, unsigned char Init_Paint)
{
    GUI_RECT gui_rect = {0};
    unsigned char str[128] = {0};
    unsigned char temp_array[24] = {0};
    __s32 width = 0, height = 0, time = 0;
    record_video_para_t *para = NULL;
    __u8    low_volume = 0;
    maple_demod_ss_t ss = {0};
    para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);

    if (para == NULL)
    {
        __wrn("%s %d para null", __FILE__, __LINE__);
        return ;
    }

    GUI_SetColor(GUI_WHITE);
    GUI_SelLayer((unsigned long)para->layer);
    dsk_display_get_size(&width, &height);
    /*起始点坐标*/
    gui_rect.x0 = (width - RECORD_VIDEO_LISTBAR_WIDTH) / 2;
    gui_rect.y0 = height / 2 - 180;
    gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_LISTBAR_WIDTH;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;

    if (Init_Paint == EPDK_TRUE)
    {
#if (MAPLE_TV_STANDAR == MAPLE_TV_CMMB)
        eLIBs_sprintf(str, "C M M B");
#elif (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T)
        eLIBs_sprintf(str, "D V B -- T");
#elif (MAPLE_TV_STANDAR == MAPLE_TV_ATSC)
        eLIBs_sprintf(str, "A T S C");
#elif (MAPLE_TV_STANDAR == MAPLE_TV_DVB_T2)
        eLIBs_sprintf(str, "D V B -- T2");
#else
        eLIBs_sprintf(str, "D T M B");
#endif
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        eLIBs_memset(str, 0x00, sizeof(str));
    }

    /*当前选择的频点*/
    GUI_SetColor(GUI_BLACK);
    gui_rect.y0 = gui_rect.y1;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;

    if (Init_Paint == EPDK_TRUE)
    {
        GUI_BMP_Draw(theme_hdl2buf(para->h_item_unfocus_bmp), gui_rect.x0, gui_rect.y0);
        //eLIBs_sprintf(str, "Frequency:        %dMHz BandWidth: %d", maple_get_search_freq(para->listbar_focus_id)/1000/1000,
        //                                                           maple_get_search_bandwidth(para->listbar_focus_id));
        eLIBs_sprintf(str, "Frequency:		%dKHz BandWidth: %d K", para->record_frequency / 1000, para->record_bandwidth);
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        eLIBs_memset(str, 0x00, sizeof(str));
    }

#if 0
    /*信号质量，信号强度*/
    gui_rect.y0 = gui_rect.y1;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;
    __wrn("%s %d", __FILE__, __LINE__);

    if (para->record_status == 1)//录制中
    {
        //maple_get_ss(&ss);
    }

    __wrn("%s %d", __FILE__, __LINE__);
    GUI_BMP_Draw(theme_hdl2buf(para->h_item_unfocus_bmp), gui_rect.x0, gui_rect.y0);
    eLIBs_sprintf(str, "Signal  S:	%d	 Signal Q:%d", ss.strength, ss.quality);
    __wrn("Signal  S:	%d	 Signal Q:%d", ss.strength, ss.quality);
    GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(str, 0x00, sizeof(str));
    __wrn("%s %d", __FILE__, __LINE__);
#endif
    /*保存文件路径*/
    gui_rect.y0 = gui_rect.y1;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;

    if (Init_Paint == EPDK_TRUE)
    {
        __wrn("%s %d File_Path:%s", __FILE__, __LINE__, para->File_Path);
        GUI_BMP_Draw(theme_hdl2buf(para->h_item_unfocus_bmp), gui_rect.x0, gui_rect.y0);
        eLIBs_sprintf(str, "File Path:		%s", para->File_Path);
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        eLIBs_memset(str, 0x00, sizeof(str));
    }

    /*录制时间*/
    gui_rect.y0 = gui_rect.y1;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(para->h_item_unfocus_bmp), gui_rect.x0, gui_rect.y0);
    record_video_uint2time(para->record_time, temp_array);//s
    eLIBs_sprintf(str, "Record Time: %s", temp_array);
    GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(str, 0x00, sizeof(str));
    eLIBs_memset(temp_array, 0x00, sizeof(temp_array));
    /*文件大小*/
    gui_rect.y0 = gui_rect.y1;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(para->h_item_unfocus_bmp), gui_rect.x0, gui_rect.y0);
    __wrn("%s %d File_Path:%s", __FILE__, __LINE__, para->File_Path);
    para->File_size = eLIBs_GetFileSize(para->File_Path);

    if (para->File_size <= 0 || para->File_Path[0] == '\0')
    {
        eLIBs_sprintf(temp_array, "0 Byte");
    }
    else
    {
        filesize2str(para->File_size, temp_array);
    }

    eLIBs_sprintf(str, "File Size:		 %s", temp_array);
    GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(str, 0x00, sizeof(str));
    /*磁盘剩余大小*/
    gui_rect.y0 = gui_rect.y1;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_LISTBAR_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(para->h_item_unfocus_bmp), gui_rect.x0, gui_rect.y0);
    eLIBs_memset(temp_array, 0x00, sizeof(temp_array));
    eLIBs_memset(str, 0x00, sizeof(str));
    record_video_get_free_volume_size(temp_array, para, &low_volume);
    eLIBs_sprintf(str, "Free Disk Capacity:		%s", temp_array);
    GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    __wrn("%s %d", __FILE__, __LINE__);

    if (low_volume == EPDK_TRUE)
    {
        H_WIN h_dialog = NULL;
        __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_RECORD_DISK_FULL};
        default_dialog(h_dialog, GUI_WinGetParent(msg->h_deswin), 0x100, ADLG_NOTHING, lang_id);
        esKRNL_TimeDly(2 * 100);
        app_dialog_destroy(h_dialog);
        {
            __gui_msg_t my_msg;
            __wrn("%s %d ", __FILE__, __LINE__);
            eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
            my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            my_msg.h_srcwin = NULL;
            my_msg.id = DSK_MSG_HOME;
            my_msg.dwAddData1 = 0;
            my_msg.dwAddData2 = 0;
            my_msg.dwReserved = 0;
            GUI_SendNotifyMessage(&my_msg);
        }
    }

    __wrn("%s %d ", __FILE__, __LINE__);
    return ;
}
static void record_video_paint_win(__gui_msg_t *msg)
{
    GUI_RECT gui_rect = {0};
    unsigned char str[128] = {0};
    __s32 width = 0, height = 0;
    record_video_para_t *para = NULL;
    para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);

    if (para == NULL)
    {
        __wrn("%s %d para null", __FILE__, __LINE__);
        return ;
    }

    GUI_SetFont(para->font);
    GUI_LyrWinSel(para->layer);
    GUI_UC_SetEncodeUTF8();
    __wrn("%s %d focus_item:%d", __FILE__, __LINE__, para->focus_item);
    dsk_display_get_size(&width, &height);
    gui_rect.x0 = (width - RECORD_VIDEO_LISTBAR_WIDTH) / 2;
    gui_rect.y0 = height / 2 + 50;
    gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_START_STOP_WIDTH;
    gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;

    if (para->focus_item == 0)// focus on the start_stop icon
    {
        __s32 string_id = 0;
        GUI_SetColor(GUI_RED);

        /*start_stop icon*/
        if (para->record_status == 1)//already start
        {
            string_id = STRING_PAUSE_RC;
            GUI_BMP_Draw(theme_hdl2buf(para->h_item_stop_icon), gui_rect.x0, gui_rect.y0);
        }
        else
        {
            string_id = STRING_RECORD_RC;
            GUI_BMP_Draw(theme_hdl2buf(para->h_item_start_icon), gui_rect.x0, gui_rect.y0);
        }

        __wrn("%s %d", __FILE__, __LINE__);
        gui_rect.y0 = gui_rect.y1;
        gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;
        GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        dsk_langres_get_menu_text(string_id, str, sizeof(str));
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        /*Freq List icon*/
        GUI_SetColor(GUI_WHITE);
        gui_rect.x0 = gui_rect.x1 + 7;
        gui_rect.y0 = height / 2 + 50;
        gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_MUSIC_LIST_WIDTH;
        gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;
        GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        GUI_BMP_Draw(theme_hdl2buf(para->h_item_freq_list_unfocus), gui_rect.x0, gui_rect.y0);
        gui_rect.y0 = gui_rect.y1;
        gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;
        dsk_langres_get_menu_text(STRING_MUSCI_LIST_RC, str, sizeof(str));
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }
    else if (para->focus_item == 1)//freq list
    {
        __s32 string_id = 0;
        GUI_SetColor(GUI_WHITE);

        /*start_stop icon*/
        if (para->record_status == 1)//already start
        {
            string_id = STRING_PAUSE_RC;
            GUI_BMP_Draw(theme_hdl2buf(para->h_item_stop_icon), gui_rect.x0, gui_rect.y0);
        }
        else
        {
            string_id = STRING_RECORD_RC;
            GUI_BMP_Draw(theme_hdl2buf(para->h_item_start_icon), gui_rect.x0, gui_rect.y0);
        }

        gui_rect.y0 = gui_rect.y1;
        gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;
        GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        dsk_langres_get_menu_text(string_id, str, sizeof(str));
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        /*Freq List icon*/
        GUI_SetColor(GUI_RED);
        gui_rect.x0 = gui_rect.x1 + 7;
        gui_rect.y0 = height / 2 + 50;
        gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_MUSIC_LIST_WIDTH;
        gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;
        GUI_ClearRect(gui_rect.x0, gui_rect.y0, gui_rect.x1, gui_rect.y1);
        GUI_BMP_Draw(theme_hdl2buf(para->h_item_freq_list_focus), gui_rect.x0, gui_rect.y0);
        gui_rect.y0 = gui_rect.y1;
        gui_rect.y1 = gui_rect.y0 + RECORD_VIDEO_ICON_HEIGHT;
        dsk_langres_get_menu_text(STRING_MUSCI_LIST_RC, str, sizeof(str));
        GUI_DispStringInRect(str, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    }

    __wrn("%s %d", __FILE__, __LINE__);
    return;
}

static void record_video_search_listbar_init(__gui_msg_t *msg)
{
    RECT rect = {0};
    __listbar_config_t config;
    record_video_para_t *para = NULL;
    para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);

    if (para == NULL)
    {
        __wrn("%s %d para null", __FILE__, __LINE__);
        return ;
    }

    dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
    //__wrn("%s %d rect.width:%d rect.height:%d", __FILE__, __LINE__, rect.width, rect.height);
    eLIBs_memset(&config, 0x00, sizeof(config));
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.start_id = para->listbar_focus_id;
    config.focus_id = para->listbar_focus_id;
    config.item_cnt = para->total_freq;
    config.item_height  = RECORD_VIDEO_LISTBAR_HIGH;
    config.item_width   = RECORD_VIDEO_LISTBAR_WIDTH;
    config.list_rect.x  = (rect.width - RECORD_VIDEO_LISTBAR_WIDTH) / 2;
    config.list_rect.y  = RECORD_VIDEO_LISTBAR_TOP_HIGH;
    config.list_rect.width = RECORD_VIDEO_LISTBAR_WIDTH;
    config.list_rect.height = (para->total_freq > 10 ? 10 : para->total_freq) * RECORD_VIDEO_LISTBAR_HIGH;
    config.list_attr = (void *)para;
    para->h_ListBar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(para->h_ListBar, record_video_search_item_paint_ex, &config);
    LISTBAR_ShowPage(para->h_ListBar);
}

static void record_video_title_update(record_video_para_t *gpara, __u32 string_id)
{
    char tmp[128] = {0};
    GUI_RECT gui_rect = {0};
    SIZE size = {0};
    GUI_LyrWinSel(gpara->layer);
    GUI_SetFont(gpara->font);
    dsk_display_get_size((__s32 *)&size.width, (__s32 *)&size.height);
    GUI_BMP_Draw(theme_hdl2buf(gpara->h_select_freq_top_bmp), (size.width - RECORD_VIDEO_LISTBAR_WIDTH) / 2, RECORD_VIDEO_SELECT_FREQ_Y);
    gui_rect.x0 = (size.width - RECORD_VIDEO_LISTBAR_WIDTH) / 2;
    gui_rect.y0 = RECORD_VIDEO_SELECT_FREQ_Y;
    gui_rect.x1 = gui_rect.x0 + RECORD_VIDEO_LISTBAR_WIDTH;
    gui_rect.y1 = RECORD_VIDEO_LISTBAR_TOP_HIGH;
    __wrn("gui_rect.x0: %d gui_rect.y1:%d", gui_rect.x0, gui_rect.y1);
    dsk_langres_get_menu_text(string_id, tmp, sizeof(tmp));
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

static __s32 _record_video_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    record_video_para_t *record_video_para = NULL;
    record_video_para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);

    if (record_video_para == NULL)
    {
        __wrn("record_video_para null");
        return EPDK_FAIL;
    }

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_PREV:
        case GUI_MSG_KEY_LONGPREV:
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                if (record_video_para->h_ListBar != NULL)
                {
                    LISTBAR_PrevItem(record_video_para->h_ListBar);
                }
                else
                {
                    if (record_video_para->focus_item == 0)
                    {
                        record_video_para->focus_item = RECORD_VIDEO_ICON_NUM - 1;
                    }
                    else
                    {
                        record_video_para->focus_item--;
                    }

                    record_video_paint_win(msg);
                }
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY_NEXT:
        case GUI_MSG_KEY_LONGNEXT:
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                if (record_video_para->h_ListBar != NULL)
                {
                    LISTBAR_NextItem(record_video_para->h_ListBar);
                }
                else
                {
                    if (record_video_para->focus_item == RECORD_VIDEO_ICON_NUM - 1)
                    {
                        record_video_para->focus_item = 0;
                    }
                    else
                    {
                        record_video_para->focus_item++;
                    }

                    __wrn("%s %d focus_item:%d", __FILE__, __LINE__, record_video_para->focus_item);
                    record_video_paint_win(msg);
                }
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY_ENTER:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                if (record_video_para->h_ListBar != NULL)
                {
                    record_video_para->listbar_focus_id = LISTBAR_GetFocusItem(record_video_para->h_ListBar);
                    __wrn("%s %d", __FILE__, __LINE__);
                    ret = LISTBAR_SuspendPage(record_video_para->h_ListBar);

                    if (EPDK_FAIL == ret)
                    {
                        __wrn("%s %d listbar suspend fail", __FILE__, __LINE__);
                    }

                    ret = LISTBAR_Delete(record_video_para->h_ListBar);

                    if (EPDK_FAIL == ret)
                    {
                        __wrn("%s %d listbar delete fail", __FILE__, __LINE__);
                    }

                    record_video_para->h_ListBar = NULL;
                    __wrn("%s %d will paint win", __FILE__, __LINE__);

                    if (record_video_para->listbar_old_focus_id == record_video_para->listbar_focus_id
                        && record_video_para->record_status == 1)
                    {
                        __wrn("%s %d", __FILE__, __LINE__);
                        GUI_Clear();
                        GUI_SetTimer(msg->h_deswin, record_video_para->refresh_time_id, REFRESH_TIME_OUT, NULL);
                        record_video_paint_info(msg, EPDK_TRUE);
                        record_video_paint_win(msg);
                    }
                    else
                    {
                        __wrn("%s %d", __FILE__, __LINE__);

                        if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->refresh_time_id) == EPDK_TRUE)
                        {
                            GUI_KillTimer(msg->h_deswin, record_video_para->refresh_time_id);
                        }

                        if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->record_time_id) == EPDK_TRUE)
                        {
                            record_video_para->record_time = 0;
                            GUI_KillTimer(msg->h_deswin, record_video_para->record_time_id);
                        }

                        if (record_video_para->record_status == 1)// stop record process
                        {
#if defined Have_TscRec_lib
                            TscRecordStop();
#endif
                            record_video_para->record_status = 0;
                        }

                        record_video_para->File_size = 0;
                        record_video_para->record_time = 0;
                        //record_video_para->record_signal_s = 0;
                        //record_video_para->record_signal_q = 0;
                        eLIBs_memset(record_video_para->File_Path, 0x00, sizeof(record_video_para->File_Path));
                        GUI_Clear();
                        record_video_para->record_frequency = maple_get_search_freq(record_video_para->listbar_focus_id);//hz
                        record_video_para->record_bandwidth = maple_get_search_bandwidth(record_video_para->listbar_focus_id);
                        record_video_paint_info(msg, EPDK_TRUE);
                        record_video_paint_win(msg);
                    }
                }
                else
                {
                    __wrn("%s %d", __FILE__, __LINE__);

                    if (record_video_para->focus_item == 0)
                    {
                        __wrn("%s %d", __FILE__, __LINE__);

                        if (record_video_para->record_status == 0)
                        {
                            record_video_create_file_name(record_video_para);
                            __wrn("%s %d record_frequency:%d", __FILE__, __LINE__, record_video_para->record_frequency);
#if defined Have_TscRec_lib
                            if (TscRecordTune(record_video_para->record_frequency / 1000, record_video_para->record_bandwidth, NULL) == EPDK_FAIL)
                            {
                                H_WIN h_dialog = NULL;
                                __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_RECORD_TUNE_FAIL};
                                default_dialog(h_dialog, GUI_WinGetParent(msg->h_deswin), 0x100, ADLG_NOTHING, lang_id);
                                esKRNL_TimeDly(2 * 100);
                                app_dialog_destroy(h_dialog);
                                GUI_WinSetFocusChild(msg->h_deswin);
                                eLIBs_memset(record_video_para->File_Path, 0x00, sizeof(record_video_para->File_Path));//keep f:
                                __wrn("%s %d TscRecordTune err", __FILE__, __LINE__);
                                return EPDK_FAIL;
                            }
                            else if (TscRecordStart(10 * 1024 * 1024, record_video_para->File_Path) == EPDK_FAIL)
                            {
                                H_WIN h_dialog = NULL;
                                __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_RECORD_ERROR};
                                default_dialog(h_dialog, GUI_WinGetParent(msg->h_deswin), 0x100, ADLG_NOTHING, lang_id);
                                esKRNL_TimeDly(2 * 100);
                                app_dialog_destroy(h_dialog);
                                GUI_WinSetFocusChild(msg->h_deswin);
                                eLIBs_memset(record_video_para->File_Path, 0x00, sizeof(record_video_para->File_Path));//keep f:
                                __wrn("%s %d TscRecordStart err", __FILE__, __LINE__);
                                return EPDK_FAIL;
                            }
                            else
                            {
                                __wrn("%s %d", __FILE__, __LINE__);
                                GUI_SetTimer(msg->h_deswin, record_video_para->refresh_time_id, REFRESH_TIME_OUT, NULL);
                                GUI_SetTimer(msg->h_deswin, record_video_para->record_time_id, RECORD_TIME_OUT, NULL);
                                record_video_paint_info(msg, EPDK_TRUE);
                                record_video_para->record_status = 1;
                            }
#endif
                        }
                        else
                        {
                            __wrn("%s %d", __FILE__, __LINE__);
#if defined Have_TscRec_lib
                            TscRecordStop();
#endif
                            if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->refresh_time_id) == EPDK_TRUE)
                            {
                                GUI_KillTimer(msg->h_deswin, record_video_para->refresh_time_id);
                            }

                            if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->record_time_id) == EPDK_TRUE)
                            {
                                record_video_para->record_time = 0;
                                GUI_KillTimer(msg->h_deswin, record_video_para->record_time_id);
                            }

                            record_video_para->record_status = 0;
                        }

                        record_video_paint_win(msg);
                    }
                    else
                    {
                        __wrn("%s %d record_status:%d", __FILE__, __LINE__, record_video_para->record_status);

                        if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->refresh_time_id) == EPDK_TRUE)
                        {
                            GUI_KillTimer(msg->h_deswin, record_video_para->refresh_time_id);
                        }

                        if (record_video_para->record_status == 1)
                        {
                            record_video_para->listbar_old_focus_id = record_video_para->listbar_focus_id;
                        }

                        _record_video_init_listbar(msg);
                    }
                }
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_LONGESCAPE:
        {
            //__log("%s %d msg->dwAddData2:%d", __FILE__, __LINE__, msg->dwAddData2);
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                if (record_video_para->h_ListBar != NULL)
                {
                    __wrn("%s %d", __FILE__, __LINE__);
                    ret = LISTBAR_SuspendPage(record_video_para->h_ListBar);

                    if (EPDK_FAIL == ret)
                    {
                        __msg("listbar suspend fail");
                    }

                    ret = LISTBAR_Delete(record_video_para->h_ListBar);

                    if (EPDK_FAIL == ret)
                    {
                        __msg("listbar delete fail");
                    }

                    record_video_para->h_ListBar = NULL;
                    GUI_Clear();

                    if (record_video_para->record_status == 1)
                    {
                        GUI_SetTimer(msg->h_deswin, record_video_para->refresh_time_id, REFRESH_TIME_OUT, NULL);
                    }

                    record_video_paint_info(msg, EPDK_TRUE);
                    record_video_paint_win(msg);
                }
                else
                {
                    __gui_msg_t mymsg;
                    __wrn("%s %d", __FILE__, __LINE__);
                    mymsg.id = GUI_MSG_CLOSE;
                    mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    GUI_SendMessage(&mymsg);
                }
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return ret;
}


static __s32 _record_video_init_listbar(__gui_msg_t *msg)
{
    record_video_para_t *record_video_para = NULL;
    record_video_para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);
    record_video_para->focus_txt_color = GUI_BLUE;
    record_video_para->unfocus_txt_color = GUI_BLUE;
    __wrn("record_video_para->total_freq = %d", record_video_para->total_freq);
    GUI_LyrWinSetSta(record_video_para->layer, GUI_LYRWIN_STA_SLEEP);
    GUI_Clear();
    record_video_search_listbar_init(msg);
    record_video_title_update(record_video_para, STRING_DTV_MANU_SELECT);
    GUI_LyrWinSetSta(record_video_para->layer, GUI_LYRWIN_STA_ON);
    return 0;
}
void record_video_res_init(record_video_para_t *record_video_para)
{
    if (record_video_para == NULL)
    {
        __wrn("record_video_para null");
        return ;
    }

    record_video_para->h_item_freq_list_unfocus = theme_open(ID_RECORD_MENU_LIST_0_BMP);
    record_video_para->h_item_freq_list_focus = theme_open(ID_RECORD_MENU_LIST_1_BMP);
    record_video_para->h_item_stop_icon = theme_open(ID_RECORD_MENU_RECORD_0_BMP);
    record_video_para->h_item_start_icon = theme_open(ID_RECORD_MENU_RECORD_1_BMP);
    record_video_para->h_item_focus_bmp = theme_open(ID_DTV_PRELIST_F_BMP);
    record_video_para->h_item_unfocus_bmp = theme_open(ID_DTV_PRELIST_UF_BMP);
    record_video_para->h_select_freq_top_bmp = theme_open(ID_DTV_SETTING_TOP_BMP);
    return;
}
void record_video_res_unit(record_video_para_t *record_video_para)
{
    if (record_video_para == NULL)
    {
        __wrn("record_video_para null");
        return ;
    }

    theme_close(record_video_para->h_item_freq_list_unfocus);
    theme_close(record_video_para->h_item_freq_list_focus);
    theme_close(record_video_para->h_item_stop_icon);
    theme_close(record_video_para->h_item_start_icon);
    theme_close(record_video_para->h_item_focus_bmp);
    theme_close(record_video_para->h_item_unfocus_bmp);
    theme_close(record_video_para->h_select_freq_top_bmp);
    return ;
}

static __s32 _record_video_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            record_video_attr_t *record_video_attr = NULL;
            record_video_para_t *record_video_para = NULL;
            record_video_attr = (record_video_attr_t *)GUI_WinGetAttr(msg->h_deswin);
            record_video_para = (record_video_para_t *)My_Malloc(0, sizeof(record_video_para_t));

            if (NULL == record_video_para)
            {
                __wrn("memory alloc fail.........");
                __wrn("%s %d", __FILE__, __LINE__);
                return EPDK_FAIL;
            }

            eLIBs_memset(record_video_para, 0x00, sizeof(record_video_para_t));
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)record_video_para);//把私有数据加进去
            record_video_para->total_freq = maple_get_cur_search_freq_tatol();
            record_video_para->focus_item = 0;
            record_video_para->record_frequency = record_video_attr->current_freq;
            record_video_para->record_bandwidth = 8000;
            record_video_para->record_time_id = RECORD_TIME_ID;
            record_video_para->refresh_time_id = REFRESH_TIME_ID;
            record_video_para->listbar_focus_id = 0;
            record_video_para->listbar_old_focus_id = 0;
            record_video_para->font = record_video_attr->font;
            record_video_para->layer = record_video_attr->layer;
            record_video_match_freq(record_video_para);
            record_video_para->media_type = record_video_check_disk_exist(record_video_para);
            __wrn("%s %d record_video_para->media_type:%d", __FILE__, __LINE__, record_video_para->media_type);
            __wrn("%s %d record_video_para->disk:%s", __FILE__, __LINE__, record_video_para->disk);

            if (record_video_para->media_type == EPDK_FALSE)
            {
                H_WIN h_dialog = NULL;
                __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_ROOT_NULL_DISK};
                default_dialog(h_dialog, GUI_WinGetParent(msg->h_deswin), 0x100, ADLG_NOTHING, lang_id);
                esKRNL_TimeDly(2 * 100);
                __wrn("%s %d %x", __FILE__, __LINE__, h_dialog);
                app_dialog_destroy(h_dialog);
                {
                    __gui_msg_t my_msg;
                    __wrn("%s %d ", __FILE__, __LINE__);
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.h_srcwin = NULL;
                    my_msg.id = DSK_MSG_HOME;
                    my_msg.dwAddData1 = 0;
                    my_msg.dwAddData2 = 0;
                    my_msg.dwReserved = 0;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }
            }
            else
            {
                __u8 fsname[16] = {0};
                __u8 fs_not_support[2][16] = {{FSYS_NTFS_NAME}, {FSYS_EXFAT_NAME}};
                eLIBs_GetFSName(record_video_para->disk, fsname);
                record_video_str2lower(fsname);
                record_video_str2lower(fs_not_support[0]);
                record_video_str2lower(fs_not_support[1]);
                __wrn("%s %d fsname:%s", __FILE__, __LINE__, fsname);

                if ((eLIBs_strcmp(fs_not_support[0], fsname) == 0))
                {
                    H_WIN h_dialog = NULL;
                    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DISK_FORMAT_NOT_SUPPORT};
                    default_dialog(h_dialog, GUI_WinGetParent(msg->h_deswin), 0x100, ADLG_NOTHING, lang_id);
                    esKRNL_TimeDly(2 * 100);
                    __wrn("%s %d %x", __FILE__, __LINE__, h_dialog);
                    app_dialog_destroy(h_dialog);
                    {
                        __gui_msg_t my_msg;
                        __wrn("%s %d ", __FILE__, __LINE__);
                        eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                        my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                        my_msg.h_srcwin = NULL;
                        my_msg.id = DSK_MSG_HOME;
                        my_msg.dwAddData1 = 0;
                        my_msg.dwAddData2 = 0;
                        my_msg.dwReserved = 0;
                        GUI_SendNotifyMessage(&my_msg);
                        return EPDK_OK;
                    }
                }
            }

            record_video_antenna_switch_ctrl_init();
            record_video_set_antenna_mode(1);// 1: 外接天线 0:拉杆天线
#if defined Have_TscRec_lib
            if (EPDK_OK != TscRecordInit())
            {
                {
                    __gui_msg_t my_msg;
                    H_WIN h_dialog = NULL;
                    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_RECORD_NOT_SUPPORT};
                    default_dialog(h_dialog, GUI_WinGetParent(msg->h_deswin), 0x100, ADLG_NOTHING, lang_id);
                    esKRNL_TimeDly(2 * 100);
                    __wrn("%s %d %x", __FILE__, __LINE__, h_dialog);
                    app_dialog_destroy(h_dialog);
                    __wrn("%s %d ", __FILE__, __LINE__);
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.h_srcwin = NULL;
                    my_msg.id = DSK_MSG_HOME;
                    my_msg.dwAddData1 = 0;
                    my_msg.dwAddData2 = 0;
                    my_msg.dwReserved = 0;
                    GUI_SendNotifyMessage(&my_msg);
                }
                __wrn("%s %d RecordInit err!", __FILE__, __LINE__);
                return EPDK_FAIL;
            }
#endif
            record_video_res_init(record_video_para);
#if 0
            _record_video_init_listbar(msg);
#else
            GUI_LyrWinSetSta(record_video_para->layer, GUI_LYRWIN_STA_SLEEP);
            record_video_paint_info(msg, EPDK_TRUE);
            record_video_paint_win(msg);
            GUI_LyrWinSetSta(record_video_para->layer, GUI_LYRWIN_STA_ON);
#endif
        }

        return EPDK_OK;

        case GUI_MSG_TIMER:
        {
            if (msg->dwAddData1 == REFRESH_TIME_ID)
            {
                record_video_paint_info(msg, EPDK_FALSE);
            }
            else if (msg->dwAddData1 == RECORD_TIME_ID)
            {
                record_video_para_t *para = NULL;
                para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);

                if (para == NULL)
                {
                    __wrn("%s %d para null", __FILE__, __LINE__);
                    return EPDK_FAIL;
                }

                para->record_time++;
            }
        }
        break;

        case GUI_MSG_CLOSE:
        {
            //__here__;
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            record_video_attr_t *record_video_attr = NULL;
            record_video_para_t *record_video_para = NULL;
            record_video_para = (record_video_para_t *)GUI_WinGetAddData(msg->h_deswin);

            if (NULL == record_video_para)
            {
                __msg("memory alloc fail.........");
                __wrn("%s %d", __FILE__, __LINE__);
                return EPDK_FAIL;
            }

            __wrn("%s %d", __FILE__, __LINE__);

            if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->refresh_time_id) == EPDK_TRUE)
            {
                GUI_KillTimer(msg->h_deswin, record_video_para->refresh_time_id);
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, record_video_para->record_time_id) == EPDK_TRUE)
            {
                GUI_KillTimer(msg->h_deswin, record_video_para->record_time_id);
            }

            __wrn("%s %d", __FILE__, __LINE__);

            if (record_video_para->record_status == 1)// recording
            {
#if defined Have_TscRec_lib
                if (TscRecordStop() != NULL)
                {
                    __wrn("%s %d RecordStop err", __FILE__, __LINE__);
                }
#endif
            }

            __wrn("%s %d", __FILE__, __LINE__);
#if defined Have_TscRec_lib
            if (TscRecordDeInit() != EPDK_OK)
            {
                __wrn("%s %d RecordDeInit err", __FILE__, __LINE__);
            }
#endif
            __wrn("%s %d", __FILE__, __LINE__);
            record_video_res_unit(record_video_para);
            record_video_attr = record_video_para->record_video_attr;

            if (NULL != record_video_attr)
            {
                __inf("My_Mfree general_para");
                My_Mfree(0, record_video_attr);
            }

            __wrn("%s %d", __FILE__, __LINE__);

            if (NULL != record_video_para)
            {
                __inf("My_Bfree general_attr");
                My_Mfree(0, record_video_para);
            }

            __wrn("%s %d", __FILE__, __LINE__);
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            _record_video_listbar_key_proc(msg);
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}


H_WIN record_video_win_create(H_WIN h_parent, record_video_attr_t *attr)
{
    __gui_framewincreate_para_t framewin_para;
    record_video_attr_t  *record_video_attr = NULL;
    FB fb;
    __inf("*********enter setting_general_win_create *****************");
    eLIBs_memset(&fb, 0, sizeof(fb));
    GUI_LyrWinGetFB(attr->layer, &fb);
    record_video_attr = (record_video_attr_t *)My_Malloc(0, sizeof(record_video_attr_t));

    if (NULL == record_video_attr)
    {
        __msg("memory alloc fail.........");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset((void *)record_video_attr, 0, sizeof(record_video_attr_t));
    record_video_attr->font = attr->font;
    record_video_attr->layer = attr->layer;
    __wrn("%s %d attr->current_freq:%d", __FILE__, __LINE__, attr->current_freq);
    record_video_attr->current_freq = attr->current_freq;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name      =   "record video win",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle   = WS_NONE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id        = RECORD_VIDEO_FRM_WIN_ID;
    framewin_para.hHosting  = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_record_video_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)record_video_attr;
    framewin_para.hLayer = attr->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

static __s32 app_record_video_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            record_video_ctrl_t *record_video_ctrl = NULL;
            record_video_attr_t record_video_para;
            record_video_ctrl = (record_video_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (record_video_ctrl == NULL)
            {
                __err("_app_setting_Proc: %d err", __LINE__);
                return EPDK_FAIL;
            }

            record_video_ctrl->lyr_setting = record_video_layer_32bpp_create();
            eLIBs_memset(&record_video_para, 0x00, sizeof(record_video_para));
            record_video_para.layer = record_video_ctrl->lyr_setting;
            record_video_para.font = record_video_ctrl->font;
            record_video_para.current_freq = record_video_ctrl->current_freq;
            __wrn("%s %d", __FILE__, __LINE__);
            record_video_ctrl->h_frm_record_video = record_video_win_create(msg->h_deswin, &record_video_para);
            GUI_WinSetFocusChild(record_video_ctrl->h_frm_record_video);
        }

        return EPDK_OK;

        case DSK_MSG_HOME:
        case GUI_MSG_CLOSE:
        {
            __log("%s %d", __FILE__, __LINE__);
            main_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, 0x9001, 0);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            record_video_ctrl_t *record_video_ctrl = NULL;
            record_video_ctrl    = (record_video_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (NULL == record_video_ctrl)
            {
                __err("setting_ctrl is null ......");
                return EPDK_FAIL;
            }

            if (record_video_ctrl->lyr_setting)
            {
                GUI_LyrWinDelete(record_video_ctrl->lyr_setting);
                record_video_ctrl->lyr_setting = 0;
            }

            My_Mfree(0, record_video_ctrl);
            record_video_antenna_switch_ctrl_uninit();
        }

        return EPDK_OK;

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __gui_msg_t my_msg;
            H_WIN h_dialog = NULL;
            __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_ROOT_DISK_PLUG_OUT};
            default_dialog(h_dialog, msg->h_deswin, 0x100, ADLG_OK, lang_id);
            esKRNL_TimeDly(2 * 100);
            app_dialog_destroy(h_dialog);
            __wrn("%s %d ", __FILE__, __LINE__);
            eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
            my_msg.h_deswin = msg->h_deswin;
            my_msg.h_srcwin = NULL;
            my_msg.id = DSK_MSG_HOME;
            my_msg.dwAddData1 = 0;
            my_msg.dwAddData2 = 0;
            my_msg.dwReserved = 0;
            GUI_SendNotifyMessage(&my_msg);
        }
        break;

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

H_WIN app_record_video_create(H_WIN parent, __s32 current_freq)
{
    __gui_manwincreate_para_t create_info;
    record_video_ctrl_t *record_video__ctrl = NULL;
    __wrn("****************************************************************************************");
    __wrn("********  enter record video app  **************");
    __wrn("****************************************************************************************");
    __wrn("----------record video create----------");
    gscene_hbar_set_state(HBAR_ST_HIDE);
    gscene_bgd_set_state(BGD_STATUS_SHOW);
    record_video__ctrl = (record_video_ctrl_t *)My_Malloc(0, sizeof(record_video_ctrl_t));

    if (NULL == record_video__ctrl)
    {
        __err("memory alloc fail.........");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset(record_video__ctrl, 0, sizeof(record_video_ctrl_t));
    __wrn("current_freq:%d", current_freq);
    record_video__ctrl->current_freq = current_freq;
    GUI_Memdev_FrameBuffer_Release(1);
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.id              = APP_RECORD_VEDIO_ID;
    create_info.name            = APP_RECORD_VEDIO;
    create_info.hParent         = parent;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_record_video_proc);
    create_info.attr            = (void *)record_video__ctrl;
    return (GUI_ManWinCreate(&create_info));
}


