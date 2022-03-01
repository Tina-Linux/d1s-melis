#include "setting_alarm_clock.h"
#include "app_setting_i.h"
#include <log.h>

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

const static __u32 id_alarm_clock_bmp[ALARM_CLOCK_BMP_MAX] =
{
    //背景
    ID_SETTING_BG_BMP,
    ID_SETTING_CLOCK_BG_BMP,
    //页
    ID_SETTING_PAGE_BMP,
    //数字
    ID_SETTING_0_BMP,
    ID_SETTING_1_BMP,
    ID_SETTING_2_BMP,
    ID_SETTING_3_BMP,
    ID_SETTING_4_BMP,
    ID_SETTING_5_BMP,
    ID_SETTING_6_BMP,
    ID_SETTING_7_BMP,
    ID_SETTING_8_BMP,
    ID_SETTING_9_BMP,
    //按钮
    ID_SETTING_DOWN_UNF_BMP,
    ID_SETTING_DOWN_F_BMP,
    ID_SETTING_UP_UNF_BMP,
    ID_SETTING_UP_F_BMP
};
static GUI_RECT alarm_clock_button_touch[ALARM_CLOCK_BUTTON_MAX] =
{
    {41, 31, 96, 56},       //小时+
    {41, 116, 96, 140},     //小时-
    {102, 31, 156, 56},     //分钟+
    {102, 116, 156, 140},   //分钟-
    {28, 156, 83, 181},     //确认
    {114, 156, 168, 181}    //取消
};

void  alarm_clock_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = NULL;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_SendNotifyMessage(&msg);
}
static H_LYR alarm_clock_frm_32bpp_layer_create(RECT *LayerRect)
{
    H_LYR layer = NULL;
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
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "ALARM_CLOCK",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = LayerRect->width;
    fb.size.height      = LayerRect->height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = LayerRect->width;
    lstlyr.src_win.height   = LayerRect->height;
    lstlyr.scn_win.x        = LayerRect->x;
    lstlyr.scn_win.y        = LayerRect->y;
    lstlyr.scn_win.width  = LayerRect->width;
    lstlyr.scn_win.height = LayerRect->height;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("alarm clock layer create error !\n");
    }

    return layer;
}


static __s32 alarm_clock_res_init(__gui_msg_t *msg)
{
    __u32 i = 0;
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    for (i = 0; i < ALARM_CLOCK_BMP_MAX; i++)
    {
        palarmClockFrm_attr->h_bmp_alarm_clock[i] = theme_open(id_alarm_clock_bmp[i]);
    }

    return EPDK_OK;
}

static __s32 alarm_clock_res_uninit(__gui_msg_t *msg)
{
    __u32 i = 0;
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    for (i = 0; i < ALARM_BMP_MAX; i++)
    {
        if (palarmClockFrm_attr->h_bmp_alarm_clock[i])
        {
            theme_close(palarmClockFrm_attr->h_bmp_alarm_clock[i]);
            palarmClockFrm_attr->h_bmp_alarm_clock[i] = NULL;
        }
    }

    return EPDK_OK;
}

static __s32 alarm_clock_frm_bg_draw(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(palarmClockFrm_attr->layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_BG_BMP]), 0, 0);
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_PAGE_BMP]), 43, 56);
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_PAGE_BMP]), 100, 56);
    return EPDK_OK;
}
static __s32 alarm_clock_frm_change_draw(__gui_msg_t *msg)
{
    char        str[GUI_NAME_MAX];
    GUI_RECT    rect = {63, 9, 140, 28};
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(palarmClockFrm_attr->layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    //调节按钮
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_UP_UNF_BMP]), 52, 37);
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_DOWN_UNF_BMP]), 52, 121);
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_UP_UNF_BMP]), 110, 37);
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_DOWN_UNF_BMP]), 110, 121);

    if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_HOUR_UP)
    {
        GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_UP_F_BMP]), 52, 37);
    }
    else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_HOUR_DOWN)
    {
        GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_DOWN_F_BMP]), 52, 121);
    }
    else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_MINUTE_UP)
    {
        GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_UP_F_BMP]), 110, 37);
    }
    else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_MINUTE_DOWN)
    {
        GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_DOWN_F_BMP]), 110, 121);
    }

    //调节时间
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_0_BMP + palarmClockFrm_attr->clock.hour / 10]), 53, 66); //小时
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_0_BMP + palarmClockFrm_attr->clock.hour % 10]), 74, 66); //小时
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_0_BMP + palarmClockFrm_attr->clock.minute / 10]), 111, 66); //分钟
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_0_BMP + palarmClockFrm_attr->clock.minute % 10]), 132, 66); //分钟

    //设置按钮
    if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_OK)
    {
        GUI_SetColor(GUI_YELLOW);
    }
    else
    {
        GUI_SetColor(GUI_BLACK);
    }

    dsk_langres_get_menu_text(STRING_SET_OK, str, GUI_NAME_MAX);
    GUI_DispStringInRect(str, &alarm_clock_button_touch[ALARM_CLOCK_OK], GUI_TA_HCENTER | GUI_TA_VCENTER);

    if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_CANCEL)
    {
        GUI_SetColor(GUI_YELLOW);
    }
    else
    {
        GUI_SetColor(GUI_BLACK);
    }

    dsk_langres_get_menu_text(STRING_SET_CANCLE, str, GUI_NAME_MAX);
    GUI_DispStringInRect(str, &alarm_clock_button_touch[ALARM_CLOCK_CANCEL], GUI_TA_HCENTER | GUI_TA_VCENTER);
    //时间显示
    GUI_BMP_Draw(theme_hdl2buf(palarmClockFrm_attr->h_bmp_alarm_clock[ALARM_CLOCK_CLOCK_BG_BMP]), 63, 9);
    eLIBs_sprintf(str, "%02d:%02d", palarmClockFrm_attr->clock.hour, palarmClockFrm_attr->clock.minute);
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(str, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return EPDK_OK;
}

