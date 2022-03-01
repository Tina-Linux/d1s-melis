
#include <log.h>
#include "setting_alarm.h"
#include "setting_uipara.h"

#define listbar_num 7
const static __s32 setting_alarm_item_string[SET_ALARM_MAX] =
{
    STRING_SET_ALARMSTATE,          //闹钟状态
    STRING_SET_ALARMTIME,           //闹钟时间
    STRING_SET_ALARM_REPEATTIME,    //重复次数
    STRING_SET_ALARM_RING,          //闹铃
    STRING_SET_ALARM_VOL,           //闹铃音量
    STRING_SET_ALARM_RUNTIME,       //闹铃持续时间
    STRING_SET_ALARM_DELAY          //闹铃贪睡时间
};
const static __s32 setting_alarm_item_titile_string[SET_ALARM_MAX] =
{
    STRING_SET_ALARMSTATE_EX1,          //闹钟状态
    STRING_SET_ALARMTIME_EX1,           //闹钟时间
    STRING_SET_ALARM_REPEATTIME_EX1,    //重复次数
    STRING_SET_ALARM_RING_EX1,          //闹铃
    STRING_SET_ALARM_VOL_EX1,           //闹铃音量
    STRING_SET_ALARM_RUNTIME_EX1,       //闹铃持续时间
    STRING_SET_ALARM_DELAY_EX1          //闹铃贪睡时间
};
const static __s32 setting_alarm_item_describe_string[SET_ALARM_MAX] =
{
    STRING_SET_ALARMSTATE_EX2,          //闹钟状态
    0,                                  //闹钟时间
    STRING_SET_ALARM_REPEATTIME_EX2,    //重复次数
    STRING_SET_ALARM_RING_EX2,          //闹铃
    STRING_SET_ALARM_VOL_EX2,           //闹铃音量
    STRING_SET_ALARM_RUNTIME_EX2,       //闹铃持续时间
    STRING_SET_ALARM_DELAY_EX2          //闹铃贪睡时间
};

static __s32 setting_alarm_item_paint(__lbar_draw_para_t *draw_param);
static __s32 setting_alarm_paint_item_detail(setting_alarm_attr_t *alarm_attr);

__s32 setting_alarm_long_string_init(setting_alarm_attr_t *alarm_attr)
{
    if (alarm_attr->long_str_handle == NULL)
    {
        __show_info_move_t show_info;
        setting_uipara_t *uipara ;
        uipara = (setting_uipara_t *)setting_get_uipara();
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = alarm_attr->layer;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = NULL;
        show_info.string = "";
        show_info.fontColor = GUI_BLUE;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = NULL;
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;
        alarm_attr->long_str_handle = GUI_LongStringCreate(&show_info);

        if (alarm_attr->long_str_handle == NULL)
        {
            __wrn("create long string failed!\n");
        }
    }
    else
    {
        __msg("long string scroll has already started!\n");
    }

    return EPDK_OK;
}

__s32 setting_alarm_long_string_uninit(setting_alarm_attr_t *alarm_attr)
{
    if (alarm_attr->long_str_handle != NULL)
    {
        GUI_LongStringDelete(alarm_attr->long_str_handle);
        alarm_attr->long_str_handle = NULL;
    }

    return EPDK_OK;
}

__s32 setting_alarm_long_string_roll(setting_alarm_attr_t *alarm_attr, char *string, GUI_RECT *txt_rect)
{
    __show_info_move_t show_info;
    __epdk_charset_enm_e encode = EPDK_CHARSET_ENM_UTF8;
    __u32 font_size = 0;

    if (alarm_attr->long_str_handle != NULL)
    {
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        font_size = GUI_GetFontDistY() ;
        show_info.hlayer = alarm_attr->layer;
        show_info.region.x = txt_rect->x0;
        show_info.region.y = txt_rect->y0;
        show_info.region.width = txt_rect->x1 - txt_rect->x0  ;
        show_info.region.height = txt_rect->y1 - txt_rect->y0  ;
        show_info.alpha_en = EPDK_FALSE;
        show_info.pFont = alarm_attr->font;
        show_info.string = string;
        show_info.encode_id = encode;
        show_info.fontColor = GUI_BLUE;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        show_info.bmp = theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_LONG_STRING_BG]);
        show_info.bmp_pos.x = 0 ;   //相对于长字符串显示区域
        show_info.bmp_pos.y = 0 ;
        GUI_LongStringStop(alarm_attr->long_str_handle);
        GUI_LongStringReset(alarm_attr->long_str_handle, &show_info);
        GUI_LongStringStart(alarm_attr->long_str_handle);
    }

    return EPDK_OK;
}

static __s32 setting_alarm_long_string_stop_roll(setting_alarm_attr_t *alarm_attr)
{
    if (alarm_attr->long_str_handle != NULL)
    {
        return GUI_LongStringStop(alarm_attr->long_str_handle);
    }

    return EPDK_FAIL;
}
static __s32 _setting_alarm_res_init(__gui_msg_t *msg)
{
    setting_alarm_attr_t *alarm_attr;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

#if 0
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_BIG_SETTING]  = theme_open(ID_SETTING_WINDOW_BG_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_BG]           = theme_open(ID_SETTING_SET_BSYSSE_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]       = theme_open(ID_SETTING_SET_ITEM_F_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_UNF]     = theme_open(ID_SETTING_SET_ITEM_UFCS_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_F]        = theme_open(ID_SETTING_RETURN_FCS_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_UNF]      = theme_open(ID_SETTING_RETURN_UFCS_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_LEFT]         = theme_open(ID_SETTING_SET_BUTT_L_F_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RIGHT]        = theme_open(ID_SETTING_SET_BUTT_R_F_BMP);
    alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_LONG_STRING_BG] = theme_open(ID_SETTING_LONG_STR_BG_BMP);
