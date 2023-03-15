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
#include <log.h>
#include "setting_power.h"
#include "setting_uipara.h"

#define   ITEM_NUM_PER_SCREEN   4   //每屏能显示item的个数
extern __s32 key_focous_left_right;

typedef enum
{
    BACKLIGHTOFF_NEVER,
    BACKLIGHTOFF_10SECOND,
    BACKLIGHTOFF_30SECOND,
    BACKLIGHTOFF_60SECOND,
    BACKLIGHTOFF_90SECOND,
    BACKLIGHTOFF_MAX,
} backlightoff_t;
typedef enum
{
    POWEROFF_NEVER,
    POWEROFF_3MINUTE,
    POWEROFF_5MINUTE,
    POWEROFF_10MINUTE,
    POWEROFF_15MINUTE,
    POWEROFF_MAX,
} poweroff_t;

typedef struct tag_power_item_id
{
    //__s32 id_string_detail_prompt;
    //__s32 id_string_detail_prompt_ex;
    __s32 id_string_title;
    __s32 id_string_content;
    __s32  id_bmp_left_uf;
    __s32  id_bmp_right_uf;
    __s32  id_bmp_left_f;//byf
    __s32  id_bmp_right_f;//    byf
} power_item_id_t;

typedef struct tag_power_item_res
{
    char string_title[128];
    char string_content[128];

    HTHEME h_bmp_item_focus;
    HTHEME h_bmp_item_unfocus;
    HTHEME h_bmp_left_button_uf;
    HTHEME h_bmp_right_button_uf;

    HTHEME h_bmp_left_button_f;
    HTHEME h_bmp_right_button_f;

    __s32 content_num;
    __s32 content_nr;

    //char  string_detail_prompt[128];      //详细信息
    //char string_detail_prompt_ex[128];

} power_item_res_t ;

typedef struct tag_power_focus_item_id
{
    //__s32  id_string_detail_prompt;
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} power_focus_item_id_t;

typedef struct tag_setting_power_attr
{
    setting_power_para_t *power_para;

    H_WIN   layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;

    char  string_prompt_power[128];
    char  string_prompt_general[128];

    power_item_res_t    power_item_res[POWER_ITEM_MAX_ID];
    res_theme_t res_theme[POWER_THEME_MAX_ID] ;
    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    __u32   checkTimerID;

} setting_power_attr_t;
extern void  main_cmd2parent(H_WIN hwin, __s32 id, unsigned long data1, unsigned long data2);

static reg_system_para_t *setting_reg_para;

static __s32 content_backlight_level_id[] = //ITEM1 (背光亮度)
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
    STRING_SET_POWER_BGT_LEVEL16_CON
};

static __s32 content_backlight_off_id[] = //ITEM2 (关闭背光)
{
    STRING_SET_POWER_BGT_NEVER_CON,
    STRING_SET_POWER_BGT_LAST10S_CON,
    STRING_SET_POWER_BGT_LAST30S_CON,
    STRING_SET_POWER_BGT_LAST60S_CON,
    STRING_SET_POWER_BGT_LAST90S_CON
};

static __s32 content_power_off_id[] = //ITEM3 (关闭电源)
{
    STRING_SET_POWER_OFF_NEVER_CON,
    STRING_SET_POWER_OFF_LAST10M_CON,       //10 分
    STRING_SET_POWER_OFF_LAST20M_CON,       // 20 分
    STRING_SET_POWER_OFF_LAST30M_CON,       // 30 分
    STRING_SET_POWER_OFF_LAST1H_CON         // 1 小时
};
#if 0
static power_focus_item_id_t power_focus_item_res_id[] =
{
    { STRING_SET_POWER_BGT_EX, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP}, // 0
    { STRING_SET_POWER_BGT_OFF_EX, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP}, // 1
    { STRING_SET_POWER_OFF_EX, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP},    // 2
};
#endif
static power_focus_item_id_t power_focus_item_res_id[] =
{
    { ID_SETTING_NEW_ITEM_R_FCS_BMP, ID_SETTING_NEW_ITEM_R_UFCS_BMP},   // 0
    { ID_SETTING_NEW_ITEM_R_FCS_BMP, ID_SETTING_NEW_ITEM_R_UFCS_BMP},   // 1
    { ID_SETTING_NEW_ITEM_R_FCS_BMP, ID_SETTING_NEW_ITEM_R_UFCS_BMP},       // 2
};

////////////////////////////////////////////////////////
static power_item_id_t setting_power_res_id[] =
{
    {STRING_SET_POWER_BGT_TITLE, STRING_SET_POWER_BGT_LEVEL1_CON, ID_SETTING_NEW_SET_BUTT_L_UF_BMP, ID_SETTING_NEW_SET_BUTT_R_UF_BMP, ID_SETTING_NEW_SET_BUTT_L_F_BMP, ID_SETTING_NEW_SET_BUTT_R_F_BMP}, // 0
    {STRING_SET_POWER_BGT_OFF_TITLE, STRING_SET_POWER_BGT_NEVER_CON, ID_SETTING_NEW_SET_BUTT_L_UF_BMP, ID_SETTING_NEW_SET_BUTT_R_UF_BMP, ID_SETTING_NEW_SET_BUTT_L_F_BMP, ID_SETTING_NEW_SET_BUTT_R_F_BMP}, // 1
    {STRING_SET_POWER_OFF_TITLE, STRING_SET_POWER_OFF_NEVER_CON, ID_SETTING_NEW_SET_BUTT_L_UF_BMP, ID_SETTING_NEW_SET_BUTT_R_UF_BMP, ID_SETTING_NEW_SET_BUTT_L_F_BMP, ID_SETTING_NEW_SET_BUTT_R_F_BMP}, // 2
};

/*由触摸的位置获得当前item的下标*/

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


static __s32 power_TouchPos_2_ItemIndex(__gui_msg_t *msg)
{
    __s32 pos_x, pos_y;
    __s32 item_index, tmp_index_y;
    setting_power_attr_t *power_attr;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);
    tmp_index_y = (pos_y - SET_ITEM_START_Y) / SET_ITEM_H;
    __msg("pos_x=%d pos_y=%d temp_index_y=%d\n", pos_x, pos_y, tmp_index_y);

    if (((pos_x >= SET_POWER_ITEM_START_X) && (pos_x <= SET_POWER_ITEM_START_X + SET_POWER_ITEM_W))
        && ((pos_y >= SET_POWER_ITEM_START_Y) && (pos_y <= SET_POWER_ITEM_START_Y + SET_ITEM_H * power_attr->item_nr)))
    {
        item_index = tmp_index_y;
        return item_index;
    }
    else
    {
        //__here__;
        return EPDK_FAIL;   //无效区域
    }
}

