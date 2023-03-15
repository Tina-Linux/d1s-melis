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
#include "dtv_setting_ca.h"
#include <emodules/mod_ca.h>
#include <log.h>

#if ((MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS) || (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS) || (MAPLE_CA_STANDAR == DTV_CA_NONE))

#define   TIPS_YES_NO_BMP_WIDTH     63
#define   TIPS_YES_NO_BMP_HEIGHT    33

static    __u32 last_ca_msg = 0;
#define SET_TIPS_W  311
#define SET_TIPS_H  171

#if (MAPLE_CA_STANDAR != DTV_CA_NONE)
extern    H_WIN g_dtv_mainwin;
static H_WIN    h_dialog = NULL;
static unsigned char Is_ca_message_menu = EPDK_FALSE;
#endif
static __s32    last_key = -1;

__s32 dtv_get_ca_user_info(char *user_info);
__s32 dtv_get_ca_author_info(char *author_info);

extern __mp *maple;
enum
{
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
    DTV_SETTING_CA_SYSTEM_VERSION = 0,
    DTV_SETTING_CA_SYSTEM_ID,
    DTV_SETTING_CA_PASSWORD_MARK,
    DTV_SETTING_CA_DDEFAULT_PIN_MARK,
    DTV_SETTING_CA_ALLOW_MATURE_RATED,
    DTV_SETTING_CA_USER_STATUS,
    DTV_SETTING_CA_GCANAME,
    DTV_SETTING_CA_SUPPLIER,
    DTV_SETTING_CA_HID,
    DTV_SETTING_CA_USER_ID,
    DTV_SETTING_CA_LIBRARY,
    DTV_SETTING_CA_SERIAR,
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
    DTV_SETTING_CA_SC_ID = 0,
    DTV_SETTING_CA_SC_TYPE,
    DTV_SETTING_CA_SC_STATUS,
    DTV_SETTING_CA_SC_NETREGION_ID,
    DTV_SETTING_CA_SC_USERLEVEL,
    DTV_SETTING_CA_SC_WORKTIME,
    DTV_SETTING_CA_SC_PAIRING_STATUS,
    DTV_SETTING_CA_VERSION,
#elif (MAPLE_CA_STANDAR == DTV_CA_NONE)
    DTV_SETTING_CA_VERSION,
#endif
    DTV_SETTING_CA_USER_INFO_NUM,
};
__s32 string_id[DTV_SETTING_CA_USER_INFO_NUM] =
{
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
    STRING_DTV_SETTING_CA_SYSTEM_VERSION,
    STRING_DTV_SETTING_CA_SYSTEM_ID,
    STRING_DTV_SETTING_CA_PASSWORD_MARK,
    STRING_DTV_SETTING_CA_DDEFAULT_PIN_MARK,
    STRING_DTV_SETTING_CA_ALLOW_MATURE_RATED,
    STRING_DTV_SETTING_CA_USER_STATUS,
    STRING_DTV_SETTING_CA_GCANAME,
    STRING_DTV_SETTING_CA_SUPPLIER,
    STRING_DTV_SETTING_CA_HID,
    STRING_DTV_SETTING_CA_USER_ID,
    STRING_DTV_SETTING_CA_LIBRARY,
    STRING_DTV_SETTING_CA_SERIAR,
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
    STRING_DTV_SETTING_CA_SC_ID,
    STRING_DTV_SETTING_CA_SC_TYPE,
    STRING_DTV_SETTING_CA_SC_STATUS,
    STRING_DTV_SETTING_CA_SC_NETREGION_ID,
    STRING_DTV_SETTING_CA_SC_USERLEVEL,
    STRING_DTV_SETTING_CA_SC_WORKTIME,
    STRING_DTV_SETTING_CA_SC_PAIRING_STATUS,
    STRING_DTV_SETTING_CA_SYSTEM_VERSION,
#elif (MAPLE_CA_STANDAR == DTV_CA_NONE)
    STRING_DTV_SETTING_CA_SYSTEM_VERSION,
#endif
};
typedef struct tag_ca_ctrl
{
    setting_ca_para_t   *ca_para;
    __s32               total;
    __s32               focus_id;
    H_LBAR              h_listbar;
    GUI_FONT            *font;
    __s32               focus_txt_color;
    __s32               unfocus_txt_color;

    H_THEME             ca_f_bmp;
    H_THEME             ca_uf_bmp;
    H_THEME             h_list_top_bmp;
} ca_ctrl_t;

typedef struct tag_Author_info
{
    __s32       current_page;
    __s32       total_page;
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
    Tfca_user_entitlement_t Author;
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
    DIVI_PackageInfo        Author;
#elif (MAPLE_CA_STANDAR == DTV_CA_NONE)
    char                    Author;
#endif
} Author_info_t;

typedef struct tag_user_info
{
    char                text[DTV_SETTING_CA_USER_INFO_NUM][128];
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
    Tfca_user_privacy_t user;
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
    DIVI_Info_SC    user;
#elif (MAPLE_CA_STANDAR == DTV_CA_NONE)
    char            user;
#endif
} user_info_t;
typedef union
{
    user_info_t         user_info;
    Author_info_t       Author_info;
} ca_sub_info_t;

typedef struct tag_ca_sub_ctrl
{
    setting_ca_sub_para_t   *ca_sub_para;
    GUI_FONT            *font;

    ca_sub_info_t       sub_info;
    H_THEME             ca_background_bmp;
    H_THEME             ca_return_bmp;
    H_THEME             h_list_top_bmp;
    H_THEME             ca_left_bmp;
    H_THEME             ca_right_bmp;
} ca_sub_ctrl_t;

typedef struct ca_uipara
{
    OFFSET   ca_bmp_pos;    //提示框背景图坐标位置
    RECT     ca_tiltle_pos;     //提示框title坐标位置
    RECT     ca_content;        //CA序列号
    OFFSET   ca_ok_bmp_pos; //OK 字符显示底图坐标

} ca_uipara_t;

#define  CA_BG_WIDTH  311
#define  CA_BG_HEIGHT 171
#define  SETTING_TOP_HIGH    35
enum
{
    DTV_SETTING_CA_USER_INFO = 0,
    DTV_SETTING_CA_AUTHORIZATION,
    DTV_SETTING_CA_NUM
};

static void dtv_setting_ca_init(ca_ctrl_t *ca_ctrl)
{
    ca_ctrl->ca_f_bmp = dsk_theme_open(ID_DTV_PRELIST_F_BMP);
    ca_ctrl->ca_uf_bmp = dsk_theme_open(ID_DTV_PRELIST_UF_BMP);
    ca_ctrl->h_list_top_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);
    return ;
}

