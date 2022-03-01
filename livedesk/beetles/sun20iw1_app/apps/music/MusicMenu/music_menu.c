/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2010, ANDY, China
*                                            All Rights Reserved
*
* File      : main_menu.c
* By        : Andy.zhang
* Func      : desk main thread
* Version   : v1.0
* ============================================================================================================
* 2009-7-20 8:51:52  andy.zhang  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include <log.h>
#include "music.h"
#include "music_menu.h"
//#define MAX_DIGITAL_SONG  BEETLES_TBL_SIZE(digital_song_800_480)

#define MUSIC_MENU_X   (0)
#define MUSIC_MENU_Y   (0)
#define MUSIC_MENU_W   (246)
#define MUSIC_MENU_H   (54)

#define MUSIC_MENU_FRM_X  (425)
#define MUSIC_MENU_FRM_Y  (46)
#define MUSIC_MENU_FRM_W   MUSIC_MENU_W
#define MUSIC_MENU_FRM_H   MUSIC_MENU_H*8

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

static H_WIN h_music_info;
#ifdef MUSIC_VIEW_CHANGE_20160811
static music_menu_AB_mode_e ab_mode_status = MUSIC_MENU_AB;
#endif
music_menu_ctrl_t       *g_music_menu_ctrl = NULL;

static __u32 muisc_auto_delete_menu_id = 0x11;  // 10 s  内无动作自动关闭弹出的菜单



static __s32  main_menu_lang[MUSIC_MAIN_MENU_MAX] =
{
    STRING_MUSIC_EQ,
    STRING_MUSIC_REPEAT,
    STRING_MUSIC_SWITCH_DISK,
#ifndef MUSIC_VIEW_CHANGE_20160811
    STRING_MUSIC_PLAY_PREVIOUS,
    STRING_MUSIC_PLAY_NEXT,
    STRING_MUSIC_PLAY_PAUSE,
#else
    STRING_MUSIC_AB,
#endif
    STRING_MUSIC_ARTIST_INFO,
    STRING_MUSIC_RETURN,
};

static __s32  eq_menu_lang[MUSIC_SUB_MENU_EQ_MAX] =
{
    STRING_MUSIC_EQ_NORMAL,
    STRING_MUSIC_EQ_DDB,
    STRING_MUSIC_EQ_JAZZ,
    STRING_MUSIC_EQ_ROCK,
    STRING_MUSIC_EQ_POP,
    STRING_MUSIC_EQ_CLASSIC,
    STRING_MUSIC_EQ_VACAL,
    STRING_MUSIC_RETURN,
};

static __s32  repeat_menu_lang[MUSIC_SUB_MENU_REPEAT_MAX] =
{
    STRING_MUSIC_PLAY_ONCE,
    STRING_MUSIC_PLAY_ROTATE_ONE,
    STRING_MUSIC_PLAY_ROTATE_ALL,
    STRING_MUSIC_PLAY_ALL,
    STRING_MUSIC_PLAY_RANDOM,
    STRING_MUSIC_RETURN,
};
__s32 music_get_ab_status(void)
{
    return ab_mode_status;
}
void music_set_ab_status(__s32 ab_mode)
{
    if (ab_mode <= MUSIC_MENU_B && ab_mode >= MUSIC_MENU_AB)
    {
        ab_mode_status = ab_mode;
    }

    return ;
}

__s32  music_menu_check_disk(__s32 root_type)
{
    __s32   ret = EPDK_FALSE, ret1 = EPDK_FAIL;
    __s32 len = 0 ;
    char disk_name[RAT_MAX_PARTITION][4] = {{0}, {NULL}};

    if (RAT_TF == root_type)
    {
        ret1  = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
        len = eLIBs_strlen(disk_name[0]);

        if ((EPDK_OK == ret1) && (len != 0))
        {
            ret = EPDK_TRUE ;
        }
    }

    if (RAT_USB == root_type)
    {
        ret1 = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
        len = eLIBs_strlen(disk_name[0]);

        if ((EPDK_OK == ret1) && (len != 0))
        {
            ret = EPDK_TRUE  ;
        }
    }

    return ret ;
}

void  music_menu_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    msg.h_deswin = GUI_WinGetParent(hwin);
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    //msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData1 = data1;
    msg.dwAddData2 = data2;
    msg.dwReserved = NULL;
    GUI_SendNotifyMessage(&msg);
}
//发送一个命令参数给主界面
__s32 music_menu_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128] = {0};
    H_WIN hDstWin;
    H_WIN hManagerWin;
    hManagerWin = GUI_WinGetParent(hwin);
    hDstWin = GUI_WinGetParent(hManagerWin);
    msg.h_deswin = hDstWin;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hManagerWin), id);//
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_WinGetName(msg.h_deswin, buf);
    __msg("%s", buf);
    GUI_SendNotifyMessage(&msg);
    return EPDK_OK;
}

__s32 music_menu_set_screen_rect(RECT *rect, music_menu_ctrl_t  *self)
{
    __s32 ret;
    //{
    //  RECT *rect1;
    ////__here__;
    //  GUI_LyrWinGetScnWindow(self->layer, rect1);
    //  //__here__;
    //__msg(" rect1->x = %d, rect1->y= %d, rect1->width=%d,rect1->height = %d",
    //      rect1->x, rect1->y, rect1->width,rect1->height);
    //}
    GUI_LyrWinCacheOn();
    ////__here__;
    ////__here__;
#if 0               //this part is blocked, for temporarily solving bugs of PMS41007 and PMS41013
    ret = GUI_LyrWinSetScnWindow(self->layer, rect);

    ////__here__;
    if (ret != EPDK_OK)
    {
        __wrn("layer set src windows failed,layer handle = %d", self->layer);
        return __LINE__;
    }

#endif
    ////__here__;
    GUI_LyrWinCacheOff();
    //{
    //RECT *rect1;
    //  //__here__;
    //GUI_LyrWinGetScnWindow(self->layer, rect1);
    //  //__here__;
    //__msg(" rect1->x = %d, rect1->y= %d, rect1->width=%d,rect1->height = %d",
    //      rect1->x, rect->y, rect1->width,rect1->height);
    //}
    return EPDK_OK;
}

__s32 init_music_menu_res(music_menu_ctrl_t *self)
{
    self->h_menu_unfcs = dsk_theme_open(ID_MUSIC_MENU_UNFCS_BMP);
    self->h_menu_fcs = dsk_theme_open(ID_MUSIC_MENU_FCS_BMP);
    self->h_menuback = dsk_theme_open(ID_MUSIC_MENU_BG_BMP);
    self->h_sel = dsk_theme_open(ID_MUSIC_MENU_SEL_BMP);
    self->h_unsel = dsk_theme_open(ID_MUSIC_MENU_UNSEL_BMP);
    return 0;
}