#endif
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]          = theme_open(ID_SETTING_NEW_ALARM_FLAG_UF_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_F_BMP]           = theme_open(ID_SETTING_NEW_ALARM_FLAG_F_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_FCS_BMP]       = theme_open(ID_SETTING_NEW_ITEM_L_FCS_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]      = theme_open(ID_SETTING_NEW_ITEM_L_UFCS_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_SCROLL_BG_BMP]        = theme_open(ID_SETTING_NEW_SCROLL_BG_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_SCROLL_BLOCK_BMP]     = theme_open(ID_SETTING_NEW_SCROLL_BLOCK_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_LEFT_BTN_UF_BMP]      = theme_open(ID_SETTING_NEW_SET_BUTT_L_UF_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_RIGHT_BTN_UF_BMP]     = theme_open(ID_SETTING_NEW_SET_BUTT_R_UF_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_LEFT_BTN_F_BMP]       = theme_open(ID_SETTING_NEW_SET_BUTT_L_F_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_RIGHT_BTN_F_BMP]      = theme_open(ID_SETTING_NEW_SET_BUTT_R_F_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_R_FCS_BMP]       = theme_open(ID_SETTING_NEW_ITEM_R_FCS_BMP);
    alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_R_UFCS_BMP]      = theme_open(ID_SETTING_NEW_ITEM_R_UFCS_BMP);
    return EPDK_OK;
}

static __s32 _setting_alarm_res_uninit(__gui_msg_t *msg)
{
    __s32 i = 0;
    setting_alarm_attr_t *alarm_attr;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    for (i = 0; i < SETTING_ALARM_BMP_MAX; i++)
    {
        theme_close(alarm_attr->h_bmp_setting_alarm[i]);
        alarm_attr->h_bmp_setting_alarm[i] = NULL;
    }

    return EPDK_OK;
}

static __s32 setting_alarm_paint(__gui_msg_t *msg, __s32 index, setting_alarm_view    view)
{
    __s16                   i = 0;
    __s32                   x, y;
    //GUI_RECT              gui_rect;
    setting_alarm_attr_t   *alarm_attr;
    char                    str[GUI_NAME_MAX], numStr[2] = {0};
    setting_uipara_t       *uipara = NULL;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    __log("======GUI_MSG_CREATE larm_attr->layer=%d\n", alarm_attr->layer);

    if (GUI_LyrWinGetSta(alarm_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_LyrWinSel(alarm_attr->layer);
    GUI_SetFont(alarm_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0xccffffff); //80%的透明度
    GUI_SetColor(SETTING_UNFOCUS_COLOR);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_SCROLL_BG_BMP]), 300, 0);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_SCROLL_BLOCK_BMP]), 300, 0);

    if (SETTING_ALARM_SELECT_VIEW == view)
    {
        switch (index)
        {
            case ID_PAINTING_ALARM_F1:
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_FCS_BMP]), uipara->item_alarm1_setting.x, uipara->item_alarm1_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm2_setting.x, uipara->item_alarm2_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm3_setting.x, uipara->item_alarm3_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), 0, uipara->item.h * 3);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_F_BMP]), 21, 30);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, uipara->item.h + 30);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, uipara->item.h * 2 + 30);
                break;

            case ID_PAINTING_ALARM_F2:
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm1_setting.x, uipara->item_alarm1_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_FCS_BMP]), uipara->item_alarm2_setting.x, uipara->item_alarm2_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm3_setting.x, uipara->item_alarm3_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), 0, uipara->item.h * 3);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, 30);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_F_BMP]), 21, uipara->item.h + 30);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, uipara->item.h * 2 + 30);
                break;

            case ID_PAINTING_ALARM_F3:
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm1_setting.x, uipara->item_alarm1_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm2_setting.x, uipara->item_alarm2_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_FCS_BMP]), uipara->item_alarm3_setting.x, uipara->item_alarm3_setting.y);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), 0, uipara->item.h * 3);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, 30);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, uipara->item.h + 30);
                GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_F_BMP]), 21, uipara->item.h * 2 + 30);
                break;
        }
    }
    else if (SETTING_ALARM_SETTING_VIEW == view)
    {
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm1_setting.x, uipara->item_alarm1_setting.y);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm2_setting.x, uipara->item_alarm2_setting.y);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), uipara->item_alarm3_setting.x, uipara->item_alarm3_setting.y);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_L_UFCS_BMP]), 0, uipara->item.h * 3);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, 30);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, uipara->item.h + 30);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_FLAG_UF_BMP]), 21, uipara->item.h * 2 + 30);
    }

#if 1

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        //x = 0;
        //y = uipara->item.y+uipara->item.h*i;

        //GUI_SetColor(SETTING_UNFOCUS_COLOR);

        //if( i == alarm_attr->focus_id )
        //{
        //GUI_SetColor(SETTING_FOCUS_COLOR);
        //GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]), x, y);
        //GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]), x, y+45);
        //}
        //if (i < REG_ALARM_NUM)
        {
            dsk_langres_get_menu_text(STRING_SET_ALARM, str, GUI_NAME_MAX);
            numStr[0] = i + '0' + 1;
            numStr[1] = '\0';
            eLIBs_strcat(str, numStr);

            if (i == index)
            {
                GUI_SetColor(SETTING_FOCUS_COLOR);
            }
            else
            {
                GUI_SetColor(SETTING_UNFOCUS_COLOR);
            }

            GUI_DispStringAt(str, 21 + 46 + 10, uipara->item.h * i + 30 + 10);
        }
    }