static void dtv_setting_ca_sub_init(ca_sub_ctrl_t *ca_sub_ctrl)
{
    //__here__;
    ca_sub_ctrl->ca_return_bmp = dsk_theme_open(ID_DTV_TIP_F_BMP);
    ca_sub_ctrl->h_list_top_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);

    if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
    {
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
        ca_sub_ctrl->ca_background_bmp = dsk_theme_open(ID_DTV_SET_INFO_BMP);
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
        ca_sub_ctrl->ca_background_bmp = dsk_theme_open(ID_DTV_SET_INFO_BMP);
#endif
        ca_sub_ctrl->ca_left_bmp = dsk_theme_open(ID_DTV_CA_LEFT_BMP);
        ca_sub_ctrl->ca_right_bmp = dsk_theme_open(ID_DTV_CA_RIGHT_BMP);
        /*init author info*/
        eLIBs_memset(&ca_sub_ctrl->sub_info.Author_info, 0x00, sizeof(ca_sub_ctrl->sub_info.Author_info));
        dtv_get_ca_author_info(&ca_sub_ctrl->sub_info.Author_info.Author);
        ca_sub_ctrl->sub_info.Author_info.current_page = 1;
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
        ca_sub_ctrl->sub_info.Author_info.total_page = ca_sub_ctrl->sub_info.Author_info.Author.m_count;
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
        ca_sub_ctrl->sub_info.Author_info.total_page = ca_sub_ctrl->sub_info.Author_info.Author.Package_Count;
#endif
    }
    else
    {
        __s32 Index = 0;
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
        ca_sub_ctrl->ca_background_bmp = dsk_theme_open(ID_DTV_INFO_BG_BMP);
#elif(MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
        ca_sub_ctrl->ca_background_bmp = dsk_theme_open(ID_DTV_SET_INFO_BMP);
#endif
        /*user info init*/
        eLIBs_memset(&ca_sub_ctrl->sub_info.user_info, 0x00, sizeof(ca_sub_ctrl->sub_info.user_info));
        dtv_get_ca_user_info(&ca_sub_ctrl->sub_info.user_info.user);

        for (; Index < DTV_SETTING_CA_USER_INFO_NUM; Index++)
        {
            dsk_langres_get_menu_text(string_id[Index], ca_sub_ctrl->sub_info.user_info.text[Index],
                                      sizeof(ca_sub_ctrl->sub_info.user_info.text[Index]));
        }
    }

    return ;
}

static void dtv_setting_ca_sub_uninit(ca_sub_ctrl_t *ca_sub_ctrl)
{
    if (ca_sub_ctrl->ca_background_bmp)
    {
        dsk_theme_close(ca_sub_ctrl->ca_background_bmp);
        ca_sub_ctrl->ca_background_bmp = NULL;
    }

    if (ca_sub_ctrl->ca_return_bmp)
    {
        dsk_theme_close(ca_sub_ctrl->ca_return_bmp);
        ca_sub_ctrl->ca_return_bmp = NULL;
    }

    if (ca_sub_ctrl->h_list_top_bmp)
    {
        dsk_theme_close(ca_sub_ctrl->h_list_top_bmp);
        ca_sub_ctrl->h_list_top_bmp = NULL;
    }

    if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
    {
        if (ca_sub_ctrl->ca_left_bmp)
        {
            dsk_theme_close(ca_sub_ctrl->ca_left_bmp);
            ca_sub_ctrl->ca_left_bmp = NULL;
        }

        if (ca_sub_ctrl->ca_right_bmp)
        {
            dsk_theme_close(ca_sub_ctrl->ca_right_bmp);
            ca_sub_ctrl->ca_right_bmp = NULL;
        }
    }

    return;
}

static void dtv_setting_ca_uninit(ca_ctrl_t *ca_ctrl)
{
    if (ca_ctrl->ca_f_bmp)
    {
        dsk_theme_close(ca_ctrl->ca_f_bmp);
        ca_ctrl->ca_f_bmp = NULL;
    }

    if (ca_ctrl->ca_uf_bmp)
    {
        dsk_theme_close(ca_ctrl->ca_uf_bmp);
        ca_ctrl->ca_uf_bmp = NULL;
    }

    if (ca_ctrl->h_list_top_bmp)
    {
        dsk_theme_close(ca_ctrl->h_list_top_bmp);
        ca_ctrl->h_list_top_bmp = NULL;
    }
}

static void dtv_setting_ca_title_update(H_THEME h_list_top_bmp, __u32 string_id)
{
    char tmp[128] = {0};
    GUI_RECT gui_rect = {0};
    GUI_BMP_Draw(dsk_theme_hdl2buf(h_list_top_bmp), 0, 0);
    gui_rect.x0 = 0;//(size.width - LISTBAR_WIDTH)/2;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH;
    gui_rect.y1 = SETTING_TOP_HIGH;
    dsk_langres_get_menu_text(string_id, tmp, sizeof(tmp));
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return ;
}

