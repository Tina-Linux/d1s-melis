/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      :
* By        : hotlee
* Func      :
* Version   :
* ============================================================================================================
* 2013-06-05   hotlee  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "dtv_setting_ca.h"
#include <emodules/mod_ca.h>

#if ((MAPLE_CA_STANDAR == MEDIAGATE_CAS) || (MAPLE_CA_STANDAR == DVN_JET_CAS))
#define   TIPS_YES_NO_BMP_WIDTH     63
#define   TIPS_YES_NO_BMP_HEIGHT    33

static    __u32 last_ca_msg = 0xFFFFFFFF;
#define SET_TIPS_W  311
#define SET_TIPS_H  171


extern    H_WIN g_dtv_mainwin;
static H_WIN    h_dialog = NULL;
static unsigned char Is_ca_message_menu = EPDK_FALSE;
static __s32    last_key = -1;

extern __mp *maple;
enum
{
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    DTV_CA_VENDOR,
    DTV_CA_CARD_MEDIAGATE,
    DTV_CA_SW_VERSION,
    DTV_CA_CARD_SN,
    DTV_CA_PARTNER_STATUS,
    DTV_CA_CARD_ENABLE,
    DTV_CA_CARD_DISABLE,
    DTV_CA_ZONE_CODE,
    DTV_CA_CTRL_CODE,
    DTV_CA_CARD_QUOTA,
    DTV_CA_CARD_CONSUME,
    DTV_CA_RECENT_EXPIRED_DAY,
    DTV_CA_EIGEN_VALUE,
    DTV_CA_CARD_NO_INSERT,
    DTV_CA_CARD_EIGEN,
#elif (MAPLE_CA_STANDAR == DVN_JET_CAS)
    DTV_CA_SMART_CARD_NUM,
    DTV_CA_USER_ATTRIBUTE,
    DTV_CA_CAK_VERSION,
    DTV_CA_PAIRING_STATE,
    DTV_CA_PAIRED,
    DTV_CA_NOT_PAIRED,
    DTV_CA_CARD_NO_INSERT,
#endif
    DTV_SETTING_CA_USER_INFO_NUM,
};

__s32 string_id[DTV_SETTING_CA_USER_INFO_NUM] =
{
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    STRING_DTV_CA_CARD_VENDOR,
    STRING_DTV_CA_CARD_MEDIAGATE,
    STRING_DTV_CA_CARD_SW_VERSION,
    STRING_DTV_CA_CARD_SN,
    STRING_DTV_CA_CARD_PARTNER_STS,
    STRING_DTV_CA_CARD_ENABLE,
    STRING_DTV_CA_CARD_DISABLE,
    STRING_DTV_CA_CARD_ZONE_CODE,
    STRING_DTV_CA_CARD_CTRL_CODE,
    STRING_DTV_CA_CARD_QUOTA,
    STRING_DTV_CA_CARD_CONSUME,
    STRING_DTV_CA_CARD_RCNT_EXPDAY,
    STRING_DTV_CA_CARD_EIGEN,
    STRING_DTV_SETTING_CA_CARD_NO_INSERT,
    STRING_DTV_CA_CARD_EIGEN,
#elif (MAPLE_CA_STANDAR == DVN_JET_CAS)
    STRING_DTV_CA_SMART_CARD_NUM,
    STRING_DTV_CA_USER_ATTRIBUTE,
    STRING_DTV_CA_CAK_VERSION,
    STRING_DTV_CA_PAIRING_STATE,
    STRING_DTV_CA_PAIRED,
    STRING_DTV_CA_NOT_PAIRED,
    STRING_DTV_SETTING_CA_CARD_NO_INSERT,
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
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    MG_CA_MSINFO            Author;
#elif (MAPLE_CA_STANDAR == DVN_JET_CAS)
    DJ_AUTH_INFO            Author;
#endif
} Author_info_t;

typedef struct tag_user_info
{
    char                text[DTV_SETTING_CA_USER_INFO_NUM][128];
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    MG_CA_INFO      user;
#elif (MAPLE_CA_STANDAR == DVN_JET_CAS)
    DJ_CA_INFO      user;
#endif
} user_info_t;
typedef union
{
    user_info_t         user_info;
    Author_info_t       Author_info;
} ca_sub_info_t;

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
MG_CA_EIGEN gEigen;
#endif


typedef struct tag_ca_sub_ctrl
{
    setting_ca_sub_para_t   *ca_sub_para;
    GUI_FONT            *font;

    ca_sub_info_t       sub_info;
    H_THEME             ca_return_bmp;
    H_THEME             ca_left_bmp;
    H_THEME             ca_right_bmp;
    H_THEME             h_list_top_bmp;
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
    DTV_SETTING_CA_USER_INFO = 0,           // ca card info & eigen number when using mediagate
    DTV_SETTING_CA_AUTHORIZATION,           // ms info when using mediagate
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    DTV_SETTING_CA_EIGEN,
#endif
    DTV_SETTING_CA_NUM
};


static void dtv_setting_ca_init(ca_ctrl_t *ca_ctrl)
{
    ca_ctrl->ca_f_bmp = dsk_theme_open(ID_DTV_PRELIST_F_BMP);
    ca_ctrl->ca_uf_bmp = dsk_theme_open(ID_DTV_PRELIST_UF_BMP);
    ca_ctrl->h_list_top_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    memset(&gEigen, 0x0, sizeof(MG_CA_EIGEN));
#endif  // #if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    return ;
}

