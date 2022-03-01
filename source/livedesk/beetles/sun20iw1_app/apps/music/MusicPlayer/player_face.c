/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
* File      :   music_player.h
*
* By        :   Libaiao
* Func  :
* Version   :   v1.0
* ============================================================================================================
* 2010-5-3 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include <log.h>
#include "beetles_app.h"
#include "music.h"
#include "music_ui.h"
#include "music_list.h"
#include "music_player.h"
#include "player_face.h"

extern __s32 willow_open(void);

extern void  willow_close(void);

extern  __s32  GUI_LyrCacheOn(void);
extern  __s32  GUI_LyrCacheOff(void);
static  __s32  set_charset(__epdk_charset_enm_e  e_charset)
{
    switch (e_charset)
    {
        case EPDK_CHARSET_ENM_GB2312      :
        case EPDK_CHARSET_ENM_GBK         :
            GUI_UC_SetEncodeGBK();
            break ;

        case EPDK_CHARSET_ENM_BIG5        :
            GUI_UC_SetEncodeBig5();
            break ;

        case EPDK_CHARSET_ENM_UTF8        :
            GUI_UC_SetEncodeUTF8();
            break ;

        case EPDK_CHARSET_ENM_UTF16BE     :
        case EPDK_CHARSET_ENM_UTF16LE     :
            break ;

        case EPDK_CHARSET_ENM_SJIS        :
            GUI_UC_SetEncodeSJIS();
            break ;

        case EPDK_CHARSET_ENM_EUC_JP      :
            GUI_UC_SetEncodeEUCJP();
            break ;

        case EPDK_CHARSET_ENM_EUC_KR      :
            GUI_UC_SetEncodeEUCKR();
            break ;

        case EPDK_CHARSET_ENM_KIO8_R      :
            GUI_UC_SetEncodeKIO8R();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_1  :
            GUI_UC_SetEncodeISO88591();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_2  :
            GUI_UC_SetEncodeISO88592();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_3  :
            GUI_UC_SetEncodeISO88593();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_4  :
            GUI_UC_SetEncodeISO88594();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_5  :
            GUI_UC_SetEncodeISO88595();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_6  :
            GUI_UC_SetEncodeISO88596();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_7  :
            GUI_UC_SetEncodeISO88597();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_8  :
            GUI_UC_SetEncodeISO88598();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_9  :
            GUI_UC_SetEncodeISO88599();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_10 :
            GUI_UC_SetEncodeISO885910();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_11 :
            GUI_UC_SetEncodeISO885911();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_12 :
            GUI_UC_SetEncodeISO885912();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_13 :
            GUI_UC_SetEncodeISO885913();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_14 :
            GUI_UC_SetEncodeISO885914();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_15 :
            GUI_UC_SetEncodeISO885915();
            break ;

        case EPDK_CHARSET_ENM_ISO_8859_16 :
            GUI_UC_SetEncodeISO885916();
            break ;

        case EPDK_CHARSET_ENM_CP874       :
            GUI_UC_SetEncodeCP874();
            break ;

        case EPDK_CHARSET_ENM_CP1250      :
            GUI_UC_SetEncodeCP1250();
            break ;

        case EPDK_CHARSET_ENM_CP1251      :
            GUI_UC_SetEncodeCP1251();
            break ;

        case EPDK_CHARSET_ENM_CP1253      :
            GUI_UC_SetEncodeCP1253();
            break ;

        case EPDK_CHARSET_ENM_CP1255      :
            GUI_UC_SetEncodeCP1255();
            break ;

        case EPDK_CHARSET_ENM_CP1256      :
            GUI_UC_SetEncodeCP1256();
            break ;

        case EPDK_CHARSET_ENM_CP1257      :
            GUI_UC_SetEncodeCP1257();
            break ;

        case EPDK_CHARSET_ENM_CP1258      :
            GUI_UC_SetEncodeCP1258();
            break ;

        default :
            break ;
    }

    return EPDK_OK ;
}

//***********************************************************************************
//以下以status结束的函数名，都属于Headbar范围内

//根据Mode在HeaderBar显示对应的状态icon
__s32 DrawMusicPlayModeStatus(rat_play_mode_e mode)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_headbar_ui_t *headbar_ui;
    headbar_ui = music_get_headbar_ui_param();
    X = headbar_ui->play_mode_rect.x;
    Y = headbar_ui->play_mode_rect.y;

    switch (mode)
    {
        case RAT_PLAY_MODE_SEQUENCE:
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ALL_STATUS_ICON);
            break;

        case RAT_PLAY_MODE_RANDOM:
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_RANDOM_STATUS_ICON);
            break;

        case RAT_PLAY_MODE_ONLY_ONCE:
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ONE_STATUS_ICON);    //
            break;

        case RAT_PLAY_MODE_ROTATE_ALL:
            pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ALL_STATUS_ICON);
            break;

        case RAT_PLAY_MODE_ROTATE_ONE:
            pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ONE_STATUS_ICON);
            break;

        default:
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ALL_STATUS_ICON);
            break;
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//根据EQ Mode在HeaderBar显示对应的状态icon
__s32 DrawMusicEQModeStatus(music_player_ctrl_t *this)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_headbar_ui_t *headbar_ui;
    GUI_RECT rect;
    char str[32];
    __s32 lang_id;
    headbar_ui = music_get_headbar_ui_param();
#if 0//显示文字 
    rect.x0 = headbar_ui->EQ_mode_rect.x - 30;
    rect.y0 = headbar_ui->EQ_mode_rect.y;
    rect.x1 = rect.x0 + 54;
    rect.y1 = rect.y0 + 20;
    GUI_LyrWinSel(this->music_layer);
    MusicSetInfoTransColor();
    GUI_ClearRectEx(&rect);
    MusicSetTransColor();

    switch (this->EQ_mode)
    {
        case MUSIC_EQ_NORMAL:
            lang_id = STRING_MUSIC_EQ_NORMAL;
            break;

        case MUSIC_EQ_DBB:
            lang_id = STRING_MUSIC_EQ_DDB;
            break;

        case MUSIC_EQ_POP:
            lang_id = STRING_MUSIC_EQ_POP;
            break;

        case MUSIC_EQ_ROCK:
            lang_id = STRING_MUSIC_EQ_ROCK;
            break;

        case MUSIC_EQ_CLASSIC:
            lang_id = STRING_MUSIC_EQ_CLASSIC;
            break;

        case MUSIC_EQ_JAZZ:
            lang_id = STRING_MUSIC_EQ_JAZZ;
            break;

        case MUSIC_EQ_VOCAL:
            lang_id = STRING_MUSIC_EQ_VACAL;
            break;

        case MUSIC_EQ_DANCE:
            lang_id = STRING_MUSIC_EQ_DANCE;
            break;

        case MUSIC_EQ_SOFT:
            lang_id = STRING_MUSIC_EQ_SOFT;
            break;

        default:
            lang_id = STRING_MUSIC_EQ_NORMAL;
    }

    dsk_langres_get_menu_text(lang_id, str, sizeof(str));
    GUI_CharSetToEncode(dsk_get_langres_charset());
    GUI_DispStringInRect(str, &rect, GUI_TA_VCENTER | GUI_TA_RIGHT);
