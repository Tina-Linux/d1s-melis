/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : dtv_setting_list_general.c
* By        : hot.lee
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  hot.lee  create this file, implements the fundemental interface;
**************************************************************************************************************
*/

#include <log.h>
#include "dtv_setting_uipara.h"
#include "dtv_setting_list.h"

#define     MAX_LISTBAR_ON_PAGE     6


static __u32 setting_string_tab[] =
{
    STRING_DTV_ALL_FREQ_SEARCH,
#ifdef DTV_AUTO_SEARCH_ENABLE
    STRING_DTV_AUTO_SEARCH,
#endif
    STRING_DTV_MANU_SEARCH,
    STRING_DTV_DISPLAY_SETTING,
    STRING_DTV_CA_INFO,
    STRING_DTV_EPG,
    STRING_DTV_WIRELESS,
    STRING_DTV_TIME_ZONE
};

typedef struct tag_dtv_setting_list_focus_item_id
{
    __s32  id_bmp_focus_item;
    __s32  id_bmp_unfocus_item;
} dtv_setting_list_focus_item_id_t;

typedef struct tag_dtv_setting_list_attr
{

    HTHEME      h_bg_bmp;
    HTHEME      h_list_top_bmp;
    HTHEME      h_item_focus_bmp, h_item_unfocus_bmp;

    __s32   focus_txt_color;
    __s32   unfocus_txt_color;

    H_LBAR   h_listbar;
    H_WIN   layer;
    GUI_FONT *font;

    __s32 new_focus;
    __s32 old_focus;

    __s32 item_nr;
    dtv_setting_list_para_t *para;

    __s32 destroy_timerid;
    __s32 total;
    //set_para* uipara;
} dtv_setting_list_attr_t;

extern void  main_cmd2parent(H_WIN hwin, __s32 id, __u32 data1, __u32 data2);


static __s32 dtv_setting_list_item_paint_ex(__lbar_draw_para_t *draw_param);
static void dtv_setting_res_init(dtv_setting_list_attr_t *attr);
static void _dtv_setting_list_res_uninit(dtv_setting_list_attr_t *attr) ;
static void dtv_setting_title_update(dtv_setting_list_attr_t *gattr, __u32 string_id);


extern dtv_ctr_t  dtv_ctr;


