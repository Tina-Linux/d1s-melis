/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : setting_general.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "setting_general.h"
#include "setting_uipara.h"
#include "app_setting.h"
//#define   LISTBAR_ITEM_NUM   6//8 adjust
extern Setting_focus_t SETTING_DISPLAY_FOCUS;

static __s32 setting_general_paint_item_detail_ex(__gui_msg_t *msg);
static __s32 is_file_exist(char *filename);

typedef struct tag_setting_item_id
{
    __s32 id_string_detail_prompt;
    __s32 id_string_detail_prompt_ex;
    __s32 id_string_title;
    __s32 id_string_content;

    __s32  id_bmp_left;
    __s32  id_bmp_right;
    __s32  id_bmp_left_f;//byf
    __s32  id_bmp_right_f;//    byf
} setting_item_id_t;

typedef struct _setting_item_res
{
    /*listbar item title*/
    char string_title[128];
    /*listbar item content*/
    char string_content[128];
    /*each item detail prompt*/
    char string_detail_prompt[128];

    __s32 content_num;
    __s32 content_nr;
} setting_item_res_t;

enum
{
    SETTING_LANGUAGE_CHINESES = 0,
#if 0
    SETTING_LANGUAGE_CHINESET,
#endif
    SETTING_LANGUAGE_ENGLISH,
    SETTING_LANGUAGE_NUM,
};
enum
{
    SETTING_CLOSE_SCREEN_OFF = 0,
    SETTING_CLOSE_SCREEN_15S,
    SETTING_CLOSE_SCREEN_30S,
    SETTING_CLOSE_SCREEN_45S,
    SETTING_CLOSE_SCREEN_60S,
    SETTING_CLOSE_SCREEN_90S,
    SETTING_CLOSE_SCREEN_NUM,
};
enum
{
    SETTING_LCD_BRIGHT_1 = 0,
    SETTING_LCD_BRIGHT_2,
    SETTING_LCD_BRIGHT_3,
    SETTING_LCD_BRIGHT_4,
    SETTING_LCD_BRIGHT_5,
    SETTING_LCD_BRIGHT_6,
    SETTING_LCD_BRIGHT_7,
    SETTING_LCD_BRIGHT_8,
    SETTING_LCD_BRIGHT_9,
    SETTING_LCD_BRIGHT_10,
    SETTING_LCD_BRIGHT_11,
    SETTING_LCD_BRIGHT_12,
    SETTING_LCD_BRIGHT_13,
    SETTING_LCD_BRIGHT_14,
    SETTING_LCD_BRIGHT_15,
    SETTING_LCD_BRIGHT_16,
    SETTING_LCD_BRIGHT_NUM,
};
enum
{
    SETTING_LCD_CONSTRACT_0 = 0,
    SETTING_LCD_CONSTRACT_1,
    SETTING_LCD_CONSTRACT_2,
    SETTING_LCD_CONSTRACT_3,
    SETTING_LCD_CONSTRACT_4,
    SETTING_LCD_CONSTRACT_5,
    SETTING_LCD_CONSTRACT_6,
    SETTING_LCD_CONSTRACT_7,
    SETTING_LCD_CONSTRACT_8,
    SETTING_LCD_CONSTRACT_9,
    SETTING_LCD_CONSTRACT_10,
    SETTING_LCD_CONSTRACT_NUM,
};

enum
{
    SETTING_BACK_LIGHT_1_LEVEL = 0,
    SETTING_BACK_LIGHT_2_LEVEL,
    SETTING_BACK_LIGHT_3_LEVEL,
    SETTING_BACK_LIGHT_4_LEVEL,
    SETTING_BACK_LIGHT_5_LEVEL,
    SETTING_BACK_LIGHT_6_LEVEL,
    SETTING_BACK_LIGHT_7_LEVEL,
    SETTING_BACK_LIGHT_8_LEVEL,
    SETTING_BACK_LIGHT_9_LEVEL,
    SETTING_BACK_LIGHT_10_LEVEL,
    SETTING_BACK_LIGHT_11_LEVEL,
    SETTING_BACK_LIGHT_12_LEVEL,
    SETTING_BACK_LIGHT_LEVEL_NUM,
};
enum
{
    SETTING_TIMING_SHUTDOWN_OFF = 0,
    SETTING_TIMING_SHUTDOWN_15MIN,
    SETTING_TIMING_SHUTDOWN_30MIN,
    SETTING_TIMING_SHUTDOWN_45MIN,
    SETTING_TIMING_SHUTDOWN_60MIN,
    SETTING_TIMING_SHUTDOWN_NUM,
};
enum
{
    SETTING_TOPIC_STYLE1_CON = 0,
    SETTING_TOPIC_STYLE2_CON,
    SETTING_TOPIC_STYLE3_CON,
    SETTING_TOPIC_STYLE_NUM,
} ;
enum
{
    SETTING_KETTONE_OFF = 0,
    SETTING_KETTONE_ON,
    SETTING_KETTONE_NUM,
};

enum
{
    SETTING_BACK_LIGHT_OFF = 0,
    SETTING_BACK_LIGHT_ON,
    SETTING_BACK_LIGHT_NUM,
};

typedef enum
{
    GENERAL_SETTING_START = 0,
    GENERAL_SETTING_LANGUAGE = GENERAL_SETTING_START,
    GENERAL_SETTING_BACKGROUND_STYLE,
    GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN,
    GENERAL_SETTING_SET_TIMING_SHUTDOWN,
    GENERAL_SETTING_SET_LCD_BRIGHT,
    GENERAL_SETTING_SET_LCD_CONSTRACT,
    GENERAL_SETTING_SET_LCD_SATURATION,
    GENERAL_SETTING_SET_LCD_ACUTANCE,
    GENERAL_SETTING_SET_LCD_DEFINITION,
    GENERAL_SETTING_BACK_LIGHT_ONOFF,
    GENERAL_SETTING_BACK_LIGHT_BRIGHT,
    GENERAL_SETTING_KEYTONE,
    GENERAL_SETTING_OUTPUT_TYPE,
    GENERAL_SETTING_FACTORY_DEFAULT,
    GENERAL_SETTING_SOFT_VERSION,
    GENERAL_SETTING_NUM,
    GENERAL_SETTING_UPDATE,
} general_setting_t;

typedef struct tag_setting_general_attr
{
    /*left general and power setting string*/
    // char general_setting_string_prompt[128];
    //char power_setting_string_prompt[128];

    /*left big setting icon*/
    HTHEME h_left_big_setting_icon;
    /* left and right when focus and unfocus bmp*/
    HTHEME h_item_focus_bmp_right, h_item_unfocus_bmp_right;
    HTHEME h_item_unfocus_bmp_left; //byf  左边的条目

    //HTHEME    h_power_setting_uf_bmp, h_power_setting_f_bmp;
    /*triangle icon bmp*/
    HTHEME  left_triangle_icon_uf, left_triangle_icon_focus;
    HTHEME  right_triangle_icon_uf, right_triangle_icon_focus;

    /*SCOLL BAR BMP*/
#if 0
    HTHEME scoll_back_bmp;
    HTHEME scoll_head_bmp;
    HTHEME scoll_body_bmp;
    HTHEME scoll_tail_bmp;
#endif
    setting_item_res_t res[GENERAL_SETTING_NUM];
    setting_uipara_t *uipara;

    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_LBAR   h_listbar;
    GUI_FONT *font;
    H_WIN layer;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;
    setting_general_para_t *general_para;
    //set_general_para* general_uipara;
} setting_general_attr_t, setting_attr_t;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, unsigned long data1, unsigned long data2);

static reg_system_para_t *setting_reg_para = NULL;

static __s32 content_lang_id[SETTING_LANGUAGE_NUM] = //ITEM1 ( 设置语言)
{
    STRING_SET_COMMON_LANGUAGE_CHINESES_CON,
#if 0
    STRING_SET_COMMON_LANGUAGE_CHINESET_CON,
#endif
    STRING_SET_COMMON_LANGUAGE_ENGLISH_CON
};
static __s32 content_timing_close_screen_id[SETTING_CLOSE_SCREEN_NUM] =
{
    STRING_SET_TIMING_SHUTDOWN_OFF,
    STRING_SET_15SECOND,
    STRING_SET_30SECOND,
    STRING_SET_45SECOND,
    STRING_SET_60SECOND,
    STRING_SET_90SECOND,
};
static __s32 content_lcd_bright_id[SETTING_LCD_BRIGHT_NUM] =
{
    STRING_SET_POWER_BGT_LEVEL1_CON,
    STRING_SET_POWER_BGT_LEVEL2_CON,
    STRING_SET_POWER_BGT_LEVEL3_CON,
    STRING_SET_POWER_BGT_LEVEL4_CON,
    STRING_SET_POWER_BGT_LEVEL5_CON,
    STRING_SET_POWER_BGT_LEVEL6_CON,
    STRING_SET_POWER_BGT_LEVEL7_CON,
    STRING_SET_POWER_BGT_LEVEL8_CON,
    STRING_SET_POWER_BGT_LEVEL9_CON,
    STRING_SET_POWER_BGT_LEVEL10_CON,
    STRING_SET_POWER_BGT_LEVEL11_CON,
    STRING_SET_POWER_BGT_LEVEL12_CON,
    STRING_SET_POWER_BGT_LEVEL13_CON,
    STRING_SET_POWER_BGT_LEVEL14_CON,
    STRING_SET_POWER_BGT_LEVEL15_CON,
    STRING_SET_POWER_BGT_LEVEL16_CON,
};
static __s32 content_lcd_constract_id[SETTING_LCD_CONSTRACT_NUM] =
{
    STRING_SET_LCD_0_PERCENT,
    STRING_SET_LCD_1_PERCENT,
    STRING_SET_LCD_2_PERCENT,
    STRING_SET_LCD_3_PERCENT,
    STRING_SET_LCD_4_PERCENT,
    STRING_SET_LCD_5_PERCENT,
    STRING_SET_LCD_6_PERCENT,
    STRING_SET_LCD_7_PERCENT,
    STRING_SET_LCD_8_PERCENT,
    STRING_SET_LCD_9_PERCENT,
    STRING_SET_LCD_10_PERCENT,
};
static __s32 content_timing_shutdown_id[SETTING_TIMING_SHUTDOWN_NUM] =
{
    STRING_SET_TIMING_SHUTDOWN_OFF,
    STRING_SET_15MINUTE,
    STRING_SET_30MINUTE,
    STRING_SET_45MINUTE,
    STRING_SET_60MINUTE
};
static __s32 content_style_id[SETTING_TOPIC_STYLE_NUM] = //ITEM2 (设置主题风格)
{
    STRING_SET_COMMON_TOPIC_STYLE1_CON,
    STRING_SET_COMMON_TOPIC_STYLE2_CON,
    STRING_SET_COMMON_TOPIC_STYLE3_CON
};