#endif
#if 0
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_BIG_SETTING]), 0, 0);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_BG]), 1, 0);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_UNF]), uipara->return_bmp.x, uipara->return_bmp.y);
    dsk_langres_get_menu_text(STRING_SET_ALARM_SET, str, GUI_NAME_MAX);
    GUI_DispStringAt(str, uipara->txt_pos.x, uipara->txt_pos.y);

    for (i = 0; i < REG_ALARM_NUM; i++)
    {
        x = uipara->item.x;
        y = uipara->item.y + uipara->item.h * i;
        GUI_SetColor(SETTING_UNFOCUS_COLOR);

        //GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_UNF]), x, y);
        if (i == alarm_attr->focus_id)
        {
            GUI_SetColor(SETTING_FOCUS_COLOR);
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]), x, y);
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]), x, y + 45);
        }

        if (i < REG_ALARM_NUM)
        {
            gui_rect.x0 = uipara->item_title_pos.x + x;
            gui_rect.y0 = uipara->item_title_pos.y + y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            dsk_langres_get_menu_text(STRING_SET_ALARM, str, GUI_NAME_MAX);
            numStr[0] = i + '0' + 1;
            numStr[1] = '\0';
            eLIBs_strcat(str, numStr);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);

            if (alarm_attr->reg_sys->alarm[i].alarm_status)
            {
                dsk_langres_get_menu_text(STRING_SET_KEYTONE_ON_CON, str, GUI_NAME_MAX);
                GUI_DispStringAt(str, x + uipara->right_button.x, y + uipara->right_button.y);
            }
            else
            {
                dsk_langres_get_menu_text(STRING_SET_KEYTONE_OFF_CON, str, GUI_NAME_MAX);
                GUI_DispStringAt(str, x + uipara->right_button.x, y + uipara->right_button.y);
            }
        }
    }

#endif
    return EPDK_OK;
}

static __s32 setting_alarm_change_paint(__gui_msg_t *msg)
{
    __s16                   i = 0;
    __s32                   x, y;
    GUI_RECT                gui_rect;
    setting_alarm_attr_t   *alarm_attr;
    char                    str[GUI_NAME_MAX], numstr[2] = {0};
    setting_uipara_t       *uipara = NULL;
    GUI_MEMDEV_Handle       draw_mem;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(alarm_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_LyrWinSel(alarm_attr->layer);
    draw_mem = GUI_MEMDEV_Create(1, 0, uipara->string_detail_pos_de.w,
                                 uipara->string_detail_pos_ex.h + uipara->string_detail_pos_de.h + 128);
    GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(alarm_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0xccffffff); //80%的透明度
    GUI_SetColor(SETTING_UNFOCUS_COLOR);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_BG]), 1, 0);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_UNF]), uipara->return_bmp.x, uipara->return_bmp.y);
    dsk_langres_get_menu_text(STRING_SET_ALARM_SET, str, GUI_NAME_MAX);
    GUI_SetColor(SETTING_UNFOCUS_COLOR);
    GUI_DispStringAt(str, uipara->txt_pos.x, uipara->txt_pos.y);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;

    for (i = 0; i < 5; i++)
    {
        x = uipara->item.x;
        y = uipara->item.y + uipara->item.h * i;
        GUI_SetColor(SETTING_UNFOCUS_COLOR);
        GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_UNF]), x, y);

        if (i == alarm_attr->focus_id)
        {
            GUI_SetColor(SETTING_FOCUS_COLOR);
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]), x, y);
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]), x, y + 45);
        }

        if (i < REG_ALARM_NUM)
        {
            gui_rect.x0 = uipara->item_title_pos.x + x;
            gui_rect.y0 = uipara->item_title_pos.y + y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            dsk_langres_get_menu_text(STRING_SET_ALARM, str, GUI_NAME_MAX);
            numstr[0] = i + '0' + 1;
            numstr[1] = '\0';
            eLIBs_strcat(str, numstr);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);

            if (alarm_attr->reg_sys->alarm[i].alarm_status)
            {
                dsk_langres_get_menu_text(STRING_SET_KEYTONE_ON_CON, str, GUI_NAME_MAX);
                GUI_DispStringAt(str, x + uipara->right_button.x, y + uipara->right_button.y);
            }
            else
            {
                dsk_langres_get_menu_text(STRING_SET_KEYTONE_OFF_CON, str, GUI_NAME_MAX);
                GUI_DispStringAt(str, x + uipara->right_button.x, y + uipara->right_button.y);
            }
        }
    }

    return EPDK_OK;
}

static __s32 setting_alarm_listbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config;
    setting_uipara_t *uipara = NULL;
    setting_alarm_attr_t *alarm_attr;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    config.alpha_status     = EPDK_FALSE;
    config.bk_color         = 0;
    config.focus_id         = 0;
    config.item_cnt         = listbar_num ;
    config.item_height      = uipara->item.h;
    config.item_width       = uipara->item.w;
    config.list_rect.x      = uipara->item.x;
    config.list_rect.y      = uipara->item.y;
    config.list_rect.width  = uipara->item.w;
    config.list_rect.height = uipara->lyr_scn.h - 56;
    config.start_id         = 0;
    config.list_attr        = (void *)alarm_attr;
    alarm_attr->h_listbar = LISTBAR_Create(msg->h_deswin);

    if (alarm_attr->h_listbar == NULL)
    {
        __msg("list bar create fail\n");
        return EPDK_FAIL;
    }

    LISTBAR_Config(alarm_attr->h_listbar, setting_alarm_item_paint, &config);
    //LISTBAR_SuspendPage(alarm_attr->h_listbar);
    LISTBAR_ShowPage(alarm_attr->h_listbar);
    __log("======LISTBAR_ShowPage=======power_setting \n");
    return EPDK_OK;
}