static __s32 alarm_clock_frm_setting(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_HOUR_UP)
    {
        palarmClockFrm_attr->clock.hour = (palarmClockFrm_attr->clock.hour + 1) % 24;
    }
    else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_HOUR_DOWN)
    {
        palarmClockFrm_attr->clock.hour = (palarmClockFrm_attr->clock.hour + 24 - 1) % 24;
    }
    else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_MINUTE_UP)
    {
        palarmClockFrm_attr->clock.minute = (palarmClockFrm_attr->clock.minute + 1) % 60;
    }
    else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_MINUTE_DOWN)
    {
        palarmClockFrm_attr->clock.minute = (palarmClockFrm_attr->clock.minute + 60 - 1) % 60;
    }

    return EPDK_OK;
}
static __s32 alarm_clock_frm_save(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    palarmClockFrm_attr->reg_alarm->alarm_time.hour = palarmClockFrm_attr->clock.hour;
    palarmClockFrm_attr->reg_alarm->alarm_time.minute = palarmClockFrm_attr->clock.minute;
    return EPDK_OK;
}
static __s32 alarm_clock_frm_close(__gui_msg_t *msg)
{
    __gui_msg_t Mymsg;
    Mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
    Mymsg.h_srcwin = NULL;
    Mymsg.id = GUI_MSG_COMMAND;
    Mymsg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), ID_SWITCH_TO_ALARM_VIEW);
    Mymsg.dwAddData2 = 0;
    Mymsg.dwReserved = 0;
    GUI_SendNotifyMessage(&Mymsg);
    return EPDK_OK;
}

static __s32 _alarm_clock_frm_on_paint(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    //设置绘画属性
    GUI_LyrWinSel(palarmClockFrm_attr->layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    return EPDK_OK;
}

static __s32 _alarm_clock_frm_on_create(__gui_msg_t *msg)
{
    ES_FILE *file_hdle;
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    palarmClockFrm_attr->buttonF = ALARM_CLOCK_OK;
    //资源初始化
    alarm_clock_res_init(msg);
    //绘画背景
    GUI_LyrWinSetSta(palarmClockFrm_attr->layer, GUI_LYRWIN_STA_ON);
    alarm_clock_frm_bg_draw(msg);
    alarm_clock_frm_change_draw(msg);
    GUI_LyrWinSetTop(palarmClockFrm_attr->layer);
    return EPDK_OK;
}
static __s32 _alarm_clock_frm_on_close(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_FrmWinDelete(msg->h_deswin);
    return EPDK_OK;
}
static __s32 _alarm_clock_frm_on_destory(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_WinGetCaptureWin() == msg->h_deswin)
    {
        GUI_WinReleaseCapture();
    }

    alarm_clock_res_uninit(msg);
    esMEMS_Mfree(0, palarmClockFrm_attr);
    GUI_WinSetAttr(msg->h_deswin, NULL);
    return EPDK_OK;
}

