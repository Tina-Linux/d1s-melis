/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dtv_setting_disp_general.c
* By        : hot.lee
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  hot.lee  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include <log.h>
#include "dtv_setting_uipara.h"
#include "dtv_setting_disp.h"

#define MAX_LISTBAR_ON_PAGE  6

static __s32 last_key = -1;

static __u32 dtv_disp_setting[] =
{
    //STRING_DTV_SETTING_ENHANCE,
    STRING_DTV_SETTING_BRIGHT,
    STRING_DTV_SETTING_CONTRAST,
    STRING_DTV_SETTING_EDGE,
    //STRING_DTV_SETTING_HUE,
    STRING_DTV_SETTING_SATURATION,
    STRING_DTV_SETTING_DETAIL,
    STRING_DTV_SETTING_RADIO,
};

static __s32 content_display_mode_id[][2] = //ITEM1 (背光亮度)
{
    {STRING_DTV_DEFAULT,      MAPLE_VID_WINDOW_BESTSHOW },             //* 不关心图片显示比例，以当前设置的比例
    {STRING_DTV_ORGIN_SIZE,   MAPLE_VID_WINDOW_ORIGINAL,  },           //* 以图片原始大小在窗口内显示，不能溢出窗口
    {STRING_DTV_VIDEO_RATIO,  MAPLE_VID_WINDOW_BESTSHOW,  },           //* 以图片本身的比例缩放至满窗口显示，图片不变形
    {STRING_DTV_SCREEN_RATIO, MAPLE_VID_WINDOW_FULLSCN,  },            //* 以窗口的比例缩放图片至满窗口显示，可能会变形
    {STRING_DTV_4R3,          MAPLE_VID_WINDOW_4R3MODE,   },           //* 强制以4:3的模式满屏显示视频图像，图像会变形
    {STRING_DTV_16R9,         MAPLE_VID_WINDOW_16R9MODE,  },           //* 强制以16:9的模式满屏显示视频图像，图像会变形
};

typedef struct tag_dtv_setting_disp_focus_item_id
{
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} dtv_setting_disp_focus_item_id_t;

typedef struct tag_dtv_setting_disp_attr
{
    HTHEME      h_item_focus_bmp;
    HTHEME      h_item_unfocus_bmp;
    HTHEME      h_item_F_icon_L_bmp;
    HTHEME      h_item_F_icon_R_bmp;
    HTHEME      h_item_UF_icon_L_bmp;
    HTHEME      h_item_UF_icon_R_bmp;


    HTHEME      h_list_top_bmp;

    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_LBAR   h_listbar;
    H_WIN   layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;
    dtv_setting_disp_para_t *para;

    __s32 total;
    //set_para* uipara;
} dtv_setting_disp_attr_t;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, __u32 data1, __u32 data2);

static __s32 dtv_setting_disp_item_paint(dtv_setting_disp_attr_t *gattr, __lbar_draw_para_t *draw_param);
static __s32 dtv_setting_disp_item_paint_ex(__lbar_draw_para_t *draw_param);

static void dtv_setting_disp_res_init(dtv_setting_disp_attr_t *attr);
static void _dtv_setting_disp_res_uninit(dtv_setting_disp_attr_t *attr) ;

__s32 get_next_mode(__s32 mode_id)
{
    int i;

    for (i = 1; i < sizeof(content_display_mode_id) / sizeof(__s32) / 2; i++)
    {
        if (mode_id == content_display_mode_id[i][1])
        {
            if (i == sizeof(content_display_mode_id) / sizeof(__s32) / 2 - 1)
            {
                return (content_display_mode_id[1][1]);
            }
            else
            {
                return (content_display_mode_id[i + 1][1]);
            }
        }
    }

    return (content_display_mode_id[0][1]);
}


static void dtv_setting_title_update(dtv_setting_disp_attr_t *gattr, __u32 string_id)
{
    char tmp[128] = {0};
    GUI_RECT gui_rect = {0};
    SIZE size = {0};
    GUI_LyrWinSel(gattr->layer);
    GUI_SetFont(gattr->font);
    dsk_display_get_size((__s32 *)&size.width, (__s32 *)&size.height);
    GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_list_top_bmp), 0, 0);
    gui_rect.x0 = 0;//(size.width - LISTBAR_WIDTH)/2;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH;
    gui_rect.y1 = SETTING_TOP_HIGH;
    dsk_langres_get_menu_text(string_id, tmp, sizeof(tmp));
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}