static void dtv_setting_ca_sub_init(ca_sub_ctrl_t *ca_sub_ctrl)
{
    __here__;
    ca_sub_ctrl->ca_return_bmp = dsk_theme_open(ID_DTV_TIP_F_BMP);
    ca_sub_ctrl->h_list_top_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);

    if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
    {
        ca_sub_ctrl->ca_left_bmp = dsk_theme_open(ID_DTV_CA_LEFT_BMP);
        ca_sub_ctrl->ca_right_bmp = dsk_theme_open(ID_DTV_CA_RIGHT_BMP);
        /*init author info*/
        eLIBs_memset(&ca_sub_ctrl->sub_info.Author_info, 0x00, sizeof(ca_sub_ctrl->sub_info.Author_info));
        dtv_get_ca_author_info((void *)&ca_sub_ctrl->sub_info.Author_info.Author);
        ca_sub_ctrl->sub_info.Author_info.current_page = 1;
    }
    else if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_USER_INFO)
    {
        __s32 Index = 0;
        /*user info init*/
        eLIBs_memset(&ca_sub_ctrl->sub_info.user_info, 0x00, sizeof(ca_sub_ctrl->sub_info.user_info));
        dtv_get_ca_user_info(&ca_sub_ctrl->sub_info.user_info.user);

        for (; Index < DTV_SETTING_CA_USER_INFO_NUM; Index++)
        {
            dsk_langres_get_menu_text(string_id[Index], ca_sub_ctrl->sub_info.user_info.text[Index],
                                      sizeof(ca_sub_ctrl->sub_info.user_info.text[Index]));
        }
    }

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
    else if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_EIGEN)
    {
        dtv_get_ca_ext_info((void *)&gEigen);
    }

#endif
    return ;
}

static void dtv_setting_ca_sub_uninit(ca_sub_ctrl_t *ca_sub_ctrl)
{
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
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

                case DTV_SETTING_CA_USER_INFO:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_USER_INFO, tmp, sizeof(tmp));
                }
                break;

                case DTV_SETTING_CA_AUTHORIZATION:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_AUTHORIZATION, tmp, sizeof(tmp));
                }
                break;

                case DTV_SETTING_CA_EIGEN:
                {
                    dsk_langres_get_menu_text(STRING_DTV_CA_CARD_EIGEN, tmp, sizeof(tmp));
                }
                break;
#else

                case DTV_SETTING_CA_USER_INFO:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_USER_INFO, tmp, sizeof(tmp));
                }
                break;

                case DTV_SETTING_CA_AUTHORIZATION:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_AUTHORIZATION, tmp, sizeof(tmp));
                }
                break;
#endif

                default:
                    break;
            }

            GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
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
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

                case DTV_SETTING_CA_USER_INFO:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_USER_INFO, tmp, sizeof(tmp));
                }
                break;

                case DTV_SETTING_CA_AUTHORIZATION:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_AUTHORIZATION, tmp, sizeof(tmp));
                }
                break;

                case DTV_SETTING_CA_EIGEN:
                {
                    dsk_langres_get_menu_text(STRING_DTV_CA_CARD_EIGEN, tmp, sizeof(tmp));
                }
                break;
#else

                case DTV_SETTING_CA_USER_INFO:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_USER_INFO, tmp, sizeof(tmp));
                }
                break;

                case DTV_SETTING_CA_AUTHORIZATION:
                {
                    dsk_langres_get_menu_text(STRING_DTV_SETTING_CA_AUTHORIZATION, tmp, sizeof(tmp));
                }
                break;
#endif

                default:
                    break;
            }

            GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
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
}

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
#define LEFT_TEXT_RECT_X        0
#define LEFT_TEXT_RECT_W        140
#define LEFT_TEXT_RECT_H        25
#define RIGHT_TEXT_RECT_X       LEFT_TEXT_RECT_X + LEFT_TEXT_RECT_W + 15
#define RIGHT_TEXT_RECT_W       160

static __s32 dtv_setting_ca_author_sub_paint(__gui_msg_t *msg)
{
    int  *temp = NULL;
    char text[256] = {0};
    GUI_RECT gui_rect = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;
    MG_CA_MSINFO *ms = NULL;
    __wrn("dtv_setting_ca_author_sub_paint\n");

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    __here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    ms = &ca_sub_ctrl->sub_info.Author_info.Author;
    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_CA_CARD_MSCARD);
    //1.bg:
    x = 0, y = SETTING_TOP_HIGH;
    //GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    GUI_SetColor(0xFFDBDCDC);
    GUI_FillRect(x, y, x + 310, y + 100);
    GUI_SetColor(GUI_BLACK);
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = y + 12;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    dsk_langres_get_menu_text(STRING_DTV_CA_CARD_M_S, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (ms->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));

        if (ms->isMain)
        {
            dsk_langres_get_menu_text(STRING_DTV_CA_CARD_MAIN, text, sizeof(text));
        }
        else
        {
            dsk_langres_get_menu_text(STRING_DTV_CA_CARD_SUB, text, sizeof(text));
        }

        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    dsk_langres_get_menu_text(STRING_DTV_CA_CARD_M_CARDNUM, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (ms->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));

        if (ms->isMain)
        {
            eLIBs_sprintf(text, "00000000");
        }
        else
        {
            eLIBs_sprintf(text, "%02x%02x%02x%02x", ms->num[0], ms->num[1], ms->num[2], ms->num[3]);
        }

        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    dsk_langres_get_menu_text(STRING_DTV_CA_CARD_RSD_ACTTIME, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (ms->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));

        if (ms->isMain)
        {
            eLIBs_sprintf(text, "0");
        }
        else
        {
            eLIBs_sprintf(text, "%d", ms->left_day);
        }

        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}