static __s32 setting_alarm_item_content_paint(setting_alarm_attr_t *alarm_attr, __s32 index, GUI_RECT *rect)
{
    char str[GUI_NAME_MAX] = {0};

    switch (index)
    {
        case 0:         //闹钟状态
        {
            if (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_status)
            {
                dsk_langres_get_menu_text(STRING_SET_KEYTONE_ON_CON, str, GUI_NAME_MAX);
            }
            else
            {
                dsk_langres_get_menu_text(STRING_SET_KEYTONE_OFF_CON, str, GUI_NAME_MAX);
            }

            GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case 1:         //闹钟时间
        {
            eLIBs_sprintf(str, "%02d:%02d", alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_time.hour
                          , alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_time.minute);
            GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case 2:         //重复次数
        {
            dsk_langres_get_menu_text(STRING_SET_EVERYDAY + alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_repeat, str, GUI_NAME_MAX);
            GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case 3:         //闹铃
        {
            char *nameMusic = NULL;
            nameMusic = eLIBs_strchrlast(alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_music_path, '\\') + 1 ;
            __msg("nameMusic=%s\n", nameMusic);

            if (nameMusic)
            {
                GUI_DispStringInRect(nameMusic, rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            }
            else
            {
                GUI_DispStringInRect("default", rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
            }
        }
        break;

        case 4:         //闹铃音量
        {
            eLIBs_sprintf(str, "%d", alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_volume);
            GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case 5:         //闹铃持续时间
        {
            dsk_langres_get_menu_text(STRING_SET_20SECOND + alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_runtime, str, GUI_NAME_MAX);
            GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case 6:         //闹铃贪睡时间
        {
            dsk_langres_get_menu_text(STRING_SET_5MINUTE + alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_delay, str, GUI_NAME_MAX);
            GUI_DispStringInRect(str, rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

static __s32 setting_alarm_item_paint(__lbar_draw_para_t *draw_param)
{
    setting_alarm_attr_t   *alarm_attr = NULL;
    setting_uipara_t       *uipara     = NULL;
    GUI_RECT                gui_rect;
    char                    str[GUI_NAME_MAX];
    //GUI_MEMDEV_Handle     draw_mem;
    alarm_attr = (setting_alarm_attr_t *)draw_param->attr;

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_SetFont(alarm_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);

    //draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y,draw_param->rect.width,draw_param->rect.height);
    //GUI_MEMDEV_Select(draw_mem);

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            __msg("LBAR_MODE_NORMAL \n");
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_R_UFCS_BMP]), draw_param->rect.x, draw_param->rect.y);

            if (3 !=  draw_param->index && 1 !=  draw_param->index)  //第1,3个item
            {
                if (2 ==  draw_param->index)
                {
                    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_LEFT_BTN_UF_BMP]), draw_param->rect.x + uipara->left_button.x - 48, draw_param->rect.y + uipara->left_button.y);
                }
                else
                {
                    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_LEFT_BTN_UF_BMP]), draw_param->rect.x + uipara->left_button.x, draw_param->rect.y + uipara->left_button.y);
                }
            }

            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_RIGHT_BTN_UF_BMP]), draw_param->rect.x + uipara->right_button.x, draw_param->rect.y + uipara->right_button.y);
            GUI_SetColor(SETTING_UNFOCUS_COLOR);
            gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            dsk_langres_get_menu_text(setting_alarm_item_string[draw_param->index], str, GUI_NAME_MAX);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);

            if (3 ==  draw_param->index)     // 第1个item ，
            {
                gui_rect.x0 = uipara->long_string_area.x + draw_param->rect.x;
                gui_rect.y0 = uipara->long_string_area.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->long_string_area.w - 1 ;
                gui_rect.y1 = gui_rect.y0 + uipara->long_string_area.h - 1 ;
            }
            else if (2 ==  draw_param->index)
            {
                gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x - 48;
                gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w + 48;
                gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            }
            else
            {
                gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
                gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
                gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            }

            setting_alarm_item_content_paint(alarm_attr, draw_param->index, &gui_rect);
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            __msg("LBAR_MODE_FOCUS \n");
            //alarm_attr->focus_id = draw_param->index;//////////////////
            //setting_alarm_paint_item_detail( alarm_attr );
            //GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_LONG_STRING_BG]),uipara->item_content_pos.x+draw_param->rect.x-48,8+draw_param->rect.y);
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_ITEM_R_FCS_BMP]), draw_param->rect.x, draw_param->rect.y);

            //GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_ITEM_F]),draw_param->rect.x,draw_param->rect.y+45);
            if (3 !=  draw_param->index && 1 !=  draw_param->index)  //第1,3个item
            {
                if (2 ==  draw_param->index)
                {
                    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_LEFT_BTN_F_BMP]), draw_param->rect.x + uipara->left_button.x - 48, draw_param->rect.y + uipara->left_button.y);
                }
                else
                {
                    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_LEFT_BTN_F_BMP]), draw_param->rect.x + uipara->left_button.x, draw_param->rect.y + uipara->left_button.y);
                }
            }

            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[NEW_ALARM_RIGHT_BTN_F_BMP]), draw_param->rect.x + uipara->right_button.x, draw_param->rect.y + uipara->right_button.y);
            GUI_SetColor(SETTING_FOCUS_COLOR);
            gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            dsk_langres_get_menu_text(setting_alarm_item_string[draw_param->index], str, GUI_NAME_MAX);
            GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;

            if (3 ==  draw_param->index)     // 第三个item ，即闹钟铃声，
            {
                gui_rect.x0 = uipara->long_string_area.x + draw_param->rect.x;
                gui_rect.y0 = uipara->long_string_area.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->long_string_area.w - 1 ;
                gui_rect.y1 = gui_rect.y0 + uipara->long_string_area.h - 1 ;
            }
            else if (2 ==  draw_param->index)
            {
                gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x - 48;
                gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w + 48;
                gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            }
            else
            {
                gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
                gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
                gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            }

            setting_alarm_item_content_paint(alarm_attr, draw_param->index, &gui_rect);

            if (3 ==  draw_param->index)     // 第三个item ，即闹钟铃声，
            {
                char *nameMusic = NULL;
                nameMusic = eLIBs_strchrlast(alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_music_path, '\\') + 1 ;
                setting_alarm_long_string_roll(alarm_attr, nameMusic, &gui_rect) ;
            }
        }
        break;

        default:
            break;
    }

    //GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    //GUI_MEMDEV_CopyToLCD(draw_mem);
    //GUI_MEMDEV_Select( NULL );
    //GUI_MEMDEV_Delete(draw_mem);
    //draw_mem = NULL;
    return EPDK_OK;
}

