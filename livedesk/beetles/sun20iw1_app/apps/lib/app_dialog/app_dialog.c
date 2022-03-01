/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : app_dialog.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/02 21:10  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "app_dialog_i.h"
//////////////////////////////////////////////////////////////////////////
#if 0

//temp//if(!(0<=(_x) && (_x)<=400 && 0<=(_y) && (_y)<= 240))
//re define GUI_BMP_Draw
#ifdef GUI_BMP_Draw
#define MY_GUI_BMP_DRAW                      (ORANGEMOD_FUNC_ENTRY->ptr_GUI_BMP_Draw            )
#undef GUI_BMP_Draw
#define GUI_BMP_Draw(_p, _x, _y)             do \
    {\
        {__msg("    GUI_BMP_Draw(%d, %d)", _x, _y);} \
        MY_GUI_BMP_DRAW(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringAt
#ifdef GUI_DispStringAt
#define MY_GUI_DispStringAt                  (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringAt        )
#undef GUI_DispStringAt
#define GUI_DispStringAt(_p, _x, _y)         \
    do\
    { \
        {__msg("    GUI_DispStringAt(%d, %d)", _x, _y);} \
        MY_GUI_DispStringAt(_p, _x, _y); \
    }while(0)
#endif

//re define GUI_DispStringInRect
#ifdef GUI_DispStringInRect
#define MY_GUI_DispStringInRect              (ORANGEMOD_FUNC_ENTRY->ptr_GUI_DispStringInRect    )
#undef GUI_DispStringInRect
#define GUI_DispStringInRect(_p, _prt, _fmt)  \
    do \
    { \
        {__msg("    GUI_DispStringInRect(%d, %d, %d, %d)(%x)", (_prt)->x0, (_prt)->y0, (_prt)->x1, (_prt)->y1, _fmt);} \
        MY_GUI_DispStringInRect(_p, _prt, _fmt); \
    } while (0)
#endif

#endif
//////////////////////////////////////////////////////////////////////////
#define APP_MUSIC_ID        0x4001//..
#define MUSIC_MENU_ARTIST_INFO (APP_MUSIC_ID+12)//..

#define APP_FM_ID           0x6001
#define FM_MAINWND_ID            (APP_FM_ID + 1)
#define ID_FM_IS_AUTO_SEARCH_DLG (FM_MAINWND_ID + 1)//..


#define BTN_Y_END_OFFSET          8
#define BTN_X_OFFSET              8
#define TITLE_X_OFFSET            4
#define TITLE_Y_OFFSET            4
//#define TITLE_WIDTH               60
#define TITLE_HEIGHT              20
#define ICON_CONTENT_X_OFFSET     4
#define CONTENT_X_OFFSET          5
#define CONTENT_X_END_OFFSET      5//CONTENT_X_OFFSET
#define CONTENT_Y_OFFSET          8
#define CONTENT_Y_END_OFFSET      5
#define ICON_X_OFFSET             5
#define ICON_Y_OFFSET             35

#define IsDialogTimerValid(_v)    ((_v) > 0)
#define ID_TIMER_DIALOG_TIMEOUT   100

//按键间的间隔
#define calc_btn_sp_w(_rt_w, _offset, _btn_w, _count)   (((_count) > 1) ? (((_rt_w) - (_offset) * 2 - (_btn_w) * (_count)) / (_count - 1)) : 0)

//取窗口私有数据
#define app_dialog_GetWndPara(_p, _t, _hwnd)  {(_p = (_t *)GUI_WinGetAttr(_hwnd)); __msg("app_dialog para ====== %x", _p);}

//去图片资源宽高
#define GET_RES_BMP_W_H_(_hbmp, _res, _w, _h) do \
    {\
        _hbmp = bmp_open(ResHandle2Data(_res)); \
        if (_hbmp != NULL) \
        { \
            _w = bmp_get_width(_hbmp); \
            _h = bmp_get_height(_hbmp); \
            bmp_close(_hbmp); \
        } \
        else \
        { \
            _w = 0; \
            _h = 0; \
            __msg("open bmp fail"); \
        } \
    }while(0)
//////////////////////////////////////////////////////////////////////////
//创建资源
static __s32 app_dialog_res_create(APP_DIALOG_RES_T *res, __u32 style, __s32 bmp_id[], __s32 str_id[], const char *str)
{
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BG], res->bmp_bg);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BTN_F], res->bmp_btn_f);
    create_bmp_res(bmp_id[ADLG_BMP_SZID_BTN_N], res->bmp_btn_n);

    if (bmp_id[ADLG_BMP_SZID_ICON] && bmp_id[ADLG_BMP_SZID_ICON] != BMP_RES_NULL)
    {
        create_bmp_res(bmp_id[ADLG_BMP_SZID_ICON], res->bmp_icon);
        res->icon_enable = 1;
    }
    else
    {
        res->icon_enable = 0;
    }

    if (str_id[ADLG_STR_SZID_TITLE] != 0)
    {
        get_lang_res(str_id[ADLG_STR_SZID_TITLE], res->str_title);
    }
    else
    {
        get_lang_res(STRING_DIALOG_DIALOG, res->str_title);
    }

    if (str_id[ADLG_STR_SZID_CONTENT] != 0)
    {
        get_lang_res(str_id[ADLG_STR_SZID_CONTENT], res->str_content);
    }
    else
    {
        __msg("str = %s", str);
        __msg("res->str_content = %s", res->str_content);

        if (str != NULL)
        {
            eLIBs_strcpy(res->str_content, str);
        }

        __msg("res->str_content = %s", res->str_content);
    }

    switch (ADLG_GET_BTN_STYLE(style))
    {
        case ADLG_OK:
        {
            get_lang_res(STRING_DIALOG_OK, res->str_btn[0]);
            res->btn_result[0] = ADLG_IDOK;
            res->btn_count = 1;
        }
        break;

        case ADLG_OKCANCEL:
        {
            get_lang_res(STRING_DIALOG_OK, res->str_btn[0]);
            get_lang_res(STRING_DIALOG_CANCEL, res->str_btn[1]);
            res->btn_result[0] = ADLG_IDOK;
            res->btn_result[1] = ADLG_IDCANCEL;
            res->btn_count = 2;
        }
        break;

        case ADLG_YESNO:
        {
            get_lang_res(STRING_DIALOG_YES, res->str_btn[0]);
            get_lang_res(STRING_DIALOG_NO, res->str_btn[1]);
            res->btn_result[0] = ADLG_IDYES;
            res->btn_result[1] = ADLG_IDNO;
            res->btn_count = 2;
        }
        break;

        case ADLG_RETRYCANCEL:
        {
            get_lang_res(STRING_DIALOG_RETRY, res->str_btn[0]);
            get_lang_res(STRING_DIALOG_CANCEL, res->str_btn[1]);
            res->btn_result[0] = ADLG_IDRETRY;
            res->btn_result[1] = ADLG_IDCANCEL;
            res->btn_count = 2;
        }
        break;

        case ADLG_ABORTRETRYIGNORE:
        {
            get_lang_res(STRING_DIALOG_ABORT, res->str_btn[0]);
            get_lang_res(STRING_DIALOG_RETRY, res->str_btn[1]);
            get_lang_res(STRING_DIALOG_IGNORE, res->str_btn[2]);
            res->btn_result[0] = ADLG_IDABORT;
            res->btn_result[1] = ADLG_IDRETRY;
            res->btn_result[2] = ADLG_IDIGNORE;
            res->btn_count = 3;
        }
        break;

        case ADLG_YESNOCANCEL:
        {
            get_lang_res(STRING_DIALOG_YES, res->str_btn[0]);
            get_lang_res(STRING_DIALOG_NO, res->str_btn[1]);
            get_lang_res(STRING_DIALOG_CANCEL, res->str_btn[2]);
            res->btn_result[0] = ADLG_IDYES;
            res->btn_result[1] = ADLG_IDNO;
            res->btn_result[2] = ADLG_IDCANCEL;
            res->btn_count = 3;
        }
        break;

        case ADLG_NOTHING:
        {
            res->btn_count = 0;
        }
        break;

        default:
            ASSERT(0);
            break;
    }

    return EPDK_OK;
}