#else//显示图片
    /* X = headbar_ui->EQ_mode_rect.x;
    Y = headbar_ui->EQ_mode_rect.y;

    switch(this->EQ_mode)
    {
     case MUSIC_EQ_NORMAL:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_NORMAL_STATUS_ICON);
         break;
         case MUSIC_EQ_DBB:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_DBB_STATUS_ICON);
         break;
         case MUSIC_EQ_POP:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_POP_STATUS_CION);
         break;
     case MUSIC_EQ_ROCK:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_ROCK_STATUS_CION);
         break;
         case MUSIC_EQ_CLASSIC:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_CLASSIC_STATUS_CION);
         break;
     case MUSIC_EQ_JAZZ:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_JAZZ_STATUS_CION);
         break;
     case MUSIC_EQ_VOCAL:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_VOCAL_STATUS_CION);
         break;
     case MUSIC_EQ_DANCE:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_DANCE_STATUS_CION);
         break;
     case MUSIC_EQ_SOFT:
         pic_buf = music_get_icon_res(ID_MUSIC_EQ_SOFT_STATUS_CION);
         break;
    }

    if(pic_buf != NULL)
    {
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
     GUI_BMP_Draw(pic_buf, X, Y);
    }
    */
    X = headbar_ui->BG_rect.x;
    Y = headbar_ui->BG_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_HEADBAR_BG);

    if (pic_buf != NULL)
    {
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//根据背光持续时间在HeaderBar显示对应的状态icon
__s32 DrawMusicBLLastTimeStatus(music_BL_time_e BL_time)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_headbar_ui_t *headbar_ui;
    headbar_ui = music_get_headbar_ui_param();
    X = headbar_ui->BLight_rect.x;
    Y = headbar_ui->BLight_rect.y;

    switch (BL_time)
    {
        case MUSIC_BL_ON:
            pic_buf = music_get_icon_res(ID_MUSIC_BL_ON_CION);
            break;

        case MUSIC_BL_LAST_10S:
            pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_10S_CION);
            break;

        case MUSIC_BL_LAST_30S:
            pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_30S_CION);
            break;

        case MUSIC_BL_LAST_60S:
            pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_60S_CION);
            break;

        case MUSIC_BL_LAST_90S:
            pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_90S_CION);
            break;
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//HeaderBar显示对应的音量状态icon
__s32 DrawMusicVolumeStatus(__s32 volume)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_headbar_ui_t *headbar_ui;
    GUI_RECT vol_rect;
    char vol_text[3];
    headbar_ui = music_get_headbar_ui_param();
    X = headbar_ui->volume_icon_rect.x;
    Y = headbar_ui->volume_icon_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_VOL_STATUS_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    MusicSetTransColor();
    //int2string();
    eLIBs_memset(vol_text, 0, sizeof(vol_text));

    //eLIBs_sprintf(vol_text,"%d", volume);
    if (volume < 100)
    {
        uint2str(volume, vol_text);
    }

    vol_rect.x0 = headbar_ui->volume_text_rect.x;
    vol_rect.y0 = headbar_ui->volume_text_rect.y;
    vol_rect.x1 = headbar_ui->volume_text_rect.x + headbar_ui->volume_text_rect.width;
    vol_rect.y1 = headbar_ui->volume_text_rect.y + headbar_ui->volume_text_rect.height;
    GUI_ClearRect(vol_rect.x0, vol_rect.y0, vol_rect.x1, vol_rect.y1);
    //GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect(vol_text, &vol_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

//HeaderBar显示对应的电池状态信息
__s32 DrawMusicBatteryStatus(power_level_e level)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_headbar_ui_t *headbar_ui;
    headbar_ui = music_get_headbar_ui_param();
    X = headbar_ui->battery_rect.x;
    Y = headbar_ui->battery_rect.y;

    switch (level)
    {
        case DSK_POWER_LEVEL_0:
            pic_buf = music_get_icon_res(ID_MUSIC_BATTERY_0_ICON);
            break;

        case DSK_POWER_LEVEL_1:
            pic_buf = music_get_icon_res(ID_MUSIC_BATTERY_1_ICON);
            break;

        case DSK_POWER_LEVEL_2:
            pic_buf = music_get_icon_res(ID_MUSIC_BATTERY_2_ICON);
            break;

        case DSK_POWER_LEVEL_3:
            pic_buf = music_get_icon_res(ID_MUSIC_BATTERY_3_ICON);
            break;

        case DSK_POWER_LEVEL_4:
            pic_buf = music_get_icon_res(ID_MUSIC_BATTERY_4_ICON);
            break;

        case DSK_POWER_LEVEL_5:
            pic_buf = music_get_icon_res(ID_MUSIC_BATTERY_5_ICON);
            break;

        default:
            break;
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//HeaderBar显示对应的系统时间
__s32 DrawMusicSystemTime(__awos_time_t *sys_time)
{
#if 0
    GUI_RECT time_rect;
    music_headbar_ui_t *headbar_ui;
    char time_text[12];
    headbar_ui = music_get_headbar_ui_param();
    eLIBs_memset(time_text, 0, sizeof(time_text));
    uint2str(sys_time->hour, time_text);            //转换为两位字符，自动加零
    eLIBs_strcat(time_text, ":");
    uint2str(sys_time->minute, time_text + 3);          //转换为两位字符，自动加零
    //eLIBs_sprintf(time_text, "%d",sys_time->hour);
    time_rect.x0  = headbar_ui->sys_time_rect.x;
    time_rect.y0  = headbar_ui->sys_time_rect.y;
    time_rect.x1  = headbar_ui->sys_time_rect.x + headbar_ui->sys_time_rect.width;
    time_rect.y1  = headbar_ui->sys_time_rect.y + headbar_ui->sys_time_rect.height;
    GUI_ClearRect(time_rect.x0, time_rect.y0, time_rect.x1, time_rect.y1);
    GUI_SetColor(GUI_WHITE);
    GUI_DispStringInRect(time_text, &time_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

__s32 DrawMusicHeadbar(music_player_ctrl_t *player_ctrl)
{
#if 0
    __awos_time_t sys_time = {0, 0, 0};
    //__wait__;
    DrawMusicPlayModeStatus(player_ctrl->play_mode);
    DrawMusicEQModeStatus(player_ctrl);
    //DrawMusicBLLastTimeStatus(player_ctrl->BL_time);
    player_ctrl->volume = dsk_volume_get();
    DrawMusicVolumeStatus(player_ctrl->volume);
    dsk_power_get_voltage_level(&(player_ctrl->battery_status));
    DrawMusicBatteryStatus(player_ctrl->battery_status);
#if BEETLES_RTC_EXIST
    esTIME_GetTime(&sys_time);
    //sys_time.hour = 12;
    //sys_time.minute= 00;
    DrawMusicSystemTime(&sys_time);
#else
    DrawMusicBLLastTimeStatus(player_ctrl->BL_time);
#endif
#endif
    return EPDK_OK;
}



//以上以status结束的函数名，都属于Headbar范围内
//---------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------

__s32 CleanMusicButtonHintText(void)
{
#if 0
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui;
    //char TextBuf[64];
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->btn_hint_rect.x;
    text_rect.y0 = player_ui->btn_hint_rect.y;
    text_rect.x1 = player_ui->btn_hint_rect.x + player_ui->btn_hint_rect.width;
    text_rect.y1 = player_ui->btn_hint_rect.y + player_ui->btn_hint_rect.height;
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
#endif
    return EPDK_OK;
}
__s32 DrawMusicPlayModeButton(rat_play_mode_e mode, music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_mode_btn_rect.x;
    Y = player_ui->play_mode_btn_rect.y;

    switch (mode)
    {
        case RAT_PLAY_MODE_SEQUENCE:
            if (status == FOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ALL_BTN_FOCUSED_ICON);
            }
            else if (status == UNFOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ALL_BTN_UNFOCUSED_ICON);
            }
            else if (status == PRESSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ALL_BTN_PRESSED_ICON);
            }

            DrawMsicButtonHintText(STRING_MUSIC_PLAY_ALL);
            break;

        case RAT_PLAY_MODE_ONLY_ONCE:
            if (status == FOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ONE_BTN_FOCUSED_ICON);   //
            }
            else if (status == UNFOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ONE_BTN_UNFOCUSED_ICON); //
            }
            else if (status == PRESSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ONE_BTN_PRESSED_ICON);   //
            }

            DrawMsicButtonHintText(STRING_MUSIC_PLAY_ONCE);
            break;

        case RAT_PLAY_MODE_RANDOM:
            if (status == FOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_RANDOM_BTN_FOCUSED_ICON);
            }
            else if (status == UNFOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_RANDOM_BTN_UNFOCUSED_ICON);
            }
            else if (status == PRESSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_PLAY_RANDOM_BTN_PRESSED_ICON);
            }

            DrawMsicButtonHintText(STRING_MUSIC_PLAY_RANDOM);
            break;

        case RAT_PLAY_MODE_ROTATE_ALL:
            if (status == FOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ALL_BTN_FOCUSED_ICON);
            }
            else if (status == UNFOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ALL_BTN_UNFOCUSED_ICON);
            }
            else if (status == PRESSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ALL_BTN_PRESSED_ICON);
            }

            DrawMsicButtonHintText(STRING_MUSIC_PLAY_ROTATE_ALL);
            break;

        case RAT_PLAY_MODE_ROTATE_ONE:
            if (status == FOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ONE_BTN_FOCUSED_ICON);
            }
            else if (status == UNFOCUSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ONE_BTN_UNFOCUSED_ICON);
            }
            else if (status == PRESSED)
            {
                pic_buf = music_get_icon_res(ID_MUSIC_REPEAT_ONE_BTN_UNFOCUSED_ICON);
            }

            DrawMsicButtonHintText(STRING_MUSIC_PLAY_ROTATE_ONE);
            break;

        default:
            //pic_buf = music_get_icon_res(ID_MUSIC_PLAY_ALL_BTN_FOCUSED_ICON);
            break;
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}