static __s32 setting_alarm_paint_item_detail(setting_alarm_attr_t *alarm_attr)
{
#define ALARM_DETAIL_LEN    64
    char                    str[ALARM_DETAIL_LEN], str1[GUI_NAME_MAX], numStr[2] = {0};
    RECT                    rect;
    GUI_RECT                gui_rect;
    setting_uipara_t       *uipara = NULL;
    GUI_MEMDEV_Handle       draw_mem;
    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_LyrWinSel(alarm_attr->layer);
    draw_mem = GUI_MEMDEV_Create(1, 0, uipara->string_detail_pos_de.w,
                                 uipara->string_detail_pos_ex.h + uipara->string_detail_pos_de.h + 128);
    GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(alarm_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_BG]), 1, 0);
    GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_UNF]), uipara->return_bmp.x, uipara->return_bmp.y);
    dsk_langres_get_menu_text(STRING_SET_TITLE, str1, GUI_NAME_MAX);
    dsk_langres_get_menu_text(STRING_SET_ALARM, str, GUI_NAME_MAX);
    numStr[0] = alarm_attr->alarm_index + '0' + 1;
    numStr[1] = '\0';
    eLIBs_strcat(str, numStr);
    eLIBs_strcat(str, str1);
    GUI_SetColor(SETTING_UNFOCUS_COLOR);
    GUI_DispStringAt(str, uipara->txt_pos.x, uipara->txt_pos.y);
    GUI_SetColor(SETTING_FOCUS_COLOR);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    dsk_langres_get_menu_text(setting_alarm_item_titile_string[alarm_attr->focus_id], str, ALARM_DETAIL_LEN);
    GUI_DispStringInRect(str, &gui_rect, GUI_TA_LEFT);
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h;
    GUI_ClearRectEx(&gui_rect);
    dsk_langres_get_menu_text(setting_alarm_item_describe_string[alarm_attr->focus_id], str, ALARM_DETAIL_LEN);
    GUI_DispStringInRectWrap(str, &gui_rect, GUI_TA_LEFT, GUI_WRAPMODE_WORD);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}

__s32 setting_alarm_on_left_button(__gui_msg_t *msg)
{
    setting_alarm_attr_t *alarm_attr;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    switch (alarm_attr->focus_id)
    {
        case 0:         //闹钟状态
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_status = !alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_status;
        }
        break;

        case 1:         //闹钟时间
        {
            ;
        }
        break;

        case 2:         //重复次数
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_repeat = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_repeat + REG_ALARM_REPEAT_MAX - 1) % REG_ALARM_REPEAT_MAX;
        }
        break;

        case 3:         //闹铃
        {
        }
        break;

        case 4:         //闹铃音量
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_volume = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_volume + (AUDIO_DEVICE_VOLUME_MAX + 1) - 1) % (AUDIO_DEVICE_VOLUME_MAX + 1);
        }
        break;

        case 5:         //闹铃持续时间
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_runtime = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_runtime + REG_ALARM_RUNTIME_MAX - 1) % REG_ALARM_RUNTIME_MAX;
        }
        break;

        case 6:         //闹铃贪睡时间
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_delay = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_delay + REG_ALARM_DELAY_MAX - 1) % REG_ALARM_DELAY_MAX;
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

__s32 setting_alarm_on_right_button(__gui_msg_t *msg)
{
    setting_alarm_attr_t *alarm_attr;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    switch (alarm_attr->focus_id)
    {
        case 0:         //闹钟状态
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_status = !alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_status;
        }
        break;

        case 1:         //闹钟时间
        {
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_CLOCK_WIN, (unsigned long)&alarm_attr->reg_sys->alarm[alarm_attr->alarm_index], 0);
        }
        break;

        case 2:         //重复次数
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_repeat = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_repeat + 1) % REG_ALARM_REPEAT_MAX;
        }
        break;

        case 3:         //闹铃
        {
            setting_alarm_long_string_stop_roll(alarm_attr);
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ALARM_SELECT_SONG_WIN, 0, 0);
        }
        break;

        case 4:         //闹铃音量
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_volume = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_volume + 1) % (AUDIO_DEVICE_VOLUME_MAX + 1);
        }
        break;

        case 5:         //闹铃持续时间
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_runtime = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_runtime + 1) % REG_ALARM_RUNTIME_MAX;
        }
        break;

        case 6:         //闹铃贪睡时间
        {
            alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_delay = (alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_delay + 1) % REG_ALARM_DELAY_MAX;
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

static __s32 _setting_alarm_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    setting_alarm_attr_t *alarm_attr;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        case GUI_MSG_KEY_PREV:
        {
            __log("======_setting_alarm=======GUI_MSG_KEY_UP \n");

            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                setting_alarm_long_string_stop_roll(alarm_attr);

                //alarm_attr->old_focus = alarm_attr->focus_id;
                if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    alarm_attr->focus_id = 0;
                    alarm_attr->alarm_index = (alarm_attr->alarm_index + REG_ALARM_NUM - 1) % REG_ALARM_NUM;
                    //alarm_attr->alarm_index = alarm_attr->focus_id;
                    setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view);
                    //..LISTBAR_SetFocusItem(alarm_attr->h_listbar, 0);
                    //LISTBAR_ResumePage(alarm_attr->h_listbar);
                    LISTBAR_ShowPage(alarm_attr->h_listbar);
                    LISTBAR_LostFocus(alarm_attr->h_listbar);
                    setting_alarm_long_string_init(alarm_attr); // 切换到闹钟设置窗口时，启动长文件名滚动
                    //setting_alarm_change_paint(msg);
                }
                else if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
                {
                    if (alarm_attr->focus_id > 0)
                    {
                        alarm_attr->focus_id--;
                    }
                    else
                    {
                        alarm_attr->focus_id = listbar_num - 1;
                    }

                    __log("======key_setting_alarm=======focus_id=%d \n", alarm_attr->focus_id);
                    LISTBAR_PrevItem(alarm_attr->h_listbar);
                }
            }

            break;
        }

        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_NEXT:
        {
            __log("======_setting_alarm=======GUI_MSG_KEY_DOWN \n");

            if ((msg->dwAddData2 == KEY_DOWN_ACTION)) //|| (msg->dwAddData2 == KEY_REPEAT_ACTION)
            {
                setting_alarm_long_string_stop_roll(alarm_attr);
#if 1

                // alarm_attr->old_focus = alarm_attr->focus_id;
                if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    alarm_attr->focus_id = 0;
                    alarm_attr->alarm_index = (alarm_attr->alarm_index + 1) % REG_ALARM_NUM;
                    // alarm_attr->alarm_index = alarm_attr->focus_id;
                    setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view);
                    //..LISTBAR_SetFocusItem(alarm_attr->h_listbar, 0);
                    LISTBAR_ShowPage(alarm_attr->h_listbar);
                    LISTBAR_LostFocus(alarm_attr->h_listbar);
                    setting_alarm_long_string_init(alarm_attr); // 切换到闹钟设置窗口时，启动长文件名滚动
                    __log("======GUI_MSG_KEY_RIGHT======= alarm_attr->alarm_index=%d\n", alarm_attr->alarm_index);
                }
                else if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
                {
                    if (alarm_attr->focus_id < listbar_num - 1)
                    {
                        alarm_attr->focus_id++;
                    }
                    else
                    {
                        alarm_attr->focus_id = 0;
                    }

                    __log("======key_setting_alarm=======focus_id=%d \n", alarm_attr->focus_id);
                    LISTBAR_NextItem(alarm_attr->h_listbar);
                }