//释放资源
static __s32 app_dialog_res_destroy(APP_DIALOG_RES_T *res)
{
    destroy_bmp_res(res->bmp_bg);
    destroy_bmp_res(res->bmp_btn_n);
    destroy_bmp_res(res->bmp_btn_f);

    if (res->icon_enable)
    {
        destroy_bmp_res(res->bmp_icon);
    }

    return EPDK_OK;
}

static __s32 app_dialog_ui_init(dialog_wnd_t *wnd_para, GUI_RECT *dialog_rt)
{
    HBMP hbmp;
    __s32 lcd_w, lcd_h;
    APP_DIALOG_UI_T  *ui;
    APP_DIALOG_RES_T *res;
    ui = &wnd_para->ui;
    res = &wnd_para->res;
    ui->colour.txt_n = GUI_BLACK;//GUI_WHITE;
    ui->colour.txt_f = GUI_YELLOW;
    dsk_display_get_size(&lcd_w, &lcd_h);

    if (dialog_rt == NULL)
    {
        GET_RES_BMP_W_H_(hbmp, res->bmp_bg, ui->rt.width, ui->rt.height);

        switch (ADLG_GET_ALIGN_H(wnd_para->style))
        {
            case ADLG_TA_LEFT:
                ui->rt.x = 0;
                break;

            case ADLG_TA_RIGHT:
                ui->rt.x = (lcd_w - ui->rt.width)/2;
                break;

            default://ADLG_TA_HCENTER
                ui->rt.x = (lcd_w - ui->rt.width) / 2;
                break;
        }

        switch (ADLG_GET_ALIGN_V(wnd_para->style))
        {
            case ADLG_TA_TOP:
                ui->rt.y = 0;
                break;

            case ADLG_TA_BOTTOM:
                ui->rt.y = (lcd_h - ui->rt.height)/2;
                break;

            default://ADLG_TA_VCENTER
                ui->rt.y = (lcd_h - ui->rt.height) / 2;
                break;
        }
    }
    else
    {
        //user define rect
        ui->rt.x      = dialog_rt->x0;
        ui->rt.width  = dialog_rt->x1 - dialog_rt->x0;
        ui->rt.y      = dialog_rt->y0;
        ui->rt.height = dialog_rt->y1 - dialog_rt->y0;
    }

    ASSERT(res->btn_count >= 0);
    GET_RES_BMP_W_H_(hbmp, res->bmp_btn_n, ui->size.btn.width, ui->size.btn.height);
    ui->size.btn_sp.height = 0;

    if (res->btn_count > 0)
    {
        ui->size.btn_sp.width  = (ui->rt.width / res->btn_count - ui->size.btn.width);
    }
    else
    {
        ui->size.btn_sp.width  = 0;
    }

    //  ui->size.btn_sp.width  = calc_btn_sp_w(ui->rt.width, BTN_X_OFFSET, ui->size.btn.width, res->btn_count);
    ui->pos.btn_start.x = ui->size.btn_sp.width / 2;
    ui->pos.btn_start.y = ui->rt.height - BTN_Y_END_OFFSET - ui->size.btn.height;
    ui->pos.title.x       = TITLE_X_OFFSET;
    ui->pos.title.y       = TITLE_Y_OFFSET;
    ui->size.title.width  = ui->rt.width - ui->pos.title.x;
    ui->size.title.height = TITLE_HEIGHT;

    if (res->icon_enable)
    {
        GET_RES_BMP_W_H_(hbmp, res->bmp_icon, ui->size.icon.width, ui->size.icon.height);
        ui->pos.icon.x = ICON_X_OFFSET;
        ui->pos.icon.y = ICON_Y_OFFSET;
    }

    ui->pos.content.x       = res->icon_enable ? (ui->pos.icon.x + ui->size.icon.width + ICON_CONTENT_X_OFFSET) : CONTENT_X_OFFSET;
    ui->pos.content.y = ui->pos.title.y + ui->size.title.height + CONTENT_Y_OFFSET;
    ui->size.content.width  = ui->rt.width - ui->pos.content.x - CONTENT_X_END_OFFSET;

    if (res->btn_count == 0)
    {
        ui->size.content.height = ui->pos.btn_start.y - CONTENT_Y_END_OFFSET - ui->pos.content.y + ui->size.btn.height;
    }
    else
    {
        ui->size.content.height = ui->pos.btn_start.y - CONTENT_Y_END_OFFSET - ui->pos.content.y;
    }

    return EPDK_OK;
}

