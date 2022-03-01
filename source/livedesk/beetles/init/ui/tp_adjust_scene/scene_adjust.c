/*
*********************************************************************************************************
*                                                   ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              calendar app sample
*
*                                   (c) Copyright 2006-2009, ANDY, China
*                                            All Rights Reserved
*
* File    : SAJT.c
* By      :lyn
* Version : V1.00
*********************************************************************************************************
*/
#include <log.h>
#include <emodules/mod_touchpanel.h>
#include "mod_init_i.h"
#include "scene_adjust_uipara.h"
#include <kconfig.h>


typedef struct
{
    GUI_FONT             *SWFFont;              /*×ÖÌå¾ä±ú*/
    int                 tp_hdle;
    __ev_tp_pos_t       cross_pos;
    H_WIN               layer;
} adjust_t;

typedef struct
{
    H_WIN   layer;
    H_WIN   frmwin;
} __scene_para_t;

static __s32 __init_ui_res(adjust_t *adjust)
{
    adjust->SWFFont = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, BEETLES_APP_ROOT"res\\fonts\\font16.sft");
    //adjust->SWFFont = GUI_SFT_CreateFont(BEETLES_FONT_SIZE_INT, "/data/apps/font16.sft");

    if (adjust->SWFFont == NULL)
    {
        return -1;
    }

    return 0;
}
static __s32 __deinit_ui_res(adjust_t *adjust)
{
    if (adjust->SWFFont != NULL)
    {
        //release font
        GUI_FONT    *DefaultFont;
        DefaultFont = GUI_GetDefaultFont();
        GUI_SetFont(DefaultFont);
        GUI_SFT_ReleaseFont(adjust->SWFFont);
    }

    return 0;
}

static __s32 __draw_point(adjust_t *adjust, __u32 flag, __u32 i)
{
    scene_adjust_para_t *scene_adjust_uipara;
    char str[128];
    scene_adjust_uipara = scene_adjust_get_uipara();

    if (flag == 0)
    {
        GUI_LyrWinSel(adjust->layer);
        GUI_SetBkColor(0xffffffff);
        GUI_Clear();
        GUI_SetColor(0xff0000ff);
        GUI_DrawLine(scene_adjust_uipara->coor_point[i].x - 25, scene_adjust_uipara->coor_point[i].y,
                     scene_adjust_uipara->coor_point[i].x + 25, scene_adjust_uipara->coor_point[i].y);
        GUI_DrawLine(scene_adjust_uipara->coor_point[i].x, scene_adjust_uipara->coor_point[i].y - 25,
                     scene_adjust_uipara->coor_point[i].x, scene_adjust_uipara->coor_point[i].y + 25);
    }
    else if (flag == 1)
    {
        GUI_LyrWinSel(adjust->layer);
        GUI_SetBkColor(0xffffffff);
        GUI_Clear();
        GUI_SetColor(0xff0000ff);
        eLIBs_memset(str, 0, 128);
        dsk_langres_get_menu_text(STRING_ADJUST_FINISH, str, 128);
        GUI_OpenAlphaBlend();
        GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
        GUI_CharSetToEncode(EPDK_CHARSET_ENM_UTF8);
        GUI_SetFont(adjust->SWFFont);
        GUI_DispStringHCenterAt(str, scene_adjust_uipara->adjust_frm_win.x + scene_adjust_uipara->adjust_frm_win.w / 2,
                                scene_adjust_uipara->adjust_frm_win.y + scene_adjust_uipara->adjust_frm_win.h / 2);
        GUI_LyrWinFlushFB(adjust->layer);
        GUI_CloseAlphaBlend();
    }

    return 0;
}

static __s32 __frmwin_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            scene_adjust_para_t *scene_adjust_uipara;
            adjust_t *adjust;
            __u32 i = 1;
            __s32 ret = 1;
            scene_adjust_uipara = scene_adjust_get_uipara();
            adjust = (adjust_t *)esMEMS_Malloc(0, sizeof(adjust_t));

            if (!adjust)
            {
                __wrn("adjust malloc error\n");
                return EPDK_FAIL;
            }

            adjust->layer = *((H_WIN *)GUI_WinGetAttr(msg->h_deswin));
            __init_ui_res(adjust);
            GUI_LyrWinSetSta(adjust->layer, GUI_LYRWIN_STA_ON);
            GUI_LyrWinSetTop(adjust->layer);
            adjust->tp_hdle = tpdrv_open();
            tp_adjust(adjust->tp_hdle, 0, 0);

            while (1)
            {
                __draw_point(adjust, 0, i - 1);
                adjust->cross_pos.disp_xpos = scene_adjust_uipara->coor_point[i - 1].x;
                adjust->cross_pos.disp_ypos = scene_adjust_uipara->coor_point[i - 1].y;
                __err("pos sent x:%d  y:%d",adjust->cross_pos.disp_xpos,adjust->cross_pos.disp_ypos);
                ret = tp_adjust(adjust->tp_hdle, i, (void *)&adjust->cross_pos);
                __msg("ret = %d", ret);

                if (ret != EPDK_OK)
                {
                    if (ret == -2)
                    {
                        i = 1;
                    }

                    continue;
                }
                else
                {
                    if (i == 5)
                    {
                        __draw_point(adjust, 1, i - 1);
                        esKRNL_TimeDly(100);
                        break;
                    }
                    else
                    {
                        i++;
                    }
                }
            }

            //tpdrv_close(adjust->tp_hdle);
            GUI_WinSetAttr(msg->h_deswin, (void *)adjust);
            break;
        }

        case GUI_MSG_DESTROY:
        {
            adjust_t *adjust;
            adjust = (adjust_t *)GUI_WinGetAttr(msg->h_deswin);
            __deinit_ui_res(adjust);
            GUI_LyrWinSetSta(adjust->layer, GUI_LYRWIN_STA_SUSPEND);
            esMEMS_Mfree(0, adjust);
            break;
        }

        case GUI_MSG_CLOSE:
        {
            GUI_FrmWinDelete(msg->h_deswin);
            break;
        }

        default:
            GUI_FrmWinDefaultProc(msg);
            break;
    }

    return 0;
}