static void dtv_setting_title_update(dtv_setting_list_attr_t *gattr, __u32 string_id)
{
    char tmp[128] = {0};
    GUI_RECT gui_rect = {0};
    SIZE size = {0};
    GUI_LyrWinSel(gattr->layer);
    GUI_SetFont(gattr->font);
    dsk_display_get_size((__s32 *)&size.width, (__s32 *)&size.height);
    GUI_BMP_Draw(theme_hdl2buf(gattr->h_list_top_bmp), 0, 0);
    gui_rect.x0 = 0;//(size.width - LISTBAR_WIDTH)/2;
    gui_rect.y0 = 0;
    gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH;
    gui_rect.y1 = SETTING_TOP_HIGH;
    dsk_langres_get_menu_text(string_id, tmp, sizeof(tmp));
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringInRect(tmp, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

static __s32 dtv_setting_list_item_paint_ex(__lbar_draw_para_t *draw_param)
{
    __s32 i;
    RECT rect;
    GUI_RECT gui_rect;
    dtv_setting_list_attr_t *gattr;
    //GUI_MEMDEV_Handle draw_mem;
    maple_schdl_event_t *shdl_event = NULL;
    char time[128];
    char tmp[128];
    char shdl_name[128];
    gattr = (dtv_setting_list_attr_t *)draw_param->attr;

    if (gattr == NULL)
    {
        return EPDK_FAIL;
    }

    //GUI_LyrWinSel(gattr->layer);
    GUI_SetFont(gattr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //GUI_LyrWinCacheOn();
    //draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y,draw_param->rect.width,draw_param->rect.height);
    //GUI_MEMDEV_Select(draw_mem);

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
        {
            __msg("LBAR_MODE_NORMAL \n");
            GUI_BMP_Draw(theme_hdl2buf(gattr->h_item_unfocus_bmp), draw_param->rect.x, draw_param->rect.y);
            GUI_SetColor(gattr->unfocus_txt_color);
            dsk_langres_get_menu_text(setting_string_tab[draw_param->index], shdl_name, sizeof(shdl_name));
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH;
            gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
            GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        }
        break;

        case LBAR_MODE_FOCUS:
        {
            __msg("LBAR_MODE_FOCUS \n");
            GUI_BMP_Draw(theme_hdl2buf(gattr->h_item_focus_bmp), draw_param->rect.x, draw_param->rect.y);
            GUI_SetColor(gattr->focus_txt_color);
            gui_rect.x0 = draw_param->rect.x;
            gui_rect.y0 = draw_param->rect.y;
            gui_rect.x1 = gui_rect.x0 + LISTBAR_WIDTH;
            gui_rect.y1 = gui_rect.y0 + LISTBAR_HIGH;
            dsk_langres_get_menu_text(setting_string_tab[draw_param->index], shdl_name, sizeof(shdl_name));
            GUI_DispStringInRect(shdl_name, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            __wrn("....\n");
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
    //GUI_LyrWinCacheOff();
    return EPDK_OK;
}



static void dtv_setting_list_listbar_init(__gui_msg_t *msg)
{
    __listbar_config_t config;
    RECT rect;
    dtv_setting_list_attr_t *attr;
    __s32 index;
    __s32 start_id, focus_id;
    attr = (dtv_setting_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    dsk_display_get_size((__s32 *)&rect.width, (__s32 *)&rect.height);
    start_id = 0;
    focus_id = attr->new_focus;
    __msg("attr->total = %d\n", attr->total);
    config.alpha_status = EPDK_FALSE;
    config.bk_color = 0;
    config.start_id = start_id;
    config.focus_id = focus_id;
    config.item_cnt = attr->total;
    config.item_height = LISTBAR_HIGH;
    config.item_width = LISTBAR_WIDTH;
    config.list_rect.x = 0;//(rect.width - LISTBAR_WIDTH)/2;
    config.list_rect.y = SETTING_TOP_HIGH;
    config.list_rect.width = LISTBAR_WIDTH;
    config.list_rect.height = attr->total * LISTBAR_HIGH;
    config.list_attr = (void *)attr;
    attr->h_listbar = LISTBAR_Create(msg->h_deswin);
    LISTBAR_Config(attr->h_listbar, dtv_setting_list_item_paint_ex, &config);
    LISTBAR_ShowPage(attr->h_listbar);
}




/*
    申请资源
*/
static void dtv_setting_res_init(dtv_setting_list_attr_t *attr)
{
    __u32   i;
    __bool  restore_flag;
    attr->h_item_focus_bmp = dsk_theme_open(ID_DTV_PRELIST_F_BMP);
    attr->h_item_unfocus_bmp = dsk_theme_open(ID_DTV_PRELIST_UF_BMP);
    attr->h_list_top_bmp = dsk_theme_open(ID_DTV_SETTING_TOP_BMP);
}

static void _dtv_setting_list_res_uninit(dtv_setting_list_attr_t *attr)
{
    __u32   i;
    theme_close(attr->h_list_top_bmp);
    attr->h_list_top_bmp = 0;
    theme_close(attr->h_item_focus_bmp);
    attr->h_item_focus_bmp = 0;
    theme_close(attr->h_item_unfocus_bmp);
    attr->h_item_unfocus_bmp = 0;
}

void dtv_setting_list_enter_action(__gui_msg_t *msg)
{
    dtv_setting_list_attr_t *attr;
    attr = (dtv_setting_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
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

static __s32 _dtv_setting_list_listbar_key_proc(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    static int active_key = -1;
    dtv_setting_list_attr_t *attr;
    attr = (dtv_setting_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
    __wrn("_dvt_setting_list_listbar_key_proc() : msg->dwAddData1 = %d, msg->dwAddData2 = %d\n", msg->dwAddData1, msg->dwAddData2);

    // 过滤不完整的按键
    if (msg->dwAddData2 == KEY_DOWN_ACTION)
    {
        active_key = msg->dwAddData1;
    }
    else if (msg->dwAddData2 == KEY_UP_ACTION)
    {
        if (msg->dwAddData1 != active_key)
        {
            return EPDK_FAIL;
        }
    }
    else if (msg->dwAddData2 == KEY_REPEAT_ACTION)
    {
        if (msg->dwAddData1 != active_key + 0x50)
        {
            return EPDK_FAIL;
        }
    }

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_UP:
        case GUI_MSG_KEY_LONGUP:
        case GUI_MSG_KEY_LEFT:
        case GUI_MSG_KEY_PREV:
        case GUI_MSG_KEY_LONGPREV:
        case GUI_MSG_KEY_LONGLEFT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                attr->old_focus = attr->new_focus;
                LISTBAR_PrevItem(attr->h_listbar);
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
            }

            break;
        }

        case GUI_MSG_KEY_DOWN:
        case GUI_MSG_KEY_LONGDOWN:
        case GUI_MSG_KEY_NEXT:
        case GUI_MSG_KEY_LONGNEXT:
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_LONGRIGHT:
        {
            if ((msg->dwAddData2 == KEY_DOWN_ACTION) || (msg->dwAddData2 == KEY_REPEAT_ACTION))
            {
                attr->old_focus = attr->new_focus;
                LISTBAR_NextItem(attr->h_listbar);
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
            }

            break;
        }

        case GUI_MSG_KEY_ENTER:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                attr->new_focus = LISTBAR_GetFocusItem(attr->h_listbar);
                __msg("attr->new_focus = %d \n", attr->new_focus);

                if (attr->new_focus >= 0 && attr->new_focus < DTV_SETTING_MAX)
                {
                    dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, attr->new_focus, 0);
                    break;
                }
            }

            break;
        }

        case GUI_MSG_KEY_ESCAPE:
        case GUI_MSG_KEY_LONGMENU:
        case GUI_MSG_KEY_MENU:
        {
            if (msg->dwAddData2 == KEY_UP_ACTION)
            {
                __wrn("dtv_ctr.h_setting    : 0x%08x\n", dtv_ctr.h_setting);
                dtv_setting_send_cmd_to_parent(msg, GUI_MSG_COMMAND, DTV_SETTING_REPLAY, 0);
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return EPDK_OK;
}

static __s32 _dtv_setting_list_listbar_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 ret = EPDK_OK;
    static __s32 is_touch_down = 0;
    static __s32 is_touch_moving = 0;
    dtv_setting_list_attr_t *attr;
    attr = (dtv_setting_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);
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

static __s32  dtv_setting_list_touch_proc(__gui_msg_t *msg)
{
    return EPDK_OK;
}

static __s32 dtv_setting_get_cur_index(void)
{
    return EPDK_OK;
}


/*
    回调
*/
static __s32 _dtv_setting_list_Proc(__gui_msg_t *msg)
{
    __msg("_dvt_setting_list_Proc()::msg->id=%08x, dwAddData1=%08x, dwAddData2=%08x\n", msg->id, msg->dwAddData1, msg->dwAddData2);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            dtv_setting_list_attr_t *attr = NULL;
            dtv_setting_list_para_t *para = NULL;
            HTHEME  bg_bmp;
            para = (dtv_setting_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
            attr = (dtv_setting_list_attr_t *)My_Malloc(0, sizeof(dtv_setting_list_attr_t));

            if (!attr)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(attr, 0, sizeof(dtv_setting_list_attr_t));
            attr->new_focus = para->focus_id;
            attr->old_focus = para->focus_id;
            attr->para = para;
            dtv_setting_res_init(attr);
            attr->focus_txt_color = dtv_SETTING_FOCUS_COLOR;
            attr->unfocus_txt_color = dtv_SETTING_UNFOCUS_COLOR;
            attr->font = para->font;
            attr->layer = para->layer;
            attr->total = DTV_SETTING_MAX;
            __msg("attr->total = %d\n", attr->total);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)attr);
            GUI_LyrWinSetSta(attr->layer, GUI_LYRWIN_STA_SLEEP);
            //GUI_LyrWinSetTop(attr->layer);
            dtv_setting_list_listbar_init(msg);
            dtv_setting_title_update(attr, STRING_DTV_SETTING);
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
            dtv_setting_list_attr_t *attr;
            dtv_setting_list_para_t *para;
            __s32 ret;
            attr = (dtv_setting_list_attr_t *)GUI_WinGetAddData(msg->h_deswin);

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
            _dtv_setting_list_res_uninit(attr);
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
            return _dtv_setting_list_listbar_key_proc(msg);

        case GUI_MSG_TOUCH:
            return dtv_setting_list_touch_proc(msg);////

        case GUI_MSG_COMMAND:
        {
            /*switch(LOWORD(msg->dwAddData1))
            {
                case DVBEPG_ID_CREATE_EPG_LIST:
                {
                    dtv_setting_list_listbar_init(msg);
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

H_WIN dtv_setting_list_win_create(H_WIN h_parent, dtv_setting_list_para_t *list_para)
{
    __gui_framewincreate_para_t framewin_para;
    dtv_setting_list_para_t  *para;
    FB fb;
    GUI_LyrWinGetFB(list_para->layer, &fb);
    para = (dtv_setting_list_para_t *)My_Malloc(0, sizeof(dtv_setting_list_para_t));

    if (!para)
    {
        __wrn("memory alloc fail.........\n");
        return NULL;
    }

    eLIBs_memset((void *)para, 0, sizeof(dtv_setting_list_para_t));
    para->font = SWFFont;
    para->layer = list_para->layer;
    para->focus_id = list_para->focus_id;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    SUBMENU_DVB_SETTING_LST;
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = SUBMENU_DVB_SETTING_LIST_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_dtv_setting_list_Proc);
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


__s32 dtv_setting_list_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}