//根据EQ Mode 设置button, focused or not
__s32 DrawMusicEQModeButton(music_EQ_mode_e mode, music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->EQ_mode_btn_rect.x;
    Y = player_ui->EQ_mode_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_EQ_MODE_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_EQ_MODE_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_EQ_MODE_BTN_PRESSED_ICON);
    }

    switch (mode)
    {
        case MUSIC_EQ_NORMAL:
            DrawMsicButtonHintText(STRING_MUSIC_EQ_NORMAL);
            break;

        case MUSIC_EQ_CLASSIC:
            DrawMsicButtonHintText(STRING_MUSIC_EQ_CLASSIC);
            break;

        case MUSIC_EQ_JAZZ:
            DrawMsicButtonHintText(STRING_MUSIC_EQ_JAZZ);
            break;

        case MUSIC_EQ_ROCK:
            DrawMsicButtonHintText(STRING_MUSIC_EQ_ROCK);
            break;

        case MUSIC_EQ_POP:
            DrawMsicButtonHintText(STRING_MUSIC_EQ_POP);
            break;

        default:
            break;
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}


//根据Background Light Time  设置button, focused or not
__s32 DrawMusicBLTimeButton(music_BL_time_e time, music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->BLight_btn_rect.x;
    Y = player_ui->BLight_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_BTN_FOCUSED_CION);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_BTN_UNFOCUSED_CION);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_BL_TIME_BTN_PRESSED_CION);
    }

    switch (time)
    {
        case MUSIC_BL_ON:
            DrawMsicButtonHintText(STRING_MUSIC_BL_ON);
            break;

        case MUSIC_BL_LAST_10S:
            DrawMsicButtonHintText(STRING_MUSIC_BL_10S);
            break;

        case MUSIC_BL_LAST_30S:
            DrawMsicButtonHintText(STRING_MUSIC_BL_30S);
            break;

        case MUSIC_BL_LAST_60S:
            DrawMsicButtonHintText(STRING_MUSIC_BL_60S);
            break;

        case MUSIC_BL_LAST_90S:
            DrawMsicButtonHintText(STRING_MUSIC_BL_90S);
            break;

        default:
            break;
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//绘制play previous button, focused or not
__s32 DrawMusicPlayPreButton(music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_pre_btn_rect.x;
    Y = player_ui->play_pre_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_PRE_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_PRE_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_PRE_BTN_PRESSED_ICON);
    }

    DrawMsicButtonHintText(STRING_MUSIC_PLAY_PREVIOUS);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//绘制play next button, focused or not
__s32 DrawMusicPlayNextButton(music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_next_btn_rect.x;
    Y = player_ui->play_next_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_NEXT_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_NEXT_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_NEXT_BTN_PRESSED_ICON);
    }

    DrawMsicButtonHintText(STRING_MUSIC_PLAY_NEXT);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}

//绘制play pause button, focused or not
__s32 DrawMusicPlayPauseButton(music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_pause_btn_rect.x;
    Y = player_ui->play_pause_btn_rect.y;

    if (robin_get_fsm_status() != CEDAR_STAT_PAUSE)
    {
        if (status == FOCUSED)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_PAUSE_BTN_FOCUSED_ICON);
        }
        else if (status == UNFOCUSED)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_PAUSE_BTN_UNFOCUSED_ICON);
        }
        else if (status == PRESSED)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY_PAUSE_BTN_PRESSED_ICON);
        }
    }
    else
    {
        if (status == FOCUSED)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY1);
        }
        else if (status == UNFOCUSED)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY0);
        }
        else if (status == PRESSED)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PLAY1);
        }
    }

    DrawMsicButtonHintText(STRING_MUSIC_PLAY_PAUSE);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}


//绘制play backward button, focused or not
__s32 DrawMusicPlayBackwardButton(music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->backward_btn_rect.x;
    Y = player_ui->backward_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_BACKWARD_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_BACKWARD_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_BACKWARD_BTN_PRESSED_ICON);
    }

    DrawMsicButtonHintText(STRING_MUSIC_PLAY_BACKWARD);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}



//绘制play backward button, focused or not
__s32 DrawMusicPlayForwardButton(music_btn_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->forward_btn_rect.x;
    Y = player_ui->forward_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_FORWARD_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_FORWARD_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PLAY_FORWARD_BTN_PRESSED_ICON);
    }

    DrawMsicButtonHintText(STRING_MUSIC_PLAY_FORWARD);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}



//绘制play backward button, focused or not
__s32 DrawMusicPlayStatusButton(music_play_status_e status)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_status_rect.x;
    Y = player_ui->play_status_rect.y;

    if (status == MUSIC_PLAYING)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PALYING_STATUS_ICON);
    }
    else if (status == MUSIC_PAUSE)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_PAUSE_STATUS_ICON);
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