/*
    申请资源
*/
static void _setting_power_res_init(setting_power_attr_t *power_attr)
{
    __u32   i;
    power_item_res_t  *p_item_res = NULL ;
    power_attr->item_nr = sizeof(setting_power_res_id) / sizeof(setting_power_res_id[0]);
    get_menu_text(STRING_SET_PROMPT_COMMON, power_attr->string_prompt_general, sizeof(power_attr->string_prompt_general));
    get_menu_text(STRING_SET_PROMPT_POWER_MANAGER, power_attr->string_prompt_power, sizeof(power_attr->string_prompt_power));
#if 0
    power_attr->res_theme[POWER_THEME_SETTING_ICON_BMP_ID].bmp_id = ID_SETTING_SET_BSYSSE_BMP ;
    power_attr->res_theme[POWER_THEME_BG_BMP_ID].bmp_id = ID_SETTING_WINDOW_BG_BMP ;
    power_attr->res_theme[POWER_THEME_RETURN_FCS_BMP_ID].bmp_id = ID_SETTING_RETURN_FCS_BMP ;
    power_attr->res_theme[POWER_THEME_RETURN_UNFCS_BMP_ID].bmp_id = ID_SETTING_RETURN_UFCS_BMP ;
#endif
    /////////////////////////////////////////////////////////////////////////////////////BYF
    power_attr->res_theme[NEW_SET_FLAG_UF_BMP].bmp_id = ID_SETTING_NEW_SET_FLAG_UF_BMP ;
    power_attr->res_theme[NEW_SET_FLAG_F_BMP].bmp_id = ID_SETTING_NEW_SET_FLAG_F_BMP ;
    power_attr->res_theme[NEW_POWER_FLAG_UF_BMP].bmp_id = ID_SETTING_NEW_POWER_FLAG_UF_BMP ;
    power_attr->res_theme[NEW_POWER_FLAG_F_BMP].bmp_id = ID_SETTING_NEW_POWER_FLAG_F_BMP;
    power_attr->res_theme[NEW_SCROLL_BG_BMP].bmp_id = ID_SETTING_NEW_SCROLL_BG_BMP ;
    power_attr->res_theme[NEW_SCROLL_BLOCK_BMP].bmp_id = ID_SETTING_NEW_SCROLL_BLOCK_BMP ;
    power_attr->res_theme[NEW_ITEM_L_FCS_BMP].bmp_id = ID_SETTING_NEW_ITEM_L_FCS_BMP ;
    power_attr->res_theme[NEW_ITEM_L_UFCS_BMP].bmp_id = ID_SETTING_NEW_ITEM_L_UFCS_BMP ;

    for (i = 0 ; i < POWER_THEME_MAX_ID ; i++)
    {
        if (power_attr->res_theme[i].bmp_id == 0)
        {
            continue ;
        }

        power_attr->res_theme[i].bmp_handle = theme_open(power_attr->res_theme[i].bmp_id);
        power_attr->res_theme[i].p_buf = theme_hdl2buf(power_attr->res_theme[i].bmp_handle);
    }

    for (i = 0 ; i < power_attr->item_nr  ; i++)
    {
        power_attr->power_item_res[i].h_bmp_left_button_uf = theme_open(setting_power_res_id[i].id_bmp_left_uf) ;
        power_attr->power_item_res[i].h_bmp_right_button_uf = theme_open(setting_power_res_id[i].id_bmp_right_uf) ;
        power_attr->power_item_res[i].h_bmp_left_button_f = theme_open(setting_power_res_id[i].id_bmp_left_f) ;
        power_attr->power_item_res[i].h_bmp_right_button_f = theme_open(setting_power_res_id[i].id_bmp_right_f) ;
        power_attr->power_item_res[i].h_bmp_item_focus = theme_open(power_focus_item_res_id[i].id_bmp_focus_item) ;
        power_attr->power_item_res[i].h_bmp_item_unfocus = theme_open(power_focus_item_res_id[i].id_bmp_unfocus_item) ;
        //get_menu_text( setting_power_res_id[i].id_string_detail_prompt, power_attr->power_item_res[i].string_detail_prompt , sizeof(power_attr->power_item_res[i].string_detail_prompt) ) ;
        //get_menu_text( setting_power_res_id[i].id_string_detail_prompt_ex, power_attr->power_item_res[i].string_detail_prompt_ex, sizeof(power_attr->power_item_res[i].string_detail_prompt_ex) ) ;
        get_menu_text(setting_power_res_id[i].id_string_title, power_attr->power_item_res[i].string_title, sizeof(power_attr->power_item_res[i].string_title)) ;
    }

    for (i = 0; i < power_attr->item_nr; i++)
    {
        switch (i)
        {
            case 0:
            {
                p_item_res = &(power_attr->power_item_res[i]);

                if (setting_reg_para)
                {
                    if ((setting_reg_para->backlight > 15) || (setting_reg_para->backlight < 0))
                    {
                        p_item_res->content_num = 10;
                    }
                    else
                    {
                        p_item_res->content_num = setting_reg_para->backlight;
                    }
                }
                else
                {
                    p_item_res->content_num = 10;
                }

                get_menu_text(content_backlight_level_id[p_item_res->content_num], power_attr->power_item_res[i].string_content, sizeof(power_attr->power_item_res[i].string_content)) ;
                power_attr->power_item_res[i].content_nr = sizeof(content_backlight_level_id) / sizeof(__s32);
                break;
            }

            case 1:
            {
                p_item_res = &(power_attr->power_item_res[i]);

                if (setting_reg_para)
                {
                    switch (setting_reg_para->backlightoff)
                    {
                        case 0:
                            p_item_res->content_num = 0;
                            break;

                        case 10:
                            p_item_res->content_num = 1;
                            break;

                        case 30:
                            p_item_res->content_num = 2;
                            break;

                        case 60:
                            p_item_res->content_num = 3;
                            break;

                        case 90:
                            p_item_res->content_num = 4;
                            break;

                        default:
                            p_item_res->content_num = 0;
                            break;
                    }
                }
                else
                {
                    p_item_res->content_num = 0;
                }

                get_menu_text(content_backlight_off_id[p_item_res->content_num], power_attr->power_item_res[i].string_content, sizeof(power_attr->power_item_res[i].string_content)) ;
                power_attr->power_item_res[i].content_nr = sizeof(content_backlight_off_id) / sizeof(__s32);
            }
            break;

            case 2:
            {
                p_item_res = &(power_attr->power_item_res[i]);

                if (setting_reg_para)
                {
                    switch (setting_reg_para->poweroff)
                    {
                        case 0:
                            p_item_res->content_num = 0;
                            break;

                        case 3*60:
                            p_item_res->content_num = 1;
                            break;

                        case 5*60:
                            p_item_res->content_num = 2;
                            break;

                        case 10*60:
                            p_item_res->content_num = 3;
                            break;

                        case 15*60:
                            p_item_res->content_num = 4;
                            break;

                        default:
                            p_item_res->content_num = 0;
                            break;
                    }
                }
                else
                {
                    p_item_res->content_num = 0;
                }

                get_menu_text(content_power_off_id[p_item_res->content_num], power_attr->power_item_res[i].string_content, sizeof(power_attr->power_item_res[i].string_content)) ;
                power_attr->power_item_res[i].content_nr = sizeof(content_power_off_id) / sizeof(__s32);
                break;
            }

            default:
                break;
        }
    }
}