__s32 uninit_music_menu_res(music_menu_ctrl_t       *self)
{
    if (self->h_menu_unfcs)
    {
        dsk_theme_close(self->h_menu_unfcs);
        self->h_menu_unfcs = NULL;
    }

    if (self->h_menu_fcs)
    {
        dsk_theme_close(self->h_menu_fcs);
        self->h_menu_fcs = NULL;
    }

    if (self->h_menuback)
    {
        dsk_theme_close(self->h_menuback);
        self->h_menuback = NULL;
    }

    if (self->h_sel)
    {
        dsk_theme_close(self->h_sel);
        self->h_sel = NULL;
    }

    if (self->h_unsel)
    {
        dsk_theme_close(self->h_unsel);
        self->h_unsel = NULL;
    }

    return 0;
}



void draw_music_menu_btn(music_menu_ctrl_t  *self, __s32 index, music_menu_status_e status)
{
    void  *pbmp = NULL;
    GUI_RECT text_rect;
    text_rect.x0 = MUSIC_MENU_X;
    text_rect.y0 = MUSIC_MENU_H * index;
    text_rect.x1 = text_rect.x0 + MUSIC_MENU_W;
    text_rect.y1 = text_rect.y0 + MUSIC_MENU_H;

    if (status == MENU_UNFOCUSED)
    {
        __msg("self->h_menu_unfcs = %x", self->h_menu_unfcs);

        if (self->h_menu_unfcs != NULL)
        {
            pbmp = dsk_theme_hdl2buf(self->h_menu_unfcs);
            GUI_BMP_Draw(pbmp, text_rect.x0, text_rect.y0);
            return;
        }
    }
    else if (status == MENU_FOCUSED || status == MENU_PRESSED)
    {
        __msg("self->h_menu_fcs = %x", self->h_menu_fcs);

        if (self->h_menu_fcs != NULL)
        {
            pbmp = dsk_theme_hdl2buf(self->h_menu_fcs);
            GUI_BMP_Draw(pbmp, text_rect.x0, text_rect.y0);
            return;
        }
    }
    else
    {
        pbmp = dsk_theme_hdl2buf(self->h_menuback);
        GUI_BMP_Draw(pbmp, text_rect.x0, text_rect.y0);
        __msg(" status is faild");
    }
}
#ifdef MUSIC_VIEW_CHANGE_20160811
void music_menu_set_ab_mode(void)
{
    switch (ab_mode_status)
    {
        case MUSIC_MENU_AB:
        {
            ab_mode_status = MUSIC_MENU_A;
            robin_set_ab_a_v2();
            robin_set_ab_loop_count_v2(1000);
        }
        break;

        case MUSIC_MENU_A:
        {
            ab_mode_status = MUSIC_MENU_B;
            robin_set_ab_b_v2();
            robin_enable_ab();
        }
        break;

        case MUSIC_MENU_B:
        {
            ab_mode_status = MUSIC_MENU_AB;
            robin_cancle_ab_v2();
        }
        break;

        default:
            break;
    }
}
#endif
void draw_music_main_menu_string(music_menu_ctrl_t  *self, __s32 index)
{
    GUI_RECT text_rect;
    char TextBuf[64];
    __cedar_status_t Play_Status = CEDAR_STAT_;
    eLIBs_memset(TextBuf, 0, sizeof(TextBuf));
    text_rect.x0 = MUSIC_MENU_X;
    text_rect.y0 = MUSIC_MENU_H * index;
    text_rect.x1 = text_rect.x0 + MUSIC_MENU_W;
    text_rect.y1 = text_rect.y0 + MUSIC_MENU_H;
    GUI_SetColor(GUI_BLACK);
#ifndef MUSIC_VIEW_CHANGE_20160811
    __wrn("path pl");

    if (index == MUSIC_MAIN_MENU_PLAY)
    {
        Play_Status = robin_get_fsm_status();

        if (Play_Status != CEDAR_STAT_STOP && Play_Status != CEDAR_STAT_PAUSE)
        {
            get_menu_text(STRING_MOVIE_PAUSE, TextBuf, sizeof(TextBuf));
        }
        else
        {
            get_menu_text(STRING_MOVIE_PLAY, TextBuf, sizeof(TextBuf));
        }
    }
    else
#else
    __wrn("path ab");

    if (index == MUSIC_MAIN_MENU_AB)
    {
        if (ab_mode_status == MUSIC_MENU_AB)
        {
            get_menu_text(STRING_MUSIC_AB, TextBuf, sizeof(TextBuf));
        }
        else if (ab_mode_status == MUSIC_MENU_A)
        {
            get_menu_text(STRING_MUSIC_A, TextBuf, sizeof(TextBuf));
        }
        else if (ab_mode_status == MUSIC_MENU_B)
        {
            get_menu_text(STRING_MUSIC_B, TextBuf, sizeof(TextBuf));
        }
    }
    else
#endif
    {
        if (index == MUSIC_MAIN_MENU_TF_USB)
        {
            __wrn("self->root_type:%d", self->root_type);

            if (self->root_type == RAT_USB)
            {
                get_menu_text(STRING_MUSIC_SWITCH_TO_TF, TextBuf, sizeof(TextBuf));
            }
            else
            {
                get_menu_text(STRING_MUSIC_SWITCH_TO_USB, TextBuf, sizeof(TextBuf));
            }
        }
        else
        {
            get_menu_text(main_menu_lang[index], TextBuf, sizeof(TextBuf));
        }
    }

    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    return;
}