#endif
    return EPDK_OK;
}
__s32 CleanMusicPlayingTime(void)
{
#if 0
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->play_time.x;
    text_rect.y0 = player_ui->play_time.y;
    text_rect.x1 = player_ui->play_time.x + player_ui->play_time.width;
    text_rect.y1 = player_ui->play_time.y + player_ui->play_time.height;
    //GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //GUI_SetBkColor(GUI_WHITE);
    MusicSetTransColor();
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
#endif
    return EPDK_OK;
}
//显示提示字符
__s32 DrawMsicButtonHintText(__s32 TextID)
{
#if 0
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui;
    char TextBuf[64];
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->btn_hint_rect.x;
    text_rect.y0 = player_ui->btn_hint_rect.y;
    text_rect.x1 = player_ui->btn_hint_rect.x + player_ui->btn_hint_rect.width;
    text_rect.y1 = player_ui->btn_hint_rect.y + player_ui->btn_hint_rect.height;
    GUI_SetBkColor(0xB3000000);
    GUI_SetColor(GUI_WHITE);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    get_menu_text(TextID, TextBuf, sizeof(TextBuf));
    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

void DrawMusicPlayingTimeState(__u32 curTime, __u32 total)
{
    GUI_MEMDEV_Handle   draw_mem;
    void *pic_buf = NULL;
    music_palyer_ui_t *player_ui;
    __u32 pointCnt, i, pixCnt;
    __s32 X = 0, Y = 0;
    GUI_RECT text_rect;
    char time_text[20];
    RECT rect;
    dsk_display_get_size(&rect.width, &rect.height);

    if (rect.width == 800 && rect.height == 480)
    {
        draw_mem = GUI_MEMDEV_Create(327, 76, 454, 70);
    }
    else
    {
        draw_mem = GUI_MEMDEV_Create(327 - 80, 76, 454, 70);
    }

    //draw_mem = GUI_MEMDEV_Create(245+100, 128, 230+100, 30);
    GUI_MEMDEV_Select(draw_mem);
    GUI_SetBkColor(0xFF202125);
    GUI_SetColor(GUI_WHITE);
    //GUI_ClearRect(245+100, 128, 245+230+100, 128+30);
    //  __msg("DrawMusicPlayingTimeState curTime = %d total = %d \n"  , curTime , total );
    player_ui = music_get_player_ui_param();
    //Draw Bar
    X = player_ui->process_bar_body_rect.x;
    Y = player_ui->process_bar_body_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_BODY_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y - 3);
    }

    {
        Y -= 3;
        pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_POINT_ICON);

        //107 = 214/2
        if (total == 0)
        {
            pointCnt = 0;
        }
        else
        {
            pointCnt = (curTime * 220) / total;
        }

        for (i = 0; i < pointCnt; i++)
        {
            GUI_BMP_Draw(pic_buf, X, Y);
            X += player_ui->process_bar_point_rect.width;
        }

        X -= 2;

        if (curTime == 0)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_HEAD_ICON);
        }
        else if (total == curTime)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_TAIL_ICON);
        }
        else
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_FLAG);
        }

        if (X > player_ui->process_bar_body_rect.x + player_ui->process_bar_body_rect.width - 10)
        {
            X = player_ui->process_bar_body_rect.x + player_ui->process_bar_body_rect.width - 10;
        }

        GUI_BMP_Draw(pic_buf, X, Y);
    }

    //Draw Cur Time
    text_rect.x0 = player_ui->play_time.x;
    text_rect.y0 = player_ui->play_time.y;
    text_rect.x1 = player_ui->play_time.x + player_ui->play_time.width;
    text_rect.y1 = player_ui->play_time.y + player_ui->play_time.height;
    eLIBs_memset(time_text, 0, sizeof(time_text));
    time2str_by_format(curTime, time_text, TIME_AUTO_HMS);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    GUI_DispStringInRect(time_text, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //Draw Total
    text_rect.x0 = player_ui->total_time.x;
    text_rect.y0 = player_ui->total_time.y;
    text_rect.x1 = player_ui->total_time.x + player_ui->total_time.width;
    text_rect.y1 = player_ui->total_time.y + player_ui->total_time.height;
    eLIBs_memset(time_text, 0, sizeof(time_text));
    time2str_by_format(total, time_text, TIME_AUTO_HMS);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    GUI_DispStringInRect(time_text, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
}


__s32 DrawMusicAlbum(music_player_ctrl_t *This, const char *file, GUI_RECT *clear)
{
    __s32       ret, w, h, cnt = 0;
    __u32       *pixls, bgra;
    FB          fb;
    music_ui_t *ui;
    SIZE        size;
    void       *buf;
#if  1
    buf = music_get_album_buf();
    ui = music_get_ui_param();
    //__here__;
    size.width  = ui->album.albumRect.width;
    size.height = ui->album.albumRect.height;
    //__here__;
    eLIBs_memset(buf, 0, size.width * size.height * 4 + 0x100);
    //__here__;
    //__msg(" file = %s\n", file);
    ret = willow_open();

    //__msg("ret = %x\n", ret);
    if (EPDK_FAIL == ret)
    {
        __wrn("willow_open fail...\n");
        return EPDK_FAIL;
    }

    //__msg("buf = %x\n", buf);
    //__msg("size.width = %x\n", size.width);
    //__msg("size.height = %x\n", size.height);
    ret = robin_get_album_cover(file, buf, &size, 0);
    willow_close();
    //__msg("buf = %x\n", buf);
    //esKRNL_SchedLock();
    GUI_LyrWinSel(This->music_layer);

    if (ret == EPDK_OK)
    {
        pixls = (__u32 *)buf;

        for (h = 0; h < size.height; h++)
        {
            for (w = 0; w < size.width; w++)
            {
                bgra = pixls[cnt];
                pixls[cnt] = (((bgra & 0xff) << 24) | ((bgra & 0xff00) << 8) | ((bgra & 0xff0000) >> 8) | ((bgra & 0xff000000) >> 24));
                cnt++;
            }
        }

        GUI_SetBkColor(0xFF2B2E36);//GUI_SetBkColor(0);
        GUI_ClearRectEx(clear);
        eLIBs_memset(&fb, 0, sizeof(FB));
        fb.size.width   = size.width;
        fb.size.height  = size.height;
        fb.addr[0]      = buf;
        fb.fmt.type     = FB_TYPE_RGB;
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        fb.fmt.fmt.rgb.br_swap  = 0;
        fb.fmt.fmt.rgb.pixseq   = (__rgb_seq_t)0;
        GUI_BitString_Draw(&fb,     ui->album.albumRect.x, ui->album.albumRect.y);
        //  __here__;
    }
    else
    {
        __msg("+++++++++++++++++++++++++++++\n");
        buf = music_get_icon_res(ID_MUSIC_DEFAULT_ALBUM_ICON);
        GUI_BMP_Draw(buf, ui->album.albumRect.x, ui->album.albumRect.y);
    }

#endif
    return ret;
}

//绘制play next button, focused or not
__s32 DrawMusicUsbDiskButton(music_btn_status_e status)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_usb_disk_btn_rect.x;
    Y = player_ui->play_usb_disk_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_USB_DISK_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_USB_DISK_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_USB_DISK_BTN_PRESSED_ICON);
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    return EPDK_OK;
}

