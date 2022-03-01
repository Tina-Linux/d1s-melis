/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : ff_rr_show.c
* By        :
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011-05-05  Bayden.chen  create this file
**************************************************************************************************************
*/
#include <log.h>
#include "ff_rr_show.h"
#include "../common/common.h"
#include "beetles_app.h"

#define MAX_STRING_LEN 256
#define strcat(s1, s2)                    eLIBs_strcat(s1, s2)


typedef struct
{
    //输入参数
    H_WIN hparent;
    __s32 scene_id;
    __s32 sub_state;


    //内部参数
    H_LYR hlyr;
    H_WIN hfrm;
} movie_ff_rr_show_scene_t;


/***********************************************************************************************************
    建立图层
************************************************************************************************************/
static H_LYR __ff_rr_show_32bpp_layer_create(RECT *rect, __s32 pipe)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        10,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "ff rr show layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    fb.size.width       = rect->width;
    fb.size.height      = rect->height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = rect->width;
    lstlyr.src_win.height   = rect->height;
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe = pipe;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}

__s32 movie_ff_rr_show_scene_update_string(void *handle, movie_ff_rr_show_info_t *show_info)
{
    char ff_rr_str[MAX_STRING_LEN];
    movie_ff_rr_show_scene_t *scene_para = (movie_ff_rr_show_scene_t *)handle;
    __wrn("[movie_ff_rr_show_scene_update_string] called");

    if (NULL == handle)
    {
        __err("[movie_ff_rr_show_scene_update_string] invalid handle..");
        return EPDK_FAIL;
    }

    if (NULL == show_info)
    {
        __err("[movie_ff_rr_show_scene_update_string] invalid show_info..");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("[movie_ff_rr_show_scene_update_string] invalid hfrm..");
        return EPDK_FAIL;
    }

    eLIBs_memset(ff_rr_str, 0, sizeof(ff_rr_str));
    dsk_langres_get_menu_text(show_info->text_id, ff_rr_str, MAX_STRING_LEN);

    switch (show_info->ff_rr_speed)
    {
        case FF_RR_SHOW_SPEED_1:
            strcat(ff_rr_str, " X8");
            break;

        case FF_RR_SHOW_SPEED_2:
            strcat(ff_rr_str, " X16");
            break;

        case FF_RR_SHOW_SPEED_3:
            strcat(ff_rr_str, " X32");
            break;

        case FF_RR_SHOW_SPEED_4:
            strcat(ff_rr_str, " X64");
            break;

        default:
            break;
    }

    {
        __gui_msg_t msg;
        eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));
        msg.h_deswin = scene_para->hfrm;
        msg.id = GUI_MSG_PAINT;
        msg.dwAddData1 = (unsigned long)ff_rr_str;
        GUI_SendMessage(&msg);
    }

    return EPDK_OK;
}