static __s32 dtv_setting_ca_info_paint_ex(__lbar_draw_para_t *draw_param)
{
    ca_ctrl_t *ca_ctrl = NULL;
    ca_ctrl = (ca_ctrl_t *)draw_param->attr;

    if (ca_ctrl == NULL)
    {
        __wrn("ca_ctrl == NULL\r\n");
        return EPDK_FAIL;
    }

    // GUI_LyrWinSel(ca_ctrl->ca_para->layer);
    GUI_SetFont(ca_ctrl->font);
    GUI_SetColor(ca_ctrl->unfocus_txt_color);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinCacheOn();

    //  __wrn("draw_param->index:%d draw_param->mode:%d\r\n", draw_param->index, draw_param->mode);
    //  __wrn("draw_param->rect.x:%d,draw_param->rect.y:%d\r\n", draw_param->rect.x,draw_param->rect.y);
    //  __wrn("draw_param->rect.width:%d,draw_param->rect.height:%d\r\n", draw_param->rect.width,draw_param->rect.height);
    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            char tmp[128] = {0};
            GUI_RECT gui_rect = {0};
            GUI_SetColor(ca_ctrl->unfocus_txt_color);
            GUI_BMP_Draw(dsk_theme_hdl2buf(ca_ctrl->ca_uf_bmp), draw_param->rect.x, draw_param->rect.y);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = draw_param->rect.x + draw_param->rect.width;
            gui_rect.y1 = draw_param->rect.y + draw_param->rect.height ;

            switch (draw_param->index)
            {
                case DTV_SETTING_CA_USER_INFO:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_USER_INFO, tmp, sizeof(tmp));
                    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                }
                break;

                case DTV_SETTING_CA_AUTHORIZATION:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_AUTHORIZATION, tmp, sizeof(tmp));
                    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                }
                break;

                default:
                    break;
            }
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            char tmp[128] = {0};
            GUI_RECT gui_rect = {0};
            GUI_SetColor(ca_ctrl->focus_txt_color);
            GUI_BMP_Draw(dsk_theme_hdl2buf(ca_ctrl->ca_f_bmp), draw_param->rect.x, draw_param->rect.y);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = draw_param->rect.x + draw_param->rect.width;
            gui_rect.y1 = draw_param->rect.y + draw_param->rect.height ;

            switch (draw_param->index)
            {
                case DTV_SETTING_CA_USER_INFO:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_USER_INFO, tmp, sizeof(tmp));
                    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                }
                break;

                case DTV_SETTING_CA_AUTHORIZATION:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_AUTHORIZATION, tmp, sizeof(tmp));
                    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
                }
                break;

                default:
                    break;
            }
        }
        break;

        default:
            break;
    }

    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32 dtv_setting_ca_lisbar_init(__gui_msg_t *msg)
{
    ca_ctrl_t *ca_ctrl = NULL;
    __listbar_config_t config = {0};
    ca_ctrl = (ca_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.start_id = 0;
    config.focus_id = ca_ctrl->focus_id;
    config.item_cnt = ca_ctrl->total;
    config.item_height = LISTBAR_HIGH;
    config.item_width = LISTBAR_WIDTH;
    config.list_rect.x = 0;
    config.list_rect.y = SETTING_TOP_HIGH;
    config.list_rect.width = LISTBAR_WIDTH;
    config.list_rect.height = ca_ctrl->total * LISTBAR_HIGH;
#ifdef debug_ca
    __msg("attr->total = %d\n", ca_ctrl->total);
    __msg("attr->x = %d\n", config.list_rect.x);
    __msg("attr->y = %d\n", config.list_rect.y);
    __msg("attr->w = %d\n", config.list_rect.width);
    __msg("attr->h = %d\n", config.list_rect.height);
#endif
    config.list_attr = (void *)ca_ctrl;
    ca_ctrl->h_listbar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(ca_ctrl->h_listbar, dtv_setting_ca_info_paint_ex, &config);
    LISTBAR_ShowPage(ca_ctrl->h_listbar);
    return 0;
}

#if (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
static __s32 dtv_setting_ca_info_sub_paint(__gui_msg_t *msg)
{
    GUI_RECT gui_rect = {0};
    unsigned char *temp = NULL;
    unsigned char text[256] = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    //__here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_SETTING_CA_USER_INFO);
    //1.bg
    x = 0;//(width-CA_BG_WIDTH)/2;
    y = SETTING_TOP_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    //2 卡号
    gui_rect.x0 = x + 10;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + 80;
    gui_rect.y1 = gui_rect.y0 + 25;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_ID], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    gui_rect.x0 = gui_rect.x1 + 10;
    gui_rect.x1 = gui_rect.x0 + 150;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.user.sc_id, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //3 类型
    gui_rect.x0 = x + 10;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 80;
    gui_rect.y1 = gui_rect.y0 + 25;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_TYPE], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 40;
    eLIBs_memset(text, 0x00, sizeof(text));

    if (ca_sub_ctrl->sub_info.user_info.user.sc_Type == 8)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_HOST, text, sizeof(text));
    }
    else if (ca_sub_ctrl->sub_info.user_info.user.sc_Type == 9)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_DAUGHTER, text, sizeof(text));
    }

    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //4 插卡状态
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 80;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_STATUS], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 80;
    eLIBs_memset(text, 0x00, sizeof(text));

    if (ca_sub_ctrl->sub_info.user_info.user.sc_Status == 0)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_NO_INSERT, text, sizeof(text));
    }
    else if (ca_sub_ctrl->sub_info.user_info.user.sc_Status == 1)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_ALREADY_INSERT, text, sizeof(text));
    }

    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //5区域ID
    gui_rect.x0 = x + 10;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 80;
    gui_rect.y1 = gui_rect.y0 + 25;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_NETREGION_ID], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 40;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.sc_NetRegionID);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //5成人级
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 80;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_USERLEVEL], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 80;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.sc_UserLevel);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //6 工作时间
    gui_rect.x0 = x + 10;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 100;
    gui_rect.y1 = gui_rect.y0 + 25;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_WORKTIME], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 200;
    temp = ca_sub_ctrl->sub_info.user_info.user.sc_WorkTime;
    eLIBs_sprintf(text, "%d:%d:%d--%d:%d:%d", *temp, *(temp + 1), *(temp + 2), *(temp + 3), *(temp + 4), *(temp + 5));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //7.机卡配对状态
    gui_rect.x0 = x + 10;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 100;
    gui_rect.y1 = gui_rect.y0 + 25;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SC_PAIRING_STATUS], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 150;
    eLIBs_memset(text, 0x00, sizeof(text));

    if (ca_sub_ctrl->sub_info.user_info.user.sc_Pairing_status == 0)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_NO_INSERT, text, sizeof(text));
    }
    else if (ca_sub_ctrl->sub_info.user_info.user.sc_Pairing_status == 1)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_MACHINE, text, sizeof(text));
    }
    else if (ca_sub_ctrl->sub_info.user_info.user.sc_Pairing_status == 2)
    {
        dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_MACHINE_NOT, text, sizeof(text));
    }

    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //8 CA_Version
    gui_rect.x0 = 10;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 100;
    gui_rect.y1 = gui_rect.y0 + 25;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_VERSION], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 150;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.user.CA_Version, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    return EPDK_OK;
}
static __s32 dtv_setting_ca_author_sub_paint(__gui_msg_t *msg)
{
    int  *temp = NULL;
    char text[256] = {0};
    GUI_RECT gui_rect = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    //__here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_SETTING_CA_AUTHORIZATION);
    //1.bg:
    x = 0, y = SETTING_TOP_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    //__here__;
    //2.ID:
    gui_rect.x0 = 40;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + 60;
    gui_rect.y1 = gui_rect.y0 + 35;
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_ID, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackageId);
    //eLIBs_printf("ID:%d\r\n", ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackageId);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + 100;
    gui_rect.y1 = gui_rect.y1;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //3 start time--end time:
    gui_rect.x0 = 40;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 60;
    gui_rect.y1 = gui_rect.y1 + 35 ;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_START_END_TIME, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 200;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d:%d:%d---%d:%d:%d", ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].StartYear,
                  ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].StartMonth,
                  ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].StartDate,
                  ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].EndYear,
                  ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].EndMonth,
                  ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].EndDate);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    /*3.1:left_icon*/
    gui_rect.x0 = 15;
    gui_rect.y0 = gui_rect.y1;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_left_bmp), gui_rect.x0, gui_rect.y0);
    /*3.2:right_icon*/
    gui_rect.x0 = 311 - 40;
    gui_rect.y0 = gui_rect.y1;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_right_bmp), gui_rect.x0, gui_rect.y0);
    //4.PackagePrice
    gui_rect.x0 = 40;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 60;
    gui_rect.y1 = gui_rect.y1 + 35 ;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_PACKAGEPRICE, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackagePrice);
    //eLIBs_printf("Price:%d\r\n", ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackagePrice);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 100;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //5.PackageScript
    gui_rect.x0 = 40;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 100;
    gui_rect.y1 = gui_rect.y1 + 35 ;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_CARD_DESCRIPTION, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1;
    gui_rect.x1 = gui_rect.x0 + 150;
    eLIBs_memset(text, 0x00, sizeof(text));
    {
        unsigned char Index = 0;
        unsigned char len = sizeof(ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackageScript);

        if (len != 0)
        {
#if 0

            for (Index = 0; Index < len;)
            {
                __u16 Unicode = 0;
                unsigned char char_temp[256] = {0};
                __u16 b = ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackageScript[Index++] & 0xFF;
                __u16 temp = ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackageScript[Index++] & 0xFF;
                temp += b << 8;
                Unicode = eLIBs_GB2312_to_Unicode(temp);
                eLIBs_unicode_to_UTF8(Unicode, char_temp, 256);
                eLIBs_strcat(text, char_temp);
            }

#endif
            {
                __u32 realNameLen = 0;
                realNameLen = eLIBs_charset_convert(EPDK_CHARSET_ENM_GB2312, EPDK_CHARSET_ENM_UTF8,
                                                    ca_sub_ctrl->sub_info.Author_info.Author.PackageInfo[ca_sub_ctrl->sub_info.Author_info.current_page - 1].PackageScript,
                                                    len, text, 256);
                //eLIBs_printf("name:%s,realNameLen:%d\n", text,realNameLen);
            }
        }
        else
        {
            eLIBs_sprintf(text, "NULL");
        }

        __wrn("name:%s\r\n", text);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    /*6:return*/
    gui_rect.x0 = x + (CA_BG_WIDTH - TIPS_YES_NO_BMP_WIDTH) / 2;
    gui_rect.y0 = CA_BG_HEIGHT;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_return_bmp), gui_rect.x0, gui_rect.y0);
    gui_rect.x1 = gui_rect.x0 + TIPS_YES_NO_BMP_WIDTH;
    gui_rect.y1 = gui_rect.y0 + TIPS_YES_NO_BMP_HEIGHT;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_RETURN, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
    /*7: page no*/
    gui_rect.x0 = gui_rect.x1 + 20;
    gui_rect.x1 = gui_rect.x0 + 50;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d/%d", ca_sub_ctrl->sub_info.Author_info.current_page, ca_sub_ctrl->sub_info.Author_info.total_page);
    __wrn("page no:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
    //__here__;
    return EPDK_OK;
}
#elif (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
static __s32 dtv_setting_ca_author_sub_paint(__gui_msg_t *msg)
{
    int  *temp = NULL;
    char text[256] = {0};
    GUI_RECT gui_rect = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    //__here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_SETTING_CA_AUTHORIZATION);
    //1.bg:
    x = 0, y = SETTING_TOP_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    //__here__;
    //2.ID:
    gui_rect.x0 = 80;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + 70;
    gui_rect.y1 = gui_rect.y0 + 35;
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_ID, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_id);
    __wrn("ID:%s\r\n", text);
    gui_rect.x0 = gui_rect.x1 + 20;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + 80;
    gui_rect.y1 = gui_rect.y1;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //3,name:
    gui_rect.x0 = 80;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 70;
    gui_rect.y1 = gui_rect.y1 + 35 ;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_NAME, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