//绘制play next button, focused or not
__s32 DrawMusicSdCardButton(music_btn_status_e status)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->play_sd_card_btn_rect.x;
    Y = player_ui->play_sd_card_btn_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_SD_CARD_BTN_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_SD_CARD_BTN_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_SD_CARD_BTN_PRESSED_ICON);
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    return EPDK_OK;
}


void DrawMusicBg(void)
{
    void *pic_buf = NULL;
    music_ui_t *ui;
    ui = music_get_ui_param();
    pic_buf = music_get_icon_res(ID_MUSIC_BG);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, ui->bg.BG_rect.x, ui->bg.BG_rect.y);
    }
}

__s32 DrawMusicMenuAll(music_player_ctrl_t *this)
{
    GUI_LyrWinSel(this->music_layer);
    DrawMusicMenuBtn(UNFOCUSED);
    DrawMusicShowModeMenu(UNFOCUSED);
#ifndef MUSIC_VIEW_CHANGE_20160811
    DrawMusicABMenu(UNFOCUSED);
#endif
    DrawMenuBtnText(STRING_MUSIC_MENU);
#ifndef MUSIC_VIEW_CHANGE_20160811
    DrawShowModeText(STRING_MUSIC_ALBUM);
    DrawABText(STRING_MUSIC_AB);
    {
        __s32 channel;
        __s32 lang_id = STRING_MUSIC_TACK_STEREO;
        channel = robin_get_channel();

        if (0 == channel)
        {
            lang_id = STRING_MUSIC_TACK_STEREO;
        }
        else if (1 == channel)
        {
            lang_id = STRING_MUSIC_TACK_LEFT;
        }
        else if (2 == channel)
        {
            lang_id = STRING_MUSIC_TACK_RIGHT;
        }

        DrawShowTACKText(lang_id);
    }
#endif
    return 0;
}


__s32 DrawMusicPlayerInfo(music_player_ctrl_t *this, __s32 index)
{
    GUI_LyrWinSel(this->music_layer);
    DrawMusicEQModeStatus(this);
#if 0
    DrawMusicPlayModeStatus(this->play_mode);

    if (index == 0)
    {
        DrawMusicPlayPauseButton(PRESSED);
    }
    else
    {
        DrawMusicPlayPauseButton(UNFOCUSED);
    }

    if (index == 1)
    {
        DrawMusicPlayBackwardButton(PRESSED);
    }
    else
    {
        DrawMusicPlayBackwardButton(UNFOCUSED);
    }

    if (index == 2)
    {
        DrawMusicPlayForwardButton(PRESSED);
    }
    else
    {
        DrawMusicPlayForwardButton(UNFOCUSED);
    }

    if (index == 3)
    {
        DrawMusicPlayPreButton(PRESSED);
    }
    else
    {
        DrawMusicPlayPreButton(UNFOCUSED);
    }

    if (index == 4)
    {
        DrawMusicPlayNextButton(PRESSED);
    }
    else
    {
        DrawMusicPlayNextButton(UNFOCUSED);
    }

#endif

    if (index == 5)
    {
        __wrn("UsbDiskButton PRESSED\r\n");
        DrawMusicUsbDiskButton(PRESSED);
    }
    else
    {
        __wrn("UsbDiskButton UNFOCUSED\r\n");
        DrawMusicUsbDiskButton(UNFOCUSED);
    }

    if (index == 6)
    {
        __wrn("SdCardButton PRESSED\r\n");
        DrawMusicSdCardButton(PRESSED);
    }
    else
    {
        __wrn("SdCardButton UNFOCUSED\r\n");
        DrawMusicSdCardButton(UNFOCUSED);
    }

    return EPDK_OK;
}

__s32 DrawMusicMenuBtn(music_btn_status_e status)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->menu_icon_rect.x;
    Y = player_ui->menu_icon_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_MENU_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_MENU_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_MENU_FOCUSED_ICON);
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    return EPDK_OK;
}

__s32 DrawMusicABMenu(music_btn_status_e status)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui = NULL;
    player_ui = music_get_player_ui_param();
    X = player_ui->ab_icon_rect.x;
    Y = player_ui->ab_icon_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_AB1);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_AB0);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_AB1);
    }

    //__here__;
    if (pic_buf != NULL)
    {
        //__here__;
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    return EPDK_OK;
}

__s32 DrawMusicShowModeMenu(music_btn_status_e status)
{
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->show_mode_icon_rect.x;
    Y = player_ui->show_mode_icon_rect.y;

    if (status == FOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_SHOW_MODE_FOCUSED_ICON);
    }
    else if (status == UNFOCUSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_SHOW_MODE_UNFOCUSED_ICON);
    }
    else if (status == PRESSED)
    {
        pic_buf = music_get_icon_res(ID_MUSIC_SHOW_MODE_FOCUSED_ICON);
    }

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    return EPDK_OK;
}

//显示提示字符
__s32 DrawMenuBtnText(__s32 TextID)
{
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui;
    char TextBuf[64];
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->menu_text_rect.x;
    text_rect.y0 = player_ui->menu_text_rect.y;
    text_rect.x1 = player_ui->menu_text_rect.x + player_ui->menu_text_rect.width;
    text_rect.y1 = player_ui->menu_text_rect.y + player_ui->menu_text_rect.height;
    GUI_SetBkColor(0xFF2B2E36);
    GUI_SetColor(GUI_WHITE);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    get_menu_text(TextID, TextBuf, sizeof(TextBuf));
    //  __msg("TextBuf = %s\n", TextBuf);
    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return EPDK_OK;
}
//显示提示字符
__s32 DrawABText(__s32 TextID)
{
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui = NULL;
    char TextBuf[64] = {0};
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->ab_text_rect.x;
    text_rect.y0 = player_ui->ab_text_rect.y;
    text_rect.x1 = player_ui->ab_text_rect.x + player_ui->ab_text_rect.width;
    text_rect.y1 = player_ui->ab_text_rect.y + player_ui->ab_text_rect.height;
    GUI_SetBkColor(0xFF2B2E36);
    GUI_SetColor(GUI_WHITE);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    get_menu_text(TextID, TextBuf, sizeof(TextBuf));
    //__msg("TextBuf = %s\n", TextBuf);
    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return EPDK_OK;
}

//显示提示字符
__s32 DrawShowModeText(__s32 TextID)
{
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui;
    char TextBuf[64];
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->show_mode_text_rect.x;
    text_rect.y0 = player_ui->show_mode_text_rect.y;
    text_rect.x1 = player_ui->show_mode_text_rect.x + player_ui->show_mode_text_rect.width;
    text_rect.y1 = player_ui->show_mode_text_rect.y + player_ui->show_mode_text_rect.height;
    GUI_SetBkColor(0xFF2B2E36);
    GUI_SetColor(GUI_WHITE);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    get_menu_text(TextID, TextBuf, sizeof(TextBuf));
    //__msg("TextBuf = %s\n", TextBuf);
    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return EPDK_OK;
}

//显示提示字符
__s32 DrawShowTACKText(__s32 TextID)
{
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui = NULL;
    char TextBuf[64] = {0};
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->tack_text_rect.x;
    text_rect.y0 = player_ui->tack_text_rect.y;
    text_rect.x1 = player_ui->tack_text_rect.x + player_ui->tack_text_rect.width;
    text_rect.y1 = player_ui->tack_text_rect.y + player_ui->tack_text_rect.height;
    GUI_SetBkColor(0xFF2B2E36);
    GUI_SetColor(GUI_WHITE);
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    get_menu_text(TextID, TextBuf, sizeof(TextBuf));
    //__msg("TextBuf = %s\n", TextBuf);
    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return EPDK_OK;
}

