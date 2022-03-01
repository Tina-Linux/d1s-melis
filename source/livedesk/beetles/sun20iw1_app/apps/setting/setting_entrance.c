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
#include "setting_entrance.h"
#include "setting_uipara.h"

typedef struct tag_setting_item_id_s
{
    __s32  id_string_title;
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} setting_item_id_t;

typedef struct tag_setting_item_res
{
    char string_title[128];
    HTHEME  h_bmp_focus;
    HTHEME  h_bmp_unfocus;
} setting_item_res_t;

typedef struct tag_setting_entrance_attr
{
    HTHEME  h_bmp_big_setting, h_bg_bmp, h_return_fcs_bmp, h_return_unfcs_bmp;
    setting_item_res_t res[4];

    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_WIN   layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;
    __s32 item_nr;

    __s32 focus_status; //
} setting_entrance_attr_t;


extern void  main_cmd2parent(H_WIN hwin, __s32 id, unsigned long data1, unsigned long data2);

static setting_item_id_t setting_entrance_res_id[] =
{
    {STRING_SET_PROMPT_COMMON, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP},
    {STRING_SET_PROMPT_POWER_MANAGER, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP},
    {STRING_SET_ALARM_SET, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP},
    {STRING_SET_GAME_HANDLE_ADJUST, ID_SETTING_SET_ITEM_F_BMP, ID_SETTING_SET_ITEM_UFCS_BMP}
};

/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR setting_entrance_layer_32bpp_create(void)
{
    setting_uipara_t   *uipara = NULL;
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        //   {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        //    DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
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
        "setting entrance layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    uipara = (setting_uipara_t *)setting_get_uipara();
    fb.size.width       = uipara->lyr_win.w;
    fb.size.height      = uipara->lyr_win.h;
    lstlyr.src_win.x        = uipara->lyr_win.x;
    lstlyr.src_win.y        = uipara->lyr_win.y;
    lstlyr.src_win.width        = uipara->lyr_win.w;
    lstlyr.src_win.height   = uipara->lyr_win.h;
    lstlyr.scn_win.x        = uipara->lyr_scn.x;
    lstlyr.scn_win.y        = uipara->lyr_scn.y;
    lstlyr.scn_win.width    = uipara->lyr_scn.w;
    lstlyr.scn_win.height   = uipara->lyr_scn.h;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !\n");
    }

    return layer;
}

/*
    申请资源
*/
static void _setting_entrance_res_init(setting_entrance_attr_t *entrance_attr)
{
#if 1
    __u32   i;
    //entrance_attr->h_bmp_big_setting = theme_open(ID_SETTING_SET_BSYSSE_BMP);
    ///entrance_attr->h_bg_bmp = theme_open(ID_SETTING_WINDOW_BG_BMP);
    //entrance_attr->h_return_fcs_bmp = theme_open(ID_SETTING_RETURN_FCS_BMP);
    //entrance_attr->h_return_unfcs_bmp = theme_open(ID_SETTING_RETURN_UFCS_BMP);
#if ADD_ALARM_SETTING
    //entrance_attr->item_nr = sizeof(setting_entrance_res_id) /sizeof(setting_entrance_res_id[0]);
#else
    //entrance_attr->item_nr = 2;
#endif

    if (!(rat_is_encryption_partition_Insert(NULL, 1))
        //||(EPDK_FAIL== CFG_get_game_and_dict_flag(ID_HOME_GAME))
       )//按理应该同时处理alarm和handle是否存在的情况，现在默认alarm存在
    {
        //#if ADD_ALARM_SETTING
        entrance_attr->item_nr -= 1;
        //#else
        //          entrance_attr->item_nr = 2;
        //#endif
    }

    for (i = 0; i < entrance_attr->item_nr; i++)
    {
        //get_menu_text(setting_entrance_res_id[i].id_string_title,entrance_attr->res[i].string_title,sizeof(entrance_attr->res[i].string_title));
        //entrance_attr->res[i].h_bmp_focus= theme_open(setting_entrance_res_id[i].id_bmp_focus_item);
        //entrance_attr->res[i].h_bmp_unfocus= theme_open(setting_entrance_res_id[i].id_bmp_unfocus_item);
    }

#endif
}