void draw_music_sub_menu_string(music_menu_ctrl_t  *self, __s32 main_index, __s32 index)
{
    GUI_RECT text_rect;
    char TextBuf[64];
    eLIBs_memset(TextBuf, 0, sizeof(TextBuf));
    text_rect.x0 = MUSIC_MENU_X;
    text_rect.y0 = MUSIC_MENU_H * index;
    text_rect.x1 = text_rect.x0 + MUSIC_MENU_W;
    text_rect.y1 = text_rect.y0 + MUSIC_MENU_H;
    GUI_SetBkColor(0);
    GUI_SetColor(GUI_BLACK);

    //__msg("text_rect.x0 = %d",text_rect.x0);
    //__msg("text_rect.y0 = %d",text_rect.y0);
    //__msg("text_rect.x1 = %d",text_rect.x1);
    //__msg("text_rect.y1 = %d",text_rect.y1);
    if (main_index == MUSIC_MAIN_MENU_EQ)
    {
        get_menu_text(eq_menu_lang[index], TextBuf, sizeof(TextBuf));
    }
    else if (main_index == MUSIC_MAIN_MENU_REPEAT)
    {
        get_menu_text(repeat_menu_lang[index], TextBuf, sizeof(TextBuf));
    }

    GUI_DispStringInRect(TextBuf, &text_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
}

//已选对勾
static void draw_music_sub_menu_select(music_menu_ctrl_t  *self, __s32 index, __s32 foused)
{
    void  *pbmp = NULL;
    GUI_RECT text_rect;
    text_rect.x0 = MUSIC_MENU_W - 42;
    text_rect.y0 = MUSIC_MENU_H * index;

    if (foused == MENU_FOCUSED)
    {
        pbmp = dsk_theme_hdl2buf(self->h_sel);
        GUI_BMP_Draw(pbmp, text_rect.x0, text_rect.y0);
    }
    else
    {
        pbmp = dsk_theme_hdl2buf(self->h_unsel);
        GUI_BMP_Draw(pbmp, text_rect.x0, text_rect.y0);
    }
}




/*
************************************************************************************************************************
*Function   :          static H_LYR music_32bpp_layer_create(__u8 pipe)
*
*Description    :       前景图层，主要用来显示前景图片，ListBar组件item, 缩略图
*                   注意与背景图应处于不同的pipe
*
*Arguments      :
*
*Return         :        图层句柄
*
************************************************************************************************************************
*/
static H_LYR music_menu_32bpp_layer_create(__u8 pipe)
{
    H_LYR layer = NULL;
    RECT rect;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        1,                                              /* pipe      */
        3,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                             /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_MUSIC_MENU",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    //dsk_display_get_size(&rect.width,&rect.height);
    fb.size.width       = MUSIC_MENU_FRM_W;
    fb.size.height      = MUSIC_MENU_FRM_H;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width    = MUSIC_MENU_FRM_W;
    lstlyr.src_win.height   = MUSIC_MENU_FRM_H;
    dsk_display_get_size(&rect.width, &rect.height);

    if (rect.width == 800 && rect.height == 480)
    {
        lstlyr.scn_win.x        = MUSIC_MENU_FRM_X;//MUSIC_MENU_X;
    }
    else//720 * 480
    {
        lstlyr.scn_win.x        = MUSIC_MENU_FRM_X - 70; //80;//MUSIC_MENU_X;  //chengf
    }

    lstlyr.scn_win.y        = MUSIC_MENU_FRM_Y;//MUSIC_MENU_Y;
    lstlyr.scn_win.width    = MUSIC_MENU_FRM_W;
    lstlyr.scn_win.height   = MUSIC_MENU_FRM_H;
    lstlyr.pipe = pipe;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app music layer create error !");
    }

    return layer;
}



static __s32  music_menu_frm_key_up(__gui_msg_t *msg)
{
    music_menu_ctrl_t       *music_menu_ctrl;
    music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    GUI_LyrWinSel(music_menu_ctrl->layer);
    GUI_SetFont(music_menu_ctrl->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    __msg("music_menu_ctrl->menu_flag = %d", music_menu_ctrl->menu_flag);

    if (music_menu_ctrl->menu_flag == MUSIC_MAIN_MENU)
    {
        draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->main_menu_index, MENU_UNFOCUSED);
        draw_music_main_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index);

        if (music_menu_ctrl->main_menu_index <= 0)
        {
            music_menu_ctrl->main_menu_index = MUSIC_MAIN_MENU_MAX - 1;
        }
        else
        {
            music_menu_ctrl->main_menu_index--;
        }

        __msg("music_menu_ctrl->main_menu_index = %d", music_menu_ctrl->main_menu_index);
        draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->main_menu_index, MENU_FOCUSED);
        draw_music_main_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index);
    }
    else if (music_menu_ctrl->menu_flag == MUSIC_SUB_MENU)
    {
        if (music_menu_ctrl->main_menu_index == MUSIC_MAIN_MENU_EQ)
        {
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->eq_menu_index, MENU_UNFOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->eq_menu_index);
            draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->eq_sel_index, MENU_UNFOCUSED);

            if (music_menu_ctrl->eq_menu_index <= 0)
            {
                music_menu_ctrl->eq_menu_index = MUSIC_SUB_MENU_EQ_MAX - 1;
            }
            else
            {
                music_menu_ctrl->eq_menu_index--;
            }

            __msg("music_menu_ctrl->sub_menu_index = %d", music_menu_ctrl->eq_menu_index);
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->eq_menu_index, MENU_FOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->eq_menu_index);

            if (music_menu_ctrl->eq_menu_index == music_menu_ctrl->eq_sel_index)
            {
                draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->eq_menu_index, MENU_FOCUSED);
            }
        }
        else if (music_menu_ctrl->main_menu_index == MUSIC_MAIN_MENU_REPEAT)
        {
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->playmode_menu_index, MENU_UNFOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->playmode_menu_index);
            draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->playmode_sel_index, MENU_UNFOCUSED);

            if (music_menu_ctrl->playmode_menu_index <= 0)
            {
                music_menu_ctrl->playmode_menu_index = MUSIC_SUB_MENU_REPEAT_MAX - 1;
            }
            else
            {
                music_menu_ctrl->playmode_menu_index--;
            }

            __msg("music_menu_ctrl->playmode_menu_index = %d", music_menu_ctrl->playmode_menu_index);
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->playmode_menu_index, MENU_FOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->playmode_menu_index);

            if (music_menu_ctrl->playmode_menu_index == music_menu_ctrl->playmode_sel_index)
            {
                draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->playmode_menu_index, MENU_FOCUSED);
            }
        }
    }

    //__here__;
    return EPDK_OK;
}