void DrawMusicBottom(music_player_ctrl_t *this, __s16 focusedIndex)
{
#if 0
    void *pic_buf = NULL;
    music_ui_t *ui;
    __u32 i = 0;
    __s16 index[5] = {0, 0, 0, 0, 0};

    if (focusedIndex >= 0 && focusedIndex < 5)
    {
        index[focusedIndex] = 5;
    }

    ui = music_get_ui_param();
    pic_buf = music_get_icon_res(ID_MUSIC_BACK0 + index[0]);
    GUI_BMP_Draw(pic_buf, ui->bottom.back_rect.x, ui->bottom.back_rect.y);
    pic_buf = music_get_icon_res(ID_MUSIC_EQ_CHANGE0 + index[1]);
    GUI_BMP_Draw(pic_buf, ui->bottom.EQ_Change_rect.x, ui->bottom.EQ_Change_rect.y);
    pic_buf = music_get_icon_res(ID_MUSIC_PLAY_MODE0 + index[2]);
    GUI_BMP_Draw(pic_buf, ui->bottom.play_mode_rect.x, ui->bottom.play_mode_rect.y);
    pic_buf = music_get_icon_res(ID_MUSIC_EQ_LYRICS0 + index[3]);
    GUI_BMP_Draw(pic_buf, ui->bottom.EQ_lyrics_rect.x, ui->bottom.EQ_lyrics_rect.y);
    pic_buf = music_get_icon_res(ID_MUSIC_AB0 + index[4]); //112350 ID_MUSIC_AB0
    GUI_BMP_Draw(pic_buf, ui->bottom.AB_rect.x, ui->bottom.AB_rect.y);
    {
        __s32 i;
        char text[128] = {0};
        RECT *prect = NULL;
        music_ui_t *ui = NULL;
        __s32 lang_id[5] =
        {
            STRING_MUSIC_RETURN,
            STRING_MUSIC_EQ,
            STRING_MUSIC_REPEAT,
            STRING_MUSIC_ALBUM,
            STRING_MUSIC_AB,
        };

        if (MUSIC_AB == this->AB_mode)
        {
            lang_id[4]  =   STRING_MUSIC_AB;
        }
        else if (MUSIC_A == this->AB_mode)
        {
            lang_id[4]  =   STRING_MUSIC_A;
        }
        else if (MUSIC_B == this->AB_mode)
        {
            lang_id[4]  =   STRING_MUSIC_B;
        }

        ui = music_get_ui_param();

        if (NULL == ui)
        {
            return ;
        }

        GUI_SetFont(SWFFont);
        GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        prect = &ui->bottom.back_rect;

        for (i = 0 ; i < sizeof(lang_id) / sizeof(lang_id[0]) ; i++)
        {
            if (focusedIndex == i)
            {
                GUI_SetColor(GUI_YELLOW);
            }
            else
            {
                GUI_SetColor(GUI_WHITE);
            }

            dsk_langres_get_menu_text(lang_id[i], text, sizeof(text));
            __wrn("text=%s  focusedIndex = %d \n", text, focusedIndex);
            __wrn("prect->x=%d, prect->y=%d\n", prect->x, prect->y);

            if ((i == (sizeof(lang_id) / sizeof(lang_id[0]) - 1)) && (MUSIC_AB != this->AB_mode))
            {
                GUI_DispStringAt(text, prect->x + 35 + 10, prect->y + (28 - 16) / 2 - 2);
            }
            else
            {
                GUI_DispStringAt(text, prect->x + 35, prect->y + (28 - 16) / 2 - 2);
            }

            prect++;
        }
    }
#endif
}


void DrawMusicBottomBg(void)
{
#if 0
    void *pic_buf = NULL;
    music_ui_t *ui;
    __u32 i = 0;
    ui = music_get_ui_param();
    pic_buf = music_get_icon_res(ID_MUSIC_BOTTOM);

    if (pic_buf != NULL)
    {
        for (i = 0; i < ui->bottom.bottom_rect.width; i++)
        {
            GUI_BMP_Draw(pic_buf, ui->bottom.bottom_rect.x + i, ui->bottom.bottom_rect.y);
        }
    }

#endif
}


//显示当前播放时间
__s32 DrawMusicPlayingTime(__u32 time)
{
#if 0
    GUI_RECT text_rect;
    char time_text[20];
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->play_time.x;
    text_rect.y0 = player_ui->play_time.y;
    text_rect.x1 = player_ui->play_time.x + player_ui->play_time.width;
    text_rect.y1 = player_ui->play_time.y + player_ui->play_time.height;
    //GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    MusicSetTransColor();
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    eLIBs_memset(time_text, 0, sizeof(time_text));
    //time2str(time, time_text);
    time2str_by_format(time, time_text, TIME_AUTO_HMS);
    GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

__s32 CleanMusicTotalTime(void)
{
#if 0
    GUI_RECT text_rect;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    text_rect.x0 = player_ui->total_time.x;
    text_rect.y0 = player_ui->total_time.y;
    text_rect.x1 = player_ui->total_time.x + player_ui->total_time.width;
    text_rect.y1 = player_ui->total_time.y + player_ui->total_time.height;
    MusicSetTransColor();
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    //eLIBs_memset(time_text, 0 ,sizeof(time_text));
    //time2str(time, time_text);
    //GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

//显示音乐播放总时间
__s32 DrawMusicTotalTime(__u32 time)
{
#if 0
    GUI_RECT text_rect;
    char time_text[20];
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    //GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    //GUI_SetBkColor(0x99000000);
    text_rect.x0 = player_ui->total_time.x;
    text_rect.y0 = player_ui->total_time.y;
    text_rect.x1 = player_ui->total_time.x + player_ui->total_time.width;
    text_rect.y1 = player_ui->total_time.y + player_ui->total_time.height;
    MusicSetTransColor();
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    eLIBs_memset(time_text, 0, sizeof(time_text));
    //time2str(time, time_text);
    time2str_by_format(time, time_text, TIME_AUTO_HMS);
    GUI_DispStringInRect(time_text, &text_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

//只画进度点，不画背景条
__s32 DrawMusicProgressBarPoint(__u32 total, __u32 cur_value)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0, BodyWidth = 0, PointWidth = 0;
    music_palyer_ui_t *player_ui;
    __u32 total_pieces = 0;
    __u32 average = 0;
    __u32 point = 0, i = 0;
    player_ui = music_get_player_ui_param();

    if ((total == 0) || (cur_value == 0))
    {
        X = player_ui->process_bar_body_rect.x;
        Y = player_ui->process_bar_body_rect.y;
        pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_BODY_ICON);
        GUI_BMP_Draw(pic_buf, X, Y);
        return EPDK_OK;                     //没有任何进度
    }

    BodyWidth = player_ui->process_bar_body_rect.width;
    PointWidth = player_ui->process_bar_point_rect.width;
    total_pieces = BodyWidth / PointWidth;
#if 1
    {
        __u32 cnt = 0, len = 0;
        __s16 offsetX = 0, offsetY = 0;

        if (cur_value >= total)
        {
            cur_value = total - 1;
        }

        len = (cur_value * total_pieces) / total;

        if (cnt == 0)
        {
            X = player_ui->process_bar_head_rect.x;
            Y = player_ui->process_bar_head_rect.y;
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_HEAD_ICON);

            if (pic_buf != NULL)
            {
                GUI_BMP_Draw(pic_buf, X, Y);
            }

            X += PointWidth;
        }

        for (cnt = 0; cnt < len; cnt++)
        {
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_POINT_ICON);
            GUI_BMP_Draw(pic_buf, X, Y);
            X += PointWidth;
        }

        if (cnt == total_pieces - 1)
        {
            X -= PointWidth;
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_POINT_ICON);
            GUI_BMP_Draw(pic_buf, X, Y);
            X = player_ui->process_bar_tail_rect.x;
            Y = player_ui->process_bar_tail_rect.y;
            pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_TAIL_ICON);
            GUI_BMP_Draw(pic_buf, X, Y);
        }
    }