static H_WIN __adjust_layer_create(void)
{
    H_WIN layer;
    FB  fb;
    __disp_layer_para_t lstlyr;
    __layerwincreate_para_t lyrcreate_info;
    scene_adjust_para_t *scene_adjust_uipara;
    scene_adjust_uipara = scene_adjust_get_uipara();
    eLIBs_memset(&lstlyr, 0, sizeof(__disp_layer_para_t));
    eLIBs_memset(&fb, 0, sizeof(FB));
    eLIBs_memset(&lyrcreate_info, 0, sizeof(__layerwincreate_para_t));
    fb.addr[0] = NULL;
    fb.addr[1] = NULL;
    fb.addr[2] = NULL;
    fb.size.width = scene_adjust_uipara->adust_layer.w;
    fb.size.height = scene_adjust_uipara->adust_layer.h;
    fb.fmt.type = FB_TYPE_RGB;
    fb.fmt.fmt.rgb.pixelfmt = (__pixel_rgbfmt_t)scene_adjust_uipara->adust_layer.fmt;
    fb.fmt.fmt.rgb.br_swap = 0;
    fb.fmt.fmt.rgb.pixseq = (__rgb_seq_t)0;
    lstlyr.pipe = 0;
    lstlyr.prio = 0xff;
    lstlyr.scn_win.x = scene_adjust_uipara->adust_layer.x;
    lstlyr.scn_win.y = scene_adjust_uipara->adust_layer.y;
    lstlyr.scn_win.width = scene_adjust_uipara->adust_layer.w;
    lstlyr.scn_win.height = scene_adjust_uipara->adust_layer.h;
    lstlyr.src_win.x = 0;
    lstlyr.src_win.y = 0;
    lstlyr.src_win.width = scene_adjust_uipara->adust_layer.w;
    lstlyr.src_win.height = scene_adjust_uipara->adust_layer.h;
    lstlyr.channel = DISP_LAYER_OUTPUT_CHN_DE_CH1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    lyrcreate_info.initsta = GUI_LYRWIN_STA_SUSPEND;
    layer = GUI_LyrWinCreate(&lyrcreate_info);
    return layer;
}

static H_WIN __adjust_frm_create(H_WIN layer, H_WIN parent)
{
    __gui_framewincreate_para_t  frmcreate_info;
    H_WIN frame_win;
    scene_adjust_para_t *scene_adjust_uipara;
    scene_adjust_uipara = scene_adjust_get_uipara();
    eLIBs_memset(&frmcreate_info, 0, sizeof(__gui_framewincreate_para_t));
    frmcreate_info.name             = NULL;
    frmcreate_info.attr             = (void *)&layer;
    frmcreate_info.BkColor.alpha    = 0;
    frmcreate_info.BkColor.red      = 0;
    frmcreate_info.BkColor.green    = 0;
    frmcreate_info.BkColor.blue     = 0;
    frmcreate_info.dwStyle          = WS_VISIBLE;
    frmcreate_info.dwExStyle        = 0;
    frmcreate_info.hOwner           = NULL;
    frmcreate_info.hHosting         = parent;
    frmcreate_info.FrameWinProc     = __frmwin_proc;
    frmcreate_info.hLayer           = layer;
    frmcreate_info.rect.x           = scene_adjust_uipara->adjust_frm_win.x;
    frmcreate_info.rect.y           = scene_adjust_uipara->adjust_frm_win.y;
    frmcreate_info.rect.width       = scene_adjust_uipara->adjust_frm_win.w;
    frmcreate_info.rect.height      = scene_adjust_uipara->adjust_frm_win.h;
    frame_win = GUI_FrmWinCreate(&frmcreate_info);

    if (!frame_win)
    {
        return frame_win;
    }

    return frame_win;
}

static __s32 __main_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __scene_para_t *scene_para;
            scene_para = (__scene_para_t *)esMEMS_Malloc(0, sizeof(__scene_para_t));

            if (!scene_para)
            {
                __wrn("scene_para malloc error\n");
                return EPDK_FAIL;
            }

            eLIBs_memset(scene_para, 0, sizeof(__scene_para_t));
            GUI_WinSetAttr(msg->h_deswin, (void *)scene_para);
            scene_para->layer = __adjust_layer_create();
            scene_para->frmwin = __adjust_frm_create(scene_para->layer, msg->h_deswin);
            break;
        }

        case GUI_MSG_DESTROY:
        {
            __scene_para_t *scene_para;
            scene_para = (__scene_para_t *)GUI_WinGetAttr(msg->h_deswin);
            GUI_LyrWinDelete(scene_para->layer);
            esMEMS_Mfree(0, scene_para);
            break;
        }

        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
            break;
        }

        default:
            GUI_ManWinDefaultProc(msg);
            break;
    }

    return 0;
}
H_WIN scene_adjust_create(H_WIN parent)
{
    __gui_manwincreate_para_t  create_info;
    H_WIN hWnd;
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.hParent         = parent;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc  = __main_proc;
    create_info.name            = NULL;
    hWnd = GUI_ManWinCreate(&create_info);

    if (hWnd == NULL)
    {
        __inf("create main windows failed!\n");
        return NULL;
    }

    return hWnd;
}

__s32 scene_adjust_destroy(H_WIN scene)
{
    GUI_ManWinDelete(scene);
    return 0;
}