static __s32 content_keytone_id[SETTING_KETTONE_NUM] = //按键音
{
    STRING_SET_KEYTONE_OFF_CON,
    STRING_SET_KEYTONE_ON_CON
};

__s32 content_back_light_onoff_id[SETTING_BACK_LIGHT_NUM] =
{
    STRING_SET_POWER_BGT_OFF_EX,
    STRING_SET_POWER_BGT_ON_EX
};
__s32 content_back_light_onoff_bright[SETTING_BACK_LIGHT_LEVEL_NUM] =
{
    STRING_SET_POWER_BGT_LEVEL1_CON,
    STRING_SET_POWER_BGT_LEVEL2_CON,
#if 0
    STRING_SET_POWER_BGT_LEVEL3_CON,
    STRING_SET_POWER_BGT_LEVEL4_CON,
    STRING_SET_POWER_BGT_LEVEL5_CON,
    STRING_SET_POWER_BGT_LEVEL6_CON,
    STRING_SET_POWER_BGT_LEVEL7_CON,
    STRING_SET_POWER_BGT_LEVEL8_CON,
    STRING_SET_POWER_BGT_LEVEL9_CON,
    STRING_SET_POWER_BGT_LEVEL10_CON,
    STRING_SET_POWER_BGT_LEVEL11_CON,
    STRING_SET_POWER_BGT_LEVEL12_CON
#else
    STRING_SET_POWER_BGT_LEVEL1_CON,
    STRING_SET_POWER_BGT_LEVEL2_CON,
    STRING_SET_POWER_BGT_LEVEL3_CON,
    STRING_SET_POWER_BGT_LEVEL4_CON,
    STRING_SET_POWER_BGT_LEVEL5_CON,
    STRING_SET_POWER_BGT_LEVEL6_CON,
    STRING_SET_POWER_BGT_LEVEL7_CON,
    STRING_SET_POWER_BGT_LEVEL8_CON,
    STRING_SET_POWER_BGT_LEVEL9_CON,
    STRING_SET_POWER_BGT_LEVEL10_CON
#endif
};


static int setting_general_item_title_string[GENERAL_SETTING_NUM] =
{
    STRING_SET_COMMON_LANGUAGE_TITLE,
    STRING_SET_COMMON_TOPIC_STYLE_TITLE,
    STRING_SET_TIMING_CLOSE_SCREEN,
    STRING_SET_TIMING_SHUTDOWN,
    STRING_SET_LCD_BRIGHT,
    STRING_SET_LCD_CONSTRACT,
    STRING_SET_LCD_SATURATION,
    STRING_SET_LCD_ACUTANCE,
    STRING_SET_LCD_DEFINITION,
    STRING_SET_POWER_BACK_LIGHT,
    STRING_SET_POWER_BGT_TITLE,
    STRING_SET_KEYTONE_TITLE,
    STRING_SET_OUTPUT_TITLE,
    STRING_SET_COMMON_FACTORY_DEFAULT_TITLE,
    STRING_SET_TIPS_SOFT_VERSION
};
static int setting_string_item_detail_prompt[GENERAL_SETTING_NUM] =
{
    STRING_SET_COMMON_LANGUAGE_EX,
    STRING_SET_COMMON_TOPIC_STYLE_EX,
    STRING_SET_TIMING_TO_CLOSE_SCREEN_EX,
    STRING_SET_COMMON_TIMING_SHUTDOWN_EX,
    STRING_SET_COMMON_LCD_BRIGHT_EX,
    STRING_SET_COMMON_LCD_CONSTRACT_EX,
    STRING_SET_COMMON_LCD_SATURATION_EX,
    STRING_SET_COMMON_LCD_ACUTANCE_EX,
    STRING_SET_COMMON_LCD_DEFINITION_EX,
    STRING_SET_COMMON_BACKLIGHT_ON_OFF_EX,
    STRING_SET_COMMON_BACKLIGHT_VALUE_EX,
    STRING_SET_COMMON_KEYTONE_EX,
    STRING_SET_OUTPUT_DE,
    STRING_SET_COMMON_FACTORY_DEFAULT_TITLE,
    STRING_SET_TIPS_SOFT_VERSION
};

static __s32 content_output_id[] = //输出类型
{
    STRING_SET_OUTPUT_NTSC_CON,
    STRING_SET_OUTPUT_PAL_CON
};


#if 0