static __s32 __ff_rr_show_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            movie_ff_rr_show_scene_t *scene_para;
            __wrn("__ff_rr_show_proc GUI_MSG_CREATE begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            scene_para->hfrm = msg->h_deswin;//必须在此初始化，因为窗口创建未返回，否则该值仍然是空
            __wrn("__ff_rr_show_proc GUI_MSG_CREATE end");
            return EPDK_OK;
        }

        case GUI_MSG_DESTROY:
        {
            movie_ff_rr_show_scene_t *scene_para;
            __wrn("__ff_rr_show_proc GUI_MSG_DESTROY begin");
            scene_para = GUI_WinGetAttr(msg->h_deswin);

            if (!scene_para)
            {
                __err("invalid para...");
                return EPDK_FAIL;
            }

            esMEMS_Mfree(0, scene_para);
            scene_para = NULL;
            __wrn("__ff_rr_show_proc GUI_MSG_DESTROY end");
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            __wrn("__ff_rr_show_proc GUI_MSG_KEY begin");
            __wrn("__ff_rr_show_proc GUI_MSG_KEY end");
            return EPDK_OK;
        }

        case GUI_MSG_TOUCH:
        {
            __wrn("__ff_rr_show_proc GUI_MSG_TOUCH begin");
            __wrn("__ff_rr_show_proc GUI_MSG_TOUCH end");
            return EPDK_OK;
        }

        case GUI_MSG_PAINT:
        {
            movie_ff_rr_show_uipara_t *ui_para = NULL;
            __wrn("__ff_rr_show_proc GUI_MSG_PAINT begin");
            GUI_LyrWinSel(GUI_WinGetLyrWin(msg->h_deswin));
            ui_para = movie_ff_rr_show_get_uipara(GUI_GetScnDir());
            GUI_SetBkColor(APP_COLOR_BLACK);
            GUI_ClearRect(ui_para->uipara_lyr.x, ui_para->uipara_lyr.y, ui_para->uipara_lyr.w, ui_para->uipara_lyr.h);
            /* draw dailog content */
            GUI_SetFont(SWFFont);
            GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
            GUI_SetColor(APP_COLOR_WHITE);
            GUI_CharSetToEncode(dsk_get_langres_charset());
            GUI_DispStringAt((char *)msg->dwAddData1, ui_para->uipara_lyr.x, ui_para->uipara_lyr.y);
            __wrn("__ff_rr_show_proc GUI_MSG_PAINT end");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __wrn("__ff_rr_show_proc GUI_MSG_CLOSE begin");
            __wrn("__ff_rr_show_proc GUI_MSG_CLOSE end");
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
        {
            __wrn("__ff_rr_show_proc GUI_MSG_CLOSE begin");
            __wrn("__ff_rr_show_proc GUI_MSG_CLOSE end");
            break;
        }

        default:
        {
            break;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}

void *movie_ff_rr_show_scene_create(movie_ff_rr_show_create_para_t *create_para)
{
    __gui_framewincreate_para_t framewin_para;
    movie_ff_rr_show_scene_t *sence_para;
    movie_ff_rr_show_uipara_t *ui_para;
    RECT lyr_rect;
    __wrn("[movie_ff_rr_show_scene_create] called");
    sence_para = (movie_ff_rr_show_scene_t *)esMEMS_Malloc(0, sizeof(movie_ff_rr_show_scene_t));

    if (!sence_para)
    {
        __wrn("mem not enough...");
        return NULL;
    }

    eLIBs_memset(sence_para, 0, sizeof(movie_ff_rr_show_scene_t));
    sence_para->hparent = create_para->hparent;
    sence_para->scene_id = create_para->scene_id;
    sence_para->sub_state = create_para->sub_state;
    ui_para = movie_ff_rr_show_get_uipara(GUI_GetScnDir());

    if (NULL == ui_para)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __wrn("movie_ff_rr_show_get_uipara fail...");
        return NULL;
    }

    lyr_rect.x      = ui_para->uipara_lyr.x;
    lyr_rect.y      = ui_para->uipara_lyr.y;
    lyr_rect.width  = ui_para->uipara_lyr.w;
    lyr_rect.height = ui_para->uipara_lyr.h;
    sence_para->hlyr = __ff_rr_show_32bpp_layer_create(&lyr_rect, ui_para->pipe);

    if (NULL == sence_para->hlyr)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __wrn("sence_para->hlyr is null...");
        return NULL;
    }

    GUI_LyrWinSetSta(sence_para->hlyr, GUI_LYRWIN_STA_ON);
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name          = "movie_ff_rr_show_win",
    framewin_para.dwExStyle     = WS_EX_NONE;
    framewin_para.dwStyle       = WS_NONE | WS_VISIBLE | WS_DISABLED;
    framewin_para.spCaption     = NULL;
    framewin_para.hOwner        = NULL;
    framewin_para.id            = sence_para->scene_id;
    framewin_para.hHosting      = sence_para->hparent;
    framewin_para.FrameWinProc  = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)__ff_rr_show_proc);
    framewin_para.rect.x        = ui_para->uipara_frm.x;
    framewin_para.rect.y        = ui_para->uipara_frm.y;
    framewin_para.rect.width    = ui_para->uipara_frm.w;
    framewin_para.rect.height   = ui_para->uipara_frm.h;
    framewin_para.BkColor.alpha = 0;
    framewin_para.BkColor.red   = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue  = 0;
    framewin_para.attr          = (void *)sence_para;
    framewin_para.hLayer        = sence_para->hlyr;
    sence_para->hfrm = GUI_FrmWinCreate(&framewin_para);

    if (NULL == sence_para->hfrm)
    {
        esMEMS_Mfree(0, sence_para);
        sence_para = NULL;
        __wrn("GUI_FrmWinCreate fail...");
        return NULL;
    }

    if (sence_para->hlyr)
    {
        GUI_LyrWinSetTop(sence_para->hlyr);
    }
    else
    {
        __err("invalid para...");
    }

    __wrn("[movie_ff_rr_show_scene_create] end");
    return sence_para;
}

__s32 movie_ff_rr_show_scene_delete(void *handle)
{
    movie_ff_rr_show_scene_t *scene_para;
    scene_para = (movie_ff_rr_show_scene_t *)handle;

    if (NULL == handle)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    if (!scene_para->hfrm)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_FrmWinDelete(scene_para->hfrm);

    if (!scene_para->hlyr)
    {
        __err("invalid para..");
        return EPDK_FAIL;
    }

    GUI_LyrWinDelete(scene_para->hlyr);
    eLIBs_memset(scene_para, 0, sizeof(movie_ff_rr_show_scene_t));//防止重复释放
    esMEMS_Mfree(0, scene_para);
    return EPDK_OK;
}

