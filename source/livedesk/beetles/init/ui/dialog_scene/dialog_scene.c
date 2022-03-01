/*
************************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                 (c) Copyright 2007-2010, Jackie, China
*                                       All Rights Reserved
*
* File          : dialog_scene.c
* By            : Jackie.chen
* Func      : dialog scene
* Version   : v1.0
* ======================================================================================================================
* 2010-10-18  Jackie.chen  create this file, implements the fundemental interface;
************************************************************************************************************************
*/
#include <log.h>
#include "mod_init_i.h"
#include "dialog_scene.h"
#include "dialog_uipara.h"
#include "../init_server.h"
#include "progress_dialog.h"
#include <kconfig.h>

#define LOWPOWER_DIALOG_ID              2001
#define POWEROFF_DIALOG_ID              2002
#define USBD_DIALOG_ID                  2003
#define ORCHID_DIALOG_ID                2004
#define FW_UPDATE_DIALOG_ID             2005

#define LOW_POWER_TOUT                  500
#define POWER_OFF_TOUT                  500
#define ORCHID_UPDATE_TOUT              200     //check whether orchid update completed every 2 sec

static H_LYR scene_dialog_layer_create(void)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0, {NULL, 0}}}, 0},    /* fmt       */
    };
    __disp_layer_para_t dialog_lyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        11,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "dialog layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = dialog_uipara.dialog_lyr.fb_w;
    fb.size.height      = dialog_uipara.dialog_lyr.fb_h;
    dialog_lyr.src_win.x  = dialog_uipara.dialog_lyr.src_x;
    dialog_lyr.src_win.y  = dialog_uipara.dialog_lyr.src_y;
    dialog_lyr.src_win.width  = dialog_uipara.dialog_lyr.src_w;
    dialog_lyr.src_win.height = dialog_uipara.dialog_lyr.src_h;
    dialog_lyr.scn_win.x        = dialog_uipara.dialog_lyr.scn_x;
    dialog_lyr.scn_win.y        = dialog_uipara.dialog_lyr.scn_y;
    dialog_lyr.scn_win.width    = dialog_uipara.dialog_lyr.scn_w;
    dialog_lyr.scn_win.height   = dialog_uipara.dialog_lyr.scn_h;
    dialog_lyr.pipe = dialog_uipara.dialog_lyr.pipe;
    dialog_lyr.fb = &fb;
    lyrcreate_info.lyrpara = &dialog_lyr;
    //lyrcreate_info.type      = GUI_LYRWIN_SPRITE;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("dialog layer create error !\n");
    }

    return layer;
}

int32_t dialog_init(H_WIN hwnd)
{
    init_scene_t    *p_scene = (init_scene_t *)GUI_WinGetAddData(hwnd);

    init_dialog_uipara();
    p_scene->cur_dialog     = NONE_DIALOG;
    p_scene->usb_connect    = EPDK_FALSE;
    p_scene->orchid_update  = EPDK_FALSE;
    p_scene->low_power      = EPDK_FALSE;
    p_scene->power_off      = EPDK_FALSE;
    p_scene->fw_update      = EPDK_FALSE;
    p_scene->usb_plug_in    = EPDK_FALSE;
    p_scene->init_win       = hwnd;
    p_scene->dialog_layer   = scene_dialog_layer_create();
    return EPDK_OK;
}

__s32 dialog_deinit(H_WIN hwnd)
{
    init_scene_t *p_scene = (init_scene_t *)GUI_WinGetAddData(hwnd);
    //__here__
    GUI_LyrWinDelete(p_scene->dialog_layer);
    //__here__
    return EPDK_OK;
}

__s32 cb_dialog_frm(__gui_msg_t *msg)
{
    return GUI_FrmWinDefaultProc(msg);
}

static H_WIN dialog_frm_win_create(H_WIN parent, H_LYR layer)
{
    H_WIN                       h_win;
    __gui_framewincreate_para_t para;
    eLIBs_memset(&para, 0, sizeof(__gui_framewincreate_para_t));
    para.dwStyle          = WS_VISIBLE;
    para.dwExStyle        = 0;
    para.name             = NULL;
    para.hOwner           = 0;
    para.hHosting         = parent;
    para.FrameWinProc     = cb_dialog_frm;
    para.id               = 0;
    para.hLayer           = layer;
    para.BkColor.alpha    = 0;
    para.BkColor.red      = 0;
    para.BkColor.green    = 0;
    para.BkColor.blue     = 0;
    GUI_LyrWinGetSrcWindow(layer, &(para.rect));
    h_win = GUI_FrmWinCreate(&para);

    if (!h_win)
    {
        __err(" dialog_frm_win_create error ! \n");
    }

    return h_win;
}