/*
    申请资源
*/
void _setting_power_get_content_string(setting_item_res_t  *p_item_res, power_setting_t Item_Index)
{
    __u32 *content_res_id = NULL ;
    __s32 content_total = 0;
    ASSERT(p_item_res != NULL);
    ASSERT((Item_Index >= POWER_SETTING_BACK_LIGHT) && (Item_Index < POWER_SETTING_NUM));

    switch (Item_Index)
    {
        case POWER_SETTING_BACK_LIGHT:
        {
            content_total = SETTING_BACK_LIGHT_NUM;
            content_res_id = (__u32 *)content_back_light_id;
        }
        break;

        default:
        {
            content_res_id = NULL;
        }
        break;
    }

    p_item_res->content_nr = content_total;
    dsk_langres_get_menu_text(content_res_id[p_item_res->content_num], p_item_res->string_content, sizeof(p_item_res->string_content));
}
static __s32 _setting_power_listbar_item_paint(__lbar_draw_para_t *draw_param)
{
    RECT rect = {0};
    GUI_RECT gui_rect = {0};
    setting_attr_t *setting_attr = NULL ;
    setting_uipara_t *uipara = NULL;
    setting_item_res_t  *p_item_res = NULL;
    setting_attr = (setting_general_attr_t *)draw_param->attr;

    if (setting_attr == NULL)
    {
        __err("%s %d general_attr null", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    ASSERT((setting_attr->new_focus >= POWER_SETTING_BACK_LIGHT) && (setting_attr->new_focus < POWER_SETTING_NUM));

    if (GUI_LyrWinGetSta(setting_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __err("%s %d general_attr->layer GUI_LYRWIN_STA_SUSPEND", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    GUI_SetFont(setting_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinCacheOn();
    setting_attr->new_focus = draw_param->index;
    p_item_res = &setting_attr->res[setting_attr->new_focus + GENERAL_SETTING_NUM];
    uipara = setting_attr->uipara;

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            __inf("%s %d draw unfocus right item!" __FILE__, __LINE__);
            /*draw unfocus right item */
            GUI_BMP_Draw(theme_hdl2buf(setting_attr->h_item_unfocus_bmp_right), draw_param->rect.x, draw_param->rect.y);
            /*draw unfocus triangle icon*/
            GUI_BMP_Draw(theme_hdl2buf(setting_attr->left_triangle_icon_uf), draw_param->rect.x + uipara->left_button.x, draw_param->rect.y + uipara->left_button.y);
            GUI_BMP_Draw(theme_hdl2buf(setting_attr->right_triangle_icon_uf), draw_param->rect.x + uipara->right_button.x, draw_param->rect.y + uipara->right_button.y);
            /*set string color*/
            GUI_SetColor(setting_attr->unfocus_txt_color);
            /*draw unfocus right item title string*/
            gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_DispStringInRect(p_item_res->string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            /*draw focus right item content string*/
            gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            __inf("%s %d draw focus right item!" __FILE__, __LINE__);

            /*draw focus right item */
            if (SETTING_DISPLAY_FOCUS == SETTING_DISPLAY_FOCUS_RIGHT)
            {
                GUI_BMP_Draw(theme_hdl2buf(setting_attr->h_item_focus_bmp_right), draw_param->rect.x, draw_param->rect.y);
                /*draw unfocus triangle icon*/
                GUI_BMP_Draw(theme_hdl2buf(setting_attr->left_triangle_icon_focus), draw_param->rect.x + uipara->left_button.x, draw_param->rect.y + uipara->left_button.y);
                GUI_BMP_Draw(theme_hdl2buf(setting_attr->right_triangle_icon_focus), draw_param->rect.x + uipara->right_button.x, draw_param->rect.y + uipara->right_button.y);
                /*set string color*/
                GUI_SetColor(setting_attr->focus_txt_color);
                /*draw focus right item title string*/
                gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
                gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
                gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
                GUI_DispStringInRect(p_item_res->string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
                /*draw focus right item content string*/
                gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
                gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
                gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
                GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            }
        }
        break;

        default:
            break;
    }

    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

__s32 _setting_power_listbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config;
    setting_uipara_t *uipara = NULL;
    setting_attr_t *setting_attr = NULL;
    setting_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (setting_attr == NULL)
    {
        __err("general_attr null %s %d", __FILE__, __LINE__);
        return ;
    }

    uipara = setting_attr->uipara;
    eLIBs_memset(&config, 0x00, sizeof(config));
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.focus_id = 0;
    config.item_cnt = POWER_SETTING_NUM;//LISTBAR_ITEM_NUM ;//一共有几个条目
    config.item_height = uipara->item.h;//106
    config.item_width = uipara->item.w;//490
    config.list_rect.x = uipara->item.x;//800-setting_uipara.item.w;
    config.list_rect.y = uipara->item.y;//0
    config.list_rect.width = uipara->item.w;//490
    config.list_rect.height = uipara->lyr_scn.h - 56;
    config.start_id = 0;
    config.list_attr = (void *)setting_attr;
    setting_attr->h_listbar = LISTBAR_Create(msg->h_deswin);    // there don't need to create, because when already create
    LISTBAR_Config(setting_attr->h_listbar, _setting_power_listbar_item_paint, &config);
    LISTBAR_ShowPage(setting_attr->h_listbar);
}
static void _setting_power_action_button(__gui_msg_t *msg)
{
    setting_item_res_t  *p_item_res = NULL ;
    setting_general_attr_t *general_attr = NULL;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        __err("%s %d general_attr null", __FILE__, __LINE__);
        return ;
    }

    p_item_res = &general_attr->res[general_attr->new_focus + GENERAL_SETTING_NUM];

    switch (general_attr->new_focus)
    {
        case POWER_SETTING_BACK_LIGHT:
        {
            //__here__
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            //__here__
            __inf("p_item_res->content_num:%d", p_item_res->content_num);
        }
        break;

        default:
            break;
    }

    return ;
}

#endif

__bool bAlreadly_draw = EPDK_FALSE;
static __s32 setting_general_item_paint_ex(__lbar_draw_para_t *draw_param);
static void _setting_general_res_init(setting_general_attr_t *general_attr);
static void _setting_general_res_uninit(setting_general_attr_t *general_attr) ;
static void _setting_general_string_res_restore(setting_general_attr_t *general_attr);

static void _setting_general_listbar_init(__gui_msg_t *msg)
{
    //__scroll_bar_t scroll_param;
    __listbar_config_t config;
    setting_uipara_t *uipara = NULL;
    setting_general_attr_t *general_attr = NULL;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        __err("general_attr null %s %d", __FILE__, __LINE__);
        return ;
    }

    uipara = general_attr->uipara;
    eLIBs_memset(&config, 0x00, sizeof(config));
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.focus_id = general_attr->new_focus;
    config.item_cnt = GENERAL_SETTING_NUM;//LISTBAR_ITEM_NUM ;//一共有几个条目
    config.item_height = uipara->item.h;//106
    config.item_width = uipara->item.w;//490
    config.list_rect.x = uipara->item.x;//800-setting_uipara.item.w;
    config.list_rect.y = uipara->item.y;//0
    config.list_rect.width = uipara->item.w;//490
    config.list_rect.height = uipara->item.h * 8;
    config.start_id = 0;
    config.list_attr = (void *)general_attr;
    //eLIBs_memset(&scroll_param, 0x00, sizeof(scroll_param));
#if 0
    scroll_param.show_rect.x = uipara->scroll_bg_rect.x;
    scroll_param.show_rect.y = uipara->scroll_bg_rect.y;
    scroll_param.show_rect.width = uipara->scroll_bg_rect.width;
    scroll_param.show_rect.height = uipara->scroll_bg_rect.height;//整个Scoll bar显示区域，
    scroll_param.scroll_rect.x = uipara->scroll_rect.x;
    scroll_param.scroll_rect.y = uipara->scroll_rect.y;
    scroll_param.scroll_rect.width = uipara->scroll_rect.width;
    scroll_param.scroll_rect.height = uipara->scroll_rect.height;   //滚动条区域(除去head,body,tail)
    scroll_param.head_height = uipara->scroll_head_height;
    scroll_param.tail_height = uipara->scroll_tail_height;              //head, tail
    scroll_param.back_bmp = theme_hdl2buf(general_attr->scoll_back_bmp);
    scroll_param.head_bmp = theme_hdl2buf(general_attr->scoll_head_bmp);
    scroll_param.body_bmp = theme_hdl2buf(general_attr->scoll_body_bmp);
    scroll_param.tail_bmp = theme_hdl2buf(general_attr->scoll_tail_bmp);
#endif
    general_attr->h_listbar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(general_attr->h_listbar, setting_general_item_paint_ex, &config);
    //LISTBAR_ScrollConfig(general_attr->h_listbar, &scroll_param);
    LISTBAR_ShowPage(general_attr->h_listbar);
}

static __bool position_in_rect(RECT *rect, __s32 x, __s32 y)
{
    if ((x > rect->x) && (y > rect->y) && (x < (rect->x + rect->width)) && (y < (rect->y + rect->height)))
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

static __s32 setting_translate_close_scn_time(__u32 choice)
{
    __s32 time = 0;

    switch (choice)
    {
        case SETTING_CLOSE_SCREEN_15S:
            time = 15;
            break;

        case SETTING_CLOSE_SCREEN_30S:
            time = 30;
            break;

        case SETTING_CLOSE_SCREEN_45S:
            time = 45;
            break;

        case SETTING_CLOSE_SCREEN_60S:
            time = 60;
            break;

        case SETTING_CLOSE_SCREEN_90S:
            time = 90;
            break;

        default:
            time = 0;
            break;
    }

    return time;
}

static void setting_restore_default(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr = NULL;
    __s32 i;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        __err("%s %d general_attr NULL ", __FILE__, __LINE__);
        return ;
    }

    dsk_reg_set_app_restore_all();

    if (setting_reg_para)
    {
        setting_reg_para->tp_adjust_finish_flag = 1 ;   //避免恢复出厂设置后，重启要重新校正
        dsk_langres_set_type(setting_reg_para->language);           //语言恢复出厂设置
        _setting_set_hbar_title(STRING_SET_TITLE);
        //gscene_bgd_set_default_bg_index(setting_reg_para->style); //恢复默认背景图
        gscene_bgd_refresh();
        __err("defualt setting some API wait to fix..");
        
        //dsk_keytone_set_state((__set_keytone_t)(setting_reg_para->keytone));//desktop模块里面1 表示on,2 表示off

        //dsk_keytone_on();

        //dsk_volume_set(setting_reg_para->volume);   //音量恢复出厂设置
        dsk_display_set_lcd_brightness((__lion_bright_t)(setting_reg_para->backlight + 1)); //背光亮度
        g_set_close_scn_time(setting_reg_para->backlightoff);   //自动关屏时间
        dsk_set_auto_off_time(setting_reg_para->poweroff);      //自动关机时间
        dsk_display_set_lcd_bright(setting_reg_para->lcd_bright * 10);
        dsk_display_set_lcd_constract(setting_reg_para->lcd_constract * 10);
        dsk_display_set_lcd_saturation(setting_reg_para->lcd_saturation * 10);
        dsk_display_set_lcd_hue(setting_reg_para->lcd_hue * 10);
        dsk_display_set_lcd_detail(setting_reg_para->lcd_detail * 10);

        if (BEETLES_RTC_EXIST)
        {
            __awos_date_t factory_date = {0,};
            __awos_time_t factory_time = {0,};
            factory_date = setting_reg_para->date;
            factory_time = setting_reg_para->time;
            esTIME_SetDate(&factory_date);
            esTIME_SetTime(&factory_time);
        }

        /*
                if (setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESES)
                {
                    general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 0;
                }
                else if (setting_reg_para->language == EPDK_LANGUAGE_ENM_ENGLISH)
                {
                    general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 1;
                }
                else
                {
                    general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 0;
                }
                general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num = setting_reg_para->style;
                //general_attr->res_output.content_num  = setting_reg_para->pal_ntsc;
                general_attr->res[GENERAL_SETTING_KEYTONE].content_num  = !setting_reg_para->keytone;
                //general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num   = setting_reg_para->timekeeper_on_off;
        */
        for (i = 0; i < general_attr->item_nr; i++)
        {
            switch (i)
            {
                case GENERAL_SETTING_LANGUAGE:
                    if (setting_reg_para != NULL)
                    {
                        if (setting_reg_para->language == EPDK_LANGUAGE_ENM_CHINESES)
                        {
                            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 0;
                        }
                        else if (setting_reg_para->language == EPDK_LANGUAGE_ENM_ENGLISH)
                        {
                            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 1;
                        }
                        else
                        {
                            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 0;
                        }
                    }
                    else
                    {
                        general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = 0;
                    }

                    break;

                case GENERAL_SETTING_BACKGROUND_STYLE:
                    if (setting_reg_para != NULL)
                    {
                        general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num = setting_reg_para->style;

                        if (general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num > 2)
                        {
                            general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num = 0;
                        }
                    }
                    else
                    {
                        general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num = 0;
                    }

                    break;

                case GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN:
                {
                    __s32 content_num = 0;

                    switch (setting_reg_para->backlightoff)
                    {
                        case 15:
                        {
                            content_num = SETTING_CLOSE_SCREEN_15S;
                        }
                        break;

                        case 30:
                        {
                            content_num = SETTING_CLOSE_SCREEN_30S;
                        }
                        break;

                        case 45:
                        {
                            content_num = SETTING_CLOSE_SCREEN_45S;
                        }
                        break;

                        case 60:
                        {
                            content_num = SETTING_CLOSE_SCREEN_60S;
                        }
                        break;

                        case 90:
                        {
                            content_num = SETTING_CLOSE_SCREEN_90S;
                        }
                        break;

                        default:
                        {
                            content_num = SETTING_CLOSE_SCREEN_OFF;
                        }
                        break;
                    }

                    general_attr->res[GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN].content_num = content_num;;
                }
                break;

                case GENERAL_SETTING_SET_TIMING_SHUTDOWN:
                {
                    __s32 content_num = 0;

                    switch (setting_reg_para->poweroff)
                    {
                        case 15*60:
                        {
                            content_num = SETTING_TIMING_SHUTDOWN_15MIN;
                        }
                        break;

                        case 30*60:
                        {
                            content_num = SETTING_TIMING_SHUTDOWN_30MIN;
                        }
                        break;

                        case 45*60:
                        {
                            content_num = SETTING_TIMING_SHUTDOWN_45MIN;
                        }
                        break;

                        case 60*60:
                        {
                            content_num = SETTING_TIMING_SHUTDOWN_60MIN;
                        }
                        break;

                        default:
                        {
                            content_num = SETTING_TIMING_SHUTDOWN_OFF;
                        }
                        break;
                    }

                    general_attr->res[GENERAL_SETTING_SET_TIMING_SHUTDOWN].content_num = content_num;
                }
                break;

                case GENERAL_SETTING_SET_LCD_BRIGHT:
                    general_attr->res[GENERAL_SETTING_SET_LCD_BRIGHT].content_num = setting_reg_para->lcd_bright;
                    break;

                case GENERAL_SETTING_SET_LCD_CONSTRACT:
                    general_attr->res[GENERAL_SETTING_SET_LCD_CONSTRACT].content_num = setting_reg_para->lcd_constract;
                    break;

                case GENERAL_SETTING_SET_LCD_SATURATION:
                    general_attr->res[GENERAL_SETTING_SET_LCD_SATURATION].content_num = setting_reg_para->lcd_saturation;
                    break;

                case GENERAL_SETTING_SET_LCD_ACUTANCE:
                    general_attr->res[GENERAL_SETTING_SET_LCD_ACUTANCE].content_num = setting_reg_para->lcd_hue;
                    break;

                case GENERAL_SETTING_SET_LCD_DEFINITION:
                    general_attr->res[GENERAL_SETTING_SET_LCD_DEFINITION].content_num = setting_reg_para->lcd_detail;
                    break;

                case GENERAL_SETTING_BACK_LIGHT_ONOFF:
                    general_attr->res[GENERAL_SETTING_BACK_LIGHT_ONOFF].content_num = 0;
                    break;

                case GENERAL_SETTING_BACK_LIGHT_BRIGHT:
                    general_attr->res[GENERAL_SETTING_BACK_LIGHT_BRIGHT].content_num = setting_reg_para->backlight;
                    break;

                case GENERAL_SETTING_KEYTONE:
                    general_attr->res[GENERAL_SETTING_KEYTONE].content_num = setting_reg_para->keytone;
                    break;

                case GENERAL_SETTING_OUTPUT_TYPE:
                    general_attr->res[i].content_num = setting_reg_para->pal_ntsc;
                    break;

                default:
                    break;
            }
        }
    }

    //_setting_general_res_uninit(general_attr);
    // _setting_general_res_init(general_attr);
    // _setting_scene_paint(msg, EPDK_FALSE);
    _setting_general_string_res_restore(general_attr);
    setting_general_paint_item_detail_ex(msg);
    LISTBAR_ShowPage(general_attr->h_listbar);
}
/*
    申请资源
*/
static void _setting_general_get_content_string(setting_item_res_t  *p_item_res, general_setting_t Item_Index)
{
    __u32 *content_res_id = NULL ;
    __s32 content_total = 0;
    ASSERT(p_item_res != NULL);
    ASSERT((Item_Index >= GENERAL_SETTING_LANGUAGE) && (Item_Index < GENERAL_SETTING_NUM));

    switch (Item_Index)
    {
        case GENERAL_SETTING_LANGUAGE:
        {
            content_total = SETTING_LANGUAGE_NUM;
            content_res_id = (__u32 *)content_lang_id;
        }
        break;

        case GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN:
        {
            content_total = SETTING_CLOSE_SCREEN_NUM;
            content_res_id = (__u32 *)content_timing_close_screen_id;
        }
        break;

        case GENERAL_SETTING_SET_LCD_BRIGHT:
        {
            // content_total = SETTING_LCD_BRIGHT_NUM;
            // content_res_id = (__u32 *)content_lcd_bright_id;
            content_total = SETTING_LCD_CONSTRACT_NUM;
            content_res_id = (__u32 *)content_lcd_constract_id;
        }
        break;

        case GENERAL_SETTING_SET_LCD_CONSTRACT:
        {
            content_total = SETTING_LCD_CONSTRACT_NUM;
            content_res_id = (__u32 *)content_lcd_constract_id;
        }
        break;

        case GENERAL_SETTING_SET_LCD_SATURATION:
        {
            content_total = SETTING_LCD_CONSTRACT_NUM;
            content_res_id = (__u32 *)content_lcd_constract_id;
        }
        break;

        case GENERAL_SETTING_SET_LCD_ACUTANCE:
        {
            content_total = SETTING_LCD_CONSTRACT_NUM;
            content_res_id = (__u32 *)content_lcd_constract_id;
        }
        break;

        case GENERAL_SETTING_SET_LCD_DEFINITION:
        {
            content_total = SETTING_LCD_CONSTRACT_NUM;
            content_res_id = (__u32 *)content_lcd_constract_id;
        }
        break;

        case GENERAL_SETTING_SET_TIMING_SHUTDOWN:
        {
            content_total = SETTING_TIMING_SHUTDOWN_NUM;
            content_res_id = content_timing_shutdown_id;
        }
        break;

        case GENERAL_SETTING_BACK_LIGHT_BRIGHT:
        {
            content_total = SETTING_BACK_LIGHT_LEVEL_NUM;
            content_res_id = content_back_light_onoff_bright;
        }
        break;

        case GENERAL_SETTING_BACKGROUND_STYLE:
        {
            content_total = SETTING_TOPIC_STYLE_NUM;
            content_res_id = (__u32 *)content_style_id;
        }
        break;

        case GENERAL_SETTING_KEYTONE:
        {
            content_total = SETTING_KETTONE_NUM;
            content_res_id = (__u32 *)content_keytone_id;
        }
        break;

        case GENERAL_SETTING_OUTPUT_TYPE:
        {
            content_total = sizeof(content_output_id) / sizeof(content_output_id[0]);
            content_res_id = (__u32 *)content_output_id;
        }
        break;

        case GENERAL_SETTING_BACK_LIGHT_ONOFF:
        {
            content_total = SETTING_BACK_LIGHT_NUM;
            content_res_id = (__u32 *)content_back_light_onoff_id;
        }
        break;

        case GENERAL_SETTING_SOFT_VERSION:
        {
            //get soft version
            reg_system_para_t *para;
            para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

            if (para)
            {
                eLIBs_memcpy(p_item_res->string_content, para->ver_inf, sizeof(para->ver_inf));
            }
            else
            {
                eLIBs_memcpy(p_item_res->string_content, "Version unknown", sizeof("version unknown"));
            }

            p_item_res->content_nr = 0;
            return ;// must return
        }
        break;

        default:
        {
            content_total = 0;
            content_res_id = NULL;
        }
        break;
    }

    p_item_res->content_nr = content_total;

    if (content_res_id != NULL)
    {
        dsk_langres_get_menu_text(content_res_id[p_item_res->content_num], p_item_res->string_content, sizeof(p_item_res->string_content));
    }
}
static void _setting_general_res_init(setting_general_attr_t *general_attr)
{
    __s32 Index = 0;
    __bool  restore_flag = EPDK_FAIL;
    setting_item_res_t  *p_item_res = NULL;
    /*left general and power setting string*/
    //get_menu_text(STRING_SET_PROMPT_COMMON, general_attr->general_setting_string_prompt, 128);//set_common(通用设置)
    //get_menu_text(STRING_SET_PROMPT_POWER_MANAGER, general_attr->power_setting_string_prompt, 128);
    /*left item bmp*/
    //  general_attr->h_power_setting_uf_bmp= theme_open(ID_SETTING_NEW_POWER_FLAG_UF_BMP);//byf
    //  general_attr->h_power_setting_f_bmp= theme_open(ID_SETTING_NEW_POWER_FLAG_F_BMP);//byf
    /* left and right when focus and unfocus bmp*/
    general_attr->h_left_big_setting_icon = theme_open(ID_HOME_NEW_SETTING_UF_BMP);
    general_attr->h_item_focus_bmp_right = theme_open(ID_SETTING_NEW_ITEM_R_FCS_BMP);   //右边条目
    general_attr->h_item_unfocus_bmp_right = theme_open(ID_SETTING_NEW_ITEM_R_UFCS_BMP);//右边条目
    general_attr->h_item_unfocus_bmp_left = theme_open(ID_SETTING_NEW_ITEM_L_UFCS_BMP); //byf//左边条目
    /*scroll bar bmp*/
    //  general_attr->h_scroll_bg= theme_open(ID_SETTING_NEW_SCROLL_BG_BMP);
    //  general_attr->h_scroll_block= theme_open(ID_SETTING_NEW_SCROLL_BLOCK_BMP);//byf
    /*triangle icon bmp*/
    general_attr->  left_triangle_icon_uf = theme_open(ID_SETTING_NEW_SET_BUTT_L_UF_BMP);
    general_attr->  left_triangle_icon_focus = theme_open(ID_SETTING_NEW_SET_BUTT_L_F_BMP);
    general_attr->right_triangle_icon_uf = theme_open(ID_SETTING_NEW_SET_BUTT_R_UF_BMP);
    general_attr->right_triangle_icon_focus = theme_open(ID_SETTING_NEW_SET_BUTT_R_F_BMP);
#if 0
    /*srcoll bar bmp */
    general_attr->scoll_back_bmp = theme_open(ID_EXPLORER_LIST_SCROLL_BG_BMP);;
    general_attr->scoll_head_bmp = NULL;
    general_attr->scoll_body_bmp = theme_open(ID_EXPLORER_LIST_SCROLL_BMP);
    general_attr->scoll_tail_bmp = NULL;
#endif
    general_attr->item_nr = GENERAL_SETTING_NUM;

    /*init general res*/
    for (Index = GENERAL_SETTING_LANGUAGE; Index < GENERAL_SETTING_NUM; Index++)
    {
        p_item_res = &general_attr->res[Index];
        dsk_langres_get_menu_text(setting_general_item_title_string[Index], p_item_res->string_title, sizeof(p_item_res->string_title));
        dsk_langres_get_menu_text(setting_string_item_detail_prompt[Index], p_item_res->string_detail_prompt, sizeof(p_item_res->string_detail_prompt));
        _setting_general_get_content_string(p_item_res, Index);
    }

    general_attr->uipara = setting_get_uipara();
}
static void _setting_general_string_res_restore(setting_general_attr_t *general_attr)
{
    general_setting_t Index = GENERAL_SETTING_NUM;
    setting_item_res_t  *p_item_res = NULL;

    /*clear left general and power setting string*/
    //eLIBs_memset(general_attr->power_setting_string_prompt, 0x00, sizeof(general_attr->power_setting_string_prompt));
    //eLIBs_memset(general_attr->general_setting_string_prompt, 0x00, sizeof(general_attr->general_setting_string_prompt));

    /*restore left general and power setting string*/
    //get_menu_text(STRING_SET_PROMPT_COMMON, general_attr->general_setting_string_prompt, 128);//set_common(通用设置)
    //get_menu_text(STRING_SET_PROMPT_POWER_MANAGER, general_attr->power_setting_string_prompt, 128);

    for (Index = GENERAL_SETTING_LANGUAGE; Index < GENERAL_SETTING_NUM; Index++)
    {
        p_item_res = &general_attr->res[Index];
        /*clear item title and content*/
        eLIBs_memset(p_item_res->string_title, 0x00, sizeof(p_item_res->string_title));
        eLIBs_memset(p_item_res->string_content, 0x00, sizeof(p_item_res->string_content));
        dsk_langres_get_menu_text(setting_general_item_title_string[Index], p_item_res->string_title, sizeof(p_item_res->string_title));
        dsk_langres_get_menu_text(setting_string_item_detail_prompt[Index], p_item_res->string_detail_prompt, sizeof(p_item_res->string_detail_prompt));
        _setting_general_get_content_string(p_item_res, Index);
    }
}
static void _setting_general_res_uninit(setting_general_attr_t *general_attr)
{
    setting_item_res_t  *p_item_res = NULL;
    /*left big setting icon*/
    theme_close(general_attr->h_left_big_setting_icon);
    /* left and right when focus and unfocus bmp*/
    theme_close(general_attr->h_item_focus_bmp_right);
    theme_close(general_attr->h_item_unfocus_bmp_right);
    theme_close(general_attr->h_item_unfocus_bmp_left);
    /*scroll bar bmp*/
    //theme_close(general_attr->h_scroll_bg);
    //theme_close(general_attr->h_scroll_block);
#if 0
    theme_close(general_attr->scoll_back_bmp);
    theme_close(general_attr->scoll_head_bmp);
    theme_close(general_attr->scoll_body_bmp);
    theme_close(general_attr->scoll_tail_bmp);
#endif
    /*left item bmp*/
    //theme_close(general_attr->h_power_setting_uf_bmp);
    //theme_close(general_attr->h_power_setting_f_bmp);
    /*triangle icon bmp*/
    theme_close(general_attr->left_triangle_icon_uf);
    theme_close(general_attr->left_triangle_icon_focus);
    theme_close(general_attr->right_triangle_icon_uf);
    theme_close(general_attr->right_triangle_icon_focus);
    /*left big setting icon*/
    general_attr->h_left_big_setting_icon = 0;
    /* left and right when focus and unfocus bmp*/
    general_attr->h_item_focus_bmp_right = 0;
    general_attr->h_item_unfocus_bmp_right = 0;
    general_attr->h_item_unfocus_bmp_left = 0;
    /*scroll bar bmp*/
    //  general_attr->h_scroll_bg = 0;
    //  general_attr->h_scroll_block = 0;
#if 0
    general_attr->scoll_back_bmp = 0;
    general_attr->scoll_head_bmp = 0;
    general_attr->scoll_body_bmp = 0;
    general_attr->scoll_tail_bmp = 0;
#endif
    /*triangle icon bmp*/
    general_attr->left_triangle_icon_uf = 0;
    general_attr->left_triangle_icon_focus = 0;
    general_attr->right_triangle_icon_uf = 0;
    general_attr->right_triangle_icon_focus = 0;
}

void setting_general_init_content(setting_general_attr_t *general_attr)
{
    __s32   content_num = 0;

    if (setting_reg_para == NULL)
    {
        __err("setting_reg_para NULL %s %d ", __FILE__, __LINE__);
        return;
    }

    //语言
    switch (setting_reg_para->language)
    {
        case EPDK_LANGUAGE_ENM_CHINESES:
        {
            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = SETTING_LANGUAGE_CHINESES;
        }
        break;
#if 0

        case EPDK_LANGUAGE_ENM_CHINESET:
        {
            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = SETTING_LANGUAGE_CHINESET;
        }
        break;
#endif

        case EPDK_LANGUAGE_ENM_ENGLISH:
        {
            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = SETTING_LANGUAGE_ENGLISH;
        }
        break;

        default:
        {
            general_attr->res[GENERAL_SETTING_LANGUAGE].content_num = SETTING_LANGUAGE_CHINESES;
        }
        break;
    }

    //风格
    general_attr->res[GENERAL_SETTING_BACKGROUND_STYLE].content_num = setting_reg_para->style;

    // set timer to shut down
    switch (setting_reg_para->poweroff)
    {
        case 15*60:
        {
            content_num = SETTING_TIMING_SHUTDOWN_15MIN;
        }
        break;

        case 30*60:
        {
            content_num = SETTING_TIMING_SHUTDOWN_30MIN;
        }
        break;

        case 45*60:
        {
            content_num = SETTING_TIMING_SHUTDOWN_45MIN;
        }
        break;

        case 60*60:
        {
            content_num = SETTING_TIMING_SHUTDOWN_60MIN;
        }
        break;

        default:
        {
            content_num = SETTING_TIMING_SHUTDOWN_OFF;
        }
        break;
    }

    general_attr->res[GENERAL_SETTING_SET_TIMING_SHUTDOWN].content_num = content_num; // first set off

    // set timer to automatic close screen
    switch (setting_reg_para->backlightoff)
    {
        case 15:
        {
            content_num = SETTING_CLOSE_SCREEN_15S;
        }
        break;

        case 30:
        {
            content_num = SETTING_CLOSE_SCREEN_30S;
        }
        break;

        case 45:
        {
            content_num = SETTING_CLOSE_SCREEN_45S;
        }
        break;

        case 60:
        {
            content_num = SETTING_CLOSE_SCREEN_60S;
        }
        break;

        case 90:
        {
            content_num = SETTING_CLOSE_SCREEN_90S;
        }
        break;

        default:
        {
            content_num = SETTING_CLOSE_SCREEN_OFF;
        }
        break;
    }

    general_attr->res[GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN].content_num = content_num;
    /*if back light on: achieve bright*/
    general_attr->res[GENERAL_SETTING_BACK_LIGHT_BRIGHT].content_num = setting_reg_para->backlight;
#if 0

    if (setting_reg_para->backlightoff == SETTING_BACK_LIGHT_OFF)
    {
        general_attr->res[GENERAL_SETTING_BACK_LIGHT_ONOFF].content_num = SETTING_BACK_LIGHT_ON;
    }
    else
    {
        general_attr->res[GENERAL_SETTING_BACK_LIGHT_ONOFF].content_num = SETTING_BACK_LIGHT_OFF;
    }

#else
    general_attr->res[GENERAL_SETTING_BACK_LIGHT_ONOFF].content_num = 0;//setting_reg_para->backlightoff;
#endif
    /*LCD SETTING*/
    general_attr->res[GENERAL_SETTING_SET_LCD_BRIGHT].content_num = setting_reg_para->lcd_bright;
    general_attr->res[GENERAL_SETTING_SET_LCD_CONSTRACT].content_num = setting_reg_para->lcd_constract;
    general_attr->res[GENERAL_SETTING_SET_LCD_SATURATION].content_num = setting_reg_para->lcd_saturation;
    general_attr->res[GENERAL_SETTING_SET_LCD_ACUTANCE].content_num = setting_reg_para->lcd_hue;
    general_attr->res[GENERAL_SETTING_SET_LCD_DEFINITION].content_num = setting_reg_para->lcd_detail;
    /*LCD SETTING End*/
    //按键音
#if 0

    if (setting_reg_para->keytone == SETTING_KETTONE_OFF)
    {
        general_attr->res[GENERAL_SETTING_KEYTONE].content_num = SETTING_KETTONE_ON;
    }
    else
    {
        general_attr->res[GENERAL_SETTING_KEYTONE].content_num = SETTING_KETTONE_OFF;
    }

#else
    general_attr->res[GENERAL_SETTING_KEYTONE].content_num = setting_reg_para->keytone;
#endif
    general_attr->res[GENERAL_SETTING_OUTPUT_TYPE].content_num = setting_reg_para->pal_ntsc;
    general_attr->res[GENERAL_SETTING_FACTORY_DEFAULT].content_num = 0;
    general_attr->res[GENERAL_SETTING_SOFT_VERSION].content_num = 0;
    return;
}
#if 0
static __s32 setting_general_content_paint(__gui_msg_t *msg)
{
    setting_general_attr_t *general_attr = NULL ;
    setting_item_res_t  *p_item_res = NULL ;
    RECT rect;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara;
    __s32   x, y, x0, y0, x1, y1 ;
    GUI_MEMDEV_Handle   draw_mem;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    //GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(general_attr->focus_txt_color);
    uipara = general_attr->uipara;
    //GUI_LyrWinCacheOn();
    draw_mem = GUI_MEMDEV_Create(uipara->item.x, uipara->item.y + uipara->item.h * general_attr->new_focus, uipara->item.w, uipara->item.h);
    GUI_MEMDEV_Select(draw_mem);
    p_item_res = &general_attr->res[general_attr->new_focus];
    x = uipara->item_content_pos.x + uipara->item.x ;
    y = uipara->item_content_pos.y + uipara->item_content_pos.h * general_attr->new_focus ;
    x0 = uipara->item_content_pos.x ;
    y0 = uipara->item_content_pos.y ;
    x1 = uipara->item_content_pos.x + uipara->item_content_pos.w - 1 ;
    y1 = uipara->item_content_pos.y + uipara->item_content_pos.h - 1 ;
    GUI_BMP_Draw(theme_hdl2buf(general_attr->h_item_focus_bmp_right), uipara->item.x, uipara->item.y + uipara->item.h * general_attr->new_focus);
    GUI_BMP_Draw(theme_hdl2buf(general_attr->h_item_focus_bmp_right), uipara->item.x, uipara->item.y + uipara->item.h * general_attr->new_focus + 45);
    GUI_BMP_Draw(theme_hdl2buf(general_attr->left_triangle_icon_uf), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * general_attr->new_focus + uipara->left_button.y);
    GUI_BMP_Draw(theme_hdl2buf(general_attr->right_triangle_icon_uf), uipara->item.x + uipara->right_button.x, uipara->item.y + uipara->item.h * general_attr->new_focus + uipara->right_button.y);
    gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
    gui_rect.y0 = uipara->item_title_pos.y + uipara->item.y * general_attr->new_focus;
    gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
    gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
    GUI_SetColor(general_attr->focus_txt_color);
    GUI_DispStringInRect(p_item_res->string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = uipara->item.x + uipara->item_content_pos.x;
    gui_rect.y0 = uipara->item_content_pos.y + uipara->item_content_pos.h * general_attr->new_focus;
    gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
    gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
    GUI_DispStringInRect(p_item_res->string_content, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    //GUI_LyrWinCacheOff();
    return EPDK_OK;
}
#endif

static __s32 setting_general_paint_item_detail_ex(__gui_msg_t *msg)
{
    __s32 i = 0;
    GUI_RECT gui_rect = {0};
    setting_uipara_t *uipara = NULL;
  //  GUI_MEMDEV_Handle   draw_mem = NULL;
    setting_general_attr_t *general_attr = NULL;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        __err("general_attr null %s %d", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    ASSERT((general_attr->new_focus < GENERAL_SETTING_NUM) && (general_attr->new_focus >= GENERAL_SETTING_START));
    uipara = general_attr->uipara;
    GUI_LyrWinSel(general_attr->layer);   
    //draw_mem = GUI_MEMDEV_Create(uipara->string_detail_pos_ex.x, uipara->string_detail_pos_ex.y, uipara->string_detail_pos_ex.w,
    //                            uipara->string_detail_pos_ex.h);
   // GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    //GUI_LyrWinCacheOn();
    GUI_SetBkColor(0xccffffff);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect); 
    GUI_SetColor(general_attr->focus_txt_color);
    __log("%d %d %d %d general_attr->new_focus:%d",gui_rect.x0,gui_rect.y0,gui_rect.x1,gui_rect.y1, general_attr->new_focus);
    GUI_DispStringInRect(general_attr->res[general_attr->new_focus].string_detail_prompt, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
#if 0
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(general_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);
#endif
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
  //  GUI_MEMDEV_CopyToLCD(draw_mem);
  //  GUI_MEMDEV_Select(NULL);
 //   GUI_MEMDEV_Delete(draw_mem);
 //   draw_mem = NULL;
    GUI_LyrWinFlushFB(general_attr->layer);//ljm add.20201102    
   
    return EPDK_OK;
}
static __s32 setting_general_item_paint_ex(__lbar_draw_para_t *draw_param)
{
    RECT rect = {0};
    GUI_RECT gui_rect = {0};
    setting_uipara_t *uipara = NULL;
    setting_general_attr_t *general_attr = NULL ;
    general_attr = (setting_general_attr_t *)draw_param->attr;

    if (general_attr == NULL)
    {
        __err("%s %d general_attr null", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    //  __wrn("general_attr->new_focus:%d", general_attr->new_focus);

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __err("%s %d general_attr->layer GUI_LYRWIN_STA_SUSPEND", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
//    GUI_LyrWinCacheOn();
    uipara = general_attr->uipara;
    //  __wrn("draw_param->mode:%d", draw_param->mode);
    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
//__log("scroll rect %d %d %d %d",uipara->scroll_rect.x,uipara->scroll_rect.y,uipara->scroll_rect.width,uipara->scroll_rect.height);
//__log("long scroll_bg_rect %d %d %d %d",uipara->scroll_bg_rect.x,uipara->scroll_bg_rect.y,uipara->scroll_bg_rect.width,uipara->scroll_bg_rect.height);
//__log("long string rect %d %d %d %d",uipara->long_string_area.x,uipara->long_string_area.y,uipara->long_string_area.w,uipara->long_string_area.h);
            /*draw unfocus right item */
            GUI_BMP_Draw(theme_hdl2buf(general_attr->h_item_unfocus_bmp_right), draw_param->rect.x, draw_param->rect.y);

            if ((draw_param->index != GENERAL_SETTING_FACTORY_DEFAULT) && (draw_param->index != GENERAL_SETTING_SOFT_VERSION))
            {
                /*draw unfocus triangle icon*/
                GUI_BMP_Draw(theme_hdl2buf(general_attr->left_triangle_icon_uf), draw_param->rect.x + uipara->left_button.x, draw_param->rect.y + uipara->left_button.y);
                GUI_BMP_Draw(theme_hdl2buf(general_attr->right_triangle_icon_uf), draw_param->rect.x + uipara->right_button.x, draw_param->rect.y + uipara->right_button.y);
            }

            /*set string color*/
            GUI_SetColor(general_attr->unfocus_txt_color);
            /*draw unfocus right item title string*/
            gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_DispStringInRect(general_attr->res[draw_param->index].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            /*draw focus right item content string*/
            gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
            gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            GUI_DispStringInRect(general_attr->res[draw_param->index].string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            /*draw focus right item */

            if (SETTING_DISPLAY_FOCUS == SETTING_DISPLAY_FOCUS_RIGHT)
            {
                general_attr->new_focus = draw_param->index;
                //bg               
                GUI_BMP_Draw(theme_hdl2buf(general_attr->h_item_focus_bmp_right), draw_param->rect.x, draw_param->rect.y);
//__log("draw_param->rect.x = %d  draw_param->rect.y = %d",draw_param->rect.x,draw_param->rect.y);
//__log("draw_param->rect.w = %d  draw_param->rect.h = %d",draw_param->rect.width,draw_param->rect.height);
                if ((draw_param->index != GENERAL_SETTING_FACTORY_DEFAULT) && (draw_param->index != GENERAL_SETTING_SOFT_VERSION))
                {
                    /*draw unfocus triangle icon*/
                    GUI_BMP_Draw(theme_hdl2buf(general_attr->left_triangle_icon_focus), draw_param->rect.x + uipara->left_button.x, draw_param->rect.y + uipara->left_button.y);
                    GUI_BMP_Draw(theme_hdl2buf(general_attr->right_triangle_icon_focus), draw_param->rect.x + uipara->right_button.x, draw_param->rect.y + uipara->right_button.y);
                }

                /*set string color*/
                GUI_SetColor(general_attr->focus_txt_color);
                /*draw focus right item title string*/
                gui_rect.x0 = uipara->item_title_pos.x + draw_param->rect.x;
                gui_rect.y0 = uipara->item_title_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
                gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
                GUI_DispStringInRect(general_attr->res[draw_param->index].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
                /*draw focus right item content string*/
                gui_rect.x0 = uipara->item_content_pos.x + draw_param->rect.x;
                gui_rect.y0 = uipara->item_content_pos.y + draw_param->rect.y;
                gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
                gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
                GUI_DispStringInRect(general_attr->res[draw_param->index].string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
   
    
            }           
        }
        break;

        default:
            break;
    }
//    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32 setting_general_paint_item_detail(__gui_msg_t *msg)
{
#if 0
    setting_general_attr_t *general_attr;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    setting_item_res_t  *p_item_res;
    setting_uipara_t *uipara = NULL;
    GUI_MEMDEV_Handle   draw_mem;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    _setting_general_content_res_reset(general_attr);
    uipara = (setting_uipara_t *)setting_get_uipara();
    //__msg( "enter setting_general_paint_item_detail  " );
    GUI_LyrWinSel(general_attr->layer);
    draw_mem = GUI_MEMDEV_Create(uipara->string_detail_pos_ex.x, uipara->string_detail_pos_ex.y, uipara->string_detail_pos_de.w,
                                 uipara->string_detail_pos_ex.h + uipara->string_detail_pos_de.h);
    GUI_MEMDEV_Select(draw_mem);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    //GUI_LyrWinCacheOn();
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(general_attr->focus_txt_color);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(general_attr->string_detail_prompt, &gui_rect, GUI_TA_LEFT);
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(general_attr->string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif
    return EPDK_OK;
}

static __s32 _setting_scene_paint(__gui_msg_t *msg, __bool init)
{
    setting_uipara_t *uipara = NULL;
    setting_item_res_t  *p_item_res = NULL;
    setting_general_attr_t *general_attr = NULL;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        __err("general_attr null %s %d", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(general_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        __err("general_attr->layer GUI_LYRWIN_STA_SUSPEND %s %d", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(general_attr->layer);
    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff); //80%的透明度
    GUI_LyrWinCacheOn();
    com_memdev_create(general_attr->layer);
    uipara = general_attr->uipara;
    /*draw the general and power item on the left*/
    GUI_BMP_Draw(theme_hdl2buf(general_attr->h_item_unfocus_bmp_left), uipara->item_general_setting.x, uipara->item_general_setting.y);
    GUI_BMP_Draw(theme_hdl2buf(general_attr->h_left_big_setting_icon), uipara->item_general_setting.x + 60, uipara->item_general_setting.y + 20);
    /*general setting string*/
    //GUI_SetColor(general_attr->unfocus_txt_color);
    //GUI_DispStringAt(general_attr->general_setting_string_prompt, 21 + 46 + 10, 30 + 10);
    com_memdev_delete();
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static void _setting_general_action_button(__gui_msg_t *msg)
{
    setting_item_res_t  *p_item_res = NULL ;
    setting_general_attr_t *general_attr = NULL;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (general_attr == NULL)
    {
        __err("%s %d general_attr null", __FILE__, __LINE__);
        return ;
    }

    p_item_res = &general_attr->res[general_attr->new_focus];

    switch (general_attr->new_focus)
    {
        case GENERAL_SETTING_LANGUAGE:
        {
            //__here__
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            __inf("p_item_res->content_num:%d", p_item_res->content_num);

            if (setting_reg_para != NULL)
            {
                if (p_item_res->content_num == SETTING_LANGUAGE_CHINESES)
                {
                    setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESES;
                }

#if 0
                else if (p_item_res->content_num == SETTING_LANGUAGE_CHINESET)
                {
                    setting_reg_para->language = EPDK_LANGUAGE_ENM_CHINESET;
                }

#endif
                else if (p_item_res->content_num == SETTING_LANGUAGE_ENGLISH)
                {
                    setting_reg_para->language = EPDK_LANGUAGE_ENM_ENGLISH;
                }

                __inf("p_item_res->content_num:%d", p_item_res->content_num);
                dsk_langres_set_type(setting_reg_para->language);
                _setting_general_string_res_restore(general_attr);
                //_setting_scene_paint(msg, EPDK_FALSE);
                LISTBAR_ShowPage(general_attr->h_listbar);
                setting_general_paint_item_detail_ex(msg);
                _setting_set_hbar_title(STRING_SET_TITLE);
            }
        }
        break;

        case GENERAL_SETTING_SET_LCD_BRIGHT:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_LCD_BRIGHT);
            LISTBAR_UpdatePage(general_attr->h_listbar);
            dsk_display_set_lcd_brightness(p_item_res->content_num);
            setting_reg_para->lcd_bright = p_item_res->content_num;
        }
        break;

        case GENERAL_SETTING_SET_LCD_CONSTRACT:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_LCD_CONSTRACT);
            LISTBAR_UpdatePage(general_attr->h_listbar);
            dsk_display_set_lcd_constract(p_item_res->content_num * 10);
            setting_reg_para->lcd_constract = p_item_res->content_num;
        }
        break;

        case GENERAL_SETTING_SET_LCD_SATURATION:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_LCD_SATURATION);
            LISTBAR_UpdatePage(general_attr->h_listbar);
            dsk_display_set_lcd_saturation(p_item_res->content_num * 10);
            setting_reg_para->lcd_saturation = p_item_res->content_num;
        }
        break;

        case GENERAL_SETTING_SET_LCD_ACUTANCE:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_LCD_ACUTANCE);
            LISTBAR_UpdatePage(general_attr->h_listbar);
            dsk_display_set_lcd_edge(p_item_res->content_num);
            setting_reg_para->lcd_hue = p_item_res->content_num;
        }
        break;

        case GENERAL_SETTING_SET_LCD_DEFINITION:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_LCD_DEFINITION);
            LISTBAR_UpdatePage(general_attr->h_listbar);
            dsk_display_set_lcd_detail(p_item_res->content_num * 10);
            setting_reg_para->lcd_detail = p_item_res->content_num;
        }
        break;

        case GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            switch (p_item_res->content_num)
            {
                case SETTING_CLOSE_SCREEN_15S:
                    setting_reg_para->backlightoff = 15;
                    break;

                case SETTING_CLOSE_SCREEN_30S:
                    setting_reg_para->backlightoff = 30;
                    break;

                case SETTING_CLOSE_SCREEN_45S:
                    setting_reg_para->backlightoff = 45;
                    break;

                case SETTING_CLOSE_SCREEN_60S:
                    setting_reg_para->backlightoff = 60;
                    break;

                case SETTING_CLOSE_SCREEN_90S:
                    setting_reg_para->backlightoff = 90;
                    break;

                default:
                    setting_reg_para->backlightoff = 0;
                    break;
            }

            __wrn("setting_reg_para->close_screen:%d", setting_reg_para->backlightoff);
            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_TIMING_CLOSE_SCREEN);
            LISTBAR_UpdatePage(general_attr->h_listbar);

            if (setting_reg_para->backlightoff == 0)
            {
                g_disable_standby();
            }
            else
            {
                g_enable_standby();
            }

            g_set_close_scn_time(setting_reg_para->backlightoff);
        }
        break;

        case GENERAL_SETTING_SET_TIMING_SHUTDOWN:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            switch (p_item_res->content_num)
            {
                case SETTING_TIMING_SHUTDOWN_15MIN:
                {
                    //setting_reg_para->poweroff = 1*60; // change for test
                    setting_reg_para->poweroff = 15 * 60;
                }
                break;

                case SETTING_TIMING_SHUTDOWN_30MIN:
                {
                    setting_reg_para->poweroff = 30 * 60;
                }
                break;

                case SETTING_TIMING_SHUTDOWN_45MIN:
                {
                    setting_reg_para->poweroff = 45 * 60;
                }
                break;

                case SETTING_TIMING_SHUTDOWN_60MIN:
                {
                    setting_reg_para->poweroff = 60 * 60;
                }
                break;

                default:
                {
                    setting_reg_para->poweroff = 0 * 60;
                }
                break;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_SET_TIMING_SHUTDOWN);
            LISTBAR_UpdatePage(general_attr->h_listbar);
            dsk_set_auto_off_time(setting_reg_para->poweroff);
            __wrn("para->poweroff=%d", setting_reg_para->poweroff);
        }
        break;

        case GENERAL_SETTING_BACKGROUND_STYLE:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            if (setting_reg_para != NULL)
            {
                __s32   bg_index = 0;
                bg_index = gscene_bgd_get_default_bg_index();
                __wrn("bg_index:%d", bg_index);

                if (bg_index != p_item_res->content_num)
                {
                    __log("bg_index != p_item_res->content_num");
                    gscene_bgd_set_default_bg_index(p_item_res->content_num);
                }

                setting_reg_para->style = p_item_res->content_num;
            }

            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_BACKGROUND_STYLE);
            LISTBAR_UpdatePage(general_attr->h_listbar);    /////////
        }
        break;

        case GENERAL_SETTING_FACTORY_DEFAULT:           //恢复出厂设置
        {
            LISTBAR_UpdatePage(general_attr->h_listbar);    /////////
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_TIPS, TIPS_FACTORY_DEFAULT, 0);
        }
        break;

        case GENERAL_SETTING_KEYTONE:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

#if 0

            if (p_item_res->content_num == SETTING_KETTONE_OFF)
            {
                setting_reg_para->keytone = SETTING_KETTONE_ON;
            }
            else
            {
                setting_reg_para->keytone = SETTING_KETTONE_OFF;
            }

#else
            setting_reg_para->keytone = p_item_res->content_num;
#endif
            dsk_keytone_set_state((__set_keytone_t)(setting_reg_para->keytone));
            dsk_keytone_on();
            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_KEYTONE);
            LISTBAR_UpdatePage(general_attr->h_listbar);     /////////
        }
        break;

        case GENERAL_SETTING_OUTPUT_TYPE:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            setting_reg_para->pal_ntsc = p_item_res->content_num;
            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_OUTPUT_TYPE);
            LISTBAR_UpdatePage(general_attr->h_listbar);     /////////
        }
        break;

        case GENERAL_SETTING_UPDATE:
        {
            __s32 i = 0;
            char *str_root[] = {"f", "g", "h", "i", "j", "k", "l", "m", "n"};
            char filename[1024];
            __wrn("begin fw update....");
            eLIBs_memset(filename, 0, sizeof(filename));

            for (i = 0 ; i < BEETLES_TBL_SIZE(str_root) ; i++)
            {
                eLIBs_strcpy(filename, str_root[i]);
                eLIBs_strcat(filename, ":\\ePDKv100.img");
                __wrn("filename=%s", filename);

                if (EPDK_TRUE == is_file_exist(filename))
                {
                    break;
                }
            }

            if (i < BEETLES_TBL_SIZE(str_root))
            {
                dsk_start_fw_update(filename);
            }
        }
        break;

        case GENERAL_SETTING_BACK_LIGHT_ONOFF:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