#else

    if (total >= total_pieces)
    {
        average = total /  total_pieces;        //获得平均每份代表的值
        point = cur_value / average;            //获得当前显示的进度点
    }
    else
    {
        point = cur_value * total / total_pieces;
    }

    //if(point == 1)
    {
        X = player_ui->process_bar_head_rect.x;
        Y = player_ui->process_bar_head_rect.y;
        pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_HEAD_ICON);

        if (pic_buf != NULL)
        {
            GUI_BMP_Draw(pic_buf, X, Y);
        }
    }
    //else
    {
        X = player_ui->process_bar_point_rect.x;
        Y = player_ui->process_bar_point_rect.y;    //point 从第二个位置开始显示
        pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_POINT_ICON);

        if (pic_buf != NULL)
        {
            for (i = 1; i <= point; i++)
            {
                if (point == total_pieces)          //画结束部分
                {
                    X = player_ui->process_bar_tail_rect.x;
                    Y = player_ui->process_bar_tail_rect.y; //point 从第二个位置开始显示
                    pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_TAIL_ICON);
                    GUI_BMP_Draw(pic_buf, X, Y);
                    break;
                }

                GUI_BMP_Draw(pic_buf, X, Y);
                X += PointWidth;
            }
        }
    }
#endif
#endif
    return EPDK_OK;
}

//画进度条,total为总量， cur_value为当前进度值
__s32 DrawMusicProgressBar(__u32 total, __u32 cur_value)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->process_bar_body_rect.x;
    Y = player_ui->process_bar_body_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_PROCESS_BAR_BODY_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    if ((total == 0) || (cur_value == 0))
    {
        //GUI_LyrWinCacheOff();
        return EPDK_OK;                     //没有任何进度
    }

    DrawMusicProgressBarPoint(total, cur_value);
    //GUI_LyrWinCacheOff();
    //GUI_LyrWinFlushFB();
#endif
    return EPDK_OK;
}



void DrawMusicInfoBg(void)
{
#if 0
    void *pic_buf = NULL;
    music_ui_t *ui;
    ui = music_get_ui_param();
    pic_buf = music_get_icon_res(ID_MUSIC_INFO_BAR);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, ui->bg.InfoBar_rect.x, ui->bg.InfoBar_rect.y);
    }

#endif
}



void DrawMusicPlayBar(void)
{
#if 0
    void *pic_buf = NULL;
    music_ui_t *ui;
    ui = music_get_ui_param();
    pic_buf = music_get_icon_res(ID_MUSIC_PLAYBAR);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, ui->bg.PlayBar_rect.x, ui->bg.PlayBar_rect.y);
    }

#endif
}


//画进度条,total为总量， cur_value为当前进度值
__s32 DrawMusicPerformerName(audio_file_info_t *file_info)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    GUI_RECT name_rect;
    music_palyer_ui_t *player_ui;
    __epdk_charset_enm_e e_charset ;
    char name_omission[RAT_MAX_FULL_PATH_LEN];
    eLIBs_memset(name_omission, 0, sizeof(name_omission));
    player_ui = music_get_player_ui_param();
    X = player_ui->performer_icon_rect.x;
    Y = player_ui->performer_icon_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_PERFORMER_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    if (eLIBs_strlen(file_info->artist) == 0)
    {
        eLIBs_strcpy(file_info->artist, "NULL");
    }

    GUI_SetColor(GUI_WHITE);
    name_rect.x0  = player_ui->performer_name_rect.x;
    name_rect.y0  = player_ui->performer_name_rect.y;
    name_rect.x1  = player_ui->performer_name_rect.x + player_ui->performer_name_rect.width;
    name_rect.y1  = player_ui->performer_name_rect.y + player_ui->performer_name_rect.height;
    MusicSetInfoTransColor();
    GUI_ClearRect(name_rect.x0, name_rect.y0, name_rect.x1, name_rect.y1);
    MusicSetTransColor();
    /*
    switch( file_info->artist_charset )
    {
        case EPDK_CHARSET_ENM_GB2312 :
            GUI_UC_SetEncodeGBK();
            break ;
        case EPDK_CHARSET_ENM_UTF8:
            GUI_UC_SetEncodeUTF8();
            break ;
        default :
            GUI_UC_SetEncodeUTF8();
            break ;
    }
    */

    if (EPDK_LANGUAGE_ENM_UNKNOWN == file_info->artist_charset)
    {
        reg_system_para_t  *reg_system_para ;
        reg_system_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

        if (reg_system_para)
        {
            if (EPDK_LANGUAGE_ENM_CHINESES == reg_system_para->charset)
            {
                e_charset = EPDK_CHARSET_ENM_GBK;
            }
            else if (EPDK_LANGUAGE_ENM_CHINESET == reg_system_para->charset)
            {
                e_charset = EPDK_CHARSET_ENM_BIG5;
            }
            else
            {
                e_charset = EPDK_CHARSET_ENM_GBK;
            }
        }
    }
    else
    {
        e_charset = file_info->artist_charset ;
    }

    __msg("e_charset = %d\n", e_charset);
    set_charset(e_charset);
    GetClippedString(&name_rect, file_info->artist, name_omission, "...");
    __msg("DrawMusicPerformerName charset = %d\n", file_info->artist_charset);
    GUI_DispStringInRect(name_omission, &name_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}


//显示文件大小
__s32 DrawMusicFileSize(__s32 file_size)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    GUI_RECT size_rect;
    char size_text[12];
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->file_size_icon_rect.x;
    Y = player_ui->file_size_icon_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_FILE_SIZE_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    if (file_size == 0)
    {
        return EPDK_OK;
    }

    eLIBs_memset(size_text, 0, sizeof(size_text));
    GUI_SetColor(GUI_WHITE);
    filesize2str(file_size, size_text);
    //eLIBs_sprintf(size_text, "%d",file_size/1024);        //M
    //eLIBs_strcat(size_text, "M");
    size_rect.x0  = player_ui->file_size_text_rect.x;
    size_rect.y0  = player_ui->file_size_text_rect.y;
    size_rect.x1  = player_ui->file_size_text_rect.x + player_ui->file_size_text_rect.width;
    size_rect.y1  = player_ui->file_size_text_rect.y + player_ui->file_size_text_rect.height;
    MusicSetInfoTransColor();
    GUI_ClearRect(size_rect.x0, size_rect.y0, size_rect.x1, size_rect.y1);
    MusicSetTransColor();
    GUI_DispStringInRect(size_text, &size_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}