static __s32 dtv_setting_ca_info_sub_paint(__gui_msg_t *msg)
{
    GUI_RECT gui_rect = {0};
    unsigned char *temp = NULL;
    unsigned char text[256] = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;
    MG_CA_INFO *user = NULL;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    __here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    user = &ca_sub_ctrl->sub_info.user_info.user;
    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_CA_CARD_COND_RECP);
    // 1. bg
    x = 0;//(width-CA_BG_WIDTH)/2;
    y = SETTING_TOP_HIGH;
    //GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    GUI_SetColor(0xFFDBDCDC);
    GUI_FillRect(x, y, x + 310, y + 250);
    GUI_SetColor(GUI_BLACK);
    // 2. DTV_SETTING_CA_VENDOR,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_VENDOR], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_MEDIAGATE], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    // 3. DTV_SETTING_CA_SW_VERSION,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_SW_VERSION], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "v%01x.%01x.%01x", (__u8)(user->ca_sw_version >> 8),
                  (__u8)((user->ca_sw_version & 0x00F0) >> 4),
                  (__u8)(user->ca_sw_version & 0x000F));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    // 4. DTV_SETTING_CA_CARD_SN,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_SN], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;
    eLIBs_memset(text, 0x00, sizeof(text));

    if (user->ca_is_card)
    {
        eLIBs_sprintf(text, "%02x%02x%02x%02x%02x%02x%02x%02x", user->ca_card_sn[0], user->ca_card_sn[1], \
                      user->ca_card_sn[2], user->ca_card_sn[3], \
                      user->ca_card_sn[4], user->ca_card_sn[5], \
                      user->ca_card_sn[6], user->ca_card_sn[7]);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 5. DTV_SETTING_CA_PARTNER_STATUS,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_PARTNER_STATUS], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        if (user->ca_partner_status == 0)
        {
            GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_DISABLE], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
        else
        {
            GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_ENABLE], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 6. DTV_SETTING_CA_ZONE_CODE,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_ZONE_CODE], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%02d", user->ca_zone_code);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 7. DTV_SETTING_CA_CTRL_CODE,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CTRL_CODE], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    eLIBs_memset(text, 0x00, sizeof(text));
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%04d", user->ca_ctrl_code);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 8. DTV_SETTING_CA_CARD_QUOTA,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_QUOTA], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%d", user->ca_card_quota);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 9. DTV_SETTING_CA_CARD_CONSUME,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_CONSUME], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%d", user->ca_card_consume);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 10. DTV_SETTING_CA_RECENT_EXPIRED_DAY,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_RECENT_EXPIRED_DAY], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%d", user->ca_recent_expire_day);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}

static __s32 dtv_setting_ca_eigen(__gui_msg_t *msg)
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

    __here__;
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
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_CA_CARD_EIGEN);
    // 1. bg
    x = 0;//(width-CA_BG_WIDTH)/2;
    y = SETTING_TOP_HIGH;
    //GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    GUI_SetColor(0xFFDBDCDC);
    GUI_FillRect(x, y, x + 310, y + 75);
    GUI_SetColor(GUI_BLACK);
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = SETTING_TOP_HIGH + 12;
    gui_rect.x1 = 310;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;

    if (gEigen.ca_is_card)
    {
        __u8 *p = gEigen.eigen;
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
                      p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
        gui_rect.y0 = gui_rect.y1;
        gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x",
                      p[16], p[17], p[18], p[19], p[20], p[21], p[22], p[23], p[24], p[25], p[26], p[27], p[28], p[29], p[30], p[31]);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_CENTER | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}
#elif (MAPLE_CA_STANDAR == DVN_JET_CAS)
#define LEFT_TEXT_RECT_X        0
#define LEFT_TEXT_RECT_W        140
#define LEFT_TEXT_RECT_H        25
#define RIGHT_TEXT_RECT_X       LEFT_TEXT_RECT_X + LEFT_TEXT_RECT_W + 15
#define RIGHT_TEXT_RECT_W       160