static __s32 app_dialog_data_init(dialog_wnd_t *wnd_para, __s32 bmp_id[], __s32 str_id[], GUI_RECT *dialog_rt, const char *str)
{
    app_dialog_res_create(&wnd_para->res, ADLG_GET_WND_STYLE(wnd_para->style), bmp_id, str_id, str);
    app_dialog_ui_init(wnd_para, dialog_rt);
    wnd_para->result = ADLG_IDM_TIMEROUT;
    return EPDK_OK;
}

static __s32 app_dialog_data_release(dialog_wnd_t *wnd_para)
{
    app_dialog_res_destroy(&wnd_para->res);
    BFreeType(wnd_para, dialog_wnd_t);
    return EPDK_OK;
}

static __s32 notify_to_close_dialog(H_WIN hwnd/*, __s32 result*/)
{
    __gui_msg_t msg;
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, hwnd);
    msg.h_deswin    = wnd_para->parent;
    msg.h_srcwin    = wnd_para->hwnd;
    msg.id          = GUI_MSG_COMMAND;
    msg.dwAddData1  = MAKELONG(wnd_para->ID, ADLG_CMD_EXIT);
    msg.dwAddData2  = wnd_para->result;
    msg.dwReserved  = 0;
    msg.p_arg       = NULL;
    __msg("%x => %x (%d, %x, %x)", msg.h_srcwin, msg.h_deswin, msg.id, msg.dwAddData1, msg.dwAddData2);
    return GUI_SendNotifyMessage(&msg);
}