//显示文件专辑名称album
__s32 DrawMusicFileAlbum(audio_file_info_t *file_info)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    GUI_RECT text_rect;
    char album_omission[ROBIN_ALBUM_SIZE + 5];
    music_palyer_ui_t *player_ui;
    eLIBs_memclr(album_omission, sizeof(album_omission));
    player_ui = music_get_player_ui_param();
    X = player_ui->album_icon_rect.x;
    Y = player_ui->album_icon_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_ALBUM_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    if (eLIBs_strlen(file_info->album) == 0)
    {
        eLIBs_strcpy(file_info->album, "NULL");
    }

    GUI_SetColor(GUI_WHITE);
    text_rect.x0  = player_ui->album_text_rect.x;
    text_rect.y0  = player_ui->album_text_rect.y;
    text_rect.x1  = player_ui->album_text_rect.x + player_ui->album_text_rect.width;
    text_rect.y1  = player_ui->album_text_rect.y + player_ui->album_text_rect.height;
    MusicSetInfoTransColor();
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1 + 2, text_rect.y1);
    MusicSetTransColor();
    set_charset(file_info->album_charset);
    GetClippedString(&text_rect, file_info->album, album_omission, "...");
    GUI_DispStringInRect(album_omission, &text_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

//显示文件比特率
__s32 DrawMusicFileBitRate(__s32 bit_rate)
{
#if 0
    void *pic_buf = NULL;
    __s32 X = 0, Y = 0;
    GUI_RECT text_rect;
    char bit_rate_text[12];
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    X = player_ui->bit_rate_icon_rect.x;
    Y = player_ui->bit_rate_icon_rect.y;
    pic_buf = music_get_icon_res(ID_MUSIC_BIT_RATE_ICON);

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, X, Y);
    }

    if (bit_rate == 0)
    {
        return EPDK_OK;
    }

    GUI_SetColor(GUI_WHITE);
    eLIBs_memset(bit_rate_text, 0, sizeof(bit_rate_text));
    eLIBs_sprintf(bit_rate_text, "%dKbps", bit_rate / 1024); //K bps
    text_rect.x0  = player_ui->bit_rate_text_rect.x;
    text_rect.y0  = player_ui->bit_rate_text_rect.y;
    text_rect.x1  = player_ui->bit_rate_text_rect.x + player_ui->bit_rate_text_rect.width;
    text_rect.y1  = player_ui->bit_rate_text_rect.y + player_ui->bit_rate_text_rect.height;
    MusicSetInfoTransColor();
    GUI_ClearRect(text_rect.x0, text_rect.y0, text_rect.x1, text_rect.y1);
    MusicSetTransColor();
    GUI_DispStringInRect(bit_rate_text, &text_rect, GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}

//绘制控制按钮, menu_id及代表button id,
//status, 表示button 当前的状态，unfocused, focused,
__s32 DrawMusicPlayerContrlButtonByID(music_player_ctrl_t *this, __s32 menu_id, music_btn_status_e status)
{
#if 0

    switch (menu_id)
    {
        case SET_PLAY_MODE:
            DrawMusicPlayModeButton(this->play_mode, status);
            break;

        case SET_EQ_MODE:
            DrawMusicEQModeButton(this->EQ_mode, status);
            break;

        case SET_BL_TIME:
            DrawMusicBLTimeButton(this->BL_time, status);
            break;

        case SET_PLAY_PREVIOUS:
            DrawMusicPlayPreButton(status);
            break;

        case SET_PLAY_NEXT:
            DrawMusicPlayNextButton(status);
            break;

        case SET_PLAY_PAUSE:
            DrawMusicPlayPauseButton(status);
            break;

        case SET_PLAY_BACKWARD:
            DrawMusicPlayBackwardButton(status);
            break;

        case SET_PLAY_FORWARD:
            DrawMusicPlayForwardButton(status);
            break;
    }

#endif
    return EPDK_OK;
}


__s32 ClearMusicPlayerContrlButtonBG(void)
{
#if 0
    GUI_RECT BG_rect;
    music_palyer_ui_t *player_ui;
    player_ui = music_get_player_ui_param();
    BG_rect.x0 = player_ui->contrl_BG_rect.x;
    BG_rect.y0 = player_ui->contrl_BG_rect.y;
    BG_rect.x1 = player_ui->contrl_BG_rect.x + player_ui->contrl_BG_rect.width;
    BG_rect.y1 = player_ui->contrl_BG_rect.y + player_ui->contrl_BG_rect.height;
    GUI_ClearRect(BG_rect.x0, BG_rect.y0, BG_rect.x1, BG_rect.y1);
    CleanMusicButtonHintText();
#endif
    return EPDK_OK;
}
//绘制所有控制按钮, unfocused status
__s32 DrawMusicPlayerContrlButton(music_player_ctrl_t *this)
{
    //ClearMusicPlayerContrlButtonBG();
    //DrawMusicPlayModeButton(this->play_mode,UNFOCUSED);
    //DrawMusicEQModeButton(this->EQ_mode, UNFOCUSED);
    //DrawMusicBLTimeButton(this->BL_time, UNFOCUSED);
    //DrawMusicPlayPreButton(UNFOCUSED);
    //DrawMusicPlayNextButton(UNFOCUSED);
    //DrawMusicPlayPauseButton(UNFOCUSED);
    //DrawMusicPlayBackwardButton(UNFOCUSED);
    //DrawMusicPlayForwardButton(UNFOCUSED);
    DrawMusicUsbDiskButton(UNFOCUSED);
    DrawMusicSdCardButton(UNFOCUSED);
    return EPDK_OK;
}
__s32 DrawMusicPlayerFileInfo(audio_file_info_t *file_info)
{
    //DrawMusicPerformerName(file_info);
    //DrawMusicFileSize(file_info->filesize);
    //DrawMusicFileAlbum(file_info);
    //DrawMusicFileBitRate(file_info->bitrate);
    return EPDK_OK;
}

__s32 DrawMusicPlayer(music_player_ctrl_t *this)
{
    DrawMusicPlayerContrlButton(this);
    //DrawMusicPlayStatusButton(MUSIC_PLAYING);
    //DrawMusicPlayingTime(0);
    //DrawMusicTotalTime(0);
    //DrawMusicProgressBar(0,0);
    //DrawMusicPlayerFileInfo(&(this->play_file_info));
    return EPDK_OK;
}

__s32 DrawPlayerCurrentTime(music_player_ctrl_t *player_ctrl)
{
    //__u32 cur_time = 0;
    //cur_time = robin_get_cur_time();
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function   :           static __s32 music_player_draw_background(music_player_ctrl_t *player_ctrl)
*
*Description    :           on paint message
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 draw_music_player_background(music_player_ctrl_t *player_ctrl)
{
#if 0
    __s32 ret;
    void *pic_buf;
    //GUI_LyrWinSel(player_ctrl->background_layer);
    GUI_SetFont(player_ctrl->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    ret = com_set_palette_by_id(ID_MUSIC_PALETTE_BMP);
    __msg("set palette return value = %d", ret);
    pic_buf = music_get_icon_res(ID_MUSIC_BACKGROUND);          //draw select border

    if (pic_buf != NULL)
    {
        GUI_BMP_Draw(pic_buf, 0, 0);
    }

#endif
    return EPDK_OK;
}