static void _setting_power_res_uninit(setting_power_attr_t *power_attr)
{
    __u32   i;
    power_item_res_t  *p_item_res;

    for (i = 0 ; i < POWER_THEME_MAX_ID ; i++)
    {
        if (power_attr->res_theme[i].bmp_id == 0)
        {
            continue ;
        }

        theme_close(power_attr->res_theme[i].bmp_handle);
        power_attr->res_theme[i].bmp_handle = NULL ;
        power_attr->res_theme[i].p_buf = NULL ;
    }

    for (i = 0; i < power_attr->item_nr; i++)
    {
        theme_close(power_attr->power_item_res[i].h_bmp_item_focus);
        power_attr->power_item_res[i].h_bmp_item_focus = NULL ;
        theme_close(power_attr->power_item_res[i].h_bmp_item_unfocus);
        power_attr->power_item_res[i].h_bmp_item_unfocus = NULL ;
        theme_close(power_attr->power_item_res[i].h_bmp_left_button_uf);
        power_attr->power_item_res[i].h_bmp_left_button_uf = NULL ;
        theme_close(power_attr->power_item_res[i].h_bmp_right_button_uf);
        power_attr->power_item_res[i].h_bmp_right_button_uf = NULL ;
        theme_close(power_attr->power_item_res[i].h_bmp_left_button_f);
        power_attr->power_item_res[i].h_bmp_left_button_f = NULL ;
        theme_close(power_attr->power_item_res[i].h_bmp_right_button_f);
        power_attr->power_item_res[i].h_bmp_right_button_f = NULL ;
    }
}

static void _setting_power_content_res_reset(setting_power_attr_t *power_attr)
{
    __u32   i, *content_id = NULL;
    power_item_res_t  *p_item_res;
    p_item_res = &(power_attr->power_item_res[power_attr->new_focus]) ;

    switch (power_attr->new_focus)
    {
        case 0:
            content_id = content_backlight_level_id;
            break;

        case 1:
            content_id = content_backlight_off_id;
            break;

        case 2:
            content_id = content_power_off_id;
            break;

        default:
            break;
    }

    if (content_id)
    {
        get_menu_text(content_id[p_item_res->content_num], p_item_res->string_content, sizeof(p_item_res->string_content));
    }
}
static __s32 setting_power_content_paint(__gui_msg_t *msg)
{
#if 1
    setting_power_attr_t *power_attr;
    power_item_res_t  *p_item_res;
    RECT rect;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara = NULL;
    __s32   ret ;
    __s32 x, y, x0, y0, x1, y1 ;
    GUI_MEMDEV_Handle   draw_mem;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (power_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(power_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(power_attr->layer);
    GUI_SetFont(power_attr->font);
    GUI_UC_SetEncodeUTF8();
    //GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(power_attr->focus_txt_color);
    //  GUI_LyrWinCacheOn();
    _setting_power_content_res_reset(power_attr);
    uipara = (setting_uipara_t *)setting_get_uipara();
    draw_mem = GUI_MEMDEV_Create(uipara->item.x, uipara->item.y + uipara->item.h * power_attr->new_focus, uipara->item.w, uipara->item.h) ;
    GUI_MEMDEV_Select(draw_mem);

    //x = uipara->item_content_pos.x+uipara->item.x ;
    //y = ( uipara->item_content_pos.y + 2 )+uipara->item.h*power_attr->new_focus ;
    //x0 = uipara->item_content_pos.x ;
    //y0 = uipara->item_content_pos.y + 2 ;
    //x1 = uipara->item_content_pos.x + uipara->item_content_pos.w - 1 ;
    //y1 = uipara->item_content_pos.y + uipara->item_content_pos.h  - 2 ;

    //ret = APP_BMP_DrawEx( power_attr->layer, theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus) , x , y , x0 , y0 , x1 , y1 );
    //if( EPDK_FAIL == ret )
    //{
    //  __msg( " app_bmp_drawex fail \n" );
    //}
    //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_unfocus) , uipara->item.x,uipara->item.y+uipara->item.h*power_attr->new_focus);
    if (key_focous_left_right)
    {
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus), uipara->item.x, uipara->item.y + uipara->item.h * power_attr->new_focus);
        //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus) , uipara->item.x,uipara->item.y+uipara->item.h*power_attr->new_focus+45);
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_f), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * power_attr->new_focus + uipara->left_button.y);
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_f), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * power_attr->new_focus + uipara->right_button.y);
    }
    else
    {
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * power_attr->new_focus);
        //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus) , uipara->item.x,uipara->item.y+uipara->item.h*power_attr->new_focus+45);
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_uf), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * power_attr->new_focus + uipara->left_button.y);
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_uf), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * power_attr->new_focus + uipara->right_button.y);
    }

    gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
    gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * power_attr->new_focus;
    gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
    gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
    GUI_SetColor(power_attr->focus_txt_color);
    GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = uipara->item_content_pos.x + uipara->item.x ;
    gui_rect.y0 = uipara->item_content_pos.y + uipara->item_content_pos.h * power_attr->new_focus ;
    gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w - 1 ;
    gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h - 1 ;
    GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_content, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    //  GUI_LyrWinCacheOff();
    return EPDK_OK;
#endif
}