static __s32 on_dialog_key_up_action(H_WIN hwnd, __u32 keycode)
{
    APP_DIALOG_UI_CTRL_PAPA_T *ctrl;
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, hwnd);
    ctrl = &wnd_para->ui.ctrl;

    if (wnd_para->res.btn_count == 0)
    {
        return EPDK_FAIL;
    }

    switch (keycode)
    {
        case VK_PREV:
        case VK_LEFT:
        case GUI_MSG_KEY_UP:
        {
            if (0 < ctrl->focus_id)
            {
                ctrl->focus_id--;
            }
            else
            {
                ctrl->focus_id = wnd_para->res.btn_count - 1;
            }

            if(wnd_para->res.btn_count >= 2)
            {
                GUI_WinUpdate(hwnd, EPDK_TRUE);
            }
        }
        break;

        case VK_NEXT:
        case GUI_MSG_KEY_RIGHT:
        case GUI_MSG_KEY_DOWN:
        {
            if (ctrl->focus_id < wnd_para->res.btn_count - 1)
            {
                ctrl->focus_id++;
            }
            else
            {
                ctrl->focus_id = 0;
            }

            if(wnd_para->res.btn_count >= 2)
            {
                GUI_WinUpdate(hwnd, EPDK_TRUE);
            }
        }
        break;

        case VK_PLAY_PAUSE:
        case VK_ENTER:
        {
            ASSERT(0 <= ctrl->focus_id && ctrl->focus_id < wnd_para->res.btn_count);
            wnd_para->result = wnd_para->res.btn_result[ctrl->focus_id];
            notify_to_close_dialog(hwnd);
        }
        break;

        case VK_ESCAPE:
        case GUI_MSG_KEY_ESCAPE:
        {
            notify_to_close_dialog(hwnd);
        }
        break;
    }

    return EPDK_OK;
}

//
static __s32 dialog_key_proc(__gui_msg_t *msg)
{
    static __u32 lastkey = VK_NULL;

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        on_dialog_key_up_action(msg->h_deswin, lastkey);
        lastkey = VK_NULL;
    }
    else
    {
        lastkey = msg->dwAddData1;
    }

    if (lastkey != VK_ESCAPE && lastkey != GUI_MSG_KEY_ESCAPE)
    {
        //有按键，重新计时
        dialog_wnd_t *wnd_para;
        app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);

        if (IsDialogTimerValid(wnd_para->timeout))
        {
            GUI_ResetTimer(msg->h_deswin, ID_TIMER_DIALOG_TIMEOUT, wnd_para->timeout, NULL);
        }
    }

    return EPDK_OK;
}

//测试是否点击到按键, 返回-1，没点到按键，否则返回点到的按键id
static __s32 test_dialog_touch(H_WIN hwnd, __s32 x, __s32 y, dialog_wnd_t *wnd_para)
{
    __s32 i;
    __s32 btn_x, btn_y, btn_w, btn_h;
    APP_DIALOG_UI_CTRL_PAPA_T *ctrl;
    APP_DIALOG_UI_T           *ui;
    ctrl = &wnd_para->ui.ctrl;
    ui   = &wnd_para->ui;
    btn_x = ui->pos.btn_start.x;
    btn_y = ui->pos.btn_start.y;
    btn_w = ui->size.btn.width;
    btn_h = ui->size.btn.height;

    for (i = 0; i < wnd_para->res.btn_count; i++)
    {
        if (ValueInRange(btn_x, x, btn_x + btn_w))
        {
            if (ValueInRange(btn_y, y, btn_y + btn_h))
            {
                break;
            }
        }

        btn_x += (btn_w + ui->size.btn_sp.width);
    }

    if (i < wnd_para->res.btn_count)
    {
        return i;
    }
    else
    {
        return -1;
    }
}

static __s32 on_dialog_touch_proc(__gui_msg_t *msg)
{
    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
        {
            __s32 id;
            dialog_wnd_t *wnd_para;
            app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);

            if (wnd_para)
            {
                if (wnd_para->res.btn_count == 0)
                {
                    return EPDK_FAIL;
                }

                id = test_dialog_touch(msg->h_deswin, LOSWORD(msg->dwAddData2), HISWORD(msg->dwAddData2), wnd_para);

                if (0 <= id)
                {
                    if (wnd_para->ui.ctrl.focus_id != id)
                    {
                        wnd_para->ui.ctrl.focus_id = id;
                        GUI_WinUpdate(msg->h_deswin, EPDK_TRUE);
                    }
                }
            }
        }
        break;

        case GUI_MSG_TOUCH_UP:
        {
            __s32 id;
            dialog_wnd_t *wnd_para;
            app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);

            if (wnd_para)
            {
                if (wnd_para->res.btn_count == 0)
                {
                    return EPDK_FAIL;
                }

                id = test_dialog_touch(msg->h_deswin, LOSWORD(msg->dwAddData2), HISWORD(msg->dwAddData2), wnd_para);

                if (0 <= id)
                {
                    if (wnd_para->ui.ctrl.focus_id == id)
                    {
                        dsk_keytone_on();
                        wnd_para->result = wnd_para->res.btn_result[wnd_para->ui.ctrl.focus_id];
                        notify_to_close_dialog(msg->h_deswin);
                    }
                }
            }
        }
        break;

        case GUI_MSG_TOUCH_MOVE:
        case GUI_MSG_TOUCH_LONGDOWN:
        case GUI_MSG_TOUCH_OVERLONGDOWN:
            break;
    }

    return EPDK_OK;
}