static __s32  music_menu_frm_key_down(__gui_msg_t *msg)
{
    music_menu_ctrl_t       *music_menu_ctrl;
    music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    GUI_LyrWinSel(music_menu_ctrl->layer);
    GUI_SetFont(music_menu_ctrl->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0);
    __wrn("music_menu_ctrl->menu_flag = %d", music_menu_ctrl->menu_flag);

    if (music_menu_ctrl->menu_flag == MUSIC_MAIN_MENU)
    {
        __wrn("++++++++++ MUSIC_MAIN_MENU");
        draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->main_menu_index, MENU_UNFOCUSED);
        draw_music_main_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index);

        if (music_menu_ctrl->main_menu_index >= MUSIC_MAIN_MENU_MAX - 1)
        {
            music_menu_ctrl->main_menu_index = 0;
        }
        else
        {
            music_menu_ctrl->main_menu_index++;
        }

        __wrn("music_menu_ctrl->main_menu_index = %d", music_menu_ctrl->main_menu_index);
        draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->main_menu_index, MENU_FOCUSED);
        draw_music_main_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index);
    }
    else if (music_menu_ctrl->menu_flag == MUSIC_SUB_MENU)
    {
        __wrn("++++++++++ main_menu_index = %d", music_menu_ctrl->main_menu_index);

        if (music_menu_ctrl->main_menu_index == MUSIC_MAIN_MENU_EQ)
        {
            __wrn("++++++++++ MUSIC_MAIN_MENU_EQ");
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->eq_menu_index, MENU_UNFOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->eq_menu_index);
            draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->eq_sel_index, MENU_UNFOCUSED);

            if (music_menu_ctrl->eq_menu_index >= MUSIC_SUB_MENU_EQ_MAX - 1)
            {
                music_menu_ctrl->eq_menu_index = 0;
            }
            else
            {
                music_menu_ctrl->eq_menu_index++;
            }

            __wrn("music_menu_ctrl->sub_menu_index = %d", music_menu_ctrl->eq_menu_index);
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->eq_menu_index, MENU_FOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->eq_menu_index);

            if (music_menu_ctrl->eq_menu_index == music_menu_ctrl->eq_sel_index)
            {
                draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->eq_menu_index, MENU_FOCUSED);
            }
        }
        else if (music_menu_ctrl->main_menu_index == MUSIC_MAIN_MENU_REPEAT)
        {
            __wrn("++++++++++ MUSIC_MAIN_MENU_REPEAT");
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->playmode_menu_index, MENU_UNFOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->playmode_menu_index);
            draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->playmode_sel_index, MENU_UNFOCUSED);

            if (music_menu_ctrl->playmode_menu_index >= MUSIC_SUB_MENU_REPEAT_MAX - 1)
            {
                music_menu_ctrl->playmode_menu_index = 0;
            }
            else
            {
                music_menu_ctrl->playmode_menu_index++;
            }

            __msg("music_menu_ctrl->playmode_menu_index = %d", music_menu_ctrl->playmode_menu_index);
            draw_music_menu_btn(music_menu_ctrl, music_menu_ctrl->playmode_menu_index, MENU_FOCUSED);
            draw_music_sub_menu_string(music_menu_ctrl, music_menu_ctrl->main_menu_index, music_menu_ctrl->playmode_menu_index);

            if (music_menu_ctrl->playmode_menu_index == music_menu_ctrl->playmode_sel_index)
            {
                draw_music_sub_menu_select(music_menu_ctrl, music_menu_ctrl->playmode_menu_index, MENU_FOCUSED);
            }
        }
    }

    //__here__;
    return EPDK_OK;
}

static __s32  music_menu_frm_key_mainmenu(music_menu_ctrl_t *self)
{
    __s32     i = 0;
    RECT      rect;
    __msg("self->menu_flag = %d", self->menu_flag);

    if (self->main_menu_index == MUSIC_MAIN_MENU_EQ)
    {
        __msg("+++++++MUSIC_MAIN_MENU_EQ+++++++++");
        self->menu_flag = MUSIC_SUB_MENU;
        self->eq_menu_index = self->eq_sel_index;
        dsk_display_get_size(&rect.width, &rect.height);

        if (rect.width == 800 && rect.height == 480)
        {
            rect.x = MUSIC_MENU_FRM_X;//MUSIC_MENU_X;
        }
        else//720 * 480
        {
            rect.x = MUSIC_MENU_FRM_X - 80;//MUSIC_MENU_X;
        }

        rect.y = MUSIC_MENU_FRM_Y;
        rect.width = MUSIC_MENU_FRM_W;
        rect.height = MUSIC_MENU_FRM_H;
        music_menu_set_screen_rect(&rect, self);
        GUI_LyrWinSetBottom(self->layer);
        com_memdev_create(self->layer);
        GUI_LyrWinSel(self->layer);
        GUI_SetFont(self->font);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetBkColor(0);

        //GUI_ClearRect(MUSIC_MENU_X, MUSIC_MENU_Y, MUSIC_MENU_FRM_W, MUSIC_MENU_FRM_H);
        for (i = 0; i < MUSIC_SUB_MENU_EQ_MAX; i++)
        {
            if (self->eq_menu_index == i)
            {
                draw_music_menu_btn(self, i, MENU_FOCUSED);
            }
            else
            {
                draw_music_menu_btn(self, i, MENU_UNFOCUSED);
            }

            draw_music_sub_menu_string(self, self->main_menu_index, i);
        }

        draw_music_sub_menu_select(self, self->eq_sel_index, MENU_FOCUSED);
        com_memdev_delete();
        GUI_LyrWinSetTop(self->layer);
    }
    else if (self->main_menu_index == MUSIC_MAIN_MENU_REPEAT)
    {
        __msg("+++++++MUSIC_MAIN_MENU_REPEAT+++++++++");
        self->menu_flag = MUSIC_SUB_MENU;
        self->playmode_menu_index = self->playmode_sel_index;
        //GUI_ClearRect(MUSIC_MENU_X, MUSIC_MENU_Y, MUSIC_MENU_FRM_W, MUSIC_MENU_FRM_H);
        //GUI_LyrWinSetSta(self->layer, GUI_LYRWIN_STA_OFF);
        dsk_display_get_size(&rect.width, &rect.height);

        if (rect.width == 800 && rect.height == 480)
        {
            rect.x = MUSIC_MENU_FRM_X;//MUSIC_MENU_X;
        }
        else//720 * 480
        {
            rect.x = MUSIC_MENU_FRM_X - 80;//MUSIC_MENU_X;
        }

        //rect.x        = MUSIC_MENU_FRM_X;
        rect.width = MUSIC_MENU_FRM_W;
        rect.height = MUSIC_MENU_H * MUSIC_SUB_MENU_REPEAT_MAX;
        rect.y = MUSIC_MENU_FRM_Y + (MUSIC_MENU_FRM_H - rect.height) / 2;
        music_menu_set_screen_rect(&rect, self);
        GUI_LyrWinSetBottom(self->layer);
        com_memdev_create(self->layer);
        GUI_LyrWinSel(self->layer);
        GUI_SetFont(self->font);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        GUI_SetBkColor(0);

        for (i = 0; i < MUSIC_MAIN_MENU_MAX; i++)
        {
            if (i < MUSIC_SUB_MENU_REPEAT_MAX)
            {
                if (self->playmode_menu_index == i)
                {
                    draw_music_menu_btn(self, i, MENU_FOCUSED);
                }
                else
                {
                    draw_music_menu_btn(self, i, MENU_UNFOCUSED);
                }

                draw_music_sub_menu_string(self, self->main_menu_index, i);
                continue;
            }
            else
            {
                draw_music_menu_btn(self, i, MENU_UNFOCUSED);
            }

            //__here__;
        }

        draw_music_sub_menu_select(self, self->playmode_sel_index, MENU_FOCUSED);
        com_memdev_delete();
        //GUI_LyrWinSetSta(self->layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(self->layer);
    }
    else if (self->main_menu_index == MUSIC_MAIN_MENU_TF_USB)
    {
        __u8    redraw_need = EPDK_FALSE;
        __msg("+++++++MUSIC_MAIN_MENU_TF_USB+++++++++");

        if (self->root_type == RAT_TF)
        {
            __wrn("RAT_TF Switch RAT_USB");

            if (music_menu_check_disk(RAT_USB) == EPDK_TRUE)
            {
                redraw_need = EPDK_TRUE;
                self->root_type = RAT_USB;
                music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_SWITCH_DISK, RAT_TF_SWITCH_TO_USB);
            }
            else
            {
                //haven't usb device
                __wrn("haven't usb device");
                music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_SWITCH_DISK, RAT_NOT_FOUND_USB);
            }
        }
        else if (self->root_type == RAT_USB)
        {
            __wrn("RAT_USB Switch RAT_TF");

            if (music_menu_check_disk(RAT_TF) == EPDK_TRUE)
            {
                redraw_need = EPDK_TRUE;
                self->root_type = RAT_TF;
                music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_SWITCH_DISK, RAT_USB_SWITCH_TO_TF);
            }
            else
            {
                //haven't TF Card
                /*music_haven't_usb_device*/
                __wrn("haven't aven't TF Card");
                music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_SWITCH_DISK, RAT_NOT_FOUND_TF);
            }
        }

        if (redraw_need)
        {
            com_memdev_create(self->layer);
            GUI_LyrWinSel(self->layer);
            GUI_SetFont(self->font);
            GUI_UC_SetEncodeUTF8();
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            draw_music_menu_btn(self, MUSIC_MAIN_MENU_TF_USB, MENU_FOCUSED);
            draw_music_main_menu_string(self, MUSIC_MAIN_MENU_TF_USB);
            GUI_LyrWinSetTop(self->layer);
            com_memdev_delete();
        }
    }