static __s32 setting_power_item_paint(__gui_msg_t *msg)
{
#if 1
    setting_power_attr_t *power_attr;
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara = NULL;
    GUI_MEMDEV_Handle   draw_mem;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (power_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(power_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    __msg("enten setting_power_item_paint ......\n");
    GUI_LyrWinSel(power_attr->layer);
    GUI_SetFont(power_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(power_attr->focus_txt_color);
    //GUI_LyrWinCacheOn();
    uipara = (setting_uipara_t *)setting_get_uipara();
    draw_mem = GUI_MEMDEV_Create(uipara->item.x, uipara->item.y, uipara->item.w, uipara->item.h * 3);
    GUI_MEMDEV_Select(draw_mem);
#if 0
    draw_mem = GUI_MEMDEV_Create(uipara->string_detail_pos_ex.x, uipara->string_detail_pos_ex.y, uipara->string_detail_pos_de.w,
                                 uipara->string_detail_pos_ex.h + uipara->string_detail_pos_de.h);
    GUI_MEMDEV_Select(draw_mem);
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_detail_prompt, &gui_rect, GUI_TA_LEFT) ;
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w - 1 ;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h - 1 ;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
#endif

    for (i = 0; i < power_attr->item_nr; i++)
    {
        if (i == power_attr->new_focus)
        {
            //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_unfocus) , uipara->item.x,uipara->item.y+uipara->item.h*i);
            //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus) , uipara->item.x,uipara->item.y+uipara->item.h*i);
            if (key_focous_left_right)
            {
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus), uipara->item.x, uipara->item.y + uipara->item.h * i);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_f), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * i + uipara->left_button.y);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_f), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * i + uipara->right_button.y);
            }
            else
            {
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_uf), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * i + uipara->left_button.y);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_uf), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * i + uipara->right_button.y);
            }

            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(power_attr->focus_txt_color);
            GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_content_pos.y + uipara->item_content_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        else if (i == power_attr->old_focus)
        {
            GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->old_focus].h_bmp_item_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_uf), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * i + uipara->left_button.y);
            GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_uf), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * i + uipara->right_button.y);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(power_attr->unfocus_txt_color);
            GUI_DispStringInRect(power_attr->power_item_res[power_attr->old_focus].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_content_pos.y + uipara->item_content_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            GUI_DispStringInRect(power_attr->power_item_res[power_attr->old_focus].string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
    }

    //GUI_LyrWinCacheOff();
    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
#endif
}

static __s32 setting_power_paint(__gui_msg_t *msg, __s32 index)
{
    setting_power_attr_t *power_attr;
    power_item_res_t  *p_item_res;
    __s32 i ;
    RECT rect;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara = NULL;
    GUI_MEMDEV_Handle   draw_mem;
    __s32 screen_width;
    __s32 screen_height;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (power_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(power_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    __msg("enten setting power paint ......\n");
    GUI_LyrWinSel(power_attr->layer);
    GUI_SetFont(power_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(power_attr->focus_txt_color);
    dsk_display_get_size(&screen_width, &screen_height);
    __wrn("====screen_width=%d,screen_height=%d\n", screen_width, screen_height);
    draw_mem = GUI_MEMDEV_Create(0, 0, screen_width, screen_height);

    if (draw_mem)
    {
        GUI_MEMDEV_Select(draw_mem);
    }

    //GUI_LyrWinCacheOn();
    uipara = (setting_uipara_t *)setting_get_uipara();

    switch (index)
    {
        case ID_PAINTING_INIT_POWER:
            __log("======setting_power_paint=======ID_PAINTING_INIT_POWER=%d\n");
            GUI_BMP_Draw(power_attr->res_theme[NEW_SCROLL_BG_BMP].p_buf, 300, 0);
            GUI_BMP_Draw(power_attr->res_theme[NEW_SCROLL_BLOCK_BMP].p_buf, 300, 0);
            GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_UFCS_BMP].p_buf, uipara->item_general_setting.x, uipara->item_general_setting.y);

            if (!key_focous_left_right)
            {
                GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_FCS_BMP].p_buf, uipara->item_power_manager.x, uipara->item_power_manager.y) ;
                GUI_BMP_Draw(power_attr->res_theme[NEW_POWER_FLAG_F_BMP].p_buf, 21, uipara->item.h + 30);
            }
            else
            {
                GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_UFCS_BMP].p_buf, uipara->item_power_manager.x, uipara->item_power_manager.y) ;
                GUI_BMP_Draw(power_attr->res_theme[NEW_POWER_FLAG_UF_BMP].p_buf, 21, uipara->item.h + 30);
            }

            GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_UFCS_BMP].p_buf, 0, 2 * uipara->item.h);
            GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_UFCS_BMP].p_buf, 0, 3 * uipara->item.h);
            GUI_BMP_Draw(power_attr->res_theme[NEW_SET_FLAG_UF_BMP].p_buf, 21, 30);
            GUI_SetColor(power_attr->unfocus_txt_color);
            GUI_DispStringAt(power_attr->string_prompt_general, 21 + 46 + 10, 30 + 10);
            GUI_SetColor(power_attr->focus_txt_color);
            GUI_DispStringAt(power_attr->string_prompt_power, 21 + 46 + 10, uipara->item.h + 30 + 10);
            break;

        case ID_PAINTING_F_GENERAL:
            GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_FCS_BMP].p_buf, uipara->item_general_setting.x, uipara->item_general_setting.y);
            GUI_BMP_Draw(power_attr->res_theme[NEW_ITEM_L_UFCS_BMP].p_buf, uipara->item_power_manager.x, uipara->item_power_manager.y) ;
            GUI_BMP_Draw(power_attr->res_theme[NEW_SET_FLAG_F_BMP].p_buf, 21, 30);
            GUI_BMP_Draw(power_attr->res_theme[NEW_POWER_FLAG_UF_BMP].p_buf, 21, uipara->item.h + 30);
            GUI_SetColor(power_attr->focus_txt_color);
            GUI_DispStringAt(power_attr->string_prompt_general, 21 + 46 + 10, 30 + 10);
            GUI_SetColor(power_attr->unfocus_txt_color);
            GUI_DispStringAt(power_attr->string_prompt_power, 21 + 46 + 10, uipara->item.h + 30 + 10);
            // GUI_LyrWinCacheOff();
            GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);

            if (draw_mem)
            {
                GUI_MEMDEV_CopyToLCD(draw_mem);
                GUI_MEMDEV_Select(NULL);
                GUI_MEMDEV_Delete(draw_mem);
                draw_mem = NULL;
            }

            return EPDK_OK;
    }

    //GUI_SetColor(0);
    //GUI_DispStringAt(power_attr->string_prompt, uipara->txt_pos.x,uipara->txt_pos.y); // 显示电源管理图标
    //GUI_SetColor(power_attr->focus_txt_color);
#if 0
    gui_rect.x0 = uipara->string_detail_pos_ex.x;
    gui_rect.y0 = uipara->string_detail_pos_ex.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_ex.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_ex.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_detail_prompt, &gui_rect, GUI_TA_LEFT);
    gui_rect.x0 = uipara->string_detail_pos_de.x;
    gui_rect.y0 = uipara->string_detail_pos_de.y;
    gui_rect.x1 = gui_rect.x0 + uipara->string_detail_pos_de.w;
    gui_rect.y1 = gui_rect.y0 + uipara->string_detail_pos_de.h;
    GUI_ClearRectEx(&gui_rect);
    GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_detail_prompt_ex, &gui_rect, GUI_TA_LEFT);