//
static __s32 on_dialog_command(__gui_msg_t *msg)
{
    __msg("+++on_dialog_command++++");
    __msg("(char *)msg->p_arg = %s", (char *)msg->p_arg);
    return EPDK_OK;
}

static void draw_dialog(APP_DIALOG_RES_T *res, APP_DIALOG_UI_T *ui, __s32   ID)
{
    __s32             i, x, y;
    void             *bmp_data;
    GUI_RECT          rect;
    GUI_SetColor(ui->colour.txt_n);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    //背景
    GUI_BMP_RES_Draw(res->bmp_bg, 0, 0);

    if (res->icon_enable)
    {
        GUI_BMP_RES_Draw(res->bmp_icon, ui->pos.icon.x, ui->pos.icon.y);
    }

    //GUI_OpenAlphaBlend();
    //标题
    GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
    GUI_DispStringAt(res->str_title, ui->pos.title.x, ui->pos.title.y);

    //内容
    /*rect.x0 = ui->pos.content.x;
    rect.y0 = ui->pos.content.y;
    rect.x1 = rect.x0 + ui->size.content.width;
    rect.y1 = rect.y0 + ui->size.content.height+50;
    //GUI_DispStringInRect(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_DispStringInRectWrap(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);*/

    //按钮
    if (ID == MUSIC_MENU_ARTIST_INFO)
    {
        //内容
        rect.x0 = ui->pos.content.x;
        rect.y0 = ui->pos.content.y;
        rect.x1 = rect.x0 + ui->size.content.width;
        rect.y1 = rect.y0 + ui->size.content.height + 50;
        //GUI_DispStringInRect(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_DispStringInRectWrap(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
    }
    else
    {
        //内容
        if (ID_FM_IS_AUTO_SEARCH_DLG == ID)
        {
            rect.x0 = ui->pos.content.x;
            rect.y0 = ui->pos.content.y + 15;
            rect.x1 = rect.x0 + ui->size.content.width;
            rect.y1 = rect.y0 + ui->size.content.height;
        }
        else
        {
            rect.x0 = ui->pos.content.x;
            rect.y0 = ui->pos.content.y;
            rect.x1 = rect.x0 + ui->size.content.width;
            rect.y1 = rect.y0 + ui->size.content.height;
        }

        //GUI_DispStringInRect(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
        GUI_DispStringInRectWrap(res->str_content, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
        x = ui->pos.btn_start.x;
        y = ui->pos.btn_start.y;
        rect.y0 = y;
        rect.y1 = y + ui->size.btn.height;

        for (i = 0; i < res->btn_count; i++)
        {
            if (ui->ctrl.focus_id == i)
            {
                bmp_data = ResHandle2Data(res->bmp_btn_f);
            }
            else
            {
                bmp_data = ResHandle2Data(res->bmp_btn_n);
            }

            GUI_BMP_Draw(bmp_data, x, y);
            rect.x0 = x;
            rect.x1 = rect.x0 + ui->size.btn.width;
            GUI_DispStringInRect(res->str_btn[i], &rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
            x = rect.x1 + ui->size.btn_sp.width;
        }

        //GUI_CloseAlphaBlend();
    }
}
static __s32 app_dialog_paint_ex(__gui_msg_t *msg)
{
    //////////GUI_MEMDEV_Handle draw_mem;
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
    __wrn("wnd_para->res->str_content = %s", wnd_para->res.str_content);
    ASSERT(wnd_para);

    if (((__s32)GUI_LYR_STA_ON == (__s32)GUI_LyrWinGetSta(wnd_para->hlyr)) || ((__s32)GUI_LYR_STA_SLEEP == (__s32)GUI_LyrWinGetSta(wnd_para->hlyr)))
    {
        GUI_LyrWinSel(wnd_para->hlyr);
        GUI_UC_SetEncodeUTF8();
        GUI_SetFont(SWFFont);
        //////////////draw_mem = GUI_MEMDEV_Create(0, 0, wnd_para->ui.rt.width, wnd_para->ui.rt.height);
        ///////////////ASSERT(draw_mem);
        //////////GUI_MEMDEV_Select(draw_mem);
        draw_dialog(&wnd_para->res, &wnd_para->ui, wnd_para->ID);
        ///////////////GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
        ///////////////GUI_MEMDEV_Select( NULL );
        //////////////GUI_MEMDEV_Delete(draw_mem);
        //////GUI_LyrWinSetSta(wnd_para->hlyr, GUI_LYR_STA_ON);
    }
    else
    {
        __wrn("lyr state error......");
        return EPDK_OK;
    }

    return EPDK_OK;
}

//
static __s32 on_dialog_paint(__gui_msg_t *msg)
{
    GUI_MEMDEV_Handle draw_mem;
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
    __msg("wnd_para->res->str_content = %s", wnd_para->res.str_content);
    ASSERT(wnd_para);

    if ((__s32)GUI_LyrWinGetSta(wnd_para->hlyr) >= (__s32)GUI_LYR_STA_SUSPEND || (__s32)GUI_LyrWinGetSta(wnd_para->hlyr) < (__s32)GUI_LYR_STA_ON)
    {
        return EPDK_OK;
    }

    GUI_LyrWinSel(wnd_para->hlyr);
    GUI_UC_SetEncodeUTF8();
    GUI_SetFont(SWFFont);
#if 1
    GUI_LyrWinCacheOn();
    com_memdev_create(wnd_para->hlyr);
#else
    draw_mem = GUI_MEMDEV_Create(0, 0, wnd_para->ui.rt.width, wnd_para->ui.rt.height);
    ASSERT(draw_mem);
    GUI_MEMDEV_Select(draw_mem);
#endif
    draw_dialog(&wnd_para->res, &wnd_para->ui, wnd_para->ID);
#if 1
    GUI_LyrWinCacheOff();
    com_memdev_delete();
    GUI_LyrWinFlushFB(wnd_para->hlyr);
#else
    GUI_MEMDEV_CopyToLCD_Ex(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
#endif
    return EPDK_OK;
}
static __s32 on_dialog_paint_ex(__gui_msg_t *msg)
{
    GUI_MEMDEV_Handle draw_mem;
    dialog_wnd_t *wnd_para;
    GUI_RECT          rect;
    //__here__;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
    //__here__;
    ASSERT(wnd_para);

    //__here__;
    if ((__s32)GUI_LyrWinGetSta(wnd_para->hlyr) != (__s32)GUI_LYR_STA_ON)
    {
        //__here__;
        return EPDK_OK;
    }

    __msg("(char *)msg->p_arg = %s", (char *)msg->p_arg);
    GUI_LyrWinSel(wnd_para->hlyr);
    GUI_UC_SetEncodeUTF8();
    GUI_SetFont(SWFFont);
    //__here__;
    GUI_SetColor(wnd_para->ui.colour.txt_n);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    rect.x0 = wnd_para->ui.pos.content.x;
    rect.y0 = wnd_para->ui.pos.content.y;
    rect.x1 = rect.x0 + wnd_para->ui.size.content.width;
    rect.y1 = rect.y0 + wnd_para->ui.size.content.height;
    __msg(" msg->p_arg = %s", (char *)msg->p_arg);
    GUI_DispStringInRectWrap(msg->p_arg, &rect, GUI_TA_HCENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
    return EPDK_OK;
}

static __s32 on_dialog_timer(__gui_msg_t *msg)
{
    if (msg->dwAddData1 == ID_TIMER_DIALOG_TIMEOUT)
    {
        dialog_wnd_t *wnd_para;
        app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
        wnd_para->result = ADLG_IDM_TIMEROUT;
        notify_to_close_dialog(msg->h_deswin);
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}


static __s32 app_on_dialog_create_ex(__gui_msg_t *msg)
{
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);
    GUI_LyrWinSetSta(wnd_para->hlyr, GUI_LYRWIN_STA_SLEEP); //先设置为sleep  状态再画
    //
    //GUI_LyrWinSetTop(wnd_para->hlyr);
    app_dialog_paint_ex(msg);

    if (wnd_para->style & ADLG_DS_VISIBLE)
    {
        //__wrn("set hlyr on and set it top  ");
        //app_show_layer(wnd_para->hlyr);
    }
    else
    {
        SEND_MSG(GUI_MSG_COMMAND, wnd_para->hwnd, wnd_para->parent, MAKELONG(wnd_para->ID, ADLG_CMD_SET_UI), 0);
    }

    GUI_LyrWinSetSta(wnd_para->hlyr, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(wnd_para->hlyr);
    GUI_WinSetFocusChild(msg->h_deswin);

    if (IsDialogTimerValid(wnd_para->timeout))
    {
        GUI_SetTimer(msg->h_deswin, ID_TIMER_DIALOG_TIMEOUT, wnd_para->timeout, NULL);
    }

    GUI_WinSetCaptureWin(msg->h_deswin);
    return EPDK_OK;
}



static __s32 on_dialog_create(__gui_msg_t *msg)
{
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);

    if (wnd_para->style & ADLG_DS_VISIBLE)
    {
        app_show_layer(wnd_para->hlyr);
        //__here__;
    }
    else
    {
        SEND_MSG(GUI_MSG_COMMAND, wnd_para->hwnd, wnd_para->parent, MAKELONG(wnd_para->ID, ADLG_CMD_SET_UI), 0);
    }

    GUI_LyrWinSetTop(wnd_para->hlyr);
    GUI_WinSetFocusChild(msg->h_deswin);

    if (IsDialogTimerValid(wnd_para->timeout))
    {
        GUI_SetTimer(wnd_para->hwnd, ID_TIMER_DIALOG_TIMEOUT, wnd_para->timeout, NULL);
    }

    GUI_WinSetCaptureWin(msg->h_deswin);
    return EPDK_OK;
}

static __s32 on_dialog_destroy(__gui_msg_t *msg)
{
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, msg->h_deswin);

    if (IsDialogTimerValid(wnd_para->timeout))
    {
        GUI_KillTimer(wnd_para->hwnd, ID_TIMER_DIALOG_TIMEOUT);
    }

    if (GUI_WinGetCaptureWin() == msg->h_deswin)
    {
        GUI_WinReleaseCapture();
    }

    app_dialog_data_release(wnd_para);
    __msg("on_dialog_destroy end");
    return EPDK_OK;
}

//对话框回调
static __s32 app_dialog_proc(__gui_msg_t *msg)
{
    DEBUG_CBMSGEx(app_dialog_proc);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            return app_on_dialog_create_ex(msg);//on_dialog_create(msg); //by chenguofeng

        case GUI_MSG_CLOSE:
            GUI_FrmWinDelete(msg->h_deswin);
            break;

        case GUI_MSG_DESTROY:
            return on_dialog_destroy(msg);

        case GUI_MSG_PAINT:
            return on_dialog_paint(msg);

        case GUI_MSG_KEY:
            return dialog_key_proc(msg);

        case GUI_MSG_TOUCH:
            return on_dialog_touch_proc(msg);

        //      break;
        case GUI_MSG_TIMER:
            if (on_dialog_timer(msg) == EPDK_OK)
            {
                return EPDK_OK;
            }

            break;

        case GUI_MSG_COMMAND:
            //__here__;
            on_dialog_command(msg);
            return EPDK_OK;

        case GUI_MSG_WIN_WAKEUP:
            GUI_InvalidateRect(msg->h_deswin, NULL, EPDK_TRUE);
            break;

        default:
            break;
    }

    return GUI_FrmWinDefaultProc(msg);
}

static H_WIN app_dialog_create_frmwin(dialog_wnd_t *wnd_para)
{
    __gui_framewincreate_para_t framewin_para;
    FB fb;
    GUI_LyrWinGetFB(wnd_para->hlyr, &fb);
    __msg("dialog size(%d, %d)", fb.size.width, fb.size.height);
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = APP_DIALOG;
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = wnd_para->ID;
    framewin_para.hHosting      = wnd_para->parent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_dialog_proc);
    framewin_para.rect.x        = 0;
    framewin_para.rect.y        = 0;
    framewin_para.rect.width    = fb.size.width;
    framewin_para.rect.height   = fb.size.height;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)wnd_para;
    framewin_para.hLayer        = wnd_para->hlyr;
    return (GUI_FrmWinCreate(&framewin_para));
}

