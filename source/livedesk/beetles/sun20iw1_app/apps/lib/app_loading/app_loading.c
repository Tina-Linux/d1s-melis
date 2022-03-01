/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : msgbox.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <emodules/mod_maple.h>
#include "app_loading.h"
#include "app_loading_uipara.h"
#include "dtv_mid.h"
#include <log.h>


typedef struct tag_loading_ctrl
{
    app_loading_para_t *loading_para;
    HTHEME h_bmp_loading;
    __s32 focus_txt_color;
    char  loading_tips_text[128];
    __u8  loading_program_info;
    __u8  loading_bmp_cnt;
    __u32 loading_task;
    __u8  loading_txt_init_flag;
} app_loading_ctrl_t;

static __s32 loading_bmp_id[] =
{
    ID_LOADING_BUFF1_BMP,
    ID_LOADING_BUFF2_BMP,
    ID_LOADING_BUFF3_BMP,
    ID_LOADING_BUFF4_BMP,
    ID_LOADING_BUFF5_BMP,
    ID_LOADING_BUFF6_BMP,
    ID_LOADING_BUFF7_BMP,
    ID_LOADING_BUFF8_BMP,
    ID_LOADING_BUFF9_BMP,
    ID_LOADING_BUFF10_BMP,
    ID_LOADING_BUFF11_BMP,
    ID_LOADING_BUFF12_BMP,
    ID_LOADING_BUFF13_BMP,
};

#define LOADING_BMP_NUM (sizeof(loading_bmp_id))

/**********************************************************************************************************************
* 图层创建接口
**********************************************************************************************************************/
static H_WIN app_loading_layer_create(RECT *rect)
{
    FB  fb =
    {
        {0, 0},
        {0, 0, 0},
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},
    };
    __disp_layer_para_t para =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,
        0,
        0,

        0,
        11,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        DISP_LAYER_OUTPUT_CHN_DE_CH1,
        NULL
    };
    __layerwincreate_para_t create_info =
    {
        "app_loading_layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        0
    };
    fb.size.width  = rect->width;
    fb.size.height = rect->height;
    para.pipe           = 1;
    para.scn_win.x      = rect->x;
    para.scn_win.y      = rect->y;
    para.scn_win.width  = rect->width;
    para.scn_win.height = rect->height;
    para.src_win.x      = 0;
    para.src_win.y      = 0;
    para.src_win.width  = rect->width;
    para.src_win.height = rect->height;
    para.fb = &fb;
    create_info.lyrpara = &para;
    return (GUI_LyrWinCreate(&create_info));
}