static __s32 _alarm_clock_frm_on_key(__gui_msg_t *msg)
{
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LONGENTER:
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
            {
                if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_OK)
                {
                    alarm_clock_frm_close(msg);
                    alarm_clock_frm_save(msg);
                }
                else if (palarmClockFrm_attr->buttonF == ALARM_CLOCK_CANCEL)
                {
                    alarm_clock_frm_close(msg);
                }
            }
            break;

            case GUI_MSG_KEY_LONGMENU:
            case GUI_MSG_KEY_MENU:
                //case GUI_MSG_KEY_MODE:
                alarm_clock_frm_close(msg);
                break;
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_PLAY_PAUSE:
                alarm_clock_frm_setting(msg);
                break;

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
                palarmClockFrm_attr->buttonF = (palarmClockFrm_attr->buttonF + 1) % ALARM_CLOCK_BUTTON_MAX;
                break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
                palarmClockFrm_attr->buttonF = (palarmClockFrm_attr->buttonF + ALARM_CLOCK_BUTTON_MAX - 1) % ALARM_CLOCK_BUTTON_MAX;
                break;
        }
    }

    alarm_clock_frm_change_draw(msg);
    return EPDK_OK;
}

static __s32 _alarm_clock_frm_on_touch(__gui_msg_t *msg)
{
    __s32 speed = 0, direct = 0;
    __u16 x = 0, y = 0;
    psetting_alarm_clock_attr palarmClockFrm_attr;
    palarmClockFrm_attr = (psetting_alarm_clock_attr)GUI_WinGetAttr(msg->h_deswin);

    if (palarmClockFrm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);

    if (x > alarm_clock_button_touch[ALARM_CLOCK_HOUR_UP].x0 && x < alarm_clock_button_touch[ALARM_CLOCK_HOUR_UP].x1 &&
        y > alarm_clock_button_touch[ALARM_CLOCK_HOUR_UP].y0 && y < alarm_clock_button_touch[ALARM_CLOCK_HOUR_UP].y1)
    {
        //小时+
        if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN || msg->dwAddData1 == GUI_MSG_TOUCH_LONGDOWN)
        {
            palarmClockFrm_attr->buttonF = ALARM_CLOCK_HOUR_UP;
            alarm_clock_frm_setting(msg);
        }
    }
    else if (x > alarm_clock_button_touch[ALARM_CLOCK_HOUR_DOWN].x0 && x < alarm_clock_button_touch[ALARM_CLOCK_HOUR_DOWN].x1 &&
             y > alarm_clock_button_touch[ALARM_CLOCK_HOUR_DOWN].y0 && y < alarm_clock_button_touch[ALARM_CLOCK_HOUR_DOWN].y1)
    {
        //小时-
        if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN || msg->dwAddData1 == GUI_MSG_TOUCH_LONGDOWN)
        {
            palarmClockFrm_attr->buttonF = ALARM_CLOCK_HOUR_DOWN;
            alarm_clock_frm_setting(msg);
        }
    }
    else if (x > alarm_clock_button_touch[ALARM_CLOCK_MINUTE_UP].x0 && x < alarm_clock_button_touch[ALARM_CLOCK_MINUTE_UP].x1 &&
             y > alarm_clock_button_touch[ALARM_CLOCK_MINUTE_UP].y0 && y < alarm_clock_button_touch[ALARM_CLOCK_MINUTE_UP].y1)
    {
        //分钟+
        if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN || msg->dwAddData1 == GUI_MSG_TOUCH_LONGDOWN)
        {
            palarmClockFrm_attr->buttonF = ALARM_CLOCK_MINUTE_UP;
            alarm_clock_frm_setting(msg);
        }
    }
    else if (x > alarm_clock_button_touch[ALARM_CLOCK_MINUTE_DOWN].x0 && x < alarm_clock_button_touch[ALARM_CLOCK_MINUTE_DOWN].x1 &&
             y > alarm_clock_button_touch[ALARM_CLOCK_MINUTE_DOWN].y0 && y < alarm_clock_button_touch[ALARM_CLOCK_MINUTE_DOWN].y1)
    {
        //分钟-
        if (msg->dwAddData1 == GUI_MSG_TOUCH_DOWN || msg->dwAddData1 == GUI_MSG_TOUCH_LONGDOWN)
        {
            palarmClockFrm_attr->buttonF = ALARM_CLOCK_MINUTE_DOWN;
            alarm_clock_frm_setting(msg);
        }
    }
    else if (x > alarm_clock_button_touch[ALARM_CLOCK_OK].x0 && x < alarm_clock_button_touch[ALARM_CLOCK_OK].x1 &&
             y > alarm_clock_button_touch[ALARM_CLOCK_OK].y0 && y < alarm_clock_button_touch[ALARM_CLOCK_OK].y1)
    {
        //确认
        if (msg->dwAddData1 == GUI_MSG_TOUCH_UP)
        {
            alarm_clock_frm_close(msg);
            alarm_clock_frm_save(msg);
            palarmClockFrm_attr->buttonF = ALARM_CLOCK_OK;
        }
    }
    else if (x > alarm_clock_button_touch[ALARM_CLOCK_CANCEL].x0 && x < alarm_clock_button_touch[ALARM_CLOCK_CANCEL].x1 &&
             y > alarm_clock_button_touch[ALARM_CLOCK_CANCEL].y0 && y < alarm_clock_button_touch[ALARM_CLOCK_CANCEL].y1)
    {
        //取消
        if (msg->dwAddData1 == GUI_MSG_TOUCH_UP)
        {
            alarm_clock_frm_close(msg);
            palarmClockFrm_attr->buttonF = ALARM_CLOCK_CANCEL;
        }
    }
    else
    {
        return EPDK_OK;
    }

    alarm_clock_frm_change_draw(msg);
    return EPDK_OK;
}