/************************************************************************/
// 建立图层
/************************************************************************/
#define app_dialog_layer_create(_layer, _prt)        \
    app_com_layer_create(_layer, _prt, 1, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SUSPEND, "dialog layer")
// static H_LYR app_dialog_layer_create(RECT *rect)
// {
//  H_LYR layer = NULL;
//  FB  fb =
//  {
//      {0, 0},                                         /* size      */
//      {0, 0, 0},                                      /* buffer    */
//      {FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
//  };
//
//  __disp_layer_para_t lstlyr =
//  {
//      DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
//      0,                                              /* ck_mode   */
//      0,                                              /* alpha_en  */
//      0,                                              /* alpha_val */
//      1,                                              /* pipe      */
//      0xff,                                           /* prio      */
//      {0, 0, 0, 0},                                   /* screen    */
//      {0, 0, 0, 0},                                   /* source    */
//      DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
//      NULL                                            /* fb        */
//  };
//
//  __layerwincreate_para_t lyrcreate_info =
//  {
//      "dialog layer",
//      NULL,
//      GUI_LYRWIN_STA_SUSPEND,
//      GUI_LYRWIN_NORMAL
//  };
//
//  fb.size.width       = rect->width;
//  fb.size.height      = rect->height;
//
//  // framebuffer source window
//  lstlyr.src_win.x        = 0;
//  lstlyr.src_win.y        = 0;
//  lstlyr.src_win.width    = rect->width;
//  lstlyr.src_win.height   = rect->height;
//
//  //screen region
//  lstlyr.scn_win.x        = rect->x;
//  lstlyr.scn_win.y        = rect->y;
//  lstlyr.scn_win.width    = rect->width;
//  lstlyr.scn_win.height   = rect->height;
//
//  lstlyr.pipe = 1;
//  lstlyr.fb = &fb;
//  lyrcreate_info.lyrpara = &lstlyr;
//
//  layer = GUI_LyrWinCreate(&lyrcreate_info);
//  if( !layer )
//  {
//      __err("app layer create error !");
//  }
//  __msg("create layer = %x", layer);
//
//  return layer;
// }