#if 0
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_name);
    {
        unsigned char Index = 0;

        for (Index = 0; Index < ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_nameLength;)
        {
            __u16 Unicode = 0;
            unsigned char char_temp[256] = {0};
            __u16 b = ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_name[Index++] & 0xFF;
            __u16 temp = ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_name[Index++] & 0xFF;
            temp += b << 8;
            Unicode = eLIBs_GB2312_to_Unicode(temp);
            eLIBs_unicode_to_UTF8(Unicode, char_temp, 256);
            eLIBs_strcat(text, char_temp);
        }

        //  eLIBs_printf("name:%s,\n", text);
        gui_rect.x0 = gui_rect.x1 + 20;
        gui_rect.x1 = gui_rect.x0 + 80;
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
#endif
    {
        __u32 realNameLen = 0;
        realNameLen = eLIBs_charset_convert(EPDK_CHARSET_ENM_GB2312, EPDK_CHARSET_ENM_UTF8,
                                            ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_name,
                                            ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_nameLength, text, 256);
        //  eLIBs_printf("name:%s,realNameLen:%d\n", text,realNameLen);
        gui_rect.x0 = gui_rect.x1 + 20;
        gui_rect.x1 = gui_rect.x0 + 80;
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    /*3.1:left_icon*/
    gui_rect.x0 = 25;
    gui_rect.y0 = gui_rect.y1;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_left_bmp), gui_rect.x0, gui_rect.y0);
    /*3.2:right_icon*/
    gui_rect.x0 = 311 - 50;
    gui_rect.y0 = gui_rect.y1;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_right_bmp), gui_rect.x0, gui_rect.y0);
    //4.type:
    gui_rect.x0 = 80;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 70;
    gui_rect.y1 = gui_rect.y1 + 35 ;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_TYPE, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_type);
    __wrn("Type:%d\r\n", ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_type);
    gui_rect.x0 = gui_rect.x1 + 20;
    gui_rect.x1 = gui_rect.x0 + 80;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //5.Expired:
    gui_rect.x0 = 80;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 70;
    gui_rect.y1 = gui_rect.y1 + 35 ;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_EXPIRED, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    temp = ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_expired;
    //temp = ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_expired[0];
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_expired);
    eLIBs_sprintf(text, "%d:%d:%d", *temp, *(temp + 1), *(temp + 2));
    __wrn("Expired:%s\r\n", text);
    gui_rect.x0 = gui_rect.x1 + 20;
    gui_rect.x1 = gui_rect.x0 + 120;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //GUI_DispStringInRect(ca_sub_ctrl->sub_info.Author_info.Author.m_entitlements[ca_sub_ctrl->sub_info.Author_info.current_page - 1].m_expired, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    /*6:return*/
    gui_rect.x0 = x + (CA_BG_WIDTH - TIPS_YES_NO_BMP_WIDTH) / 2;
    gui_rect.y0 = CA_BG_HEIGHT;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_return_bmp), gui_rect.x0, gui_rect.y0);
    gui_rect.x1 = gui_rect.x0 + TIPS_YES_NO_BMP_WIDTH;
    gui_rect.y1 = gui_rect.y0 + TIPS_YES_NO_BMP_HEIGHT;
    eLIBs_memset(text, 0x00, sizeof(text));
    dsk_langres_get_menu_text(STRING_DTV_RETURN, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
    /*7: page no*/
    gui_rect.x0 = gui_rect.x1 + 20;
    gui_rect.x1 = gui_rect.x0 + 50;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d/%d", ca_sub_ctrl->sub_info.Author_info.current_page, ca_sub_ctrl->sub_info.Author_info.total_page);
    __wrn("page no:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
    //__here__;
    return EPDK_OK;
}

static __s32 dtv_setting_ca_info_sub_paint(__gui_msg_t *msg)
{
    GUI_RECT gui_rect = {0};
    unsigned char text[256] = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    //__here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_SETTING_CA_USER_INFO);
    //1.bg
    x = 0;//(width-CA_BG_WIDTH)/2;
    y = SETTING_TOP_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    //2 ca 系统版本
    gui_rect.x0 = x + 5;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_SYSTEM_VERSION, text, sizeof(text));
    //GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SYSTEM_VERSION], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_version);
    __wrn("system version:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //3 ca 系统号
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    // dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_SYSTEM_ID, text, sizeof(text));
    // GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SYSTEM_ID], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_superCASID);
    __wrn("csystem ID:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //4.密码控制标志
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_PASSWORD_MARK, text, sizeof(text));
    //GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_PASSWORD_MARK], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_pinAvailabilityFlag);
    __wrn("Password Control Mark:%s\t\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //5.默认pin码标示位
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_DDEFAULT_PIN_MARK, text, sizeof(text));
    //GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_DDEFAULT_PIN_MARK], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_defaultPINFlag);
    __wrn("Default Pin Mark:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //6 许可成人级
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_ALLOW_MATURE_RATED, text, sizeof(text));
    //GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_ALLOW_MATURE_RATED], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_systemApprovalCertificate);
    __wrn("m_systemApprovalCertificate:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //7.用户状态
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_ALLOW_MATURE_RATED, text, sizeof(text));
    //GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_USER_STATUS], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%d", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_userState);
    __wrn("m_userState:%s\r\n", text);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //8 GCANAME
    gui_rect.x0 = 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 160;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_GCANAME, text, sizeof(text));
    //GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_GCANAME], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_gcaName);
    __wrn("m_gcaName:%s\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_gcaName);
    __wrn("m_gcaNameLength:%d\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_gcaNameLength);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_gcaName, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //9 m_casProviderName
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    // dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_SUPPLIER, text, sizeof(text));
    // GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SUPPLIER], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderName);
    __wrn("m_casProviderNameLength:%d\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderNameLength);
    {
#if 0
        unsigned char Index = 0;

        for (Index = 0; Index < ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderNameLength;)
        {
            __u16 Unicode = 0;
            unsigned char char_temp[256] = {0};
            __u16 temp = 0, b = ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderName[Index++] & 0xFF;
            temp = ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderName[Index++] & 0xFF;
            temp += b << 8;
            Unicode = eLIBs_GB2312_to_Unicode(temp);
            eLIBs_unicode_to_UTF8(Unicode, char_temp, 256);
            eLIBs_strcat(text, char_temp);
        }

#endif
        __u32 realNameLen = 0;
        realNameLen = eLIBs_charset_convert(EPDK_CHARSET_ENM_GB2312, EPDK_CHARSET_ENM_UTF8,
                                            ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderName,
                                            ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_casProviderNameLength, text, 256);
        //eLIBs_printf("name:%s,realNameLen:%d\n", text,realNameLen);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        __wrn("m_casProviderName:%s\r\n", text);
    }
    //10 HID
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_HID, text, sizeof(text));
    // GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_HID], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HID);
    __wrn("HID:%s\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HID);
    __wrn("HID len:%d\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HIDlen);
    eLIBs_memset(text, 0x00, sizeof(text));

    if (ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HIDlen > 20)
    {
        eLIBs_memcpy(text, ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HID, 20);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        gui_rect.y0 = gui_rect.y1;
        gui_rect.y1 = gui_rect.y0 + 25;
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_memcpy(text, ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HID + 20,
                     ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HIDlen - 20);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        eLIBs_memcpy(text, ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HID,
                     ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_HIDlen);
    }

    //11 USER ID
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_HID, text, sizeof(text));
    // GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_USER_ID], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_UserID);
    __wrn("m_UserID:%s\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_UserID);
    __wrn("m_UserIDlen:%d\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_UserIDlen);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_UserID, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    //12 LIBRARY VERSION
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_HID, text, sizeof(text));
    // GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_LIBRARY], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersion);
    __wrn("m_CalibVersion:%s\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersion);
    __wrn("m_CalibVersionLen:%d\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersionLen);

    if (ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersionLen > 20)
    {
        eLIBs_memcpy(text, ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersion, 20);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_memcpy(text, ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersion + 20,
                     ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersionLen - 20);
        gui_rect.x0 = x + 5;
        gui_rect.y0 = gui_rect.y1;
        gui_rect.x1 = gui_rect.x0 + 130;
        gui_rect.y1 = gui_rect.y0 + 25;
        gui_rect.x0 = gui_rect.x1 + 5;
        gui_rect.x1 = gui_rect.x0 + 161;
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CalibVersion, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    //13 SERIAR NUMBER
    gui_rect.x0 = x + 5;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + 130;
    gui_rect.y1 = gui_rect.y0 + 25;
    //eLIBs_memset(text, 0x00, sizeof(text));
    //dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_HID, text, sizeof(text));
    // GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_SETTING_CA_SERIAR], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    gui_rect.x0 = gui_rect.x1 + 5;
    gui_rect.x1 = gui_rect.x0 + 161;
    eLIBs_memset(text, 0x00, sizeof(text));
    //eLIBs_sprintf(text, "%s", ca_sub_ctrl->sub_info.user_info.user.m_stUser_privacy.m_serialNumber);
    __wrn("m_serialNumber:%s\r\n", ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CaSN);
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.user.m_stHID_UserID_info.m_CaSN, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
#if 0
    gui_rect.x0 =  x + (CA_BG_WIDTH - TIPS_YES_NO_BMP_WIDTH) / 2;
    gui_rect.y0 = CA_BG_HEIGHT;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_return_bmp), gui_rect.x0, gui_rect.y0);
    gui_rect.x1 = gui_rect.x0 + TIPS_YES_NO_BMP_WIDTH;
    gui_rect.y1 = gui_rect.y0 + TIPS_YES_NO_BMP_HEIGHT;
    dsk_langres_get_menu_text(STRING_DTV_RETURN, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
#endif
    return EPDK_OK;
}
#endif
static __s32 dtv_setting_send_cmd_to_parent(__gui_msg_t *msg, __u32 id, __u32 data1, __u32 data2)
{
    __gui_msg_t dmsg;
    dmsg.id = id;
    dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
    dmsg.h_srcwin = NULL;
    dmsg.dwAddData1 = data1 ;
    dmsg.dwAddData2 = data2;
    dmsg.dwReserved = 0;
    GUI_SendNotifyMessage(&dmsg);
    return EPDK_OK;
}

static __s32  dtv_setting_ca_key_proc(__gui_msg_t *msg)
{
    ca_ctrl_t *ca_ctrl;
    ca_ctrl = (ca_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    //__here__;
    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (last_key)
        {
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                //__here__;
                LISTBAR_PrevItem(ca_ctrl->h_listbar);
            }
            break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            {
                //__here__;
                LISTBAR_PrevItem(ca_ctrl->h_listbar);
            }
            break;

            case GUI_MSG_KEY_ENTER:
            {
                __s32 Current_Item = LISTBAR_GetFocusItem(ca_ctrl->h_listbar);
                __wrn("Current_Item:%d\r\n", Current_Item);

                if (Current_Item == DTV_SETTING_CA_USER_INFO || Current_Item == DTV_SETTING_CA_AUTHORIZATION)
                {
#if (DTV_CA_NONE != MAPLE_CA_STANDAR)
                    dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, DTV_SETTING_CA_ID, Current_Item + 0x2008);
#endif
                }
                else
                {
                    __wrn("Current_Item error:\r\n");
                    return EPDK_FAIL;
                }

                return EPDK_OK;
            }

            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_ESCAPE:
            {
                dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, DTV_SETTING_BACK2LIST, 0);
                return EPDK_OK;
            }

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else if (KEY_DOWN_ACTION == msg->dwAddData2)
    {
        //__here__;
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}





/*
    回调
*/
static __s32 _dtv_setting_ca_proc(__gui_msg_t *msg)
{
    __wrn("msg->id:%d\n", msg->id);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            ca_ctrl_t *ca_ctrl = NULL;
            setting_ca_para_t *ca_para = NULL;
            ca_para = (setting_ca_para_t *)GUI_WinGetAttr(msg->h_deswin);
            ca_ctrl = (ca_ctrl_t *)My_Malloc(0, sizeof(ca_ctrl_t));

            if (!ca_ctrl || !ca_para)
            {
                __err("memory balloc fail........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(ca_ctrl, 0, sizeof(ca_ctrl_t));
            //__here__;
            ca_ctrl->ca_para = ca_para;
            ca_ctrl->total = DTV_SETTING_CA_NUM;
            ca_ctrl->focus_id = ca_para->focus_id;
            ca_ctrl->focus_txt_color = dtv_SETTING_FOCUS_COLOR;
            ca_ctrl->unfocus_txt_color = dtv_SETTING_UNFOCUS_COLOR;
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)ca_ctrl);
            dtv_setting_ca_init(ca_ctrl);
            GUI_LyrWinSetSta(ca_ctrl->ca_para->layer, GUI_LYRWIN_STA_SLEEP);
            dtv_setting_ca_lisbar_init(msg);
            GUI_LyrWinSel(ca_ctrl->ca_para->layer);
            dtv_setting_ca_title_update(ca_ctrl->h_list_top_bmp, STRING_DTV_CA_INFO);
            last_key = -1;
            GUI_LyrWinSetSta(ca_ctrl->ca_para->layer, GUI_LYRWIN_STA_ON);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            dtv_setting_ca_win_delete(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __s32 ret = 0;
            ca_ctrl_t *ca_ctrl = NULL ;
            ca_ctrl = (ca_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!ca_ctrl)
            {
                __wrn("ca_ctrl is null........\n");
                return EPDK_FAIL;
            }

            ret = LISTBAR_SuspendPage(ca_ctrl->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar suspend fail\n");
            }

            ret = LISTBAR_Delete(ca_ctrl->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar delete fail\n");
            }

            ca_ctrl->h_listbar = NULL;
            dtv_setting_ca_uninit(ca_ctrl);

            if (ca_ctrl->ca_para)
            {
                My_Mfree(0, ca_ctrl->ca_para);
                ca_ctrl->ca_para = NULL ;
            }

            if (ca_ctrl)
            {
                My_Mfree(0, ca_ctrl);
                ca_ctrl = NULL ;
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            //dtv_setting_ca_paint(msg);
            return EPDK_OK;

        case GUI_MSG_KEY:
            dtv_setting_ca_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            break;

        case GUI_MSG_TIMER:
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}
H_WIN dtv_setting_ca_win_create(H_WIN h_parent, setting_ca_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_ca_para_t *ca_para = NULL;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    //__here__;
    ca_para = (setting_ca_para_t *)My_Malloc(0, sizeof(setting_ca_para_t));

    if (!ca_para)
    {
        __wrn("memory balloc fail........\n");
        return NULL;
    }

    eLIBs_memset(ca_para, 0, sizeof(setting_ca_para_t));
    ca_para->focus_id = para->focus_id;
    ca_para->layer = para->layer;
    //__here__;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    SUBMENU_DVB_SETTING_CA;
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = SUBMENU_DVB_CA_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dtv_setting_ca_proc);
    __wrn("dtv_setting_ca_win_create framewin_para.FrameWinProc:0x%x\n", framewin_para.FrameWinProc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width =  fb.size.width;
    framewin_para.rect.height =  fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)ca_para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

void  dtv_setting_ca_win_delete(H_WIN ca_win)
{
    GUI_FrmWinDelete(ca_win);
}
#if (MAPLE_CA_STANDAR == DTV_CA_NONE)// 无ca
H_WIN dtv_setting_ca_sub_win_create(H_WIN h_parent, setting_ca_sub_para_t *para)
{
    return NULL;
}
void dtv_ca_msg_clear(void)
{
    return;
}
void dtv_ca_msg_resume(void)
{
    return;
}
__s32 dtv_ca_msg_is_exist(void)
{
    return 0;
}
void dtv_ca_msg_off(void)
{
    return;
}
void dtv_ca_msg_display(__u32 ca_msg_type)
{
    return;
}
__s32 dtv_ca_msg_callback(void *arg)
{
    return 0;
}
#else
static __s32  dtv_setting_ca_sub_key_proc(__gui_msg_t *msg)
{
    ca_sub_ctrl_t *ca_sub_ctrl;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (last_key)
        {
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
                {
                    if (ca_sub_ctrl->sub_info.Author_info.total_page == 0)
                    {
                        __wrn("ca_sub_ctrl->sub_info.Author_info.total_page = 0\r\n");
                        return EPDK_FAIL;
                    }

                    if (ca_sub_ctrl->sub_info.Author_info.current_page == 1)
                    {
                        ca_sub_ctrl->sub_info.Author_info.current_page = ca_sub_ctrl->sub_info.Author_info.total_page;
                    }
                    else
                    {
                        ca_sub_ctrl->sub_info.Author_info.current_page--;
                    }

                    //__here__;
                    dtv_setting_ca_author_sub_paint(msg);
                }
            }

            return EPDK_OK;

            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
                {
                    //__here__;
                    if (ca_sub_ctrl->sub_info.Author_info.total_page == 0)
                    {
                        __wrn("ca_sub_ctrl->sub_info.Author_info.total_page = 0\r\n");
                        return EPDK_FAIL;
                    }

                    if (ca_sub_ctrl->sub_info.Author_info.current_page == ca_sub_ctrl->sub_info.Author_info.total_page)
                    {
                        ca_sub_ctrl->sub_info.Author_info.current_page = 1;
                    }
                    else
                    {
                        ca_sub_ctrl->sub_info.Author_info.current_page++;
                    }

                    //__here__;
                    dtv_setting_ca_author_sub_paint(msg);
                }
            }

            return EPDK_OK;

            case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_ESCAPE:
            {
                dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, DTV_SETTING_CA_ID, 0);
                return EPDK_OK;
            }

            default:
                break;
        }
    }

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
    }
    else if (KEY_DOWN_ACTION == msg->dwAddData2)
    {
        last_key = msg->dwAddData1;
    }

    return EPDK_OK;
}

static __s32 _dtv_setting_ca_sub_proc(__gui_msg_t *msg)
{
    __wrn("_dtv_setting_ca_sub_proc msg->id:%d\r\n", msg->id);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            ca_sub_ctrl_t *ca_sub_ctrl = NULL;
            setting_ca_sub_para_t *ca_sub_para = NULL;
            //__here__;
            ca_sub_para = (setting_ca_sub_para_t *)GUI_WinGetAttr(msg->h_deswin);
            ca_sub_ctrl = (ca_sub_ctrl_t *)My_Malloc(0, sizeof(ca_sub_ctrl_t));

            if (!ca_sub_ctrl)
            {
                __err("memory balloc fail........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(ca_sub_ctrl, 0, sizeof(ca_ctrl_t));
            //__here__;
            ca_sub_ctrl->ca_sub_para = ca_sub_para;
            GUI_WinSetAddData(msg->h_deswin, (__u32)ca_sub_ctrl);
            GUI_LyrWinSetSta(ca_sub_ctrl->ca_sub_para->layer, GUI_LYRWIN_STA_SLEEP);

            if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_USER_INFO)
            {
                //__here__;
                dtv_setting_ca_sub_init(ca_sub_ctrl);
                dtv_setting_ca_info_sub_paint(msg);
            }
            else if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
            {
                //__here__;
                dtv_setting_ca_sub_init(ca_sub_ctrl);
                dtv_setting_ca_author_sub_paint(msg);
                //__here__;
            }

            last_key = -1;
            GUI_LyrWinSetSta(ca_sub_ctrl->ca_sub_para->layer, GUI_LYRWIN_STA_ON);
            //__here__;
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            dtv_setting_ca_sub_win_delete(msg->h_deswin);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            ca_sub_ctrl_t *ca_sub_ctrl = NULL ;
            ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!ca_sub_ctrl)
            {
                __wrn("ca_ctrl is null........\n");
                return EPDK_FAIL;
            }

            dtv_setting_ca_sub_uninit(ca_sub_ctrl);

            if (ca_sub_ctrl->ca_sub_para)
            {
                My_Mfree(0, ca_sub_ctrl->ca_sub_para);
                ca_sub_ctrl->ca_sub_para = NULL ;
            }

            if (ca_sub_ctrl)
            {
                My_Mfree(0, ca_sub_ctrl);
                ca_sub_ctrl = NULL ;
            }

            Is_ca_message_menu = EPDK_FALSE;
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
        } return EPDK_OK;

        case GUI_MSG_KEY:
            dtv_setting_ca_sub_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            break;

        case GUI_MSG_TIMER:
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

/*
    建立窗口
*/
H_WIN dtv_setting_ca_sub_win_create(H_WIN h_parent, setting_ca_sub_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_ca_sub_para_t *ca_sub_para = NULL;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    //__here__;
    ca_sub_para = (setting_ca_sub_para_t *)My_Malloc(0, sizeof(setting_ca_para_t));

    if (!ca_sub_para)
    {
        __wrn("memory balloc fail........\n");
        return NULL;
    }

    Is_ca_message_menu = EPDK_TRUE;
    eLIBs_memset(ca_sub_para, 0x00, sizeof(setting_ca_para_t));
    ca_sub_para->layer = para->layer;
    ca_sub_para->Current_item = para->Current_item;
    //__here__;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name = SUBMENU_DVB_SETTING_CA_SUB;
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE;
    framewin_para.spCaption = NULL;
    framewin_para.hOwner = NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id = SUBMENU_DVB_CA_SUB_ID;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dtv_setting_ca_sub_proc);
    __wrn("dtv_setting_ca_sub_win_create framewin_para.FrameWinProc:0x%x\n", framewin_para.FrameWinProc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)ca_sub_para;
    framewin_para.hLayer = para->layer;
    //__here__;
    return (GUI_FrmWinCreate(&framewin_para));
}

void  dtv_setting_ca_sub_win_delete(H_WIN ca_sub_win)
{
    GUI_FrmWinDelete(ca_sub_win);
}
__s32 dtv_ca_msg_callback(void *arg)
{
    __u32 *ca_msg_type = 0;
    __gui_msg_t dmsg;
    ca_msg_type = (__u32 *)arg;

    if (g_dtv_mainwin == NULL || maple_get_cur_service_list_num() <= 0)
    {
        eLIBs_printf("g_dtv_mainwin null, can't send message.cur_service_list_num:%d\n", maple_get_cur_service_list_num());
        return;
    }

    dmsg.h_deswin = g_dtv_mainwin;
    dmsg.id = GUI_MSG_COMMAND;
    dmsg.h_srcwin = NULL;
    dmsg.dwAddData1 = DTV_CA_MSG_DISPLAY ;
    dmsg.dwAddData2 = *ca_msg_type;
    dmsg.dwReserved = 0;
    GUI_SendNotifyMessage(&dmsg);
    return EPDK_OK;
}
__s32 dtv_ca_msg_is_exist(void)
{
    __wrn("[dtv_ca_msg_is_exist]h_dialog : %08x\n", h_dialog);

    if (h_dialog != NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void dtv_ca_msg_off(void)
{
    if (h_dialog != NULL)
    {
        __wrn("[dtv_ca_msg_off]h_dialog:0x%x\n", h_dialog);
        app_dialog_destroy(h_dialog);
        h_dialog = NULL;
    }
    else
    {
        __wrn("[dtv_ca_msg_off]no dialog\n");
    }

    return;
}
void dtv_ca_msg_clear(void)
{
    last_ca_msg = 0;
    return ;
}
void dtv_ca_msg_resume(void)
{
    dtv_ca_msg_display(last_ca_msg);
    return ;
}
void dtv_ca_msg_display(__u32 ca_msg_type)
{
    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_SETTING_CA_ERR};
    __s32 delay_time = 0;
    __wrn("msg_type:0x%x,g_dtv_mainwin:0x%x\r\n", ca_msg_type, g_dtv_mainwin);
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)

    switch (ca_msg_type)
    {
        case 0xD8:
            lang_id[1] = STRING_DTV_SETTING_CA_NO_AUTHORIZATION;
            //showCAMessage("没有授权，无法收看此节目");
            break;

        case 0xD4:
            lang_id[1] = STRING_DTV_SETTING_CA_BLACK_LIST;
            //showCAMessage("您被列入了黑名单，无法收看此节目");
            break;

        case 0xD2:
            lang_id[1] = STRING_DTV_SETTING_CA_ENCRYPTION;
            //showCAMessage("非本系统加密，无法收看");
            break;

        case 0xD0:
            //mCAToast.cancel();
            break;

        case 0xE7:
            lang_id[1] = STRING_DTV_SETTING_CA_ACTIVATE_WRONG;
            //showCAMessage("激活错误");
            break;

        case 0xE6:
            lang_id[1] = STRING_DTV_SETTING_CA_SYSTEM_NOT_ACTIVATE;
            //showCAMessage("系统尚未激活");
            break;

        case 0xb2:
            lang_id[1] = STRING_DTV_SETTING_CA_MATURE_RATED;
            //showCAMessage("数据内容为节目分级");
            break;

        case 0xDA:
            lang_id[1] = STRING_DTV_SETTING_CA_AUTHOR_OVERDUE;
            //showCAMessage("用户授权已过期");
            break;

        case 8888:
        {
            lang_id[1] = STRING_DTV_PAY_SERVICE;
            break;
        }

        default:
            lang_id[1] = STRING_DTV_SETTING_CA_ERR;
            return ;
    }

#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)

    switch (HIWORD(ca_msg_type))
    {
        case 0x03:
        {
            lang_id[1] = STRING_DTV_SETTING_CA_NO_AUTHORIZATION;
            //showCAMessage("没有授权，无法收看此节目");
        }
        break;

        case 0x06:
        {
            switch (LOWORD(ca_msg_type))
            {
                case 0x00:
                {
                    lang_id[1] = STRING_DTV_SETTING_CA_CARD_INIT_INSTALL;
                    //showCAMessage("初始化安装卡不可用");
                }
                break;

                case 0x01:
                {
                    lang_id[1] = STRING_DTV_SETTING_CA_CARD_INSTALLING;
                    //showCAMessage("正在安装，卡等待激活");
                }
                break;

                case 0x02:
                {
                    ca_msg_type = 0x00020000;//let other process
                }
                break;

                case 0x03:
                {
                    lang_id[1] = STRING_DTV_SETTING_CA_CARD_OWE;
                    //showCAMessage("欠费停用");
                }
                break;

                case 0x04:
                {
                    lang_id[1] = STRING_DTV_SETTING_CA_CARD_MAINTENANCE;
                    //showCAMessage("维修停用");
                }
                break;

                case 0x05:
                {
                    lang_id[1] = STRING_DTV_SETTING_CA_CARD_APPLY_STOP;
                    //showCAMessage("申请报停停用");
                }
                break;

                case 0x09:
                {
                    lang_id[1] = STRING_DTV_SETTING_CA_CARD_LOGGED_OUT;
                    //showCAMessage("没有授权，无法收看此节目");
                }
                break;
            }
        }
        break;

        case 0x0a:
        {
            if (LOWORD(ca_msg_type) == 0x00)
            {
                ca_msg_type = 0x00020000;//let other process
            }
            else if (LOWORD(ca_msg_type) == 0x01)
            {
                lang_id[1] = STRING_DTV_SETTING_CA_DAUGCARD_ACTIVATE;
                //showCAMessage("子卡可临时激活");
            }
            else if (LOWORD(ca_msg_type) == 0x02)
            {
                lang_id[1] = STRING_DTV_SETTING_CA_DAUGCARD_EXPIRED;
                //showCAMessage("子卡已过期，请激活");
            }
        }
        break;

        case 0x0b:
        {
            lang_id[1] = STRING_DTV_SETTING_CA_CARD_MACHINE_ERR;
            //showCAMessage("机卡配对出错");
        }
        break;

        case 0x0d:
        {
            lang_id[1] = STRING_DTV_SETTING_CA_CARD_INVALID;
            //showCAMessage("无效卡");
        }
        break;

        case 0x30:
        case 0x0e:
        {
            lang_id[1] = STRING_DTV_SETTING_CA_CARD_NO_INSERT;
            //showCAMessage("未插卡");
        }
        break;

        case 0x31:
        {
            lang_id[1] = STRING_DTV_SETTING_CA_CARD_INIT;
            //showCAMessage("正在初始化卡")
            delay_time = 7;//7s
        }
        break;

        case 0x32:
        {
            lang_id[1] = STRING_DTV_SETTING_CA_CARD_ERR;
            //showCAMessage("非本系统ca节目，无法观看")
        }

        case 0x02: //let other process
            break;

        case 0x22:
        {
            else if (LOWORD(ca_msg_type) == 0xB8)
            {
                lang_id[1] = STRING_DTV_PAY_SERVICE;
            }
        }
        break;

        default:
            lang_id[1] = STRING_DTV_SETTING_CA_ERR;
            return ;
    }

#endif
    last_ca_msg = ca_msg_type;

    if (Is_ca_message_menu == EPDK_TRUE)
    {
        return ;
    }

    dtv_delete_all_menu(0);

    if (g_dtv_mainwin == NULL)
    {
        char buf[128] = {0};
        GUI_WinGetName(g_dtv_mainwin, buf);
        __wrn("current active win:%s\r\n", buf);
        return;
    }

#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)

    if (ca_msg_type == 0xD0)
    {
        if (h_dialog != NULL)
        {
            app_dialog_destroy(h_dialog);
            h_dialog = NULL;
        }

        return ;
    }
    else
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
    if (HIWORD(ca_msg_type) == 0x02)
    {
        if (h_dialog != NULL)
        {
            app_dialog_destroy(h_dialog);
            h_dialog = NULL;
        }

        return ;
    }
    else
#endif
    {
        if (h_dialog != NULL)
        {
            app_dialog_destroy(h_dialog);
            h_dialog = NULL;
            esKRNL_TimeDly(50);
        }

        //eLIBs_printf("11msg_type:0x%x\r\n", ca_msg_type);
        default_dialog(h_dialog, g_dtv_mainwin, 200, ADLG_OK, lang_id);

        //eLIBs_printf("[]h_dialog:0x%x\r\n", h_dialog);
        if (delay_time != 0)
        {
            esKRNL_TimeDly(delay_time * 100);
            app_dialog_destroy(h_dialog);
            h_dialog = NULL;
            last_ca_msg = 0;
        }
    }

    return;
}
#endif
#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
__s32 dtv_get_ca_user_info(Tfca_user_privacy_t *user_info)
{
    if (maple == NULL)
    {
        __wrn("maple null\r\n");
        return EPDK_OK;
    }

    if (user_info == NULL)
    {
        __wrn("user info null \r\n");
        return EPDK_FAIL;
    }

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_USER_INFO, 0, (void *)user_info))
    {
        __wrn("get user info ok\r\n");
        return EPDK_OK;
    }
    else
    {
        __wrn("get user info wrong\r\n");
        return EPDK_FAIL;
    }
}
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
__s32 dtv_get_ca_user_info(DIVI_Info_SC *user_info)
{
    if (maple == NULL)
    {
        __wrn("maple null\r\n");
        return EPDK_OK;
    }

    if (user_info == NULL)
    {
        __wrn("user info null \r\n");
        return EPDK_FAIL;
    }

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_USER_INFO, 0, (void *)user_info))
    {
        __wrn("get user info ok\r\n");
        return EPDK_OK;
    }
    else
    {
        __wrn("get user info wrong\r\n");
        return EPDK_FAIL;
    }
}
#elif (MAPLE_CA_STANDAR == DTV_CA_NONE)
__s32 dtv_get_ca_user_info(char *user_info)
{
    return EPDK_OK;
}
#endif