static void scene_dialog_create(init_scene_t *p_scene, __s32 str_id, __s32 dialog_id)
{
    pdialog_create_t pdialog_para;
    GUI_LyrWinSetSta(p_scene->dialog_layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(p_scene->dialog_layer);
    p_scene->dialog_win  = dialog_frm_win_create(p_scene->init_win, p_scene->dialog_layer);
    /* 弹出对话框*/
    pdialog_para.text_id    = str_id;
    pdialog_para.dialog_id  = dialog_id;
    __msg("before progress_dialog_create\n");
    progress_dialog_create(p_scene->dialog_win, p_scene->dialog_layer, &pdialog_para);  // 有消息循环
    __msg("after progress_dialog_create\n");
    GUI_LyrWinSetSta(p_scene->dialog_layer, GUI_LYRWIN_STA_SUSPEND);
}

void scene_on_dialog(__gui_msg_t *msg)
{
    init_scene_t *p_scene = (init_scene_t *)GUI_WinGetAddData(msg->h_deswin);
    __msg("p_scene->cur_dialog=%d\n", p_scene->cur_dialog);

    switch (p_scene->cur_dialog)
    {
        case NONE_DIALOG:
        {
            //__here__;
            if (p_scene->low_power)
            {
                //__here__;
                p_scene->cur_dialog = LOWPOWER_DIALOG;
                GUI_SetTimer(msg->h_deswin, LOW_POWER_DIALOG_TIME_ID, LOW_POWER_TOUT, NULL);    // 低电对话框存在时间计数
                scene_dialog_create(p_scene, STRING_LOW_POWER, LOWPOWER_DIALOG_ID);
                //__here__;
            }

            if (p_scene->power_off)
            {
                //__here__;
                p_scene->cur_dialog = POWEROFF_DIALOG;
                GUI_SetTimer(msg->h_deswin, POWER_OFF_DIALOG_TIME_ID, POWER_OFF_TOUT, NULL);
                scene_dialog_create(p_scene, STRING_POWER_OFF, POWEROFF_DIALOG_ID);
                //__here__;
            }

            //__here__;
            if (p_scene->usb_connect)
            {
                //__here__;
                p_scene->cur_dialog = USBD_DIALOG;
                scene_dialog_create(p_scene, STRING_USB_CONNECTING, USBD_DIALOG_ID);
                //__here__;
            }

            if (p_scene->orchid_update)
            {
                //p_scene->cur_dialog = ORCHID_DIALOG;
                //GUI_SetTimer(msg->h_deswin, ORCHID_UPDATE_DIALOG_TIME_ID, ORCHID_UPDATE_TOUT , NULL);
                //scene_dialog_create(p_scene, STRING_UPDATE_ORCHID, ORCHID_DIALOG_ID);
            }

            if (p_scene->fw_update)
            {
                p_scene->cur_dialog = FW_UPDATA_DIALOG;
                scene_dialog_create(p_scene, STRING_FW_UPDATE, FW_UPDATE_DIALOG_ID);
            }

            break;
        }

        case LOWPOWER_DIALOG:
        {
            //__here__;
            if (!p_scene->low_power)
            {
                //__here__;
                GUI_FrmWinDelete(p_scene->dialog_win);
                p_scene->cur_dialog = NONE_DIALOG;
                //__here__;
            }

            break;
        }

        case POWEROFF_DIALOG:
        {
            //__here__;
            __msg("p_scene->power_off=%d\n", p_scene->power_off);

            if (!p_scene->power_off)
            {
                __wrn("**********before del pwoff dlg****************\n");
                dsk_reg_flush();
                esKRNL_TimeDly(100);
                GUI_FrmWinDelete(p_scene->dialog_win);
                p_scene->cur_dialog = NONE_DIALOG;
                __wrn("**********after del pwoff dlg****************\n");
            }

            break;
        }

        case USBD_DIALOG:
        {
            //__here__;
            if (!p_scene->usb_connect)
            {
                //__here__;
                GUI_FrmWinDelete(p_scene->dialog_win);
                p_scene->cur_dialog = NONE_DIALOG;
                //__here__;
            }

            break;
        }

        case ORCHID_DIALOG:
        {
            //if( !p_scene->orchid_update )
            //{
            //  GUI_FrmWinDelete(p_scene->dialog_win);
            //  p_scene->cur_dialog = NONE_DIALOG;
            //}
            break;
        }

        case FW_UPDATA_DIALOG:
        {
            if (!p_scene->fw_update)
            {
                GUI_FrmWinDelete(p_scene->dialog_win);
                p_scene->cur_dialog = NONE_DIALOG;
                //__here__
            }

            break;
        }

        default:
            break;
    }
}