H_WIN app_dialog_create(H_WIN h_parent, __s32 win_id, __u32 style,
                        __s32 bmp_id[], __s32 str_id[], __u32 timeout, const char *str)
{
    //TODO:
    dialog_wnd_t *wnd_para;
    __inf("****************************************************************");
    __inf("********  enter dialog app  **************");
    __inf("****************************************************************");
    BallocType(wnd_para, dialog_wnd_t);

    if (wnd_para == NULL)
    {
        return NULL;
    }
    else
    {
        __msg("app_dialog wnd para ============== %x", wnd_para);
    }

    eLIBs_memset(wnd_para, 0, sizeof(dialog_wnd_t));
    wnd_para->parent  = h_parent;
    wnd_para->ID      = win_id;
    wnd_para->style   = style;
    wnd_para->timeout = timeout;
    app_dialog_data_init(wnd_para, bmp_id, str_id, NULL, str);
    app_dialog_layer_create(wnd_para->hlyr, &wnd_para->ui.rt);

    if (wnd_para->hlyr == NULL)
    {
        //TODO:release mem
        __err("app_dialog_layer_create error(%d, %d)", wnd_para->ui.rt.width, wnd_para->ui.rt.height);
    }

    wnd_para->hwnd = app_dialog_create_frmwin(wnd_para);
    GUI_WinSetFocusChild(wnd_para->hwnd);
    return wnd_para->hwnd;
}