static __s32 _alarm_clock_frm_manager_proc(__gui_msg_t *msg)
{
    __msg("_alarm_frm_manager_proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return _alarm_clock_frm_on_create(msg);

        case GUI_MSG_CLOSE:
            return _alarm_clock_frm_on_close(msg);

        case GUI_MSG_DESTROY:
            return _alarm_clock_frm_on_destory(msg);

        case GUI_MSG_PAINT:
            return _alarm_clock_frm_on_paint(msg);

        case GUI_MSG_KEY:
            return _alarm_clock_frm_on_key(msg);

        case GUI_MSG_TOUCH:
            return _alarm_clock_frm_on_touch(msg);

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_alarm_clock_frm_create(H_WIN parentWin, __u16 id, reg_alarm_para_t *reg_alarm)
{
    H_WIN retWin = NULL;
    __gui_framewincreate_para_t framewin_para;
    psetting_alarm_clock_attr               palarmClockFrm_attr;
    RECT winRect                = {0, 0, 200, 193};
    RECT layerRect              = {0, 0, 200, 193};
    __s32 width, height;
    dsk_display_get_size(&width, &height);
    layerRect.x = (width - 193) / 2;
    layerRect.y = (height - 193) / 2;
    //分配空间
    palarmClockFrm_attr = (psetting_alarm_clock_attr)esMEMS_Malloc(0, sizeof(setting_alarm_clock_attr));

    if (palarmClockFrm_attr == NULL)
    {
        __msg("esMEMS_Malloc fail\n");
        return NULL;
    }

    eLIBs_memset(palarmClockFrm_attr, 0, sizeof(setting_alarm_clock_attr));
    palarmClockFrm_attr->reg_alarm = reg_alarm;
    palarmClockFrm_attr->clock.hour = palarmClockFrm_attr->reg_alarm->alarm_time.hour;
    palarmClockFrm_attr->clock.minute = palarmClockFrm_attr->reg_alarm->alarm_time.minute;
    //创建图层
    palarmClockFrm_attr->layer = alarm_clock_frm_32bpp_layer_create(&layerRect);

    if (palarmClockFrm_attr->layer == NULL)
    {
        __msg("setting_alarm_clock_frm_create fail\n");
        esMEMS_Mfree(0, palarmClockFrm_attr);
        return NULL;
    }

    //创建窗体
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = "alarm_clock_frame",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = id;
    framewin_para.hHosting      = parentWin;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_alarm_clock_frm_manager_proc);
    framewin_para.rect.x        = winRect.x;
    framewin_para.rect.y        = winRect.y;
    framewin_para.rect.width    = winRect.width;
    framewin_para.rect.height   = winRect.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)palarmClockFrm_attr;
    framewin_para.hLayer        = palarmClockFrm_attr->layer;
    retWin = GUI_FrmWinCreate(&framewin_para);

    if (retWin == NULL)
    {
        __msg("create alarm clock frame fail\n");
        GUI_LyrWinDelete(palarmClockFrm_attr->layer);
        esMEMS_Mfree(0, palarmClockFrm_attr);
    }

    GUI_WinSetCaptureWin(retWin);
    GUI_WinSetFocusChild(retWin);
    return retWin;
}

__s32 setting_alarm_clock_frm_destory(H_WIN alarmWin)
{
    __gui_msg_t msg;
    psetting_alarm_clock_attr palarmFrm_ctl;
    msg.h_deswin = alarmWin;
    palarmFrm_ctl = (psetting_alarm_clock_attr)GUI_WinGetAttr(alarmWin);

    if (palarmFrm_ctl == NULL)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinDelete(palarmFrm_ctl->layer);
    return EPDK_TRUE;
}