static void _setting_entrance_res_uninit(setting_entrance_attr_t *entrance_attr)
{
    __u32   i;

    if (entrance_attr->h_bmp_big_setting != NULL)
    {
        theme_close(entrance_attr->h_bmp_big_setting);
        entrance_attr->h_bmp_big_setting = NULL;
    }

    if (entrance_attr->h_bg_bmp != NULL)
    {
        theme_close(entrance_attr->h_bg_bmp);
        entrance_attr->h_bg_bmp = NULL;
    }

    if (entrance_attr->h_return_fcs_bmp != NULL)
    {
        theme_close(entrance_attr->h_return_fcs_bmp);
        entrance_attr->h_return_fcs_bmp = NULL;
    }

    if (entrance_attr->h_return_unfcs_bmp != NULL)
    {
        theme_close(entrance_attr->h_return_unfcs_bmp);
        entrance_attr->h_return_unfcs_bmp = NULL;
    }

    for (i = 0; i < entrance_attr->item_nr; i++)
    {
        if (entrance_attr->res[i].h_bmp_focus)
        {
            theme_close(entrance_attr->res[i].h_bmp_focus);
            entrance_attr->res[i].h_bmp_focus = NULL;
        }

        if (entrance_attr->res[i].h_bmp_unfocus != NULL)
        {
            theme_close(entrance_attr->res[i].h_bmp_unfocus);
            entrance_attr->res[i].h_bmp_unfocus = NULL;
        }
    }
}
static __s32 setting_entrance_item_paint(__gui_msg_t *msg)
{
    __s32 i;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara = NULL;
    setting_entrance_attr_t *entrance_attr;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (entrance_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(entrance_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(entrance_attr->layer);
    GUI_SetFont(entrance_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(entrance_attr->focus_txt_color);
    GUI_LyrWinCacheOn();
    uipara = (setting_uipara_t *)setting_get_uipara();

    for (i = 0; i < entrance_attr->item_nr; i++)
    {
        if (i == entrance_attr->new_focus)
        {
            GUI_DispStringAt(entrance_attr->res[entrance_attr->new_focus].string_title, uipara->txt_pos.x, uipara->txt_pos.y);
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_focus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_focus), uipara->item.x, uipara->item.y + uipara->item.h * i + 45);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(entrance_attr->focus_txt_color);
            GUI_DispStringInRect(entrance_attr->res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
        else if (i == entrance_attr->old_focus)
        {
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(entrance_attr->unfocus_txt_color);
            GUI_DispStringInRect(entrance_attr->res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
    }

    GUI_LyrWinCacheOff();
    return EPDK_OK;
}

static __s32 setting_entrance_paint(__gui_msg_t *msg)
{
#if 1
    __s32 i;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara = NULL;
    setting_entrance_attr_t *entrance_attr;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (entrance_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(entrance_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(entrance_attr->layer);
    GUI_SetFont(entrance_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    GUI_SetColor(entrance_attr->unfocus_txt_color);
    //GUI_LyrWinCacheOn();
    uipara = (setting_uipara_t *)setting_get_uipara();
    GUI_BMP_Draw(theme_hdl2buf(entrance_attr->h_bg_bmp), 0, 0);
    GUI_BMP_Draw(theme_hdl2buf(entrance_attr->h_bmp_big_setting), 1, 0);
    GUI_BMP_Draw(theme_hdl2buf(entrance_attr->h_return_unfcs_bmp), uipara->return_bmp.x, uipara->return_bmp.y);

    for (i = 0; i < entrance_attr->item_nr; i++)
    {
        if (i == entrance_attr->new_focus)
        {
            GUI_DispStringAt(entrance_attr->res[entrance_attr->new_focus].string_title, uipara->txt_pos.x, uipara->txt_pos.y);
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_focus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_focus), uipara->item.x, uipara->item.y + uipara->item.h * i + 45);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(entrance_attr->focus_txt_color);
            GUI_DispStringInRect(entrance_attr->res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
        else
        {
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(entrance_attr->unfocus_txt_color);
            GUI_DispStringInRect(entrance_attr->res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
    }

    //GUI_LyrWinCacheOff();
    return EPDK_OK;
#endif
}

static __s32 setting_entrance_change_paint(__gui_msg_t *msg)
{
#if 1
    __s32 i;
    GUI_RECT gui_rect;
    setting_uipara_t *uipara = NULL;
    setting_entrance_attr_t *entrance_attr;
    GUI_MEMDEV_Handle   draw_mem;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (entrance_attr == NULL)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(entrance_attr->layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(entrance_attr->layer);
    GUI_SetFont(entrance_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    uipara = (setting_uipara_t *)setting_get_uipara();

    for (i = 0; i < entrance_attr->item_nr; i++)
    {
        draw_mem = GUI_MEMDEV_Create(uipara->item.x, uipara->item.y + uipara->item.h * i, uipara->item.w, uipara->item.h);
        GUI_MEMDEV_Select(draw_mem);

        if (i == entrance_attr->new_focus)
        {
            GUI_DispStringAt(entrance_attr->res[entrance_attr->new_focus].string_title, uipara->txt_pos.x, uipara->txt_pos.y);
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_focus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_focus), uipara->item.x, uipara->item.y + uipara->item.h * i + 45);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(entrance_attr->focus_txt_color);
            GUI_DispStringInRect(entrance_attr->res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }
        else
        {
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->res[i].h_bmp_unfocus), uipara->item.x, uipara->item.y + uipara->item.h * i);
            gui_rect.x0 = uipara->item_title_pos.x + uipara->item.x;
            gui_rect.y0 = uipara->item_title_pos.y + uipara->item_title_pos.h * i;
            gui_rect.x1 = gui_rect.x0 + uipara->item_title_pos.w;
            gui_rect.y1 = gui_rect.y0 + uipara->item_title_pos.h;
            GUI_SetColor(entrance_attr->unfocus_txt_color);
            GUI_DispStringInRect(entrance_attr->res[i].string_title, &gui_rect, GUI_TA_LEFT | GUI_TA_VCENTER);
        }

        GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
        GUI_MEMDEV_CopyToLCD(draw_mem);
        GUI_MEMDEV_Select(NULL);
        GUI_MEMDEV_Delete(draw_mem);
        draw_mem = NULL;
    }

    return EPDK_OK;
#endif
}

static __s32  setting_entrance_key_proc(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    setting_entrance_attr_t *entrance_attr;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_ENTER == last_key)
        {
            if (SETTING_ENTER_GENERAL == entrance_attr->new_focus)
            {
                main_cmd2parent(msg->h_deswin, SETTING_ENTER_GENERAL, 0, 0);            //发送消息给父窗口，进入通用设置
                GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
            }
            else if (SETTING_ENTER_POWER_MANAGEMENT == entrance_attr->new_focus)
            {
                __wrn("@@@@@@@ SETTING_ENTER_POWER_MANAGEMENT @@@@@@\n");
                main_cmd2parent(msg->h_deswin, SETTING_ENTER_POWER_MANAGEMENT, 0, 0);   //发送消息给父窗口，进入电源管理
                GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
            }
            else if (SETTING_ENTER_ALARM == entrance_attr->new_focus)
            {
                main_cmd2parent(msg->h_deswin, SETTING_ENTER_ALARM, 0, 0);              //发送消息给父窗口，进入闹钟
                GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
            }
            else
            {
            }
        }
    }
    else
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            {
                entrance_attr->old_focus = entrance_attr->new_focus;

                if (entrance_attr->new_focus <= 0)
                {
                    entrance_attr->new_focus = entrance_attr->item_nr - 1;
                }
                else
                {
                    entrance_attr->new_focus--;
                }

                //setting_entrance_item_paint( msg);
                setting_entrance_change_paint(msg);
            }
            break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            {
                entrance_attr->old_focus = entrance_attr->new_focus;

                if (entrance_attr->new_focus >= entrance_attr->item_nr - 1)
                {
                    entrance_attr->new_focus = 0;
                }
                else
                {
                    entrance_attr->new_focus++;
                }

                //setting_entrance_item_paint( msg);
                setting_entrance_change_paint(msg);
            }
            break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            {
                entrance_attr->old_focus = entrance_attr->new_focus;

                if (entrance_attr->new_focus <= 0)
                {
                    entrance_attr->new_focus = entrance_attr->item_nr - 1;
                }
                else
                {
                    entrance_attr->new_focus--;
                }

                //setting_entrance_item_paint( msg);
                setting_entrance_change_paint(msg);
            }
            break;

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            {
                entrance_attr->old_focus = entrance_attr->new_focus;

                if (entrance_attr->new_focus >= entrance_attr->item_nr - 1)
                {
                    entrance_attr->new_focus = 0;
                }
                else
                {
                    entrance_attr->new_focus++;
                }

                //setting_entrance_item_paint( msg);
                setting_entrance_change_paint(msg);
            }
            break;

            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_LONGMENU:
            {
                __gui_msg_t mymsg;
                mymsg.id = GUI_MSG_CLOSE;
                mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                GUI_SendMessage(&mymsg);
                break;
            }

            default:
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

void setting_entrance_enter_action(__gui_msg_t *msg)
{
    setting_item_res_t  *p_item_res;
    setting_entrance_attr_t *entrance_attr;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (SETTING_ENTER_GENERAL == entrance_attr->new_focus)
    {
        main_cmd2parent(msg->h_deswin, SETTING_ENTER_GENERAL, 0, 0); //发送消息给父窗口，进入通用设置
        GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
    }
    else if (SETTING_ENTER_POWER_MANAGEMENT == entrance_attr->new_focus)
    {
        __wrn("@@@@@@@ SETTING_ENTER_POWER_MANAGEMENT @@@@@@\n");
        main_cmd2parent(msg->h_deswin, SETTING_ENTER_POWER_MANAGEMENT, 0, 0);   //发送消息给父窗口，进入电源管理
        GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
    }
    else if (SETTING_ENTER_ALARM == entrance_attr->new_focus)
    {
        main_cmd2parent(msg->h_deswin, SETTING_ENTER_ALARM, 0, 0);              //发送消息给父窗口，进入闹钟
        GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
    }
    else if (SETTING_ENTER_HANDLE_ADJUST == entrance_attr->new_focus)
    {
        main_cmd2parent(msg->h_deswin, SETTING_ENTER_HANDLE_ADJUST, 0, 0);              //发送消息给父窗口，进入闹钟
        GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYR_STA_SUSPEND);
    }
}

/*由触摸的位置获得当前item的下标*/
static __s32 entrance_TouchPos_2_ItemIndex(__gui_msg_t *msg)
{
    __s32 pos_x, pos_y;
    __s32 item_index, tmp_index_y;
    setting_entrance_attr_t *entrance_attr = NULL;
    setting_uipara_t *uipara = NULL;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!entrance_attr)
    {
        __msg("entrance_attr is null \n");
    }

    uipara = (setting_uipara_t *)setting_get_uipara();
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);

    if (((pos_x >= SET_ITEM_START_X) && (pos_x <= SET_ITEM_START_X + SET_ITEM_W))
        && ((pos_y >= SET_ITEM_START_Y) && (pos_y <= SET_ITEM_START_Y + SET_ITEM_H * entrance_attr->item_nr)))
    {
        tmp_index_y = (pos_y - SET_ITEM_START_Y)  / SET_ITEM_H;
        item_index = tmp_index_y;
        return item_index;
    }
    else
    {
        //__here__;
        return EPDK_FAIL;   //不在item区域
    }
}

static __s32  setting_entrance_touch_proc(__gui_msg_t *msg)
{
    setting_entrance_attr_t *entrance_attr;
    setting_uipara_t *uipara = NULL;
    __s32 pos_x, pos_y;
    entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);
    uipara = (setting_uipara_t *)setting_get_uipara();
    __msg("touch action msg->dwAddData1=%d\n", msg->dwAddData1);
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);
    __msg("pos.x=%d  pos.y=%d\n", LOSWORD(msg->dwAddData2), HISWORD(msg->dwAddData2));

    if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
    {
        __s32 ret;

        //触摸区域在返回按钮范围内
        if ((pos_x > uipara->return_bmp.x) && (pos_x < uipara->return_bmp.x + uipara->return_bmp.w)
            && (pos_y > uipara->return_bmp.y) && (pos_y < uipara->return_bmp.y + uipara->return_bmp.h))
        {
            //GUI_BMP_Draw(theme_hdl2buf(entrance_attr->h_return_fcs_bmp),uipara->return_bmp.x,uipara->return_bmp.y);
            {
                __gui_msg_t mymsg;
                mymsg.id = GUI_MSG_CLOSE;
                mymsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                GUI_SendMessage(&mymsg);
            }
        }
        else
        {
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->h_return_unfcs_bmp), uipara->return_bmp.x, uipara->return_bmp.y);
            ret = entrance_TouchPos_2_ItemIndex(msg);   //获取触摸的位置区域

            if (EPDK_FAIL == ret)
            {
                __msg("touch position invalid\n");
            }
            else
            {
                if ((ret >= 0) && (ret < entrance_attr->item_nr))
                {
                    entrance_attr->old_focus = entrance_attr->new_focus;
                    entrance_attr->new_focus = ret;
                    setting_entrance_enter_action(msg);
                }
            }
        }
    }
    else if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
    {
        if ((pos_x > uipara->return_bmp.x) && (pos_x < uipara->return_bmp.x + uipara->return_bmp.w)
            && (pos_y > uipara->return_bmp.y) && (pos_y < uipara->return_bmp.y + uipara->return_bmp.h))
        {
            GUI_BMP_Draw(theme_hdl2buf(entrance_attr->h_return_fcs_bmp), uipara->return_bmp.x, uipara->return_bmp.y);
        }
    }

    return EPDK_OK;
}

/*
    回调
*/
static __s32 _Setting_Entrance_Proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            setting_entrance_attr_t *entrance_attr;
            entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);
            _setting_entrance_res_init(entrance_attr);//获得图片资源
            entrance_attr->focus_txt_color = SETTING_FOCUS_COLOR;
            entrance_attr->unfocus_txt_color = SETTING_UNFOCUS_COLOR;
            //GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYRWIN_STA_ON);
            //__msg("setting entrance layer on\n");
            //GUI_LyrWinSetTop(entrance_attr->layer);
            //__msg("setting entrance layer set top\n");
            //setting_entrance_paint(msg);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            setting_entrance_attr_t *entrance_attr;
            entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (!entrance_attr)
            {
                __err("entrance_attr is null ......\n");
                return EPDK_FAIL;
            }

            _setting_entrance_res_uninit(entrance_attr);

            if (entrance_attr)
            {
                My_Mfree(0, entrance_attr);
            }
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
            setting_entrance_paint(msg);
            return EPDK_OK;

        case GUI_MSG_KEY:
            setting_entrance_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TOUCH:
            setting_entrance_touch_proc(msg);
            return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            if (SETTING_ENTRANCE_CREATE_ALARM == msg->dwAddData1)
            {
                setting_entrance_attr_t *entrance_attr;
                entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

                if (!entrance_attr)
                {
                    __err("entrance_attr is null ......\n");
                    return EPDK_FAIL;
                }

                entrance_attr->old_focus = SETTING_ENTER_ALARM;
                entrance_attr->new_focus = SETTING_ENTER_ALARM;
                setting_entrance_enter_action(msg);
            }
            else if (SETTING_ENTRANCE_PAINT == msg->dwAddData1)
            {
                setting_entrance_attr_t *entrance_attr;
                entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);
                GUI_LyrWinSetSta(entrance_attr->layer, GUI_LYRWIN_STA_ON);
                GUI_LyrWinSetTop(entrance_attr->layer);
            }

            return EPDK_OK;
        }

        case GUI_MSG_WIN_WAKEUP:
        {
            setting_entrance_attr_t *entrance_attr;
            entrance_attr = (setting_entrance_attr_t *)GUI_WinGetAttr(msg->h_deswin);

            if (entrance_attr == NULL)
            {
                break;
            }

            _setting_entrance_res_uninit(entrance_attr);
            _setting_entrance_res_init(entrance_attr);
            setting_entrance_paint(msg);
        }
        break;

        //case DSK_MSG_FS_PART_PLUGOUT:
        //{
        //  main_cmd2parent(msg->h_deswin,ID_SWITCH_TO_ENTRANCE,0,0);

        //}
        //  break;
        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