#if 0

            if (p_item_res->content_num == SETTING_BACK_LIGHT_OFF)
            {
                setting_reg_para->backlightoff = p_item_res->content_num;
            }
            else
            {
                setting_reg_para->backlightoff = SETTING_BACK_LIGHT_OFF;
            }

#else
            //setting_reg_para->backlightoff = p_item_res->content_num;
#endif
            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_BACK_LIGHT_ONOFF);
            LISTBAR_UpdatePage(general_attr->h_listbar);     /////////
        }
        break;

        case GENERAL_SETTING_BACK_LIGHT_BRIGHT:
        {
            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                //p_item_res->content_num = 0;
                p_item_res->content_num = SETTING_BACK_LIGHT_3_LEVEL;
            }
            else
            {
                p_item_res->content_num++;
            }

            setting_reg_para->backlight = p_item_res->content_num;
            _setting_general_get_content_string(p_item_res, GENERAL_SETTING_BACK_LIGHT_BRIGHT);
            LISTBAR_UpdatePage(general_attr->h_listbar);       /////////
            dsk_display_set_lcd_brightness((__lion_bright_t)(setting_reg_para->backlight + 1)); //背光亮度
            gscene_hbar_update_text(HBAR_BRIGHT_PAINT);
        }
        break;

        default:
            break;
    }

    return ;
}
static void _setting_focus_on_right_button(__gui_msg_t *msg)
{
    __inf("%s %d will in _setting_general_action_button ", __FILE__, __LINE__);
    _setting_general_action_button(msg);
}