static __s32 dtv_setting_ca_author_sub_paint(__gui_msg_t *msg)
{
    int  *temp = NULL;
    char text[64] = {0};
    GUI_RECT gui_rect = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;
    DJ_AUTH_INFO *djAuth = NULL;
    __wrn("dtv_setting_ca_author_sub_paint\n");

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    __here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    djAuth = &ca_sub_ctrl->sub_info.Author_info.Author;
    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(STRING_DTV_SETTING_CA_AUTHORIZATION);
    //1.bg:
    x = 0, y = SETTING_TOP_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = y + 30;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    dsk_langres_get_menu_text(STRING_DTV_CA_PRODUCT_ID, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (djAuth->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%04x", djAuth->ca_product_id);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = y + 30;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    dsk_langres_get_menu_text(STRING_DTV_CA_START_TIME, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (djAuth->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%02u%02u %02u %02u %02u:%02u:%02u", djAuth->ca_start_time[0], djAuth->ca_start_time[1], djAuth->ca_start_time[2],
                      djAuth->ca_start_time[3], djAuth->ca_start_time[4], djAuth->ca_start_time[5], djAuth->ca_start_time[6]);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = y + 30;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    dsk_langres_get_menu_text(STRING_DTV_CA_END_TIME, text, sizeof(text));
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (djAuth->ca_is_card)
    {
        eLIBs_memset(text, 0x00, sizeof(text));
        eLIBs_sprintf(text, "%04x", djAuth->ca_product_id);
        eLIBs_sprintf(text, "%02u%02u %02u %02u %02u:%02u:%02u", djAuth->ca_end_time[0], djAuth->ca_end_time[1], djAuth->ca_end_time[2],
                      djAuth->ca_end_time[3], djAuth->ca_end_time[4], djAuth->ca_end_time[5], djAuth->ca_end_time[6]);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}

static __s32 dtv_setting_ca_info_sub_paint(__gui_msg_t *msg)
{
    GUI_RECT gui_rect = {0};
    unsigned char *temp = NULL;
    unsigned char text[32] = {0};
    __u32 x = 0, y = 0, h = 0, w = 0;
    ca_sub_ctrl_t *ca_sub_ctrl = NULL;
    DJ_CA_INFO *user = NULL;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(msg->h_deswin)) == GUI_LYRWIN_STA_SUSPEND)
    {
        __wrn("GUI_LyrWinGetSta  == GUI_LYRWIN_STA_SUSPEND \r\n");
        return EPDK_FAIL;
    }

    __here__;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (NULL == ca_sub_ctrl)
    {
        __wrn("ca_sub_ctrl == NULL\n");
        return EPDK_FAIL;
    }

    user = &ca_sub_ctrl->sub_info.user_info.user;
    GUI_LyrWinSel(ca_sub_ctrl->ca_sub_para->layer);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    /*title*/
    dtv_setting_ca_title_update(ca_sub_ctrl->h_list_top_bmp, STRING_DTV_CA_CARD_COND_RECP);
    // 1. bg
    x = 0;//(width-CA_BG_WIDTH)/2;
    y = SETTING_TOP_HIGH;
    GUI_BMP_Draw(theme_hdl2buf(ca_sub_ctrl->ca_background_bmp), x, y);
    // 2. DTV_CA_SMART_CARD_NUM,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = y;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_SMART_CARD_NUM], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);

    if (user->ca_is_card)
    {
        eLIBs_sprintf(text, "%02x%02x%02x%02x%02x%02x%02x%02x", user->ca_card_sn[0], user->ca_card_sn[1], \
                      user->ca_card_sn[2], user->ca_card_sn[3], \
                      user->ca_card_sn[4], user->ca_card_sn[5], \
                      user->ca_card_sn[6], user->ca_card_sn[7]);
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 3. DTV_CA_USER_ATTRIBUTE,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_USER_ATTRIBUTE], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;
    eLIBs_memset(text, 0x00, sizeof(text));
    eLIBs_sprintf(text, "%s", user->ca_user_property);
    text[16] = text[17] = 0;
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    // 4. DTV_CA_CAK_VERSION,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CAK_VERSION], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;
    eLIBs_memset(text, 0x00, sizeof(text));

    if (user->ca_is_card)
    {
        eLIBs_sprintf(text, "%s", user->ca_version);
        text[6] = text[7] = 0;
        GUI_DispStringInRect(text, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    // 5. DTV_CA_PAIRING_STATE,
    gui_rect.x0 = LEFT_TEXT_RECT_X;
    gui_rect.y0 = gui_rect.y1;
    gui_rect.x1 = gui_rect.x0 + LEFT_TEXT_RECT_W;
    gui_rect.y1 = gui_rect.y0 + LEFT_TEXT_RECT_H;
    GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_PAIRING_STATE], &gui_rect, GUI_TA_RIGHT | GUI_TA_VCENTER);
    gui_rect.x0 = RIGHT_TEXT_RECT_X;
    gui_rect.x1 = gui_rect.x0 + RIGHT_TEXT_RECT_W;

    if (user->ca_is_card)
    {
        if (user->ca_partner_status == 0)
        {
            GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_NOT_PAIRED], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
        else
        {
            GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_PAIRED], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
    }
    else
    {
        GUI_DispStringInRect(ca_sub_ctrl->sub_info.user_info.text[DTV_CA_CARD_NO_INSERT], &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
    }

    return EPDK_OK;
}
#endif
extern H_WIN g_dtv_mainwin;
static __s32 dtv_setting_send_cmd_to_parent(__gui_msg_t *msg, __u32 id, __u32 data1, __u32 data2)
{
    __gui_msg_t dmsg;
    dmsg.id = id;

    if (msg)
    {
        dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
    }
    else
    {
        dmsg.h_deswin = g_dtv_mainwin;
    }

    dmsg.h_srcwin = NULL;
    dmsg.dwAddData1 = data1;
    dmsg.dwAddData2 = data2;
    dmsg.dwReserved = 0;
    GUI_SendNotifyMessage(&dmsg);
    return EPDK_OK;
}

static __s32  dtv_setting_ca_key_proc(__gui_msg_t *msg)
{
    ca_ctrl_t *ca_ctrl;
    ca_ctrl = (ca_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    __here__;

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (last_key)
        {
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                __here__;
                LISTBAR_PrevItem(ca_ctrl->h_listbar);
            }
            break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                __here__;
                LISTBAR_NextItem(ca_ctrl->h_listbar);
            }
            break;

            case GUI_MSG_KEY_ENTER:
            {
                __s32 Current_Item = LISTBAR_GetFocusItem(ca_ctrl->h_listbar);
                __wrn("Current_Item:%d\r\n", Current_Item);
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

                if (Current_Item == DTV_SETTING_CA_USER_INFO || Current_Item == DTV_SETTING_CA_AUTHORIZATION
                    || Current_Item == DTV_SETTING_CA_EIGEN)
#else
                if (Current_Item == DTV_SETTING_CA_USER_INFO || Current_Item == DTV_SETTING_CA_AUTHORIZATION)
#endif
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
        __here__;
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
            __here__;
            ca_ctrl->ca_para = ca_para;
            ca_ctrl->total = DTV_SETTING_CA_NUM;
            ca_ctrl->focus_id = ca_para->focus_id;
            ca_ctrl->focus_txt_color = dtv_SETTING_FOCUS_COLOR;
            ca_ctrl->unfocus_txt_color = dtv_SETTING_UNFOCUS_COLOR;
            GUI_WinSetAddData(msg->h_deswin, (__u32)ca_ctrl);
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
    __here__;
    ca_para = (setting_ca_para_t *)My_Malloc(0, sizeof(setting_ca_para_t));

    if (!ca_para)
    {
        __wrn("memory balloc fail........\n");
        return NULL;
    }

    eLIBs_memset(ca_para, 0, sizeof(setting_ca_para_t));
    ca_para->focus_id = para->focus_id;
    ca_para->layer = para->layer;
    __here__;
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

static __s32  dtv_setting_ca_sub_key_proc(__gui_msg_t *msg)
{
    ca_sub_ctrl_t *ca_sub_ctrl;
    ca_sub_ctrl = (ca_sub_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (last_key)
        {
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

                    __here__;
                    dtv_setting_ca_author_sub_paint(msg);
                }
            }

            return EPDK_OK;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
                {
                    __here__;

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

                    __here__;
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
            __here__;
            ca_sub_para = (setting_ca_sub_para_t *)GUI_WinGetAttr(msg->h_deswin);
            ca_sub_ctrl = (ca_sub_ctrl_t *)My_Malloc(0, sizeof(ca_sub_ctrl_t));

            if (!ca_sub_ctrl)
            {
                __err("memory balloc fail........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(ca_sub_ctrl, 0, sizeof(ca_ctrl_t));
            __here__;
            ca_sub_ctrl->ca_sub_para = ca_sub_para;
            GUI_WinSetAddData(msg->h_deswin, (__u32)ca_sub_ctrl);
            GUI_LyrWinSetSta(ca_sub_ctrl->ca_sub_para->layer, GUI_LYRWIN_STA_SLEEP);

            if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_USER_INFO)
            {
                __here__;
                dtv_setting_ca_sub_init(ca_sub_ctrl);
                dtv_setting_ca_info_sub_paint(msg);
            }
            else if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_AUTHORIZATION)
            {
                __here__;
                dtv_setting_ca_sub_init(ca_sub_ctrl);
                dtv_setting_ca_author_sub_paint(msg);
                __here__;
            }

#if (MEDIAGATE_CAS == MAPLE_CA_STANDAR)
            else if (ca_sub_ctrl->ca_sub_para->Current_item == DTV_SETTING_CA_EIGEN)
            {
                __here__;
                dtv_setting_ca_sub_init(ca_sub_ctrl);
                dtv_setting_ca_eigen(msg);
                __here__;
            }

#endif
            last_key = -1;
            GUI_LyrWinSetSta(ca_sub_ctrl->ca_sub_para->layer, GUI_LYRWIN_STA_ON);
            __here__;
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
    __here__;
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
    __here__;
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
    __here__;
    return (GUI_FrmWinCreate(&framewin_para));
}

void  dtv_setting_ca_sub_win_delete(H_WIN ca_sub_win)
{
    GUI_FrmWinDelete(ca_sub_win);
}

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
DTMB_CA_MSG mgCaMsg;
#endif  // #if (MAPLE_CA_STANDAR != MEDIAGATE_CAS)
__s32 dtv_ca_msg_callback(void *arg)
{
    __u32 *ca_msg_type = 0;
    __gui_msg_t dmsg;
    __wrn("dtv_ca_msg_callback\n");
    ca_msg_type = (__u32 *)arg;

    if (g_dtv_mainwin == NULL || maple_get_cur_service_list_num() <= 0)
    {
        eLIBs_printf("g_dtv_mainwin null, can't send message.cur_service_list_num:%d\n", maple_get_cur_service_list_num());
        return;
    }

    dmsg.h_deswin = g_dtv_mainwin;
    dmsg.id = GUI_MSG_COMMAND;
    dmsg.h_srcwin = NULL;
    dmsg.dwAddData1 = DTV_CA_MSG_DISPLAY;
#if (MAPLE_CA_STANDAR != MEDIAGATE_CAS)
    dmsg.dwAddData2 = *ca_msg_type;
#else
    dmsg.dwAddData2 = ((DTMB_CA_MSG *)ca_msg_type)->msg;
    eLIBs_memset(&mgCaMsg, 0x00, sizeof(mgCaMsg));
    eLIBs_memcpy(&mgCaMsg, ca_msg_type, sizeof(DTMB_CA_MSG));
#endif
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
    __wrn("dtv_ca_msg_clear\r\n");
    return ;
}
void dtv_ca_msg_resume(void)
{
    __wrn("dtv_ca_msg_resume last_ca_msg :0x%x\n", last_ca_msg);

    if (last_ca_msg != 0xFFFFFFFF)
    {
        dtv_ca_msg_display(last_ca_msg);
    }

    return ;
}

__u8 dtvTmpTxt[128] = {0, };
__u8 dtvTmpTxt1[128] = {0, };
__u8 dtvTmpTxt2[128] = {0, };
__u8 dtvMsgTxt[512] = {0, };
__u8 dtvCardIdTxt[32] = {0, };
__u8 dtvRollingBarType = 0;
__u8 dtvRollingBarTimes = 0;
__u8 dtvRollingTxt[512] = {0, };
__u16 dtvRollingTxtLen = 0;
__s32 urgentSvcId = 0;

void dtv_ca_msg_display(__u32 ca_msg_type)
{
    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_DTV_SETTING_CA_ERR};
    __s32 delay_time = 0;
    __u8  useStrBuf = 0;
    eLIBs_printf("%s %d\r\n", __FILE__, __LINE__);
    eLIBs_printf("msg_type:0x%x,g_dtv_mainwin:0x%x\n", ca_msg_type, g_dtv_mainwin);

    if (dtv_ctr.h_scan)
    {
        return;
    }

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)

    if (ca_msg_type != DTV_CA_EMM_FIGEIDENT)
    {
        if (mgCaMsg.svcId != dtv_get_cur_service_id())
        {
            __wrn("service id mismatch(%u:%u)\n", dtv_get_cur_service_id(), mgCaMsg.svcId);
            return;
        }
    }

    if (ca_msg_type == 9999)
    {
        __wrn("ca_msg_type 9999!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        dtv_show_preAuth_dialog();
        return;
    }

    switch (ca_msg_type)
    {
        case DTV_CA_ERR_AUTH_RM: //let other process
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            eLIBs_memset(dtvTmpTxt, 0x0, sizeof(dtvTmpTxt));
            eLIBs_memset(dtvTmpTxt1, 0x0, sizeof(dtvTmpTxt1));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_AUTH_RM1, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_AUTH_RM2, dtvTmpTxt1, sizeof(dtvTmpTxt1));
            __wrn("HIWORD(ca_msg_type) :%u\r\n", mgCaMsg.param1);
            eLIBs_sprintf(dtvMsgTxt, "%s %d\n%s", dtvTmpTxt, mgCaMsg.param1, dtvTmpTxt1);
            lang_id[1] = 0;
            useStrBuf = 1;
            break;

        case DTV_CA_ERR_AUTH_OVERDUE: //let other process
            lang_id[1] = STRING_DTV_CA_ERR_AUTH_OVERDUE;
            break;

        case DTV_CA_ERR_CARD_NOTFOUND:
            lang_id[1] = STRING_DTV_CA_ERR_CARD_NOTFOUND;
            break;

        case DTV_CA_SCARD_NOT_ACT:
            lang_id[1] = STRING_DTV_CA_SCARD_NOT_ACT;
            break;

        case DTV_CA_SCARD_ERR_ZONE:
            lang_id[1] = STRING_DTV_CA_SCARD_ERR_ZONE;
            break;

        case DTV_CA_ERR_AUTHEXPIRE:
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            eLIBs_memset(dtvTmpTxt, 0x0, sizeof(dtvTmpTxt));
            eLIBs_memset(dtvTmpTxt1, 0x0, sizeof(dtvTmpTxt1));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_AUTHEXPIRE1, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_AUTHEXPIRE2, dtvTmpTxt1, sizeof(dtvTmpTxt1));
            eLIBs_sprintf(dtvMsgTxt, "%s\n%s", dtvTmpTxt, dtvTmpTxt1);
            lang_id[1] = 0;
            useStrBuf = 1;
            break;

        case DTV_CA_ERR_NOT_AUTHORED:
            lang_id[1] = STRING_DTV_CA_ERR_NOT_AUTHORED;
            break;

        case DTV_CA_ERR_CARD_LOCKED:
            lang_id[1] = STRING_DTV_CA_ERR_CARD_LOCKED;
            break;

        case DTV_CA_ERR_APP_LOCKED:
            lang_id[1] = STRING_DTV_CA_ERR_APP_LOCKED;
            break;

        case DTV_CA_ERR_EXTER_AUTHEN:
            lang_id[1] = STRING_DTV_CA_ERR_EXTER_AUTHEN;
            break;

        case DTV_CA_ERR_GET_CARD_DATA:
            lang_id[1] = STRING_DTV_CA_ERR_GET_CARD_DATA;
            break;

        case DTV_CA_ERR_ILLEGAL_CARD:
            lang_id[1] = STRING_DTV_CA_ERR_ILLEGAL_CARD;
            break;

        case DTV_CA_ERR_CARD_NOTMATCH:
            lang_id[1] = STRING_DTV_CA_ERR_CARD_NOTMATCH;
            break;

        case DTV_CA_ERR_UPDATE_GK:
            lang_id[1] = STRING_DTV_CA_ERR_UPDATE_GK;
            break;

        case DTV_CA_ERR_CONTROL_PLAY:
            lang_id[1] = STRING_DTV_CA_ERR_CONTROL_PLAY;
            break;

        case DTV_CA_ERR_AUTH_CLOSED:
            lang_id[1] = STRING_DTV_CA_ERR_AUTH_CLOSED;
            break;

        case DTV_CA_ERR_AUTH_UPDATE:
            lang_id[1] = STRING_DTV_CA_ERR_AUTH_UPDATE;
            break;

        case DTV_CA_ERR_ILLEGAL_AUTH:
            lang_id[1] = STRING_DTV_CA_ERR_ILLEGAL_AUTH;
            break;

        case DTV_CA_ERR_CARD_OPERATE:
            lang_id[1] = STRING_DTV_CA_ERR_CARD_OPERATE;
            break;

        case DTV_CA_ERR_PPV:
            lang_id[1] = STRING_DTV_CA_ERR_PPV;
            break;

        case DTV_CA_ERR_PINERR_DEV_PAIRED:
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            eLIBs_memset(dtvTmpTxt, 0x0, sizeof(dtvTmpTxt));
            eLIBs_memset(dtvTmpTxt1, 0x0, sizeof(dtvTmpTxt1));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_PIN_AUTHEN, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_PINERR_DEV_PAIRED, dtvTmpTxt1, sizeof(dtvTmpTxt1));
            eLIBs_sprintf(dtvMsgTxt, "%s\n%s", dtvTmpTxt, dtvTmpTxt1);
            lang_id[1] = 0;
            useStrBuf = 1;
            break;

        case DTV_CA_ERR_PINERR_CARD_PAIRED:
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            eLIBs_memset(dtvTmpTxt, 0x0, sizeof(dtvTmpTxt));
            eLIBs_memset(dtvTmpTxt1, 0x0, sizeof(dtvTmpTxt1));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_PIN_AUTHEN, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_PINERR_CARD_PAIRED, dtvTmpTxt1, sizeof(dtvTmpTxt1));
            eLIBs_sprintf(dtvMsgTxt, "%s\n%s", dtvTmpTxt, dtvTmpTxt1);
            lang_id[1] = 0;
            useStrBuf = 1;
            break;

        case DTV_CA_ERR_CARD_WILL_LOCK:
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            eLIBs_memset(dtvTmpTxt, 0x0, sizeof(dtvTmpTxt));
            eLIBs_memset(dtvTmpTxt1, 0x0, sizeof(dtvTmpTxt1));
            eLIBs_memset(dtvTmpTxt2, 0x0, sizeof(dtvTmpTxt2));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_PIN_AUTHEN, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_CARD_WILL_LOCK1, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_ERR_CARD_WILL_LOCK2, dtvTmpTxt2, sizeof(dtvTmpTxt2));
            eLIBs_sprintf(dtvMsgTxt, "%s\n%s%d%s", dtvTmpTxt, dtvTmpTxt1, mgCaMsg.param1, dtvTmpTxt2);
            lang_id[1] = 0;
            useStrBuf = 1;
            break;

        case DTV_CA_EMM_APP_STOP:
            lang_id[1] = STRING_DTV_CA_EMM_APP_STOP;
            break;

        case DTV_CA_EMM_APP_RESUME:
            lang_id[1] = STRING_DTV_CA_EMM_APP_RESUME;
            break;

        case DTV_CA_EMM_PROG_UNAUTH:
            lang_id[1] = STRING_DTV_CA_EMM_PROG_UNAUTH;
            break;

        case DTV_CA_EMM_PROG_AUTH:
            lang_id[1] = STRING_DTV_CA_EMM_PROG_AUTH;
            break;

        case DTV_CA_EMM_EN_PARTNER:
            lang_id[1] = STRING_DTV_CA_EMM_EN_PARTNER;
            break;

        case DTV_CA_EMM_DIS_PARTNER:
            lang_id[1] = STRING_DTV_CA_EMM_DIS_PARTNER;
            break;

        case DTV_CA_EMM_SONCARD:
            lang_id[1] = STRING_DTV_CA_EMM_SONCARD;
            break;

        case DTV_CA_EMM_UPDATE_GNO:
            lang_id[1] = STRING_DTV_CA_EMM_UPDATE_GNO;
            break;

        case DTV_CA_EMM_UPDATE_GK:
            lang_id[1] = STRING_DTV_CA_EMM_UPDATE_GK;
            break;

        case DTV_CA_EMM_FIGEIDENT:
            eLIBs_sprintf(dtvCardIdTxt, "%02x%02x%02x%02x%02x%02x%02x%02x", mgCaMsg.msgTxt[0], mgCaMsg.msgTxt[1]
                          , mgCaMsg.msgTxt[2], mgCaMsg.msgTxt[3], mgCaMsg.msgTxt[4], mgCaMsg.msgTxt[5],
                          mgCaMsg.msgTxt[6], mgCaMsg.msgTxt[7]);
            dtv_show_cardId_dialog();
            return;

        case DTV_CA_EMM_URGENT_SERVICE:
        {
            __s32 svcIdx;
            lang_id[1] = STRING_DTV_CA_EMM_URGENT_SERVICE;
            urgentSvcId = mgCaMsg.param1;
            svcIdx = maple_check_serviceid_exist(urgentSvcId);

            if (svcIdx != -1)
            {
                if (svcIdx != maple_get_cur_channel_Idx())
                {
                    dtv_setting_send_cmd_to_parent(NULL, GUI_MSG_COMMAND, DTV_SETTING_SWITCH_PLAY, svcIdx);
                }
            }
        }
        break;

        case DTV_CA_EMM_MODIFY_ZONECODE:
            lang_id[1] = STRING_DTV_CA_EMM_MODIFY_ZONECODE;
            break;

        case DTV_CA_EMM_UPDATE_CRT:
            lang_id[1] = STRING_DTV_CA_EMM_UPDATE_CRT;
            break;

        case DTV_CA_EMM_MODIFY_CTRLCODE:
            lang_id[1] = STRING_DTV_CA_EMM_MODIFY_CTRLCODE;
            break;

        case DTV_CA_EMM_CHARGE:
        {
            eLIBs_memset(dtvTmpTxt, 0x0, sizeof(dtvTmpTxt));
            eLIBs_memset(dtvTmpTxt1, 0x0, sizeof(dtvTmpTxt1));
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_EMM_CHARGE1, dtvTmpTxt, sizeof(dtvTmpTxt));
            dsk_langres_get_menu_text(STRING_DTV_CA_EMM_CHARGE2, dtvTmpTxt1, sizeof(dtvTmpTxt1));
            eLIBs_sprintf(dtvMsgTxt, "%s\n%d%s", dtvTmpTxt, mgCaMsg.param1, dtvTmpTxt1);
            lang_id[1] = 0;
            useStrBuf = 1;
        }
        break;

        case DTV_CA_EMM_MSG:
        {
            __s32 ret = 0, z = 0, k = 0;
            __u8 tmpTxt[256] = {0}, MsgTxt[256] = {0};
            lang_id[1] = 0;
            eLIBs_printf("MEMSET dtvMsgTxt:%s %d\n", __FILE__, __LINE__);
            eLIBs_printf("%s %d mgCaMsg.msgTxt len:%d\n", __FILE__, __LINE__, eLIBs_strlen(mgCaMsg.msgTxt));

            if (eLIBs_strlen(mgCaMsg.msgTxt) == 0)
            {
                return ;
            }

            eLIBs_memset(dtvMsgTxt, 0x0, sizeof(dtvMsgTxt));
            eLIBs_memcpy(MsgTxt, mgCaMsg.msgTxt, 128);
#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
            ret = eLIBs_charset_convert(EPDK_CHARSET_ENM_GB2312, EPDK_CHARSET_ENM_UTF8, MsgTxt, 128, tmpTxt, 256);
#endif
            tmpTxt[ret] = '\0';
            k = 0;

            for (z = 0; z < ret; z++)
            {
                if (tmpTxt[z] == '\0')
                {
                    break;
                }

                if (tmpTxt[z] == 0x0d && tmpTxt[z + 1] == 0x0a)
                {
                    dtvMsgTxt[k++] = 0x20;
                    dtvMsgTxt[k++] = 0x20;
                    dtvMsgTxt[k++] = 0x20;
                    dtvMsgTxt[k++] = 0x20;
                    dtvMsgTxt[k++] = 0x20;
                    z++;
                }
                else
                {
                    dtvMsgTxt[k++] = tmpTxt[z];
                }
            }

            useStrBuf = 1;
        }
        break;

        case DTV_CA_EMM_OSD:
        {
            dtvRollingBarType = (__u8)mgCaMsg.param1;
            dtvRollingBarTimes = (__u8)mgCaMsg.param2;
            eLIBs_printf("dtvRollingBarType : %u, dtvRollingBarTimes : %u\n", dtvRollingBarType, dtvRollingBarTimes);
            eLIBs_printf("%s %d dtvRollingBarTimes;%d\r\n", __FILE__, __LINE__, dtvRollingBarTimes);

            if (dtvRollingBarType == 0x21 || dtvRollingBarType == 0x22)
            {
                int dummyLen = (SCREEN_WIDTH + 7) / 8;
                int z = 0;
                int k = 0;
                eLIBs_memset(dtvRollingTxt, 0x0, sizeof(dtvRollingTxt));
                eLIBs_memcpy(dtvRollingTxt, mgCaMsg.msgTxt, 128);
                dtv_show_rollingTxt_dialog(dtvRollingBarType);
            }

            return;
        }

        case DTV_CA_EMM_EMAIL:
            return;

        case DTV_CA_CARD_INSERTED:
            lang_id[1] = STRING_DTV_SETTING_CA_CARD_ALREADY_INSERT;
            delay_time = 3;
            break;

        case DTV_CA_PAY_SERVICE:
        {
            lang_id[1] = STRING_DTV_PAY_SERVICE;
            break;
        }

        default:
            lang_id[1] = STRING_DTV_SETTING_CA_ERR;
            return ;
    }