#endif
            }

            break;
        }

        case GUI_MSG_KEY_RIGHT:
            //case GUI_MSG_KEY_LONGNEXT:
            //case GUI_MSG_KEY_LONGRIGHT:
        {
            __log("======_setting_alarm=======GUI_MSG_KEY_RIGHT \n");

            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                setting_alarm_long_string_stop_roll(alarm_attr);

                if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    alarm_attr->view = SETTING_ALARM_SETTING_VIEW;
                    setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view);
                    setting_alarm_long_string_init(alarm_attr);
                    LISTBAR_SetFocusItem(alarm_attr->h_listbar, 0);
                }
                else
                {
                    alarm_attr->view = SETTING_ALARM_SELECT_VIEW;
                    LISTBAR_LostFocus(alarm_attr->h_listbar);
                    setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view);
                }

#if 0
                alarm_attr->old_focus = alarm_attr->focus_id;

                if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    __log("======GUI_MSG_KEY_RIGHT======= alarm_attr->focus_id=%d\n", alarm_attr->focus_id);
                    alarm_attr->focus_id = (alarm_attr->focus_id + REG_ALARM_NUM - 1) % REG_ALARM_NUM;
                    alarm_attr->alarm_index = alarm_attr->focus_id;
                    setting_alarm_paint(msg, alarm_attr->alarm_index);
                    LISTBAR_ShowPage(alarm_attr->h_listbar);
                    setting_alarm_long_string_init(alarm_attr); // 切换到闹钟设置窗口时，启动长文件名滚动
                }
                else if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
                {
                    LISTBAR_PrevItem(alarm_attr->h_listbar);
                }

#endif
            }

            break;
        }

        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_LONGPLAY_PAUSE:
            //case GUI_MSG_KEY_LONGLEFT:
        {
            __log("======_setting_alarm=======GUI_MSG_KEY_LEFT \n");

            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                setting_alarm_long_string_stop_roll(alarm_attr);

                if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    alarm_attr->view = SETTING_ALARM_SETTING_VIEW;
                    setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view);
                    setting_alarm_long_string_init(alarm_attr);
                    LISTBAR_SetFocusItem(alarm_attr->h_listbar, 0);
                }
                else
                {
                    alarm_attr->view = SETTING_ALARM_SELECT_VIEW;
                    LISTBAR_LostFocus(alarm_attr->h_listbar);
                    setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view);
                }

#if 0
                alarm_attr->old_focus = alarm_attr->focus_id;

                if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    alarm_attr->focus_id = (alarm_attr->focus_id + 1) % REG_ALARM_NUM;
                    alarm_attr->alarm_index = alarm_attr->focus_id;
                    setting_alarm_paint(msg, alarm_attr->alarm_index);
                    //setting_alarm_change_paint(msg);
                }
                else if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
                {
                    LISTBAR_PrevItem(alarm_attr->h_listbar);
                }

#endif
            }

            break;
        }

        case GUI_MSG_KEY_ENTER:
        case GUI_MSG_KEY_PLAY_PAUSE:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                __log("======key_setting_alarm=======focus_id=%d \n", alarm_attr->focus_id);
                setting_alarm_long_string_stop_roll(alarm_attr);
                setting_alarm_on_right_button(msg);
                LISTBAR_UpdatePage(alarm_attr->h_listbar);
#if 0

                if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
                {
                    setting_alarm_on_right_button(msg);
                    LISTBAR_UpdatePage(alarm_attr->h_listbar);
                }
                else if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    alarm_attr->view = SETTING_ALARM_SETTING_VIEW;
                    alarm_attr->alarm_index = alarm_attr->focus_id;
                    LISTBAR_SetFocusItem(alarm_attr->h_listbar, 0);
                    LISTBAR_ResumePage(alarm_attr->h_listbar);
                    LISTBAR_ShowPage(alarm_attr->h_listbar);
                }

#endif
            }

            break;
        }

        // case GUI_MSG_KEY_MENU:
        // case GUI_MSG_KEY_LONGMENU:
        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_LONGESCAPE:
        case GUI_MSG_KEY_MODE:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                __gui_msg_t mymsg;
                setting_alarm_long_string_stop_roll(alarm_attr);
                mymsg.id = GUI_MSG_CLOSE;
                mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                GUI_SendMessage(&mymsg);