#ifndef MUSIC_VIEW_CHANGE_20160811
    else if (self->main_menu_index == MUSIC_MAIN_MENU_PRE)
    {
        __msg("+++++++MUSIC_MAIN_MENU_PRE+++++++++");
        music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_PRE, 0);
    }
    else if (self->main_menu_index == MUSIC_MAIN_MENU_NEXT)
    {
        __msg("+++++++MUSIC_MAIN_MENU_NEXT+++++++++");
        music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_NEXT, 0);
    }
    else if (self->main_menu_index == MUSIC_MAIN_MENU_PLAY)
    {
        __msg("+++++++MUSIC_MAIN_MENU_PLAY+++++++++");

        if (robin_get_fsm_status() == CEDAR_STAT_PLAY)
        {
            robin_set_cmd_pause();
            gscene_hbar_set_music_state(BG_PAUSE);
        }
        else
        {
            robin_set_cmd_play();
            gscene_hbar_set_music_state(BG_MUSIC);
        }

        com_memdev_create(self->layer);
        GUI_LyrWinSel(self->layer);
        GUI_SetFont(self->font);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        draw_music_menu_btn(self, MUSIC_MAIN_MENU_PLAY, MENU_FOCUSED);
        draw_music_main_menu_string(self, MUSIC_MAIN_MENU_PLAY);
        GUI_LyrWinSetTop(self->layer);
        com_memdev_delete();
    }

#else
    else if (self->main_menu_index == MUSIC_MAIN_MENU_AB)
    {
        music_menu_set_ab_mode();
        com_memdev_create(self->layer);
        GUI_LyrWinSel(self->layer);
        GUI_SetFont(self->font);
        GUI_UC_SetEncodeUTF8();
        GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
        draw_music_menu_btn(self, MUSIC_MAIN_MENU_AB, MENU_FOCUSED);
        draw_music_main_menu_string(self, MUSIC_MAIN_MENU_AB);
        GUI_LyrWinSetTop(self->layer);
        com_memdev_delete();
    }

#endif
    else if (self->main_menu_index == MUSIC_MAIN_MENU_ARTIST_INFO)
    {
        char pstr[RAT_MAX_FULL_PATH_LEN] = {0};
        char *str_tmp = NULL;
        char cur_playing_file[256] = {0};
        robin_audio_info_t audio_file_info;
        __u32   filesize = {0};
        char str_name_tmp[32] = {0};
        char str_filesize[64] = {0};
        __msg("+++++++MUSIC_MAIN_MENU_ARTIST_INFO+++++++++");
        eLIBs_memset(&audio_file_info, 0, sizeof(robin_audio_info_t));
        robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
        __msg(" cur_playing_file=%s",  cur_playing_file);
        str_tmp = eLIBs_strchrlast(cur_playing_file, '\\');
        str_tmp++;
        __msg("str_tmp = %s", str_tmp);
        filesize = eLIBs_GetFileSize(cur_playing_file);
        robin_get_audio_info(cur_playing_file, &audio_file_info);
        filesize2str(filesize, str_filesize);
        __msg("filesize = %s", str_filesize);
        __msg("audio_file_info.artist = %s ", audio_file_info.artist);
        __msg("audio_file_info.album = %s ", audio_file_info.album);
        //eLIBs_strcpy(pstr, "Name:");
        //eLIBs_strcat(pstr, str_tmp);
        eLIBs_strcat(pstr, "");
        eLIBs_strcat(pstr, "FileSize:");
        eLIBs_strcat(pstr, str_filesize);
        eLIBs_strcat(pstr, "");
        eLIBs_strcat(pstr, "Artist:");
        eLIBs_strcat(pstr, audio_file_info.artist);
        eLIBs_strcat(pstr, "");
        eLIBs_strcat(pstr, "Album:");
        eLIBs_strcat(pstr, audio_file_info.album);
        eLIBs_strcat(pstr, '\0');
        __msg("pstr=%s", pstr);

        if (1)
        {
            __s32 lang_id[] = {STRING_MOVIE_TIPS, NULL};
            /*
	    HBarState state;
            gscene_hbar_get_state(&state);

            if (state == HBAR_ST_SHOW)
            {
                gscene_hbar_set_state(HBAR_ST_HIDE);
            }
	    */
            default_dialog_ex(h_music_info, GUI_WinGetParent(self->h_framewin), MUSIC_MENU_ARTIST_INFO, ADLG_OK, lang_id, pstr);
            esKRNL_TimeDly(200);
            app_dialog_destroy(h_music_info);
	    /*
            if (state == HBAR_ST_SHOW)
            {
                gscene_hbar_set_state(HBAR_ST_SHOW);
            }
	    */
        }
    }
    else if (self->main_menu_index == MUSIC_MAIN_MENU_RETURN)
    {
        __msg("+++++++MUSIC_MAIN_MENU_RETURN+++++++++");
        music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_DESTROY, 0);
    }

    //__here__;
    return EPDK_OK;
}