#endif

    for (i = 0; i < power_attr->item_nr; i++)
    {
        if (i == power_attr->new_focus)
        {
            //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_unfocus) , uipara->item.x,uipara->item.y+uipara->item.h*i);
            if (key_focous_left_right)
            {
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus), uipara->item.x, uipara->item.y + uipara->item.h * i);
                //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus) , uipara->item.x,uipara->item.y+uipara->item.h*i+45);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_f), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * i + uipara->left_button.y);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_f), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * i + uipara->right_button.y);
            }
            else
            {
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
                //GUI_BMP_Draw( theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_item_focus) , uipara->item.x,uipara->item.y+uipara->item.h*i+45);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_uf), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * i + uipara->left_button.y);
                GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_uf), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * i + uipara->right_button.y);
            }

            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(power_attr->focus_txt_color);
            GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_content_pos.y + uipara->item_content_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            GUI_DispStringInRect(power_attr->power_item_res[power_attr->new_focus].string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        else
        {
            GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[i].h_bmp_item_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_left_button_uf), uipara->item.x + uipara->left_button.x, uipara->item.y + uipara->item.h * i + uipara->left_button.y);
            GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[power_attr->new_focus].h_bmp_right_button_uf), uipara->item.x + uipara->right_button.x,  uipara->item.y + uipara->item.h * i + uipara->right_button.y);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(power_attr->unfocus_txt_color);
            GUI_DispStringInRect(power_attr->power_item_res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
            gui_rect.x0 = uipara->item_content_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_content_pos.y + uipara->item_content_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_content_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_content_pos.h;
            GUI_DispStringInRect(power_attr->power_item_res[i].string_content, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
    }

    for (i = power_attr->item_nr ; i < ITEM_NUM_PER_SCREEN ; i++)
    {
        GUI_BMP_Draw(theme_hdl2buf(power_attr->power_item_res[0].h_bmp_item_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
    }

    //..GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);

    if (draw_mem)
    {
        GUI_MEMDEV_CopyToLCD(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
    }

    //GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32  setting_power_key_proc(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    static __s32 last_key = -1;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (last_key == GUI_MSG_KEY_ENTER
            && key_focous_left_right)
        {
            main_cmd2parent(msg->h_deswin, ID_OP_ENTER, power_attr->new_focus, 0);
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_LEFT:  //作up键使用
                //case GUI_MSG_KEY_LONGLEFT:
            {
                __log("======POWER_SETTING=======GUI_MSG_KEY_LEFT=%d\n");

                if (key_focous_left_right)
                {
                    key_focous_left_right = 0;
                    setting_power_paint(msg, ID_PAINTING_INIT_POWER);
                    setting_power_item_paint(msg);
                }
                else
                {
                    key_focous_left_right = 1;
                    setting_power_paint(msg, ID_PAINTING_INIT_POWER);
                    setting_power_item_paint(msg);
                }

                //main_cmd2parent(msg->h_deswin, ID_OP_UP, power_attr->new_focus, 0);
            }
            break;

            case GUI_MSG_KEY_RIGHT: //作down键使用
                //case GUI_MSG_KEY_LONGRIGHT:
            {
                __log("======POWER_SETTING=======GUI_MSG_KEY_RIGHT=%d\n");

                if (key_focous_left_right)
                {
                    key_focous_left_right = 0;
                    setting_power_paint(msg, ID_PAINTING_INIT_POWER);
                    setting_power_item_paint(msg);
                }
                else
                {
                    key_focous_left_right = 1;
                    setting_power_paint(msg, ID_PAINTING_INIT_POWER);
                    setting_power_item_paint(msg);
                }

#if 0

                if (key_focous_left_right)
                {
                    main_cmd2parent(msg->h_deswin, ID_OP_DOWN, power_attr->new_focus, 0);
                }
                else
                {
                    setting_power_paint(msg, ID_PAINTING_F_GENERAL);
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_GENERAL_SETTING, 0, 0);  //发送消息给父窗口，进入电源管理
                    GUI_FrmWinDelete(msg->h_deswin);
                }

#endif
            }
            break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            {
                __log("======POWER_SETTING=======GUI_MSG_KEY_UP=%d\n");

                if (key_focous_left_right)
                {
                    main_cmd2parent(msg->h_deswin, ID_OP_UP, power_attr->new_focus, 0);
                }
                else
                {
                    setting_power_paint(msg, ID_PAINTING_F_GENERAL);
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_GENERAL_SETTING, 0, 0);  //发送消息给父窗口，进入电源管理
                    GUI_FrmWinDelete(msg->h_deswin);
                }
            }
            break;

            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_DOWN:
            {
                __log("======POWER_SETTING=======GUI_MSG_KEY_DOWN=%d\n");

                if (key_focous_left_right)
                {
                    main_cmd2parent(msg->h_deswin, ID_OP_DOWN, power_attr->new_focus, 0);
                }
                else
                {
                    setting_power_paint(msg, ID_PAINTING_F_GENERAL);
                    main_cmd2parent(msg->h_deswin, ID_SWITCH_GENERAL_SETTING, 0, 0);  //发送消息给父窗口，进入电源管理
                    GUI_FrmWinDelete(msg->h_deswin);
                }
            }
            break;

            case GUI_MSG_KEY_ESCAPE:

            //case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_VDEC:
            {
                main_cmd2parent(msg->h_deswin, ID_OP_EXIT, power_attr->new_focus, 0);
            }
            break;

            default :
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}
static void setting_power_on_left_button(__gui_msg_t  *msg)
{
    setting_power_attr_t *power_attr;
    power_item_res_t  *p_item_res;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    p_item_res = &(power_attr->power_item_res[power_attr->new_focus]) ;
    p_item_res->content_num--;

    if (p_item_res->content_num  <  0)
    {
        //p_item_res->content_num = p_item_res->content_nr-1 ;
        p_item_res->content_num = 0 ;
    }

    setting_power_content_paint(msg);

    switch (power_attr->new_focus)
    {
        case 0:
        {
            if (setting_reg_para)
            {
                setting_reg_para->backlight = p_item_res->content_num;
            }

            dsk_display_set_lcd_brightness((__lion_bright_t)(p_item_res->content_num));
            gscene_hbar_update_text(HBAR_BRIGHT_PAINT);
            break;
        }

        case 1:
        {
            __u32  close_scn_time ;

            switch (p_item_res->content_num)
            {
                case BACKLIGHTOFF_NEVER:
                    g_set_close_scn_time(0);
                    close_scn_time = 0;
                    break;

                case BACKLIGHTOFF_10SECOND:
                    g_set_close_scn_time(10);//10s
                    close_scn_time = 10;
                    break;

                case BACKLIGHTOFF_30SECOND:
                    g_set_close_scn_time(30);// 30s
                    close_scn_time = 30;
                    break;

                case BACKLIGHTOFF_60SECOND:
                    g_set_close_scn_time(60);
                    close_scn_time = 60;
                    break;

                case BACKLIGHTOFF_90SECOND:
                    g_set_close_scn_time(90);
                    close_scn_time = 90;
                    break;

                default:
                    g_set_close_scn_time(0);
                    close_scn_time = 0;
                    break;
            }

            if (setting_reg_para)
            {
                setting_reg_para->backlightoff = close_scn_time;
            }
        }
        break;

        case 2:
        {
            __u32  auto_off_time ;

            switch (p_item_res->content_num)
            {
                case POWEROFF_NEVER:
                    dsk_set_auto_off_time(0);
                    auto_off_time = 0;
                    break;

                case POWEROFF_3MINUTE:
                    dsk_set_auto_off_time(3 * 60);
                    auto_off_time = 3 * 60;
                    break;

                case POWEROFF_5MINUTE:
                    dsk_set_auto_off_time(5 * 60);
                    auto_off_time = 5 * 60;
                    break;

                case POWEROFF_10MINUTE:
                    dsk_set_auto_off_time(10 * 60);
                    auto_off_time = 10 * 60;
                    break;

                case POWEROFF_15MINUTE:
                    dsk_set_auto_off_time(15 * 60);
                    auto_off_time = 15 * 60;
                    break;

                default:
                    dsk_set_auto_off_time(0);
                    auto_off_time = 0;
                    break;
            }

            if (setting_reg_para)
            {
                setting_reg_para->poweroff = auto_off_time;
            }
        }
        break;

        default:
            break;
    }
}

static void setting_power_on_right_button(__gui_msg_t  *msg)
{
    setting_power_attr_t *power_attr;
    power_item_res_t  *p_item_res;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    p_item_res = &power_attr->power_item_res[power_attr->new_focus] ;
    p_item_res->content_num++ ;

    if (p_item_res->content_num  >= p_item_res->content_nr)
    {
        //p_item_res->content_num = 0 ;
        p_item_res->content_num = p_item_res->content_nr - 1 ;
    }

    setting_power_content_paint(msg);

    switch (power_attr->new_focus)
    {
        case 0:
        {
            if (setting_reg_para)
            {
                setting_reg_para->backlight = p_item_res->content_num;
            }

            dsk_display_set_lcd_brightness((__lion_bright_t)(p_item_res->content_num));
            gscene_hbar_update_text(HBAR_BRIGHT_PAINT);
            break;
        }

        case 1:
        {
            __u32  close_scn_time ;

            switch (p_item_res->content_num)
            {
                case BACKLIGHTOFF_NEVER:
                    g_set_close_scn_time(0);
                    close_scn_time = 0;
                    break;

                case BACKLIGHTOFF_10SECOND:
                    g_set_close_scn_time(10);//10s
                    close_scn_time = 10;
                    break;

                case BACKLIGHTOFF_30SECOND:
                    g_set_close_scn_time(30);// 30s
                    close_scn_time = 30;
                    break;

                case BACKLIGHTOFF_60SECOND:
                    g_set_close_scn_time(60);
                    close_scn_time = 60;
                    break;

                case BACKLIGHTOFF_90SECOND:
                    g_set_close_scn_time(90);
                    close_scn_time = 90;
                    break;

                default:
                    g_set_close_scn_time(0);
                    close_scn_time = 0;
                    break;
            }

            if (setting_reg_para)
            {
                setting_reg_para->backlightoff = close_scn_time;
            }
        }
        break;

        case 2:
        {
            __u32  auto_off_time ;

            switch (p_item_res->content_num)
            {
                case POWEROFF_NEVER:
                    dsk_set_auto_off_time(0);
                    auto_off_time = 0;
                    break;

                case POWEROFF_3MINUTE:
                    dsk_set_auto_off_time(3 * 60);
                    auto_off_time = 3 * 60;
                    break;

                case POWEROFF_5MINUTE:
                    dsk_set_auto_off_time(5 * 60);
                    auto_off_time = 5 * 60;
                    break;

                case POWEROFF_10MINUTE:
                    dsk_set_auto_off_time(10 * 60);
                    auto_off_time = 10 * 60;
                    break;

                case POWEROFF_15MINUTE:
                    dsk_set_auto_off_time(15 * 60);
                    auto_off_time = 15 * 60;
                    break;

                default:
                    dsk_set_auto_off_time(0);
                    auto_off_time = 0;
                    break;
            }

            if (setting_reg_para)
            {
                setting_reg_para->poweroff = auto_off_time;
            }
        }
        break;

        default:
            break;
    }
}

void setting_power_enter_action(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    power_item_res_t  *p_item_res;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    p_item_res = &power_attr->power_item_res[power_attr->new_focus] ;

    if (p_item_res->content_num >= p_item_res->content_nr - 1)
    {
        p_item_res->content_num = 0;
    }
    else
    {
        p_item_res->content_num++;
    }

    setting_power_content_paint(msg);

    switch (power_attr->new_focus)
    {
        case 0:
        {
            if (setting_reg_para)
            {
                setting_reg_para->backlight = p_item_res->content_num;
            }

            dsk_display_set_lcd_brightness((__lion_bright_t)(p_item_res->content_num));
            break;
        }

        case 1:
        {
            __u32  close_scn_time ;

            switch (p_item_res->content_num)
            {
                case BACKLIGHTOFF_NEVER:
                    g_set_close_scn_time(0);
                    close_scn_time = 0;
                    break;

                case BACKLIGHTOFF_10SECOND:
                    g_set_close_scn_time(10);//10s
                    close_scn_time = 10;
                    break;

                case BACKLIGHTOFF_30SECOND:
                    g_set_close_scn_time(30);// 30s
                    close_scn_time = 30;
                    break;

                case BACKLIGHTOFF_60SECOND:
                    g_set_close_scn_time(60);
                    close_scn_time = 60;
                    break;

                case BACKLIGHTOFF_90SECOND:
                    g_set_close_scn_time(90);
                    close_scn_time = 90;
                    break;

                default:
                    g_set_close_scn_time(0);
                    close_scn_time = 0;
                    break;
            }

            if (setting_reg_para)
            {
                setting_reg_para->backlightoff = close_scn_time;
            }
        }
        break;

        case 2:
        {
            __u32  auto_off_time ;

            switch (p_item_res->content_num)
            {
                case POWEROFF_NEVER:
                    dsk_set_auto_off_time(0);
                    auto_off_time = 0;
                    break;

                case POWEROFF_3MINUTE:
                    dsk_set_auto_off_time(3 * 60);
                    auto_off_time = 3 * 60;
                    break;

                case POWEROFF_5MINUTE:
                    dsk_set_auto_off_time(5 * 60);
                    auto_off_time = 5 * 60;
                    break;

                case POWEROFF_10MINUTE:
                    dsk_set_auto_off_time(10 * 60);
                    auto_off_time = 10 * 60;
                    break;

                case POWEROFF_15MINUTE:
                    dsk_set_auto_off_time(15 * 60);
                    auto_off_time = 15 * 60;
                    break;

                default:
                    dsk_set_auto_off_time(0);
                    auto_off_time = 0;
                    break;
            }

            if (setting_reg_para)
            {
                setting_reg_para->poweroff = auto_off_time;
            }
        }
        break;

        default:
            break;
    }
}

static __s32  setting_power_touch_proc(__gui_msg_t *msg)
{
    setting_power_attr_t *power_attr;
    __s32 pos_x, pos_y;
    setting_uipara_t *uipara;
    power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    uipara = (setting_uipara_t *)setting_get_uipara();
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);

    if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
    {
        __s32 tmp_index;
        //触摸区域在返回按钮范围内
#if 0

        if ((pos_x > uipara->return_bmp.x) && (pos_x < uipara->return_bmp.x + uipara->return_bmp.w)
            && (pos_y > uipara->return_bmp.y) && (pos_y < uipara->return_bmp.y + uipara->return_bmp.h))
        {
            main_cmd2parent(msg->h_deswin, ID_SWITCH_TO_ENTRANCE, 0, 0);
            dsk_keytone_on();
            //GUI_BMP_Draw( power_attr->res_theme[POWER_THEME_RETURN_FCS_BMP_ID].p_buf  , uipara->return_bmp.x , uipara->return_bmp.y );
        }
        else
#endif
        {
            tmp_index = power_TouchPos_2_ItemIndex(msg);    //获取触摸的位置区域

            //__here__ ;
            if ((tmp_index >= 0) && (tmp_index < power_attr->item_nr))
            {
                RECT rect  ;
                __bool ret ;
                power_attr->old_focus = power_attr->new_focus;
                power_attr->new_focus = tmp_index;
                setting_power_item_paint(msg);
                //__here__ ;
                rect.x  = uipara->left_button.x + uipara->item.x ;
                rect.y  = uipara->left_button.y + uipara->item.y ;
                rect.width  = uipara->left_button.w ;
                rect.height  = uipara->left_button.h ;
                ret = position_in_rect(&rect, pos_x, pos_y % uipara->item.h);

                if (EPDK_TRUE == ret)
                {
                    //setting_power_on_left_button(msg);
                }

                //__here__ ;
                rect.x  = uipara->right_button.x + uipara->item.x ;
                rect.y  = uipara->right_button.y + uipara->item.y ;
                rect.width  = uipara->right_button.w ;
                rect.height  = uipara->right_button.h ;
                ret = position_in_rect(&rect, pos_x, pos_y % uipara->item.h);

                if (EPDK_TRUE == ret)
                {
                    //setting_power_on_right_button(msg);
                }

                //__here__ ;
            }
        }
    }
    else if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1 || GUI_MSG_TOUCH_LONGDOWN == msg->dwAddData1)
    {
        if ((pos_x > uipara->item_general_setting.x) && (pos_x < uipara->item_general_setting.x + uipara->item_general_setting.w)
            && (pos_y > uipara->item_general_setting.y) && (pos_y < uipara->item_general_setting.y + uipara->item_general_setting.h))
        {
            setting_power_paint(msg, ID_PAINTING_F_GENERAL);
            dsk_keytone_on();
            __log("======ID_SWITCH_GENERAL_SETTING=======GUI_MSG_TOUCH_DOWN=%d\n");
            main_cmd2parent(msg->h_deswin, ID_SWITCH_GENERAL_SETTING, 0, 0);  //发送消息给父窗口，进入电源管理
            GUI_FrmWinDelete(msg->h_deswin);
            // GUI_LyrWinSetSta(power_attr->layer,GUI_LYR_STA_OFF);
        }
        else
        {
            __s32 tmp_index;
            tmp_index = power_TouchPos_2_ItemIndex(msg);    //获取触摸的位置区域

            //__here__ ;
            if ((tmp_index >= 0) && (tmp_index < power_attr->item_nr))
            {
                __bool ret = EPDK_FALSE;

                if (power_attr->new_focus == tmp_index)
                {
                    dsk_keytone_on();
                    ret = EPDK_TRUE;
                }
                else
                {
                    dsk_keytone_on();
                }

                power_attr->old_focus = power_attr->new_focus;
                power_attr->new_focus = tmp_index;

                if (pos_x >= 509 - 10 && pos_x <= 509 + 46 + 10 && ret == EPDK_TRUE)
                {
                    setting_power_on_left_button(msg);
                }
                else if (pos_x >= 724 - 10 && pos_x <= 724 + 46 && ret == EPDK_TRUE)
                {
                    setting_power_on_right_button(msg);
                }
                else
                {
                    setting_power_item_paint(msg);
                }
            }
        }
    }

    return EPDK_OK;
}