#if 0

                if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
                {
                    alarm_attr->view = SETTING_ALARM_SELECT_VIEW;
                    LISTBAR_SuspendPage(alarm_attr->h_listbar);
                    alarm_attr->focus_id = alarm_attr->alarm_index;
                    setting_alarm_change_paint(msg);
                    * /
                }
                else if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
                {
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
                }

#endif
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}
static __s32 _setting_alarm_listbar_touch_proc(__gui_msg_t *msg)
{
    static __s32 is_touch_down = 0;
    static __s32 is_touch_moving = 0;
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 ret = EPDK_OK;
    setting_alarm_attr_t *alarm_attr;
    setting_uipara_t *uipara;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        case GUI_MSG_TOUCH_LONGDOWN:
        {
            RECT rect;
            setting_alarm_long_string_stop_roll(alarm_attr);
            alarm_attr->focus_id = LISTBAR_GetFocusItem(alarm_attr->h_listbar);
            LISTBAR_GetItemRect(alarm_attr->h_listbar, alarm_attr->focus_id, &rect);

            if (x > rect.x && (x < rect.x + rect.width) && (y > rect.y) && (y < rect.y + rect.height))
            {
                //if( (alarm_attr->focus_id != 2 && x >= 338 && x <= 383) ||
                //(alarm_attr->focus_id == 2 && x >= 338-48 && x <= 383-48) )
                if ((alarm_attr->focus_id != 2 && x >= 509 - 30 && x <= 509 + 46 + 10) ||
                    (alarm_attr->focus_id == 2 && x >= 509 - 30 - 48 && x <= 509 + 46 + 10 - 48))
                {
                    setting_alarm_on_left_button(msg);
                    LISTBAR_SetFocusItem(alarm_attr->h_listbar, alarm_attr->focus_id);
                }
                //else if( x >= 426 && x <= 474 )
                else if (x >= 724 - 30 && x <= 724 + 46)
                {
                    setting_alarm_on_right_button(msg);
                    LISTBAR_SetFocusItem(alarm_attr->h_listbar, alarm_attr->focus_id);
                }
                else
                {
                    GUI_WinSetCaptureWin(msg->h_deswin);
                    ret = LISTBAR_TouchDown(alarm_attr->h_listbar, x, y, direct, speed);
                    is_touch_down = 1;
                }
            }
            else
            {
                GUI_WinSetCaptureWin(msg->h_deswin);
                ret = LISTBAR_TouchDown(alarm_attr->h_listbar, x, y, direct, speed);
                is_touch_down = 1;
            }

            break;
        }

        case GUI_MSG_TOUCH_MOVE:
        {
            if (is_touch_down == 1)
            {
                setting_alarm_long_string_stop_roll(alarm_attr);
                LISTBAR_TouchMove(alarm_attr->h_listbar, x, y, direct, speed);
                is_touch_moving = 1;
            }

            break;
        }

        case GUI_MSG_TOUCH_UP:
        {
            is_touch_down   = 0;
            is_touch_moving = 0;

            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
                LISTBAR_TouchUp(alarm_attr->h_listbar, x, y, direct, speed);
            }
            else
            {
                LISTBAR_UpdatePage(alarm_attr->h_listbar);
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

static __s32 _setting_alarm_touch_ItemIndex(__gui_msg_t *msg)
{
    __s32 pos_x, pos_y;
    __s32 item_index, tmp_index_y;
    setting_uipara_t *uipara = NULL;
    uipara = (setting_uipara_t *)setting_get_uipara();
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);

    if (((pos_x >= SET_ITEM_START_X) && (pos_x <= SET_ITEM_START_X + SET_ITEM_W))
        && ((pos_y >= SET_ITEM_START_Y) && (pos_y <= SET_ITEM_START_Y + SET_ITEM_H * REG_ALARM_NUM)))
    {
        tmp_index_y = (pos_y - SET_ITEM_START_Y)  / SET_ITEM_H;
        item_index = tmp_index_y;
        return item_index;
    }
    else
    {
        return EPDK_FAIL;   //不在item区域
    }
}

static __s32  setting_alarm_touch_proc(__gui_msg_t *msg)
{
    setting_alarm_attr_t *alarm_attr;
    __s32 pos_x, pos_y;
    setting_uipara_t *uipara;
    alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (alarm_attr == NULL)
    {
        return EPDK_FAIL;
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);
#if 0

    if ((pos_x > uipara->return_bmp.x) && (pos_x < uipara->return_bmp.x + uipara->return_bmp.w)
        && (pos_y > uipara->return_bmp.y) && (pos_y < uipara->return_bmp.y + uipara->return_bmp.h))
    {
        if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
        {
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_UNF]), uipara->return_bmp.x, uipara->return_bmp.y);

            if (alarm_attr->view == SETTING_ALARM_SELECT_VIEW)
            {
                main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
            }
            else if (alarm_attr->view == SETTING_ALARM_SETTING_VIEW)
            {
                setting_alarm_long_string_uninit(alarm_attr);   //切换到闹钟选择界面时，停掉长字符串滚动
                alarm_attr->view = SETTING_ALARM_SELECT_VIEW;
                LISTBAR_SuspendPage(alarm_attr->h_listbar);
                alarm_attr->focus_id = alarm_attr->alarm_index;
                setting_alarm_change_paint(msg);
            }
        }
        else if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
        {
            GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_F]), uipara->return_bmp.x, uipara->return_bmp.y);
        }
    }
    else