static __s32 is_file_exist(char *filename)
{
    ES_FILE *fp;
    fp = eLIBs_fopen(filename, "rb");

    if (fp)
    {
        eLIBs_fclose(fp);
        return EPDK_TRUE;
    }

    return EPDK_FALSE;
}

static __s32 _setting_general_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    setting_general_attr_t *general_attr = NULL;
    general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    //__here__
    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_PREV:
        case GUI_MSG_KEY_LONGPREV:
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                if (SETTING_DISPLAY_FOCUS == SETTING_DISPLAY_FOCUS_RIGHT)
                {
                    //__here__
                    general_attr->old_focus = general_attr->new_focus;
                    LISTBAR_PrevItem(general_attr->h_listbar);
                    setting_general_paint_item_detail_ex(msg);
                }
            }
        }
        break;

        case GUI_MSG_KEY_NEXT:
        case GUI_MSG_KEY_LONGNEXT:
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                //__here__
                if (SETTING_DISPLAY_FOCUS_RIGHT == SETTING_DISPLAY_FOCUS)
                {
                    general_attr->old_focus = general_attr->new_focus;
                    LISTBAR_NextItem(general_attr->h_listbar);
                    setting_general_paint_item_detail_ex(msg);
                }
            }
        }
        break;

        case GUI_MSG_KEY_PLAY_PAUSE:
        case GUI_MSG_KEY_ENTER:
        {
            //__here__
            if (msg->dwAddData2 == KEY_UP_ACTION && SETTING_DISPLAY_FOCUS_RIGHT == SETTING_DISPLAY_FOCUS)
            {
                //__here__
                general_attr->new_focus = LISTBAR_GetFocusItem(general_attr->h_listbar);
                _setting_focus_on_right_button(msg);
            }
        }
        break;

        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_LONGESCAPE:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                __gui_msg_t mymsg;
                mymsg.id = GUI_MSG_CLOSE;
                mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                GUI_SendMessage(&mymsg);
            }
        }
        break;

        default:
            break;
    }

    return ret;
}