static __s32  music_menu_frm_key_submenu(music_menu_ctrl_t  *self)
{
    __s32 i = 0;
    RECT      rect;
    __msg("self->menu_flag = %d", self->menu_flag);

    if (self->main_menu_index == MUSIC_MAIN_MENU_EQ)
    {
        __msg("+++++++MUSIC_MAIN_MENU_EQ+++++++++");

        if (self->eq_menu_index == MUSIC_SUB_MENU_EQ_RETURN)
        {
            self->menu_flag = MUSIC_MAIN_MENU;
            com_memdev_create(self->layer);
            GUI_LyrWinSetBottom(self->layer);
            GUI_LyrWinSel(self->layer);
            GUI_SetFont(self->font);
            GUI_UC_SetEncodeUTF8();
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_ClearRect(MUSIC_MENU_X, MUSIC_MENU_Y, MUSIC_MENU_FRM_W, MUSIC_MENU_FRM_H);
#ifndef MUSIC_VIEW_CHANGE_20160811

            for (i = 0; i < MUSIC_MAIN_MENU_MAX; i++)
#else
            for (i = 0; i < 8; i++)
#endif
            {
                if (self->main_menu_index == i)
                {
                    draw_music_menu_btn(self, i, MENU_FOCUSED);
                }
                else
                {
                    if (i < MUSIC_MAIN_MENU_MAX)
                    {
                        draw_music_menu_btn(self, i, MENU_UNFOCUSED);
                    }
                    else
                    {
                        draw_music_menu_btn(self, i, MENU_UNKNOW);
                        continue;
                    }
                }

                draw_music_main_menu_string(self, i);
            }

            GUI_LyrWinSetTop(self->layer);
            com_memdev_delete();
        }
        else
        {
            if (self->eq_sel_index != self->eq_menu_index)
            {
                com_memdev_create(self->layer);
                //GUI_LyrWinSetBottom(self->layer);
                GUI_LyrWinSel(self->layer);
                GUI_SetFont(self->font);
                GUI_UC_SetEncodeUTF8();
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetBkColor(0);
                draw_music_menu_btn(self, self->eq_sel_index, MENU_UNFOCUSED);
                draw_music_sub_menu_string(self, self->main_menu_index, self->eq_sel_index);
                draw_music_menu_btn(self, self->eq_menu_index, MENU_FOCUSED);
                draw_music_sub_menu_string(self, self->main_menu_index, self->eq_menu_index);
                draw_music_sub_menu_select(self, self->eq_menu_index, MENU_FOCUSED);
                //GUI_LyrWinSetTop(self->layer);
                com_memdev_delete();
                self->eq_sel_index = self->eq_menu_index;
                music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_EQ, self->eq_menu_index);
            }
        }
    }
    else if (self->main_menu_index == MUSIC_MAIN_MENU_REPEAT)
    {
        __msg("self->playmode_menu_index = %d ", self->playmode_menu_index);

        if (self->playmode_menu_index == MUSIC_SUB_MENU_REPEAT_RETURN)
        {
            self->menu_flag = MUSIC_MAIN_MENU;
            dsk_display_get_size(&rect.width, &rect.height);

            if (rect.width == 800 && rect.height == 480)
            {
                rect.x = MUSIC_MENU_FRM_X;//MUSIC_MENU_X;
            }
            else//720 * 480
            {
                rect.x = MUSIC_MENU_FRM_X - 80;//MUSIC_MENU_X;
            }

            rect.y = MUSIC_MENU_FRM_Y;
            rect.width = MUSIC_MENU_FRM_W;
            rect.height = MUSIC_MENU_FRM_H;
            music_menu_set_screen_rect(&rect, self);
            com_memdev_create(self->layer);
            GUI_LyrWinSel(self->layer);
            GUI_LyrWinSetBottom(self->layer);
            GUI_SetFont(self->font);
            GUI_UC_SetEncodeUTF8();
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            GUI_SetBkColor(0);
#ifndef MUSIC_VIEW_CHANGE_20160811

            for (i = 0; i < MUSIC_MAIN_MENU_MAX; i++)
#else
            for (i = 0; i < 8; i++)
#endif
            {
                if (self->main_menu_index == i)
                {
                    draw_music_menu_btn(self, i, MENU_FOCUSED);
                }
                else
                {
                    if (i < MUSIC_MAIN_MENU_MAX)
                    {
                        draw_music_menu_btn(self, i, MENU_UNFOCUSED);
                    }
                    else
                    {
                        draw_music_menu_btn(self, i, MENU_UNKNOW);
                        continue;
                    }
                }

                draw_music_main_menu_string(self, i);
            }

            GUI_LyrWinSetTop(self->layer);
            com_memdev_delete();
        }
        else
        {
            if (self->playmode_sel_index != self->playmode_menu_index)
            {
                GUI_SetFont(self->font);
                GUI_UC_SetEncodeUTF8();
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetBkColor(0);
                com_memdev_create(self->layer);
                GUI_LyrWinSel(self->layer);
                draw_music_menu_btn(self, self->playmode_sel_index, MENU_UNFOCUSED);
                draw_music_sub_menu_string(self, self->main_menu_index, self->playmode_sel_index);
                draw_music_menu_btn(self, self->playmode_menu_index, MENU_FOCUSED);
                draw_music_sub_menu_string(self, self->main_menu_index, self->playmode_menu_index);
                draw_music_sub_menu_select(self, self->playmode_menu_index, MENU_FOCUSED);
                //GUI_LyrWinSetTop(self->layer);
                com_memdev_delete();
                self->playmode_sel_index = self->playmode_menu_index;
                music_menu_cmd2parent(self->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_PLAYMODE, self->playmode_menu_index);
            }
        }
    }

    //__here__;
    return EPDK_OK;
}

static __s32  music_menu_frm_key_enter(__gui_msg_t *msg)
{
    music_menu_ctrl_t       *music_menu_ctrl;
    music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    __msg("music_menu_ctrl->menu_flag = %d ", music_menu_ctrl->menu_flag);

    if (music_menu_ctrl->menu_flag == MUSIC_MAIN_MENU)
    {
        //__here__;
        music_menu_frm_key_mainmenu(music_menu_ctrl);
    }
    else if (music_menu_ctrl->menu_flag == MUSIC_SUB_MENU)
    {
        //__here__;
        music_menu_frm_key_submenu(music_menu_ctrl);
    }
    else
    {
        __msg("music menu flag is err ");
    }

    return EPDK_OK;
}