H_WIN app_dialog_create_ex(H_WIN h_parent, __s32 win_id, __u32 btn_style,
                           __s32 bmp_id[], __s32 str_id[], __u32 timeout, GUI_RECT *dialog_rt)
{
    dialog_wnd_t *wnd_para;
    __inf("****************************************************************");
    __inf("********  enter dialog app  **************");
    __inf("****************************************************************");
    BallocType(wnd_para, dialog_wnd_t);

    if (wnd_para == NULL)
    {
        return NULL;
    }

    eLIBs_memset(wnd_para, 0, sizeof(dialog_wnd_t));
    wnd_para->parent  = h_parent;
    wnd_para->ID      = win_id;
    wnd_para->style   = btn_style;
    wnd_para->timeout = timeout;
    //  app_dialog_data_init(wnd_para, bmp_id, str_id, dialog_rt);
    return NULL;
}

__s32 app_dialog_destroy(H_WIN h_dialog)
{
    dialog_wnd_t *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, h_dialog);
    ASSERT(wnd_para);

    if (wnd_para->hlyr != NULL)
    {
        H_WIN parent = wnd_para->parent;
        app_com_layer_destroy(wnd_para->hlyr);//GUI_LyrWinDelete(wnd_para->hlyr);
        //      GUI_WinSetFocusChild(parent);
        __msg("end of dialog");
    }

    return 0;
}

__s32 app_dialog_set_ui(H_WIN h_dialog, __s32 ui_id, __s32 wParam, signed long lParam)
{
    APP_DIALOG_UI_T *ui;
    dialog_wnd_t    *wnd_para;
    app_dialog_GetWndPara(wnd_para, dialog_wnd_t, h_dialog);
    ASSERT(wnd_para);
    ui = &wnd_para->ui;

    switch (ui_id)
    {
        case ADLG_UI_CMD_TITLE:
        {
            ui->pos.title.x = wParam;
            ui->pos.title.y = lParam;
            ui->size.title.width  = ui->rt.width - wParam;
            //ui->size.title.height = ui->size.title.height;
        }
        break;

        case ADLG_UI_CMD_CONTENT:
        {
            RECT *pRt = (RECT *)lParam;
            ui->pos.content.x = pRt->x;
            ui->pos.content.y = pRt->y;
            ui->size.content.width  = pRt->width;
            ui->size.content.height = pRt->height;
        }
        break;

        case ADLG_UI_CMD_ICON:
        {
            ui->pos.icon.x = wParam;
            ui->pos.icon.y = lParam;
        }
        break;

        case ADLG_UI_CMD_START_BTN:
        {
            ui->pos.btn_start.x = wParam;
            ui->pos.btn_start.y = lParam;

            if (wnd_para->res.btn_count > 1)
            {
                //ui->size.btn_sp.width = ((ui->rt.width - wParam * 2) -  ui->size.btn.width * wnd_para->res.btn_count) / (wnd_para->res.btn_count - 1);
                ui->size.btn_sp.width = calc_btn_sp_w(ui->rt.width, wParam, ui->size.btn.width, wnd_para->res.btn_count);
                ASSERT(0 < ui->size.btn_sp.width);
            }
        }
        break;

        case ADLG_END_UI_CMD:
        {
            app_show_layer(wnd_para->hlyr);
        }
        break;

        default:
            ASSERT(0);
            break;
    }

    return EPDK_OK;
}