void setting_power_on_set_content_num(setting_power_attr_t *power_attr)
{
    if (setting_reg_para == NULL)
    {
        __err("%s %d setting_reg_para null\r\n", __FILE__, __LINE__);
        return;
    }

    setting_reg_para->backlight = dsk_display_get_lcd_brightness();
    power_attr->power_item_res[0].content_num = setting_reg_para->backlight;
}
/*
    回调
*/
static __s32 _setting_power_Proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            setting_power_attr_t *power_attr = NULL;
            setting_power_para_t *power_para = NULL;
            __log("======SETTING_POWER=======GUI_MSG_CREATE=%d\n");
            setting_reg_para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
            power_para = (setting_power_para_t *)GUI_WinGetAttr(msg->h_deswin);
            power_attr = (setting_power_attr_t *)My_Malloc(0, sizeof(setting_power_attr_t));

            if (NULL == power_attr)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(power_attr, 0, sizeof(setting_power_attr_t));
            power_attr->power_para = power_para;
            power_attr->new_focus = 0;
            power_attr->old_focus = 0;
            setting_power_on_set_content_num(power_attr);
            _setting_power_res_init(power_attr);
            power_attr->focus_txt_color = SETTING_FOCUS_COLOR;
            power_attr->unfocus_txt_color = SETTING_UNFOCUS_COLOR;
            power_attr->font = power_para->font;
            power_attr->layer = power_para->layer;
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)power_attr);
            GUI_LyrWinSetSta(power_attr->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(power_attr->layer);
            setting_power_paint(msg, ID_PAINTING_INIT_POWER);
            power_attr->checkTimerID = 0xff;
            GUI_SetTimer(msg->h_deswin, power_attr->checkTimerID, 50, NULL);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            setting_power_attr_t *power_attr;
            setting_power_para_t *power_para;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!power_attr)
            {
                __err("power_attr is null ......\n");
                return EPDK_FAIL;
            }

            _setting_power_res_uninit(power_attr);
            //dsk_reg_flush();
            GUI_KillTimer(msg->h_deswin, power_attr->checkTimerID);
            power_para = power_attr->power_para;

            if (!power_para)
            {
                __err("power_para is null ......\n");
                return EPDK_FAIL;
            }

            if (power_para)
            {
                My_Mfree(0, power_para);
            }

            if (power_attr)
            {
                My_Mfree(0, power_attr);
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            __log("======SETTING_POWER=======GUI_MSG_PAINT=%d\n");
            return EPDK_OK;

        case GUI_MSG_KEY:
            setting_power_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            setting_power_touch_proc(msg);
            break;

        case MSG_OP_UP:
        {
            setting_power_attr_t *power_attr = NULL;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            power_attr->old_focus = power_attr->new_focus;

            if (power_attr->new_focus <= 0)
            {
                power_attr->new_focus = power_attr->item_nr - 1;
            }
            else
            {
                power_attr->new_focus--;
            }

            setting_power_item_paint(msg);
        }

        return EPDK_OK;

        case MSG_OP_DOWN:
        {
            setting_power_attr_t *power_attr = NULL;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            power_attr->old_focus = power_attr->new_focus;

            if (power_attr->new_focus >= power_attr->item_nr - 1)
            {
                power_attr->new_focus = 0;
            }
            else
            {
                power_attr->new_focus++;
            }

            setting_power_item_paint(msg);
        }

        return EPDK_OK;

        case MSG_OP_LEFT:
        {
            setting_power_attr_t *power_attr = NULL;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            power_attr->old_focus = power_attr->new_focus;

            if (power_attr->new_focus <= 0)
            {
                power_attr->new_focus = power_attr->item_nr - 1;
            }
            else
            {
                power_attr->new_focus--;
            }

            setting_power_item_paint(msg);
        }

        return EPDK_OK;

        case MSG_OP_RIGHT:
        {
            setting_power_attr_t *power_attr = NULL;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            power_attr->old_focus = power_attr->new_focus;

            if (power_attr->new_focus >= power_attr->item_nr - 1)
            {
                power_attr->new_focus = 0;
            }
            else
            {
                power_attr->new_focus++;
            }

            setting_power_item_paint(msg);
        }

        return EPDK_OK;

        case MSG_OP_ENTER:
        {
            setting_power_attr_t *power_attr = NULL;
            power_item_res_t  *p_item_res = NULL;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);
            p_item_res = &(power_attr->power_item_res[power_attr->new_focus]);

            if (p_item_res->content_num >= p_item_res->content_nr - 1)
            {
                p_item_res->content_num = 0;
            }
            else
            {
                p_item_res->content_num++;
            }

            setting_power_content_paint(msg);

            switch (power_attr->new_focus)
            {
                case 0:
                {
                    if (setting_reg_para)
                    {
                        setting_reg_para->backlight = p_item_res->content_num;
                    }

                    dsk_display_set_lcd_brightness((__lion_bright_t)(p_item_res->content_num));
                    break;
                }

                case 1:
                {
                    __u32  close_scn_time ;

                    switch (p_item_res->content_num)
                    {
                        case BACKLIGHTOFF_NEVER:
                            g_set_close_scn_time(0);
                            close_scn_time = 0;
                            break;

                        case BACKLIGHTOFF_10SECOND:
                            g_set_close_scn_time(10);//10s
                            close_scn_time = 10;
                            break;

                        case BACKLIGHTOFF_30SECOND:
                            g_set_close_scn_time(30);// 30s
                            close_scn_time = 30;
                            break;

                        case BACKLIGHTOFF_60SECOND:
                            g_set_close_scn_time(60);
                            close_scn_time = 60;
                            break;

                        case BACKLIGHTOFF_90SECOND:
                            g_set_close_scn_time(90);
                            close_scn_time = 90;
                            break;

                        default:
                            g_set_close_scn_time(0);
                            close_scn_time = 0;
                            break;
                    }

                    if (setting_reg_para)
                    {
                        setting_reg_para->backlightoff = close_scn_time;
                    }
                }
                break;

                case 2:
                {
                    __u32  auto_off_time ;

                    switch (p_item_res->content_num)
                    {
                        case POWEROFF_NEVER:
                            dsk_set_auto_off_time(0);
                            auto_off_time = 0;
                            break;

                        case POWEROFF_3MINUTE:
                            dsk_set_auto_off_time(3 * 60);
                            auto_off_time = 3 * 60;
                            break;

                        case POWEROFF_5MINUTE:
                            dsk_set_auto_off_time(5 * 60);
                            auto_off_time = 5 * 60;
                            break;

                        case POWEROFF_10MINUTE:
                            dsk_set_auto_off_time(10 * 60);
                            auto_off_time = 10 * 60;
                            break;

                        case POWEROFF_15MINUTE:
                            dsk_set_auto_off_time(15 * 60);
                            auto_off_time = 15 * 60;
                            break;

                        default:
                            dsk_set_auto_off_time(0);
                            auto_off_time = 0;
                            break;
                    }

                    if (setting_reg_para)
                    {
                        setting_reg_para->poweroff = auto_off_time;
                    }
                }
                break;

                default:
                    break;
            }
        }

        return EPDK_OK;

        case MSG_OP_EXIT:
        {
            //main_cmd2parent(msg->h_deswin,ID_SWITCH_TO_ENTRANCE,0,0);
            __gui_msg_t mymsg;
            mymsg.id = GUI_MSG_CLOSE;
            mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
            GUI_SendMessage(&mymsg);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
            break;

        case GUI_MSG_TIMER:
        {
            setting_power_attr_t *power_attr = NULL;
            power_item_res_t  *p_item_res = NULL;
            power_attr = (setting_power_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (power_attr == NULL)
            {
                return EPDK_OK;
            }

            p_item_res = &(power_attr->power_item_res[0]) ;

            if (p_item_res->content_num != dsk_display_get_lcd_brightness())
            {
                power_attr->old_focus = power_attr->new_focus;
                power_attr->new_focus = 0;
                p_item_res->content_num = dsk_display_get_lcd_brightness();
                setting_reg_para->backlight = p_item_res->content_num;
                setting_power_item_paint(msg);
                setting_power_content_paint(msg);
            }
        }

        return EPDK_OK;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_power_win_create(H_WIN h_parent, setting_power_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_power_para_t *power_para;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    power_para = (setting_power_para_t *)My_Malloc(0, sizeof(setting_power_para_t));

    if (!power_para)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset((void *)power_para, 0, sizeof(setting_power_para_t));
    power_para->font = para->font;
    power_para->layer = para->layer;
    power_para->focus_id = para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "power setting win",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = POWER_MANAGE_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_setting_power_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)power_para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 setting_power_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}