#endif
    {
        extern dtv_ctr_t  dtv_ctr;

        if (dtv_ctr.h_loading != NULL)
        {
            app_loading_win_hide(dtv_ctr.h_loading);
        }
    }

    if (ca_msg_type == last_ca_msg && h_dialog)
    {
        return;
    }

    last_ca_msg = ca_msg_type;

    if (Is_ca_message_menu == EPDK_TRUE || dtv_ctr.h_setting || dtv_ctr.h_serverlist)
    {
        return;
    }

    //dtv_delete_all_menu(0);

    if (g_dtv_mainwin == NULL)
    {
        char buf[128] = {0};
        GUI_WinGetName(g_dtv_mainwin, buf);
        __wrn("current active win:%s\r\n", buf);
        return;
    }

    {
        if (h_dialog != NULL)
        {
            app_dialog_destroy(h_dialog);
            h_dialog = NULL;
            esKRNL_TimeDly(50);
        }

        __wrn("11msg_type:0x%x\r\n", ca_msg_type);

        if (useStrBuf == 0)
        {
            default_dialog(h_dialog, g_dtv_mainwin, 200, ADLG_OK, lang_id);
        }
        else
        {
            default_dialog_ex(h_dialog, g_dtv_mainwin, 200, ADLG_NOTHING, lang_id, dtvMsgTxt);
        }

        __wrn("[]h_dialog:0x%x\r\n", h_dialog);

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

__s32 dtv_get_ca_user_info(void *user_info)
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

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_USER_INFO, 0, user_info))
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

__s32 dtv_get_ca_author_info(void *author_info)
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

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_AUTHOR_INFO, 0, author_info))
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

#if (MAPLE_CA_STANDAR == MEDIAGATE_CAS)
__s32 dtv_get_ca_ext_info(void *ext_info)
{
    if (maple == NULL)
    {
        __log("maple null\r\n");
        return EPDK_OK;
    }

    if (ext_info == NULL)
    {
        __log("user info null \r\n");
        return EPDK_FAIL;
    }

    if (EPDK_OK == esMODS_MIoctrl(maple, MAPLE_CMD_GET_CA_EXT_INFO, 0, ext_info))
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
#endif
#endif