H_WIN setting_entrance_frm_win_create(H_WIN h_parent, setting_entrance_para_t  *para)
{
    __gui_framewincreate_para_t framewin_para;
    setting_entrance_attr_t *entrance_attr;
    FB fb;
    GUI_LyrWinGetFB(para->layer, &fb);
    __msg("enter setting entrance win\n");
    entrance_attr = (setting_entrance_attr_t *)My_Malloc(0, sizeof(setting_entrance_attr_t));

    if (!entrance_attr)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    eLIBs_memset((void *)entrance_attr, 0, sizeof(setting_entrance_attr_t));
    entrance_attr->font = para->font;
    entrance_attr->layer = para->layer;
    entrance_attr->new_focus = para->focus_id;
    entrance_attr->old_focus = para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    " setting entrance win",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = ENTRANCE_MENU_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_Setting_Entrance_Proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)entrance_attr;
    framewin_para.hLayer = para->layer;
    return (GUI_FrmWinCreate(&framewin_para));
}

H_WIN setting_entrance_win_create(H_WIN h_parent)
{
    setting_entrance_para_t setting_entrance_para;
    setting_entrance_para.font = SWFFont;
    setting_entrance_para.layer = setting_entrance_layer_32bpp_create();
    setting_entrance_para.focus_id = SETTING_ENTER_GENERAL;
    setting_entrance_para.old_focus = SETTING_ENTER_GENERAL;
    return (setting_entrance_frm_win_create(h_parent, &setting_entrance_para));
}


__s32 setting_entrance_win_delete(H_WIN h_entrance_win)
{
    if (h_entrance_win)
    {
        GUI_LyrWinDelete(GUI_WinGetLyrWin(h_entrance_win));
        h_entrance_win = 0;
    }

    return EPDK_OK;
}