#endif
    {
        //GUI_BMP_Draw(theme_hdl2buf(alarm_attr->h_bmp_setting_alarm[SETTING_ALARM_RET_UNF]),uipara->return_bmp.x,uipara->return_bmp.y);
        //if( alarm_attr->view == SETTING_ALARM_SELECT_VIEW )
        //{
        alarm_attr->alarm_index = _setting_alarm_touch_ItemIndex(msg);

        if (alarm_attr->alarm_index >= 0 && alarm_attr->alarm_index < REG_ALARM_NUM && GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
        {
            alarm_attr->focus_id = 0;
            //alarm_attr->view = SETTING_ALARM_SETTING_VIEW;
            //alarm_attr->alarm_index = alarm_attr->focus_id;
            setting_alarm_paint(msg, alarm_attr->alarm_index, alarm_attr->view); //..
            LISTBAR_SetFocusItem(alarm_attr->h_listbar, 0);
            //LISTBAR_ResumePage(alarm_attr->h_listbar);
            LISTBAR_ShowPage(alarm_attr->h_listbar);
            setting_alarm_long_string_init(alarm_attr); // 切换到闹钟设置窗口时，启动长文件名滚动
        }
        else //if( alarm_attr->view == SETTING_ALARM_SETTING_VIEW )
        {
            _setting_alarm_listbar_touch_proc(msg);
        }
    }

    return EPDK_OK;
}

static __s32 _setting_alarm_Proc(__gui_msg_t *msg)
{
    __msg("_setting_alarm_Proc msg->id=%d, msg->h_deswin=%x, msg->dwAddData1=%d, msg->dwAddData2=%d\n"
          , msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            setting_alarm_attr_t   *alarm_attr;
            __log("======GUI_MSG_CREATE=======power_setting \n");
            alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (alarm_attr == NULL)
            {
                return EPDK_FAIL;
            }

            alarm_attr->alarm_index = 0;
            alarm_attr->focus_id = 0;
            alarm_attr->reg_sys = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
            alarm_attr->view    = SETTING_ALARM_SETTING_VIEW;//SETTING_ALARM_SELECT_VIEW;
            _setting_alarm_res_init(msg);
            //GUI_LyrWinSetSta(alarm_attr->layer,GUI_LYRWIN_STA_ON);
            //GUI_LyrWinSetTop(alarm_attr->layer);
            alarm_attr->first_in_paint_list = 1;
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
            GUI_FrmWinDelete(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            setting_alarm_attr_t *alarm_attr = NULL;
            __s32 ret;
            alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (alarm_attr == NULL)
            {
                return EPDK_FAIL;
            }

            standby_startup_alarm();
            setting_alarm_long_string_uninit(alarm_attr);
            ret = LISTBAR_SuspendPage(alarm_attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar suspend fail\n");
            }

            ret = LISTBAR_Delete(alarm_attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar delete fail\n");
            }

            alarm_attr->h_listbar = NULL;
            _setting_alarm_res_uninit(msg);

            if (alarm_attr)
            {
                My_Mfree(0, alarm_attr);
                alarm_attr = NULL ;
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            setting_alarm_attr_t   *alarm_attr;
            alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (alarm_attr == NULL)
            {
                return EPDK_FAIL;
            }

            __log("======GUI_MSG_PAINT=======power_setting \n");
            setting_alarm_paint(msg, ID_PAINTING_ALARM_F1, SETTING_ALARM_SETTING_VIEW);

            if (alarm_attr->first_in_paint_list == 1)
            {
                alarm_attr->first_in_paint_list = 0;
                setting_alarm_listbar_init(msg);
            }
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
            _setting_alarm_listbar_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            setting_alarm_touch_proc(msg);
            return EPDK_OK;

        case MSG_OP_EXIT:
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
            return EPDK_OK;

        case GUI_MSG_COMMAND:
            return EPDK_OK;

        case ALARM_CLOCK_MSG:
        {
            setting_alarm_attr_t *alarm_attr = NULL;
            alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (alarm_attr == NULL)
            {
                return EPDK_FAIL;
            }

            alarm_settingTime_cleanHappen(alarm_attr->alarm_index);
            LISTBAR_UpdatePage(alarm_attr->h_listbar);
        }

        return EPDK_OK;

        case ALARM_MUSIC_PATH_MSG:
        {
            char *p ;
            setting_alarm_attr_t   *alarm_attr;
            p = (char *)msg->dwReserved ;
            __msg("music name = %s\n", p);
            alarm_attr = (setting_alarm_attr_t *)GUI_WinGetAttr(msg->h_deswin);
            eLIBs_strcpy(alarm_attr->reg_sys->alarm[alarm_attr->alarm_index].alarm_music_path, (char *)(msg->dwReserved));
            LISTBAR_UpdatePage(alarm_attr->h_listbar) ;
        }

        return EPDK_OK ;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_alarm_win_create(H_WIN h_parent, setting_alarm_attr_t *para)
{
    __gui_framewincreate_para_t         framewin_para;
    setting_alarm_attr_t                *alarm_attr;
    FB fb;
    H_WIN hfrm;
    __msg("*********enter  alarm setting win proc *****************\n");
    GUI_LyrWinGetFB(para->layer, &fb);
    alarm_attr = (setting_alarm_attr_t *)My_Malloc(0, sizeof(setting_alarm_attr_t));

    if (!alarm_attr)
    {
        __msg("memory alloc fail.........\n");
        return NULL;
    }

    eLIBs_memset((void *)alarm_attr, 0, sizeof(setting_alarm_attr_t));
    alarm_attr->font        = para->font;
    alarm_attr->layer       = para->layer;
    alarm_attr->first_in_paint_list = 0;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = "alarm setting win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = ALARM_MENU_ID;
    framewin_para.hHosting      = h_parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_alarm_Proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)alarm_attr;
    framewin_para.hLayer        = para->layer;
    hfrm = GUI_FrmWinCreate(&framewin_para);
    //GUI_LyrWinCacheOn();
    //GUI_LyrWinSetSta(para->layer, GUI_LYRWIN_STA_SLEEP);
    GUI_LyrWinSetSta(para->layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(para->layer);
    //GUI_LyrWinFlushFB(para->layer);
    //GUI_LyrWinCacheOff();
    return hfrm;
}


__s32 setting_alarm_win_delete(H_WIN alarm_win)
{
    GUI_FrmWinDelete(alarm_win);
    return EPDK_OK;
}