static __s32 dtv_setting_disp_item_paint(dtv_setting_disp_attr_t *gattr, __lbar_draw_para_t *draw_param)
{
    char shdl_name[32] = {0};
    char shdl_name1[32] = {0};
    GUI_RECT gui_rect;
    reg_system_para_t   *system_para = NULL;

    if (NULL == gattr)
    {
        return EPDK_FAIL;
    }

    system_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    //GUI_LyrWinSel(gattr->layer);
    __msg("gui_rect.x0 = %d\n", gui_rect.x0);
    __msg("gui_rect.x1 = %d\n", gui_rect.x1);
    __msg("gui_rect.y0 = %d\n", gui_rect.y0);
    __msg("gui_rect.y1 = %d\n", gui_rect.y1);
    //eLIBs_memset(shdl_name,0,sizeof(shdl_name));
    __msg("draw_param->index=%d \n", draw_param->index);

    switch (draw_param->index)
    {
#if 0

        case    DTV_DISP_SET_DENOISE:
        {
            __s32 enable = 0;
            enable = dsk_display_get_lcd_denoise();

            if (enable)
            {
                dsk_langres_get_menu_text(STRING_DTV_SETTING_ENHANCE_ON, shdl_name, sizeof(shdl_name));
            }
            else
            {
                dsk_langres_get_menu_text(STRING_DTV_SETTING_ENHANCE_OFF, shdl_name, sizeof(shdl_name));
            }

            break;
        }

#endif

        case    DTV_DISP_BLIGHT:  //亮度
        {
            __u32 blight = 0;
            blight = system_para->lcd_bright;//dsk_display_get_lcd_bright();
            __msg("blight = %d \n", blight);

            if (10 < blight)
            {
                blight = 0;
            }

            eLIBs_sprintf(shdl_name1, "%d\%\n", blight * 10);
            break;
        }

        case    DTV_DISP_CONTRAST://对比度
        {
            __u32 constract = 0;
            constract = system_para->lcd_constract;//dsk_display_get_lcd_constract();
            __msg("constract = %d \n", constract);
            eLIBs_sprintf(shdl_name1, "%d\%\n", constract * 10);
            break;
        }

        case    DTV_DISP_EDGE://锐度
        {
            __u32 lcd_hue = 0;
            lcd_hue = system_para->lcd_hue;//dsk_display_get_lcd_edge();
            __msg("constract = %d \n", lcd_hue);
            eLIBs_sprintf(shdl_name1, "%d\%\n", lcd_hue * 10);
            break;
        }

#if 0

        case    DTV_DISP_HUE:       //色调
        {
            __u32 hue = 0;
            hue = dsk_display_get_lcd_hue();
            __msg("hue = %d \n", hue);
            eLIBs_sprintf(shdl_name, "%s%d0\%\n", shdl_name, hue);
            break;
        }

#endif

        case    DTV_DISP_SATURATION://饱和度
        {
            __u32 saturation = 0;
            saturation = system_para->lcd_saturation;//dsk_display_get_lcd_saturation();
            __msg("saturation = %d \n", saturation);
            eLIBs_sprintf(shdl_name1, "%d\%\n", saturation * 10);
            break;
        }

        case    DTV_DISP_SET_DETAIL:
        {
            __u32 detail = 0;
            detail = system_para->lcd_detail;//dsk_display_get_lcd_detail();
            __msg("detail = %d \n", detail);
            eLIBs_sprintf(shdl_name1, "%d\%\n", detail * 10);
            break;
            break;
        }

        case    DTV_DISP_RADIO :   //图像比例
        {
            __s8 index = maple_get_screensize();

            if (index < MAPLE_VID_WINDOW_ORIGINAL)
            {
                index = MAPLE_VID_WINDOW_ORIGINAL;
            }
            else if (index > MAPLE_VID_WINDOW_16R9MODE)
            {
                index = MAPLE_VID_WINDOW_16R9MODE;
            }

            dsk_langres_get_menu_text(content_display_mode_id[index][0], shdl_name1, sizeof(shdl_name1));
            break;
        }
    }

    if (LBAR_MODE_FOCUS == draw_param->mode)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_focus_bmp), draw_param->rect.x, draw_param->rect.y);
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_F_icon_L_bmp), draw_param->rect.x + LISTBAR_WIDTH / 2, draw_param->rect.y + (LISTBAR_HIGH - 16) / 2);
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_F_icon_R_bmp), draw_param->rect.x + LISTBAR_WIDTH - 16, draw_param->rect.y + (LISTBAR_HIGH - 16) / 2);
    }
    else if (LBAR_MODE_NORMAL == draw_param->mode)
    {
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_unfocus_bmp), draw_param->rect.x, draw_param->rect.y);
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_UF_icon_L_bmp), draw_param->rect.x + LISTBAR_WIDTH / 2, draw_param->rect.y + (LISTBAR_HIGH - 16) / 2);
        GUI_BMP_Draw(dsk_theme_hdl2buf(gattr->h_item_UF_icon_R_bmp), draw_param->rect.x + LISTBAR_WIDTH - 16, draw_param->rect.y + (LISTBAR_HIGH - 16) / 2);
    }

    GUI_SetColor(gattr->unfocus_txt_color);
    dsk_langres_get_menu_text(dtv_disp_setting[draw_param->index], shdl_name, sizeof(shdl_name));
    gui_rect.x0 = draw_param->rect.x;
    gui_rect.y0 = draw_param->rect.y;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH / 2;
    gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
    GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    __msg("shdl_name=%s \n", shdl_name);
    gui_rect.x0 = draw_param->rect.x + LISTBAR_WIDTH / 2;
    gui_rect.y0 = (draw_param->index != DTV_DISP_RADIO) ? (draw_param->rect.y + 8) : draw_param->rect.y;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH / 2;
    gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
    GUI_DispStringInRect(shdl_name1, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return 0;
}