#if (MAPLE_CA_STANDAR == TONG_FANG_LING_XUN_CAS)
__s32 dtv_get_ca_author_info(Tfca_user_entitlement_t *author_info)
{
    if (maple == NULL)
    {
        __log("maple null\r\n");
        return EPDK_OK;
    }

    if (author_info == NULL)
    {
        __log("user info null \r\n");
        return EPDK_FAIL;
    }

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_AUTHOR_INFO, 0, (void *)author_info))
    {
        __log("get user info ok\r\n");
        return EPDK_OK;
    }
    else
    {
        __log("get user info wrong\r\n");
        return EPDK_FAIL;
    }
}
#elif (MAPLE_CA_STANDAR == SHU_SHI_TONG_CAS)
__s32 dtv_get_ca_author_info(DIVI_PackageInfo *author_info)
{
    if (maple == NULL)
    {
        __log("maple null\r\n");
        return EPDK_OK;
    }

    if (author_info == NULL)
    {
        __log("user info null \r\n");
        return EPDK_FAIL;
    }

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_AUTHOR_INFO, 0, (void *)author_info))
    {
        __log("get user info ok\r\n");
        return EPDK_OK;
    }
    else
    {
        __log("get user info wrong\r\n");
        return EPDK_FAIL;
    }
}
#elif (MAPLE_CA_STANDAR == DTV_CA_NONE)
__s32 dtv_get_ca_author_info(char *author_info)
{
    return EPDK_OK;
}
#endif
#endif