/*
    回调
*/
static __s32 _setting_general_Proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            setting_general_attr_t *general_attr = NULL;
            setting_general_para_t *general_para = NULL;
            __inf("%s %d======_setting_general_Proc=======GUI_MSG_CREATE", __FILE__, __LINE__);
            bAlreadly_draw = EPDK_FALSE;
            setting_reg_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
            general_para = (setting_general_para_t *)GUI_WinGetAttr(msg->h_deswin);
            general_attr = (setting_general_attr_t *)My_Malloc(0, sizeof(setting_general_attr_t));

            if (NULL == general_attr)
            {
                __msg("memory alloc fail.........");
                return EPDK_FAIL;
            }

            eLIBs_memset(general_attr, 0x00, sizeof(setting_general_attr_t));
            general_attr->new_focus = general_para->focus_id;
            general_attr->old_focus = general_para->focus_id;
            general_attr->general_para = general_para;
            setting_general_init_content(general_attr);//初始化 语言种类 风格 制式 按键音
            _setting_general_res_init(general_attr);//获得图片资源
            general_attr->focus_txt_color = SETTING_FOCUS_COLOR;
            general_attr->unfocus_txt_color = SETTING_UNFOCUS_COLOR;
            general_attr->font = general_para->font;
            general_attr->layer = general_para->layer;
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)general_attr);//把私有数据加进去
            GUI_LyrWinSetSta(general_attr->layer, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSetTop(general_attr->layer);
            {
                __gui_msg_t msgex = {0};
                msgex.id = GUI_MSG_PAINT;
                msgex.h_deswin = msg->h_deswin;
                GUI_SendMessage(&msgex);//同步消息
            }
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            //__here__;
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __s32 ret = 0;
            setting_general_attr_t *general_attr = NULL;
            setting_general_para_t *general_para = NULL;
            general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (NULL == general_attr)
            {
                __err("general_attr is null ......");
                return EPDK_FAIL;
            }

            ret = LISTBAR_SuspendPage(general_attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar suspend fail");
            }

            ret = LISTBAR_Delete(general_attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar delete fail");
            }

            general_attr->h_listbar = NULL;
            _setting_general_res_uninit(general_attr);
            //dsk_reg_flush();  //写进文件里面
            general_para = general_attr->general_para;

            if (NULL != general_para)
            {
                __inf("My_Mfree general_para");
                My_Mfree(0, general_para);
            }

            if (NULL != general_attr)
            {
                __inf("My_Bfree general_attr");
                My_Mfree(0, general_attr);
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            setting_general_attr_t *general_attr = NULL;
            __inf("%s %d======SETTING_GENERAL=======GUI_MSG_PAINT", __FILE__, __LINE__);
            general_attr = (setting_general_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (general_attr == NULL)
            {
                __err("general_attr null %s %d", __FILE__, __LINE__);
                return EPDK_FAIL;
            }

            __inf("%s %d bAlreadly_draw", __FILE__, __LINE__, bAlreadly_draw);

            if (!bAlreadly_draw)
            {
                bAlreadly_draw = EPDK_TRUE;
                _setting_general_listbar_init(msg);
                //LISTBAR_LostFocus(general_attr->h_listbar);
                _setting_scene_paint(msg, EPDK_TRUE);
                setting_general_paint_item_detail_ex(msg);
            }

            _setting_set_hbar_title(STRING_SET_TITLE);
            gscene_hbar_set_state(HBAR_ST_SHOW);
            GUI_LyrWinSetSta(general_attr->layer, GUI_LYRWIN_STA_ON);
        }

        return EPDK_OK;

        case GUI_MSG_KEY:
        {
            __inf("");
            _setting_general_listbar_key_proc(msg);
            return EPDK_OK;
        }

        case MSG_RESTORE_FACTORY:
        {
            __log("xxxxxx MSG_RESTORE_FACTORY");
            setting_restore_default(msg);
        }

        return EPDK_OK;

        case MSG_OP_EXIT:
        {
            __wrn("send MSG_OP_EXIT ok.......");
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_GENERAL, 0, 0);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_general_win_create(H_WIN h_parent, setting_general_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_general_para_t  *general_para = NULL;
    FB fb;
    __inf("*********enter setting_general_win_create *****************");
    eLIBs_memset(&fb, 0, sizeof(fb));
    GUI_LyrWinGetFB(para->layer, &fb);
    general_para = (setting_general_para_t *)My_Malloc(0, sizeof(setting_general_para_t));

    if (NULL == general_para)
    {
        __msg("memory alloc fail.........");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset((void *)general_para, 0, sizeof(setting_general_para_t));
    general_para->font = para->font;
    general_para->layer = para->layer;
    general_para->focus_id = para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "general setting win",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = GENERAL_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_general_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)general_para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 setting_general_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}