static __s32 dtv_setting_disp_item_paint_ex(__lbar_draw_para_t *draw_param)
{
    __s32 i = 0;
    RECT rect = {0};
    GUI_RECT gui_rect;
    dtv_setting_disp_attr_t *gattr = NULL;
    //GUI_MEMDEV_Handle draw_mem;
    maple_schdl_event_t *shdl_event = NULL;
    char time[32] = {0};
    char tmp[32] = {0};
    char shdl_name[32] = {0};
    gattr = (dtv_setting_disp_attr_t *)draw_param->attr;

    if (gattr == NULL)
    {
        return EPDK_FAIL;
    }

    //GUI_LyrWinSel(gattr->layer);
    GUI_SetFont(gattr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_LyrWinCacheOn();
    //draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y,draw_param->rect.width,draw_param->rect.height);
    //GUI_MEMDEV_Select(draw_mem);

    if (LBAR_MODE_MINIATURE != draw_param->mode)
    {
        __msg("draw_param->mode====%d \n", draw_param->mode);
    }

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
            dtv_setting_disp_item_paint(gattr, draw_param);
            break;

        case LBAR_MODE_FOCUS:
        {
            dtv_setting_disp_item_paint(gattr, draw_param);
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
    GUI_LyrWinCacheOff();
    return EPDK_OK;
}



static void dtv_setting_disp_listbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config = {0};
    RECT rect = {0};
    dtv_setting_disp_attr_t *attr = NULL;
    __s32 index;
    __s32 start_id, focus_id;
    attr = (dtv_setting_disp_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
    {
        start_id = 0;
        focus_id = attr->new_focus;
    }
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.start_id = start_id;
    config.focus_id = focus_id;
    config.item_cnt = attr->total;
    config.item_height = LISTBAR_HIGH;
    config.item_width = LISTBAR_WIDTH;
    config.list_rect.x = 0;
    config.list_rect.y = SETTING_TOP_HIGH;
    config.list_rect.width = LISTBAR_WIDTH;
    config.list_rect.height = attr->total * LISTBAR_HIGH;
    __msg("attr->total = %d\n", attr->total);
    __msg("attr->x = %d\n", config.list_rect.x);
    __msg("attr->y = %d\n", config.list_rect.y);
    __msg("attr->w = %d\n", config.list_rect.width);
    __msg("attr->h = %d\n", config.list_rect.height);
    config.list_attr = (void *)attr;
    attr->h_listbar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(attr->h_listbar, dtv_setting_disp_item_paint_ex, &config);
    LISTBAR_ShowPage(attr->h_listbar);
}




/*
    申请资源
*/
static void dtv_setting_disp_res_init(dtv_setting_disp_attr_t *attr)
{
    __u32   i;
    __bool  restore_flag;
    attr->h_item_focus_bmp = dsk_theme_open(ID_DTV_PRELIST_F_BMP);
    attr->h_item_unfocus_bmp = dsk_theme_open(ID_DTV_PRELIST_UF_BMP);
    attr->h_list_top_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);
    attr->h_item_F_icon_L_bmp = dsk_theme_open(ID_DTV_F_ICON_L_BMP);
    attr->h_item_F_icon_R_bmp = dsk_theme_open(ID_DTV_F_ICON_R_BMP);
    attr->h_item_UF_icon_L_bmp = dsk_theme_open(ID_DTV_UF_ICON_L_BMP);
    attr->h_item_UF_icon_R_bmp = dsk_theme_open(ID_DTV_UF_ICON_R_BMP);
}

static void _dtv_setting_disp_res_uninit(dtv_setting_disp_attr_t *attr)
{
    __u32   i;
    theme_close(attr->h_item_focus_bmp);
    attr->h_item_focus_bmp = 0;
    theme_close(attr->h_item_unfocus_bmp);
    attr->h_item_unfocus_bmp = 0;
    theme_close(attr->h_list_top_bmp);
    attr->h_list_top_bmp = 0;
    theme_close(attr->h_item_F_icon_L_bmp);
    attr->h_item_F_icon_L_bmp = 0;
    theme_close(attr->h_item_F_icon_R_bmp);
    attr->h_item_F_icon_R_bmp = 0;
    theme_close(attr->h_item_UF_icon_L_bmp);
    attr->h_item_UF_icon_L_bmp = 0;
    theme_close(attr->h_item_UF_icon_R_bmp);
    attr->h_item_UF_icon_R_bmp = 0;
}

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

static __s32 dtv_setting_disp_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    dtv_setting_disp_attr_t *attr;
    reg_system_para_t   *system_para = NULL;
    system_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    attr = (dtv_setting_disp_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    __msg("msg->dwAddData1 = %d msg->dwAddData2=%d \n", msg->dwAddData1, msg->dwAddData2);

    if (msg->dwAddData2 == KEY_DOWN_ACTION)
    {
        last_key = msg->dwAddData1;
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
                attr->old_focus = attr->new_focus;
                LISTBAR_PrevItem(attr->h_listbar);
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
                __msg("new_focus : %d \n", attr->new_focus);
            }

            break;
        }

        case GUI_MSG_KEY_NEXT:
        case GUI_MSG_KEY_LONGNEXT:
        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                attr->old_focus = attr->new_focus;
                LISTBAR_NextItem(attr->h_listbar);
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
                __msg("new_focus : %d \n", attr->new_focus);
            }

            break;
        }

        case GUI_MSG_KEY_LEFT:
            //case GUI_MSG_KEY_LONGLEFT:
        {
            __msg("attr->new_focus = %d \n", attr->new_focus);

            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                switch (attr->new_focus)
                {
#if 0

                    case     DTV_DISP_SET_DENOISE:
                    {
                        __s32 enable = 0;
                        enable = dsk_display_get_lcd_denoise();
                        dsk_display_set_lcd_denoise(!enable);
                        break;
                    }

#endif

                    case    DTV_DISP_BLIGHT:  //亮度
                    {
                        __s8 blight = 0;
                        blight = system_para->lcd_bright;//dsk_display_get_lcd_bright();
                        blight--;

                        if (0 > blight)
                        {
                            blight = 10;
                        }

                        __msg("blight======%d \n", blight);
                        dsk_display_set_lcd_bright(blight * 10);
                        system_para->lcd_bright = blight;
                        break;
                    }

                    case    DTV_DISP_CONTRAST://对比度
                    {
                        __s8 constract = 0;
                        constract = system_para->lcd_constract;//dsk_display_get_lcd_constract();
                        constract--;

                        if (0 > constract)
                        {
                            constract = 10;
                        }

                        __msg("blight======%d \n", constract);
                        dsk_display_set_lcd_constract(constract * 10);
                        system_para->lcd_constract = constract;
                        break;
                    }

                    case    DTV_DISP_EDGE://锐度
                    {
                        __s8 lcd_hue = 0;
                        lcd_hue = system_para->lcd_hue;//dsk_display_get_lcd_edge();
                        lcd_hue--;

                        if (0 > lcd_hue)
                        {
                            lcd_hue = 10;
                        }

                        __msg("lcd_hue======%d \n", lcd_hue);
                        dsk_display_set_lcd_hue(lcd_hue * 10);
                        system_para->lcd_hue = lcd_hue;//
                        break;
                    }

                    case    DTV_DISP_SATURATION://饱和度
                    {
                        __s8 saturation = 0;
                        saturation = system_para->lcd_saturation;//dsk_display_get_lcd_saturation();
                        saturation--;

                        if (0 > saturation)
                        {
                            saturation = 10;
                        }

                        __msg("blight======%d \n", saturation);
                        dsk_display_set_lcd_saturation(saturation * 10);
                        system_para->lcd_saturation = saturation; //
                        break;
                    }

                    case    DTV_DISP_SET_DETAIL://饱和度
                    {
                        __s8 detail = 0;
                        detail = system_para->lcd_detail;//dsk_display_get_lcd_detail();
                        detail--;

                        if (0 > detail)
                        {
                            detail = 10;
                        }

                        __msg("detail======%d \n", detail);
                        dsk_display_set_lcd_detail(detail * 10);
                        system_para->lcd_detail = detail; //
                        break;
                    }

                    case    DTV_DISP_RADIO :   //图像比例
                    {
                        __s8 radio = 0;
                        radio = maple_get_screensize();
                        radio--;

                        if (MAPLE_VID_WINDOW_ORIGINAL >= radio)
                        {
                            radio = MAPLE_VID_WINDOW_16R9MODE;
                        }

                        __msg("radio======%d \n", radio);
                        maple_set_screensize(radio);
                        maple_set_video_window_ratio_mode(radio);
                        break;
                    }
                }
            }

            LISTBAR_ShowPage(attr->h_listbar);
            break;
        }

        case GUI_MSG_KEY_RIGHT:

        //case GUI_MSG_KEY_LONGRIGHT:
        case GUI_MSG_KEY_ENTER:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                switch (attr->new_focus)
                {
#if 0

                    case     DTV_DISP_SET_DENOISE:
                    {
                        __s32 enable = 0;
                        enable = dsk_display_get_lcd_denoise();
                        dsk_display_set_lcd_denoise(!enable);
                        break;
                    }

#endif

                    case    DTV_DISP_BLIGHT:  //亮度
                    {
                        __s8 blight = 0;
                        blight = system_para->lcd_bright;//dsk_display_get_lcd_bright();
                        blight++;

                        if (10 < blight)
                        {
                            blight = 0;
                        }

                        dsk_display_set_lcd_bright(blight * 10);
                        system_para->lcd_bright = blight; //
                        break;
                    }

                    case    DTV_DISP_CONTRAST://对比度
                    {
                        __s8 contrast = 0;
                        contrast = system_para->lcd_constract;//dsk_display_get_lcd_constract();
                        contrast++;

                        if (10 < contrast)
                        {
                            contrast = 0;
                        }

                        dsk_display_set_lcd_constract(contrast * 10);
                        system_para->lcd_constract = contrast; //
                        break;
                    }

                    case    DTV_DISP_EDGE://锐度
                    {
                        __s8 lcd_hue = 0;
                        lcd_hue = system_para->lcd_hue;//dsk_display_get_lcd_edge();
                        lcd_hue++;

                        if (10 < lcd_hue)
                        {
                            lcd_hue = 0;
                        }

                        dsk_display_set_lcd_hue(lcd_hue * 10);
                        system_para->lcd_hue = lcd_hue; //
                        break;
                    }

                    case    DTV_DISP_SATURATION://饱和度
                    {
                        __s8 saturation = 0;
                        saturation = system_para->lcd_saturation;//dsk_display_get_lcd_saturation();
                        saturation++;

                        if (10 < saturation)
                        {
                            saturation = 0;
                        }

                        dsk_display_set_lcd_saturation(saturation * 10);
                        system_para->lcd_saturation = saturation; //
                        break;
                    }

                    case    DTV_DISP_SET_DETAIL://饱和度
                    {
                        __s8 detail = 0;
                        detail = system_para->lcd_detail;//dsk_display_get_lcd_detail();
                        detail++;

                        if (10 < detail)
                        {
                            detail = 0;
                        }

                        dsk_display_set_lcd_detail(detail * 10);
                        system_para->lcd_detail = detail; //
                        break;
                    }

                    case    DTV_DISP_RADIO :   //图像比例
                    {
                        __s8 radio = 0;
                        radio = maple_get_screensize();
                        radio++;

                        if (MAPLE_VID_WINDOW_16R9MODE  <= radio)
                        {
                            radio = MAPLE_VID_WINDOW_ORIGINAL;
                        }

                        maple_set_screensize(radio);
                        maple_set_video_window_ratio_mode(radio);
                        break;
                    }
                }
            }

            LISTBAR_ShowPage(attr->h_listbar);
            break;
        }