static __s32  music_menu_frm_on_key(__gui_msg_t *msg)
{
    static __s32 last_key = -1;
    __s32       i;

    //__here__;
    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        /**if (GUI_MSG_KEY_ENTER == last_key)
        {
            __msg("GUI_MSG_KEY_ENTER");
            music_menu_frm_key_enter(msg);
        }**/
    }
    else
    {
        switch (msg->dwAddData1)
        {
            //case GUI_MSG_KEY_LEFT:
            //case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            {
                __msg("GUI_MSG_KEY_UP");

                if (GUI_IsTimerInstalled(msg->h_deswin, muisc_auto_delete_menu_id))
                {
                    __log("+++++++++++ reset timer ");
                    GUI_ResetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                }
                else
                {
                    __log("+++++++++++ create timer ");
                    GUI_SetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                }

                music_menu_frm_key_up(msg);
            }
            break;

            //case GUI_MSG_KEY_RIGHT:
            //case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            {
                __msg("GUI_MSG_KEY_DOWN");

                if (GUI_IsTimerInstalled(msg->h_deswin, muisc_auto_delete_menu_id))
                {
                    __log("+++++++++++ reset timer ");
                    GUI_ResetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                }
                else
                {
                    __log("+++++++++++ create timer ");
                    GUI_SetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                }

                music_menu_frm_key_down(msg);
            }
            break;

            case GUI_MSG_KEY_ENTER:
            {
                if (GUI_IsTimerInstalled(msg->h_deswin, muisc_auto_delete_menu_id))
                {
                    __log("+++++++++++ reset timer ");
                    GUI_ResetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                }
                else
                {
                    __log("+++++++++++ create timer ");
                    GUI_SetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                }

                return music_menu_frm_key_enter(msg);
            }
            break;

            case GUI_MSG_KEY_PLAY_PAUSE:
            {
                __wrn("GUI_MSG_KEY_PLAY_PAUSE,msg->dwReserved=%d", msg->dwReserved);

                if (0 == msg->dwReserved) //面板play_pause 做enter键
                {
                    if (GUI_IsTimerInstalled(msg->h_deswin, muisc_auto_delete_menu_id))
                    {
                        __log("+++++++++++ reset timer ");
                        GUI_ResetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                    }
                    else
                    {
                        __log("+++++++++++ create timer ");
                        GUI_SetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
                    }

                    return music_menu_frm_key_enter(msg);
                }
            }
            break;

            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_LONGESCAPE:
            {
                music_menu_ctrl_t       *music_menu_ctrl;
                music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (music_menu_ctrl->menu_flag == MUSIC_MAIN_MENU)
                {
                    __msg("+++++++MUSIC_MAIN_MENU_RETURN+++++++++");
                    music_menu_cmd2parent(music_menu_ctrl->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_DESTROY, 0);
                }
                else if (music_menu_ctrl->menu_flag == MUSIC_SUB_MENU)
                {
                    RECT      rect;
                    dsk_display_get_size(&rect.width, &rect.height);

                    if (rect.width == 800 && rect.height == 480)
                    {
                        rect.x      = MUSIC_MENU_FRM_X;//MUSIC_MENU_X;
                    }
                    else//720 * 480
                    {
                        rect.x      = MUSIC_MENU_FRM_X - 80;//MUSIC_MENU_X;
                    }

                    rect.y      = MUSIC_MENU_FRM_Y;
                    rect.width  = MUSIC_MENU_FRM_W;
                    rect.height = MUSIC_MENU_FRM_H;
                    //__here__;
                    music_menu_set_screen_rect(&rect, music_menu_ctrl);
                    music_menu_ctrl->menu_flag = MUSIC_MAIN_MENU;
                    com_memdev_create(music_menu_ctrl->layer);
                    GUI_LyrWinSetBottom(music_menu_ctrl->layer);
                    GUI_LyrWinSel(music_menu_ctrl->layer);
                    GUI_SetFont(music_menu_ctrl->font);
                    GUI_UC_SetEncodeUTF8();
                    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                    GUI_SetBkColor(0);
#ifdef MUSIC_VIEW_CHANGE_20160811

                    for (i = 0; i < 8; i++)
#else
                    for (i = 0; i < MUSIC_MAIN_MENU_MAX; i++)
#endif
                    {
                        if (music_menu_ctrl->main_menu_index == i)
                        {
                            draw_music_menu_btn(music_menu_ctrl, i, MENU_FOCUSED);
                        }
                        else
                        {
                            if (i < MUSIC_MAIN_MENU_MAX)
                            {
                                draw_music_menu_btn(music_menu_ctrl, i, MENU_UNFOCUSED);
                            }
                            else
                            {
                                draw_music_menu_btn(music_menu_ctrl, i, MENU_UNKNOW);
                                continue;
                            }
                        }

                        draw_music_main_menu_string(music_menu_ctrl, i);
                    }

                    GUI_LyrWinSetTop(music_menu_ctrl->layer);
                    com_memdev_delete();
                }
            }
            break;

            case GUI_MSG_KEY_EQ:
            {
                music_menu_ctrl_t       *music_menu_ctrl;
                RECT      rect;
                music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (music_menu_ctrl->main_menu_index != MUSIC_MAIN_MENU_EQ
                    || music_menu_ctrl->menu_flag == MUSIC_MAIN_MENU)
                {
                    music_menu_ctrl->main_menu_index = MUSIC_MAIN_MENU_EQ;
                    music_menu_frm_key_mainmenu(music_menu_ctrl);
                }
            }
            break;

            case GUI_MSG_KEY_REPEATE:
            {
                music_menu_ctrl_t       *music_menu_ctrl;
                music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (music_menu_ctrl->main_menu_index != MUSIC_MAIN_MENU_REPEAT
                    || music_menu_ctrl->menu_flag == MUSIC_MAIN_MENU)
                {
                    music_menu_ctrl->main_menu_index = MUSIC_MAIN_MENU_REPEAT;
                    music_menu_frm_key_mainmenu(music_menu_ctrl);
                }
            }
            break;

            default:
            {
                //__here__;
            } break;
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