static __s32 loading_paint(H_WIN h_win)
{
    app_loading_ctrl_t *loading_ctrl;
    GUI_RECT gui_rect;
    app_loading_uipara_t *app_loading_uipara;

    if (GUI_LyrWinGetSta(GUI_WinGetLyrWin(h_win)) != GUI_LYRWIN_STA_ON)
    {
        return EPDK_FAIL;
    }

    loading_ctrl = (app_loading_ctrl_t *)GUI_WinGetAddData(h_win);
    app_loading_uipara = (app_loading_uipara_t *)get_app_loading_uipara();
    GUI_LyrWinSel(loading_ctrl->loading_para->loading_layer);
    GUI_SetFont(loading_ctrl->loading_para->font);
    GUI_UC_SetEncodeUTF8();

    if (loading_ctrl->h_bmp_loading)
    {
        dsk_theme_close(loading_ctrl->h_bmp_loading);

        if (loading_ctrl->loading_bmp_cnt >= LOADING_BMP_NUM)
        {
            loading_ctrl->loading_bmp_cnt = 0;
        }

        loading_ctrl->h_bmp_loading = dsk_theme_open(loading_bmp_id[loading_ctrl->loading_bmp_cnt]);
        loading_ctrl->loading_bmp_cnt++;
    }
    else
    {
        loading_ctrl->h_bmp_loading = dsk_theme_open(loading_bmp_id[loading_ctrl->loading_bmp_cnt]);
    }

    GUI_BMP_Draw(theme_hdl2buf(loading_ctrl->h_bmp_loading), app_loading_uipara->loading_bmp_pos.x, app_loading_uipara->loading_bmp_pos.y);

    if (loading_ctrl->loading_txt_init_flag == EPDK_FALSE)
    {
        extern H_WIN g_dtv_mainwin;
        extern HTHEME dtv_sel_bg_bmp;

        if (g_dtv_mainwin != NULL && loading_ctrl->loading_program_info == EPDK_TRUE)
        {
            maple_demod_ss_t ss;
            __u8    str_text[64] = {0};
            extern maple_serv_list_t   *service_list;
            GUI_SetColor(GUI_WHITE);
            gui_rect.x0 = 20;
            gui_rect.y0 = 46;
            GUI_BMP_Draw(theme_hdl2buf(dtv_sel_bg_bmp), gui_rect.x0, gui_rect.y0);
            gui_rect.x0 += 20;
            gui_rect.y0 += 20;
            gui_rect.x1 = gui_rect.x0 + 200;
            gui_rect.y1 = gui_rect.y0 + 30;
            eLIBs_sprintf(str_text, " %d  %s", service_list->curServIdx + 1,
                          service_list->pServItems[service_list->curServIdx].servName);
            GUI_DispStringInRectWrap(str_text, &gui_rect, GUI_TA_VCENTER | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
            eLIBs_memset(str_text, 0x00, sizeof(str_text));
            gui_rect.y0 = gui_rect.y1;
            gui_rect.y1 = gui_rect.y1 + 30;
            eLIBs_sprintf(str_text, " Freq: %d MHz", service_list->pServItems[service_list->curServIdx].demod_pm.frequency / 1000 / 1000);
            GUI_DispStringInRectWrap(str_text, &gui_rect, GUI_TA_VCENTER | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
            eLIBs_memset(str_text, 0x00, sizeof(str_text));
            gui_rect.y0 = gui_rect.y1;
            gui_rect.y1 = gui_rect.y1 + 30;
            eLIBs_memset(&ss, 0x00, sizeof(ss));
            maple_get_ss(&ss);
            eLIBs_sprintf(str_text, " SS:%d  SQ:%d", ss.strength, ss.quality);
            GUI_DispStringInRectWrap(str_text, &gui_rect, GUI_TA_VCENTER | GUI_TA_LEFT, GUI_WRAPMODE_WORD);
            loading_ctrl->loading_program_info = EPDK_FALSE;
        }

        GUI_SetColor(GUI_BLUE);
        get_menu_text(STRING_LOADING, loading_ctrl->loading_tips_text, 128);
        gui_rect.x0 = app_loading_uipara->loading_txt_pos.x;
        gui_rect.y0 = app_loading_uipara->loading_txt_pos.y;
        gui_rect.x1 = gui_rect.x0 + app_loading_uipara->loading_txt_pos.w;
        gui_rect.y1 = gui_rect.y0 + app_loading_uipara->loading_txt_pos.h;
        GUI_DispStringInRectWrap(loading_ctrl->loading_tips_text, &gui_rect, GUI_TA_VCENTER | GUI_TA_HCENTER, GUI_WRAPMODE_WORD);
        loading_ctrl->loading_txt_init_flag = EPDK_FALSE;
    }

    return EPDK_OK;
}

static void __show_task(void *p_arg)
{
    H_WIN       hwin;
    hwin = (H_WIN)p_arg;

    while (1)
    {
        if (esKRNL_TDelReq(EXEC_prioself) == OS_TASK_DEL_REQ)
        {
            goto EXIT_DEC_TASK;
        }

        loading_paint(hwin);
        esKRNL_TimeDly(10);
    }

EXIT_DEC_TASK:
    esKRNL_TDel(EXEC_prioself);
    return;
}

static __s32 _app_loading_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            app_loading_ctrl_t *loading_ctrl = NULL;
            app_loading_para_t *loading_para = NULL;
            loading_ctrl = (app_loading_ctrl_t *)My_Malloc(0, sizeof(app_loading_ctrl_t));

            if (!loading_ctrl)
            {
                __msg("memory alloc fail.........\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(loading_ctrl, 0, sizeof(app_loading_ctrl_t));
            loading_para = (app_loading_para_t *)GUI_WinGetAttr(msg->h_deswin);
            loading_ctrl->loading_para = loading_para;
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)loading_ctrl);
            GUI_LyrWinCacheOn();
            GUI_LyrWinSetSta(loading_ctrl->loading_para->loading_layer, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSetTop(loading_ctrl->loading_para->loading_layer);
            GUI_LyrWinCacheOff();
            loading_ctrl->loading_task = esKRNL_TCreate(__show_task, (void *)(msg->h_deswin), 0x1000, KRNL_priolevel3);
        }
        break;

        case GUI_MSG_CLOSE:
            break;

        case GUI_MSG_DESTROY:
        {
            app_loading_ctrl_t *loading_ctrl = NULL;
            loading_ctrl = (app_loading_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!loading_ctrl)
            {
                __err("loading_ctrl is null.......\n");
                return EPDK_FAIL;
            }

            if (loading_ctrl->h_bmp_loading)
            {
                dsk_theme_close(loading_ctrl->h_bmp_loading);
                loading_ctrl->h_bmp_loading = 0;
            }

            if (loading_ctrl->loading_task != 0)
            {
                while (esKRNL_TDelReq(loading_ctrl->loading_task) != OS_TASK_NOT_EXIST)
                {
                    esKRNL_TimeDly(5);
                }

                loading_ctrl->loading_task = 0;
            }

            if (loading_ctrl->loading_para)
            {
                My_Mfree(0, loading_ctrl->loading_para);
                loading_ctrl->loading_para = NULL;
            }

            if (loading_ctrl)
            {
                My_Mfree(0, loading_ctrl);
            }
        }
        break;

        case GUI_MSG_PAINT:
            break;

        case GUI_MSG_KEY:
            break;

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
H_WIN app_loading_frm_win_create(H_WIN h_parent, app_loading_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    app_loading_para_t *loading_para = NULL;
    FB fb;
    eLIBs_memset(&fb, 0, sizeof(fb));
    GUI_LyrWinGetFB(para->loading_layer, &fb);
    loading_para = (app_loading_para_t *)My_Malloc(0, sizeof(app_loading_para_t));

    if (NULL == loading_para)
    {
        __msg("memory alloc fail.........\n");
        return (H_WIN)EPDK_FAIL;
    }

    loading_para->font = para->font;
    loading_para->loading_layer = para->loading_layer;
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "app_loading_win";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.hHosting = h_parent;
    framewin_para.id         = loading_para->id ;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_loading_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width =  fb.size.width;
    framewin_para.rect.height =  fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)loading_para;
    framewin_para.hLayer = para->loading_layer;
    return (GUI_FrmWinCreate(&framewin_para));
}


void  app_loading_win_hide(H_WIN loading_win)//add by lihaoyi
{
    H_LYR h_layer;

    if (loading_win == NULL)
    {
        return ;
    }

    h_layer = GUI_WinGetLyrWin(loading_win);

    if (h_layer != NULL)
    {
        GUI_LyrWinSetSta(h_layer, GUI_LYRWIN_STA_SUSPEND);
    }
}
void  app_loading_win_show(H_WIN loading_win)
{
    H_LYR h_layer;
    app_loading_ctrl_t *loading_ctrl = NULL;
    extern H_WIN g_dtv_mainwin;
    h_layer = GUI_WinGetLyrWin(loading_win);
    loading_ctrl = (app_loading_ctrl_t *)GUI_WinGetAddData(loading_win);

    if (g_dtv_mainwin != NULL)
    {
        loading_ctrl->loading_program_info = EPDK_TRUE;
        eLIBs_printf("%s %d g_dtv_mainwin:0x%x loading_program_info:%d\r\n", __FILE__, __LINE__, g_dtv_mainwin, loading_ctrl->loading_program_info);
    }

    if (h_layer != NULL)
    {
        loading_ctrl->loading_bmp_cnt = 0;
        GUI_LyrWinSetSta(h_layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(h_layer);
    }
}

void  app_loading_win_delete(H_WIN loading_win)
{
    H_LYR h_layer;

    if (loading_win == NULL)
    {
        return ;
    }

    h_layer = GUI_WinGetLyrWin(loading_win);

    if (h_layer != NULL)
    {
        GUI_LyrWinDelete(h_layer);
    }
}
H_WIN app_loading_win_create(H_WIN hparent, __u16 id)
{
    app_loading_para_t   app_loading_para = {0};
    RECT rect = {0};
    H_WIN h_wnd;
    dsk_display_get_size(&rect.width, &rect.height);
    app_loading_para.loading_layer = app_loading_layer_create(&rect);
    app_loading_para.font = SWFFont;
    app_loading_para.id = id ;
    h_wnd = app_loading_frm_win_create(hparent, &app_loading_para);

    if (h_wnd)
    {
        __msg("app_loading_frm_win_create succeed\n");
        return h_wnd;
    }
    else
    {
        __msg("app_loading_frm_win_create fail\n");
        return h_wnd;
    }
}