#if 0

        case GUI_MSG_KEY_ENTER:
        {
            if ((msg->dwAddData2 == KEY_UP_ACTION) && (last_key == msg->dwAddData1))
            {
                //attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
                //maple_set_screensize(content_display_mode_id[attr->new_focus][1]);
                dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, DTV_SETTING_BACK2LIST, 0);
            }

            break;
        }

#endif

        case GUI_MSG_KEY_LONGMENU:
        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_MENU:
        {
            if ((msg->dwAddData2 == KEY_UP_ACTION) && (last_key == msg->dwAddData1))
            {
                //__here__;
                dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, DTV_SETTING_BACK2LIST, 0);
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

static __s32 _dtv_setting_disp_listbar_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 ret = EPDK_OK;
    static __s32 is_touch_down = 0;
    static __s32 is_touch_moving = 0;
    dtv_setting_disp_attr_t *attr;
    attr = (dtv_setting_disp_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);

    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        case GUI_MSG_TOUCH_LONGDOWN:
        {
            GUI_WinSetCaptureWin(msg->h_deswin);
            //  LISTBAR_UpdatePage( attr->h_listbar );   /////////
            ret = LISTBAR_TouchDown(attr->h_listbar, x, y, direct, speed);
            is_touch_down = 1;
            break;
        }

        case GUI_MSG_TOUCH_MOVE:
        {
            if (is_touch_down == 1)
            {
                LISTBAR_TouchMove(attr->h_listbar, x, y, direct, speed);
                is_touch_moving = 1;
            }

            break;
        }

        case GUI_MSG_TOUCH_UP:
        {
            __gui_msg_t send;
            RECT rect;
            __bool chg = EPDK_FALSE;

            if (is_touch_down == 0)
            {
                break;
            }
            else
            {
                is_touch_down = 0;
            }

            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }

            LISTBAR_TouchUp(attr->h_listbar, x, y, direct, speed);
            __msg("----LISTBAR_TouchUp--x: %d, y: %d, direct: %d, speed: %d\n", x, y, direct, speed);
            {
                index = LISTBAR_GetFocusItem(attr->h_listbar);
                //__msg("index = %d\n" , index);
            }
            LISTBAR_GetItemRect(attr->h_listbar, index, &rect);
            __msg("----rect -- x: %d, y: %d, width: %d, height: %d\n", rect.x, rect.y, rect.width, rect.height);

            if (attr->old_focus == attr->new_focus)
            {
                chg = EPDK_TRUE;
            }

            attr->old_focus = attr->new_focus ;

            if ((x < rect.x) || (x > rect.x + rect.width) || (y < rect.y) || (y > rect.y + rect.height))
            {
                //__here__
                break;
            }
            else if (index != attr->new_focus)
            {
                attr->new_focus = index;
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

static __s32  dtv_setting_disp_touch_proc(__gui_msg_t *msg)
{
    return EPDK_OK;
}

static __s32 dtv_setting_get_cur_index(void)
{
    return EPDK_OK;
}

static __s32 create_flag = 0;

/*
    回调
*/
static __s32 _dtv_setting_disp_Proc(__gui_msg_t *msg)
{
    __wrn("msg->id:%d\n", msg->id);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            dtv_setting_disp_attr_t *attr;
            dtv_setting_disp_para_t *para;
            HTHEME  bg_bmp;
            para = (dtv_setting_disp_para_t *)GUI_WinGetAttr(msg->h_deswin);
            attr = (dtv_setting_disp_attr_t *)My_Malloc(0, sizeof(dtv_setting_disp_attr_t));

            if (!attr)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(attr, 0, sizeof(dtv_setting_disp_attr_t));
            attr->new_focus = para->focus_id;
            attr->old_focus = para->focus_id;
            attr->para = para;
            dtv_setting_disp_res_init(attr);
            attr->focus_txt_color = dtv_SETTING_FOCUS_COLOR;
            attr->unfocus_txt_color = dtv_SETTING_UNFOCUS_COLOR;
            attr->font = para->font;
            attr->layer = para->layer;
            attr->total = DTV_DISP_MAX;
            __msg("attr->total = %d\n", attr->total);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)attr);
            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_SLEEP);
            {
                dtv_setting_disp_listbar_init(msg);
                dtv_setting_title_update(attr, STRING_DTV_DISPLAY_SETTING);
            }
            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_ON);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            __wrn("-------------------------------------GUI_MSG_CLOSE enter, err.\n");
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            dtv_setting_disp_attr_t *attr = NULL;
            dtv_setting_disp_para_t *para = NULL;
            __s32 ret;
            attr = (dtv_setting_disp_attr_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!attr)
            {
                __wrn("attr is null ......\n");
                return EPDK_FAIL;
            }

            ret = LISTBAR_SuspendPage(attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar suspend fail\n");
            }

            ret = LISTBAR_Delete(attr->h_listbar);

            if (EPDK_FAIL == ret)
            {
                __msg("listbar delete fail\n");
            }

            attr->h_listbar = NULL;
            _dtv_setting_disp_res_uninit(attr);
            //dsk_reg_flush();  //写进文件里面
            para = attr->para;

            if (para)
            {
                My_Mfree(0, para);
                para = NULL;
            }

            if (attr)
            {
                My_Mfree(0, attr);
                attr = NULL;
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            return EPDK_OK;

        case GUI_MSG_KEY:
            return dtv_setting_disp_listbar_key_proc(msg);

        case GUI_MSG_TOUCH:
            return dtv_setting_disp_touch_proc(msg);////

        case GUI_MSG_COMMAND:
        {
            /*switch(LOWORD(msg->dwAddData1))
            {
                case DVBEPG_ID_CREATE_EPG_LIST:
                {
                    dtv_setting_disp_listbar_init(msg);
                }
                break;
                default:
                    break;
            }*/
        }
        break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN dtv_setting_disp_win_create(H_WIN h_parent, dtv_setting_disp_para_t *list_para)
{
    __gui_framewincreate_para_t framewin_para;
    dtv_setting_disp_para_t  *para;
    FB fb;
    __msg("*********enter  general dtv_setting_disp win proc *****************\n");
    GUI_LyrWinGetFB(list_para->layer, &fb);
    para = (dtv_setting_disp_para_t *)My_Malloc(0, sizeof(dtv_setting_disp_para_t));

    if (!para)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset((void *)para, 0, sizeof(dtv_setting_disp_para_t));
    para->font = SWFFont;
    para->layer = list_para->layer;
    para->focus_id = list_para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    SUBMENU_DVB_SETTING_DISP;
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = SUBMENU_DVB_SETTING_DISP_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dtv_setting_disp_Proc);
    __wrn("framewin_para.name:%s, framewin_para.FrameWinProc:0x%x\n", framewin_para.name, framewin_para.FrameWinProc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)para;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}


__s32 dtv_setting_disp_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}