/*

*/
static __s32 music_menu_frm_manager_proc(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            music_menu_ctrl_t       *music_menu_ctrl = NULL;
            __s32                    i;
            __msg("++++++GUI_MSG_CREATE++++++");
            music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (music_menu_ctrl == NULL)
            {
                __err("music_menu_ctrl is null");
                return EPDK_FAIL;
            }

            init_music_menu_res(music_menu_ctrl);
            music_menu_ctrl->font   = SWFFont;
            music_menu_ctrl->h_framewin = msg->h_deswin;
            //DE_INFO;
            GUI_LyrWinSetSta(music_menu_ctrl->layer, GUI_LYRWIN_STA_SLEEP);
            GUI_LyrWinSetBottom(music_menu_ctrl->layer);
            GUI_LyrWinSel(music_menu_ctrl->layer);
            GUI_SetFont(music_menu_ctrl->font);
            GUI_UC_SetEncodeUTF8();
            GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
            __wrn("+++++++++++ local menu level = %d", music_menu_ctrl->menu_level);
            __wrn("+++++++++++ local menu_flag = %d", music_menu_ctrl->menu_flag);
            __wrn("+++++++++++ local main_menu_index = %d", music_menu_ctrl->main_menu_index);

            if (0 == music_menu_ctrl->menu_level)
            {
                __wrn("++++++++ menu_level = 0");
                com_memdev_create(music_menu_ctrl->layer);
#ifdef MUSIC_VIEW_CHANGE_20160811

                for (i = 0; i < 8; i++)
#else
                for (i = 0; i < MUSIC_MAIN_MENU_MAX; i++)
#endif
                {
                    if (music_menu_ctrl->main_menu_index == i)
                    {
                        draw_music_menu_btn(music_menu_ctrl, i, MENU_FOCUSED);
                    }
                    else
                    {
                        if (i < MUSIC_MAIN_MENU_MAX)
                        {
                            draw_music_menu_btn(music_menu_ctrl, i, MENU_UNFOCUSED);
                        }
                        else
                        {
                            draw_music_menu_btn(music_menu_ctrl, i, MENU_UNKNOW);
                            continue;
                        }
                    }

                    draw_music_main_menu_string(music_menu_ctrl, i);
                }

                com_memdev_delete();
                GUI_LyrWinSetSta(music_menu_ctrl->layer, GUI_LYRWIN_STA_ON);
            }
            else
            {
                __wrn("++++++++ menu_level != 0");

                if (1 == music_menu_ctrl->menu_level) //EQ
                {
                    //music_menu_ctrl->menu_flag = MUSIC_SUB_MENU;
                    music_menu_ctrl->main_menu_index = MUSIC_MAIN_MENU_EQ;
                }
                else if (2 == music_menu_ctrl->menu_level) //repeat
                {
                    //music_menu_ctrl->menu_flag = MUSIC_SUB_MENU;
                    music_menu_ctrl->main_menu_index = MUSIC_MAIN_MENU_REPEAT;
                }

                music_menu_frm_key_mainmenu(music_menu_ctrl);//做快捷处理
            }

            GUI_LyrWinSetTop(music_menu_ctrl->layer);
            GUI_SetTimer(msg->h_deswin, muisc_auto_delete_menu_id, MUSIC_AUTO_DELETE_MENU_CNT, NULL);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            __msg("++++++GUI_MSG_CLOSE++++++");
            GUI_FrmWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            music_menu_ctrl_t *music_menu_ctrl = NULL;
            __msg("++++++GUI_MSG_DESTROY++++++");
            music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (music_menu_ctrl == NULL)
            {
                return EPDK_FAIL;
            }

            uninit_music_menu_res(music_menu_ctrl);

            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, muisc_auto_delete_menu_id))
            {
                __wrn("++++++++++ kill delete menu time id ");
                GUI_KillTimer(msg->h_deswin, muisc_auto_delete_menu_id);
            }

            /*if(music_menu_ctrl != NULL)
            {
                //__here__;
                esMEMS_Mfree(0,music_menu_ctrl);
            }*/
            GUI_WinSetAttr(msg->h_deswin, NULL);
        }

        return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            //__here__;
        }

        case GUI_MSG_KEY:
        {
            __msg("++++++GUI_MSG_KEY++++++");
            __msg("++++++%d %d %d++++++", msg->id, msg->dwAddData1, msg->dwAddData2);
            return music_menu_frm_on_key(msg);
            break;
        }

        case GUI_MSG_TOUCH:
        {
            //__msg("++++GUI_MSG_TOUCH +++++");
            return EPDK_OK;
        }

        case GUI_MSG_TIMER:
        {
            __wrn("++++++++ timer ");

            if (muisc_auto_delete_menu_id == msg->dwAddData1) // 10 s 后关闭菜单
            {
                music_menu_ctrl_t       *music_menu_ctrl;
                music_menu_ctrl = (music_menu_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (music_menu_ctrl == NULL)
                {
                    return EPDK_FAIL;
                }

                __wrn("+++++++++++ timer to auto delete menu ");
                g_music_menu_ctrl->menu_level = 0;
                g_music_menu_ctrl->menu_flag  = MUSIC_MAIN_MENU;
                music_menu_cmd2parent(music_menu_ctrl->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_DESTROY, 0);
                GUI_KillTimer(msg->h_deswin, muisc_auto_delete_menu_id);
            }

            return EPDK_OK;
        }

        case GUI_MSG_COMMAND:
        {
            //__here__;
        } return EPDK_OK;

        case ALARM_MSG_CLOSE:
            __msg("++++++ALARM_MSG_CLOSE++++++");
            return EPDK_OK;

        case DSK_MSG_ALARM:
            __msg("++++++DSK_MSG_ALARM++++++");
            return EPDK_OK;

        case DSK_MSG_HOME:
            __msg("++++++DSK_MSG_HOME++++++");
            return EPDK_OK;

        case GUI_MSG_WIN_WAKEUP:
        {
            return EPDK_OK;
        }
    }

    return GUI_FrmWinDefaultProc(msg);
}
signed long music_menu_create(H_WIN h_parent, __s32 root_type, __s32 menu_level, __s32 play_mode)
{
    __gui_framewincreate_para_t framewin_para;
    music_menu_ctrl_t       *music_menu_ctrl = NULL;
    RECT                        layerRect;
    FB fb;
    __msg("++++++++++++++music_menu_create++++++++++++++++++++++++");

    //分配空间
    //esKSRV_SysInfo();
    if (NULL == g_music_menu_ctrl)
    {
        music_menu_ctrl = (music_menu_ctrl_t *)esMEMS_Malloc(0, sizeof(music_menu_ctrl_t));

        if (music_menu_ctrl == NULL)
        {
            __wrn("esMEMS_Malloc fail");
            return NULL;
        }

        eLIBs_memset(music_menu_ctrl, 0, sizeof(music_menu_ctrl_t));
        __msg("g_music_menu_ctrl == null");
        g_music_menu_ctrl = music_menu_ctrl;
    }
    else
    {
        music_menu_ctrl = g_music_menu_ctrl;
    }

    __wrn("+++++++++++ grobal menu level = %d", g_music_menu_ctrl->menu_level);
    __wrn("+++++++++++ grobal menu_flag = %d", g_music_menu_ctrl->menu_flag);
    __wrn("+++++++++++ grobal main_menu_index = %d", g_music_menu_ctrl->main_menu_index);
    music_menu_ctrl->menu_level = menu_level;
    __wrn("%s play_mode:%d", __FUNCTION__, play_mode);
    music_menu_ctrl->playmode_sel_index = play_mode;
    music_menu_ctrl->root_type = root_type;
    music_menu_ctrl->layer = music_menu_32bpp_layer_create(1);

    if (music_menu_ctrl->layer == NULL)
    {
        __wrn("music_menu_create fail");
        esMEMS_Mfree(0, music_menu_ctrl);
        music_menu_ctrl = NULL;
        return NULL;
    }

    GUI_LyrWinGetFB(music_menu_ctrl->layer, &fb);
#ifdef MUSIC_VIEW_CHANGE_20160811
    ab_mode_status = MUSIC_MENU_AB;
#endif
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "music_menu_frame";
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id         = MUSIC_MENU_FRM_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)music_menu_frm_manager_proc);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 0;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)music_menu_ctrl;
    framewin_para.hLayer = music_menu_ctrl->layer;
    return ((signed long)GUI_FrmWinCreate(&framewin_para));
}


__s32 music_menu_destory(H_WIN h_parent)
{
    //return GUI_FrmWinDelete(h_parent);
    H_LYR  hyr;
    hyr = GUI_WinGetLyrWin(h_parent);
    return GUI_LyrWinDelete(hyr);
}


