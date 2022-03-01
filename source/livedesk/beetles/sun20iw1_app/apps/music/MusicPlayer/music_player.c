/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
* File      :   music_player.h
*
* By        :   Libaiao
* Func  :
* Version   :   v1.0
* ============================================================================================================
* 2010-5-3 15:24:52  Libaiao  create this file, implements the fundemental interface;
**************************************************************************************************************
*/
#include "beetles_app.h"
#include "music.h"
#include "music_ui.h"
#include "music_player.h"
#include "player_face.h"
#include "spectrum_i.h"
#include "music_lyrics.h"
#include "spctrm.h"
#include <string.h>
#include <log.h>

__s32 npl_create(rat_media_type_t  media_type);
__s32 music_send_parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2, __s32 reserved);
void  npl_delete(void);

static __u32 MusicTimerId = 0x01;
static __u32 Music_FF_RR_TimerId = 0x05;
static __u32 MusicSysTimerId = 0x10;
static __u32 MusicCheckFocusTimerId = 0x10;             //检测当前条目是否与实际播放条目相同

#define NAME_MUSIC_BKPOINT                 "MUSICINF"

/*#define KARAOKE_LYRIC_USE_MALLOC 1

#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/
H_WIN           music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para);
static __s32    _music_player_win_cb(__gui_msg_t *msg);
static __s32    music_player_win_key_proc(__gui_msg_t *msg);
static __s32    music_player_scene_create(H_WIN  music_frame_win);                              //创建场景
static __s32    music_player_win_on_close(__gui_msg_t *msg);
static __s32    music_player_win_on_destroy(__gui_msg_t *msg);
static __s32    music_player_win_on_paint(H_WIN  music_frame_win);


static __s32    music_play_file(music_player_ctrl_t *player_ctrl, char *filename, __bool breakplay);

static __s32    music_player_device_init(music_player_ctrl_t *player_ctrl); //播放音频设备初始化
static __s32    music_player_device_uninit(music_player_ctrl_t *player_ctrl);//关闭音频设备

static __s32    music_player_win_on_timer(__gui_msg_t *msg);

static __s32    music_player_check_play_status_on_timer(music_player_ctrl_t *this);
__s32           music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

__s32           music_player_get_last_para(music_player_ctrl_t *This);
__s32           music_player_save_last_para(music_player_ctrl_t *This);

__s32           create_music_lyrics_scene(music_player_ctrl_t *This);
__s32           delete_music_lyrics_scene(music_player_ctrl_t *This);

static void  music_player_reset_ab_string(music_player_ctrl_t *this);

__s32 music_player_win_on_sys_timer(music_player_ctrl_t *this);


__s32 music_restore_play_file_info(music_player_ctrl_t *this);

//reg_root_type，0:sd 1:u盘
static __s32 music_player_switch_disk(music_player_ctrl_t *this, __u32 reg_root_type);
static __s32 music_explorer_on_enter_key(__gui_msg_t *msg);

static __s32 music_player_set_play_backward(music_player_ctrl_t *this);

static __s32 music_player_set_play_forward(music_player_ctrl_t *this);

static __s32 music_player_set_play_pause(music_player_ctrl_t *this, music_btn_status_e status);

static __s32 music_player_set_play_previous(music_player_ctrl_t *this);

static __s32 music_player_set_play_next(music_player_ctrl_t *this);

static __s32 music_player_set_play_mode(music_player_ctrl_t *this);

static __s32 music_player_set_EQ_mode(music_player_ctrl_t *this);
static void music_player_set_ab_mode(music_player_ctrl_t *this);



__s32 music_player_win_check_focus_on_timer(music_player_ctrl_t *this);

#ifdef BEETLES_SUPPORT_KARAOKE

static __s32 AiTalkGetStr(char str[AITALK_MAX_STR][AITALK_MAX_STR_SIZE]);
#endif

//extern __s32 draw_music_player_background(music_player_ctrl_t *player_ctrl);

/*
************************************************************************************************************************
*Function   :          H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para)
*
*Description    :       创建MusicPlayer framewin
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
H_WIN music_player_win_create(H_WIN h_parent, music_player_ctrl_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    music_player_ctrl_t *music_player_ctrl = NULL;
    FB fb;
    //app_print_memory_usage("music player win create", NULL);
    GUI_LyrWinGetFB(para->music_layer, &fb);
    music_player_ctrl = (music_player_ctrl_t *)esMEMS_Malloc(0, sizeof(music_player_ctrl_t));
    eLIBs_memset((void *)music_player_ctrl, 0, sizeof(music_player_ctrl_t));

    if (sizeof(music_player_ctrl_t) > 1024)
    {
        __wrn("music_player_win_create() size > 1024 .....");
    }

    music_player_ctrl->digit_timmer_id = 0x16; // 30234384
    music_player_ctrl->font = para->font;
    //music_player_ctrl->background_layer = para->background_layer;
    music_player_ctrl->music_layer = para->music_layer;
    music_player_ctrl->media_type = para->media_type;       //photo, music, movie
    music_player_ctrl->root_type = para->root_type;                         //SD or USB device
    music_player_ctrl->visiable = para->visiable;               //是否背景播放
    music_player_ctrl->explorer_play_music = para->explorer_play_music;
    music_player_ctrl->music_file_name = para->music_file_name;
    music_player_ctrl->AB_mode = para->AB_mode;
    music_player_ctrl->menu_index = MUSIC_LIST_MENU_ICON;
#ifndef MUSIC_VIEW_CHANGE_20160811
    music_player_ctrl->showMode = MUSIC_ALBUM;
#else
    music_player_ctrl->showMode = MUSIC_SPECTRUM;
#endif
    //music_player_ctrl->view_mode = para->view_mode;                           //list or square
    //ExplorerListWinGetSearchPath(list_para);
    //eLIBs_memcpy(music_player_ctrl->search_path,"e:", sizeof("e:"));
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "Music List window",
    framewin_para.dwExStyle = WS_EX_NONE;
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id        = MUSIC_LIST_WIN_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_music_player_win_cb);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 255;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)music_player_ctrl;
    framewin_para.hLayer = music_player_ctrl->music_layer;          //会被listbar调用
    return (GUI_FrmWinCreate(&framewin_para));                      //在framewin 的create消息中将handle,传送给music_player_ctrl->h_framewin
}

__s32 music_player_on_power_charge(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    power_level_e power_level = 0;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (this == NULL)
    {
        return EPDK_FAIL;
    }

    if (!this->visiable)
    {
        return EPDK_FAIL;
    }

    power_level = msg->dwAddData1;
    DrawMusicBatteryStatus(power_level);
    return EPDK_OK;
}

static __s32 music_player_music_list_touch(__gui_msg_t *msg, __u32 keyState, __s32 x, __s32 y, __s32 direct, __s32 speed)
{
    music_player_ctrl_t *player_ctrl = NULL;
    music_list_ctrl_t *ListCtrl = NULL;
    char *filename = NULL;
    __s32 ret = EPDK_FAIL;
    music_ui_t *ui_param;
    ui_param = music_get_ui_param();
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ListCtrl = player_ctrl->list_ctrl;

    if (keyState == GUI_MSG_TOUCH_DOWN)
    {
        if (x > ui_param->list_ctrl.listbar_rect.x && y > ui_param->list_ctrl.listbar_rect.y &&
            (x < ui_param->list_ctrl.listbar_rect.x + ui_param->list_ctrl.listbar_rect.width)  &&
            (y < ui_param->list_ctrl.listbar_rect.y + ui_param->list_ctrl.listbar_rect.height))
        {
            if (ListCtrl->OnTouchDown((void *)ListCtrl, x, y, direct, speed) == EPDK_OK)
            {
                player_ctrl->isTouchMusicList = EPDK_TRUE;
                GUI_WinSetCaptureWin(msg->h_deswin);
                ret = EPDK_OK;
            }
        }
    }
    else if (keyState == GUI_MSG_TOUCH_UP)
    {
        if (GUI_WinGetCaptureWin() == msg->h_deswin && player_ctrl->isTouchMusicList)
        {
            ListCtrl->OnTouchUp((void *)ListCtrl, x, y, direct, speed);
            GUI_WinReleaseCapture();

            if (player_ctrl->isTouchMusicListMove == EPDK_FALSE)
            {
                music_explorer_on_enter_key(msg);
            }

            player_ctrl->isTouchMusicListMove = EPDK_FALSE;
            player_ctrl->isTouchMusicList = EPDK_FALSE;
            ret = EPDK_OK;
        }
    }
    else if (keyState == GUI_MSG_TOUCH_MOVE)
    {
        if (player_ctrl->isTouchMusicList)
        {
            player_ctrl->isTouchMusicListMove = EPDK_TRUE;
            ret = ListCtrl->OnTouchMove((void *)ListCtrl, x, y, direct, speed);
        }
    }

    if (ret == EPDK_OK)
    {
        player_ctrl->CheckFocusItemCnt = 0;
    }

    return ret;
}

static __s32 music_player_playing_bar_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __u32 total_time = 0, cur_time = 0;
    __u16 width = 0;
    GUI_RECT *rect = NULL;
    __s32 ret = EPDK_FAIL;
    music_player_ctrl_t *player_ctrl;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (keyState == GUI_MSG_TOUCH_UP)
    {
        if (GUI_WinGetCaptureWin() == msg->h_deswin && player_ctrl->isTouchPlayingBar)
        {
            GUI_WinReleaseCapture();
            robin_set_cmd_jump(player_ctrl->curTouchPlayingBarTime);
            player_ctrl->isTouchPlayingBar = EPDK_FALSE;
            ret = EPDK_OK;
        }
    }
    else if (keyState == GUI_MSG_TOUCH_MOVE || keyState == GUI_MSG_TOUCH_DOWN || keyState == GUI_MSG_TOUCH_LONGDOWN)
    {
        if (player_ctrl->isTouchPlayingBar == EPDK_TRUE)
        {
            rect = music_get_touch_rect(MUSIC_PLAYING_BAR_TOUCH);
            total_time = robin_get_total_time();
            width = rect->x1 - rect->x0;

            if (x > rect->x1)
            {
                x = rect->x1;
            }
            else if (x < rect->x0)
            {
                x = rect->x0;
            }

            cur_time = (x - rect->x0) * total_time / width;
            player_ctrl->curTouchPlayingBarTime = cur_time;
            total_time = robin_get_total_time();
            cur_time = player_ctrl->curTouchPlayingBarTime;
            DrawMusicPlayingTimeState(cur_time, total_time);
            ret = EPDK_OK;
        }
    }

    if (music_is_touch_icon(MUSIC_PLAYING_BAR_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_DOWN)
        {
            player_ctrl->isTouchPlayingBar = EPDK_TRUE;
            GUI_WinSetCaptureWin(msg->h_deswin);
        }

        ret = EPDK_OK;
    }

    return ret;
}

static __s32 music_player_play_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __cedar_status_t play_status;
    __u32 total_time = 0, cur_time = 0;
    __s32 ret = EPDK_FAIL;
    __s16 focusedIndex = -1;
    music_player_ctrl_t *player_ctrl;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (music_is_touch_icon(MUSIC_PLAY_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 0;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_set_play_pause(player_ctrl, PRESSED);
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_BACKWARD_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 1;
            music_player_set_play_backward(player_ctrl);
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            play_status = robin_get_fsm_status();

            if (play_status == CEDAR_STAT_STOP)
            {
                music_player_set_play_backward(player_ctrl);
            }
            else
            {
                robin_set_cmd_play();
                total_time = robin_get_total_time();
                cur_time = robin_get_cur_time();
                DrawMusicPlayingTimeState(cur_time, total_time);
            }
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_FORWARD_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 2;
            music_player_set_play_forward(player_ctrl);
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            play_status = robin_get_fsm_status();

            if (play_status == CEDAR_STAT_STOP)
            {
                music_player_set_play_forward(player_ctrl);
            }
            else
            {
                robin_set_cmd_play();
                total_time = robin_get_total_time();
                cur_time = robin_get_cur_time();
                DrawMusicPlayingTimeState(cur_time, total_time);
            }
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_PREV_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 3;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_set_play_previous(player_ctrl);
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_NEXT_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 4;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_set_play_next(player_ctrl);
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_USB_DISK_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 5;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_switch_disk(player_ctrl, 1);
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_SD_CARD_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 6;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_switch_disk(player_ctrl, 0);
        }

        ret = EPDK_OK;
    }

    if (EPDK_OK == ret)
    {
        if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
        {
            if (GUI_WinGetCaptureWin() != msg->h_deswin)
            {
                GUI_WinSetCaptureWin(msg->h_deswin);
            }
        }
        else if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
        {
            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }
        }
    }

    DrawMusicPlayerInfo(player_ctrl, focusedIndex);
    return ret;
}

static __s32 music_player_bottom_touch(__gui_msg_t *msg, __u32 keyState, __u16 x, __u16 y)
{
    __s32       ret = EPDK_FAIL;
    GUI_RECT    clear;
    __s16 focusedIndex = -1;
    music_player_ctrl_t *player_ctrl;
    music_ui_t *ui;
    __u32 x0, y0, x1, y1;
    memset(&clear, 0x00, sizeof(GUI_RECT));
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
#if 1

    if (music_is_touch_icon(MUSIC_BACK_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 0;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
#if 0

            if (robin_get_fsm_status() != CEDAR_STAT_PLAY)          //关闭音乐
            {
                music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_EXPLORER, 0);
            }
            else
            {
                //退出时，正在播放则进入背景播放状?
                //__wait__;
                //music_send_command(msg->h_deswin, GUI_MSG_COMMAND, EXPLR_SW_TO_MAIN, MUSICPLAYER_BACKGROUND);
                {
                    __gui_msg_t my_msg;
                    //my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.h_deswin = msg->h_deswin;
                    my_msg.h_srcwin = msg->h_deswin;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_BACKGROUND;//发送消息给音乐AP要进入背景播放状态，释放场景
                    //GUI_SendNotifyMessage(&msg);
                    GUI_SendMessage(&my_msg);
                }
            }

#else
            {
                __gui_msg_t my_msg;
                eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
                my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.id = DSK_MSG_HOME;
                my_msg.dwAddData1 = 0;
                //GUI_SendNotifyMessage(&msg);
                GUI_SendMessage(&my_msg);
            }
#endif
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_EQ_CHANGE_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 1;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_set_EQ_mode(player_ctrl);
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_PLAY_MODE_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 2;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_player_set_play_mode(player_ctrl);
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_EQ_LYRICS_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 3;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            music_LYRICS_e temp = player_ctrl->showMode;
            clear.x0 = 226;
            clear.y0 = 37;
            clear.x1 = 459;
            clear.y1 = 130;
            player_ctrl->showMode = (player_ctrl->showMode + 1) % MUSIC_LYRICS_MAX; //0-2一直循环。

            if (player_ctrl->showMode == MUSIC_LYRICS)
            {
                if (player_ctrl->h_lyrics != NULL)
                {
                    GUI_ClearRectEx(&clear);
                    update_music_lyrics(player_ctrl->h_lyrics);
                }
                else
                {
                    player_ctrl->showMode = (player_ctrl->showMode + 1) % MUSIC_LYRICS_MAX;
                }
            }

            if (player_ctrl->showMode == MUSIC_ALBUM)
            {
                ret = get_music_list_focus_item_full_filename(player_ctrl->list_ctrl, player_ctrl->play_file_info.filename);

                if (EPDK_FAIL == ret)
                {
                    return EPDK_FAIL;
                }

                if (DrawMusicAlbum(player_ctrl, player_ctrl->play_file_info.filename, &clear))
                {
                    player_ctrl->showMode = (player_ctrl->showMode + 1) % MUSIC_LYRICS_MAX;
                }
            }

            if (player_ctrl->showMode == MUSIC_SPECTRUM && temp != player_ctrl->showMode)
            {
                GUI_ClearRectEx(&clear);
                SPCTRM_Clear(0);
                SPCTRM_Update((H_WIN)(player_ctrl->h_framewin));
            }
        }

        ret = EPDK_OK;
    }
    else if (music_is_touch_icon(MUSIC_AB_TOUCH, x, y))
    {
        if (keyState == GUI_MSG_TOUCH_LONGDOWN || keyState == GUI_MSG_TOUCH_DOWN)
        {
            focusedIndex = 4;
        }
        else if (keyState == GUI_MSG_TOUCH_UP)
        {
            ui = music_get_ui_param();
            x0 = ui->bottom.AB_rect.x + 35;
            y0 = ui->bottom.AB_rect.y - 5;
            x1 = x0 + 42;
            y1 = y0 + 32;
            //music_player_set_play_mode(player_ctrl);
            music_player_set_ab_mode(player_ctrl);
            GUI_ClearRect(x0, y0, x1, y1);
            __wrn(".............");
        }

        ret = EPDK_OK;
    }

#endif
    DrawMusicBottom(player_ctrl, focusedIndex);
    return ret;
}

static __s32 music_player_on_touch(__gui_msg_t *msg)
{
    __s32 speed = 0, direct = 0;
    __u16 x = 0, y = 0;
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);

    //__msg("photo_main_frw_touch_proc(%d:   %02d, %02d, %02d, %02d)", msg->dwAddData1, x, y, speed, direct);

    if (music_player_playing_bar_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
    {
        return EPDK_OK;
    }

    if (music_player_music_list_touch(msg, msg->dwAddData1, x, y, direct, speed) == EPDK_OK)
    {
        return EPDK_OK;
    }

    if (music_player_play_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
    {
        return EPDK_OK;
    }

    if (music_player_bottom_touch(msg, msg->dwAddData1, x, y) == EPDK_OK)
    {
        return EPDK_OK;
    }

    return EPDK_OK;
}

__s32 music_player_win_on_command(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    __msg("++++++++++++++++++++++++++++++++++++++");

    switch (msg->dwAddData1)
    {
        case MUSICPLAYER_BACKGROUND:
        {
            music_player_scene_delete(msg->h_deswin);
            //send message to main menu
            music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_EXPLORER, MUSICPLAYER_BACKGROUND);
        }
        break;

        case MUSICPLAYER_BACKGROUND_HOME://请注意，该消息没有return ok，会往父窗口传，父窗口会继续响应
        {
            __wrn("music player:MUSICPLAYER_BACKGROUND_HOME");
            music_player_scene_delete(msg->h_deswin);
            //send message to main menu
            music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, MUSICPLAYER_BACKGROUND);
        }
        break;

        case MUSICPLAYER_MUSIC_PAUSE:
        {
            this->alarmComing = EPDK_TRUE;
        }
        break;

        case MUSICPLAYER_MUSIC_PLAY:
        {
            this->alarmComing = EPDK_FALSE;
        }
        break;
        return EPDK_OK;
    }

    return EPDK_OK;
}


//================================================30234384
static void DrawMusic_playing(GUI_FONT *font)
{
    /*__u32 index;
    char ucStringBuf[20] = {0};
    __u32 x1=90;
    __u32 y1=225;
    GUI_RECT gui_rect2;
    char item_str[1][128];
    __s32 lang_id_2 = STRING_MUSIC_MUSIC_PLAYING;

    memset(&gui_rect2, 0x00, sizeof(GUI_RECT));

    MusicSetTransColor();//设置颜色等
    GUI_SetFont(font);
    GUI_UC_SetEncodeUTF8();

    gui_rect2.x0 = x1;
    gui_rect2.y0 = y1;
    gui_rect2.x1 = gui_rect2.x0+85;
    gui_rect2.y1 = gui_rect2.y0+22;

    GUI_ClearRectEx(&gui_rect2);//清除区域

    dsk_langres_get_menu_text(lang_id_2, item_str, GUI_TITLE_MAX);//显示正在播放
    GUI_DispStringInRect((const char *)item_str, &gui_rect2, GUI_TA_HCENTER|GUI_TA_VCENTER);

    index = robin_npl_get_cur()+1;//显示正在播放数字
    eLIBs_sprintf(ucStringBuf, "%d", index);

    gui_rect2.x0 = x1+85;
    gui_rect2.y0 = y1;
    gui_rect2.x1 = gui_rect2.x0+80;
    gui_rect2.y1 = gui_rect2.y0+22;

    GUI_ClearRectEx(&gui_rect2);//清除区域
    GUI_DispStringInRect(ucStringBuf, &gui_rect2, GUI_TA_LEFT|GUI_TA_VCENTER);
    */
}
__s32 music_listbar_on_index(void *list_ctrl, __s32 index)
{
    music_list_ctrl_t *list_para = (music_list_ctrl_t *)list_ctrl;
    __wrn("index=%d", index);

    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->total == 0)
    {
        return EPDK_FAIL;
    }

    if (!list_para->visiable)
    {
        return EPDK_FAIL;
    }

    music_list_long_string_stop_roll(list_para);
    LISTBAR_SetFocusItemExt(list_para->listbar_handle, index);
    //LISTBAR_NextPage(list_para->listbar_handle);
    return EPDK_OK;
}

__s32 music_restore_play_file_list(music_player_ctrl_t *this, __s32 disk_status)
{
    __s32 ret = 0;
    char *filename = NULL;
    __rat_list_t *pRatList = NULL;
    __cedar_status_t status = CEDAR_STAT_;
    extern __s32 music_menu_destory_dialog(void);
    __wrn("%s this->root_type:%d", __FUNCTION__, this->root_type);
    __wrn("%s this->list_ctrl->root_type:%d", __FUNCTION__, this->list_ctrl->root_type);
    this->list_ctrl->root_type = this->root_type;
    status = robin_get_fsm_status();
    __wrn("status:%d", status);

    if (CEDAR_STAT_STOP != robin_get_fsm_status())
    {
        robin_set_cmd_stop();
    }

    rat_close(this->list_ctrl->rat_handle);

    if (EPDK_OK != music_rat_get_search_path(this->list_ctrl))
    {
        //  __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_EXPLR_OPEN_DISK_FAIL};
        //  H_WIN h_dialog = NULL;
        __gui_msg_t my_msg;
        __wrn("%s music_rat_get_search_path err", __FUNCTION__);
        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
        my_msg.h_deswin = GUI_WinGetParent(this->h_framewin);
        my_msg.h_srcwin = this->h_framewin;

        if (disk_status == DSK_MSG_FS_PART_PLUGOUT)
        {
            music_menu_destory_dialog();
            my_msg.dwAddData1 = 0;
            my_msg.id = DSK_MSG_HOME;
        }
        else
        {
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_SWITCH_DISK;

            if ((__s32)this->root_type == (__s32)RAT_TF)
            {
                my_msg.dwAddData2 = RAT_TF_SWITCH_TO_USB;
            }
            else if ((__s32)this->root_type == (__s32)RAT_USB)
            {
                my_msg.dwAddData2 = RAT_USB_SWITCH_TO_TF;
            }
            else
            {
                music_menu_destory_dialog();
                my_msg.dwAddData1 = 0;
                my_msg.id = DSK_MSG_HOME;
            }
        }

        GUI_SendNotifyMessage(&my_msg);
        return EPDK_FAIL;
    }

    //__here__;
    this->list_ctrl->rat_handle = rat_open_ex(this->list_ctrl->root_type, this->list_ctrl->media_type, C_SEARCH_FILE_CNT_AT_FIRST);

    if (this->list_ctrl->rat_handle == NULL)
    {
        //__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_EXPLR_OPEN_DISK_FAIL};
        //H_WIN h_dialog = NULL;
        __gui_msg_t my_msg;
        __wrn("!!!!!!!!!!!!!this->list_ctrl->rat_handle == NULL!!!!!!!!!!!!!!!!!");
        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
        my_msg.h_deswin = GUI_WinGetParent(this->h_framewin);
        my_msg.h_srcwin = this->h_framewin;

        if (disk_status == DSK_MSG_FS_PART_PLUGOUT)
        {
            music_menu_destory_dialog();
            my_msg.dwAddData1 = 0;
            my_msg.id = DSK_MSG_HOME;
        }
        else
        {
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_SWITCH_DISK;

            if ((__s32)this->root_type == (__s32)RAT_TF)
            {
                my_msg.dwAddData2 = RAT_TF_SWITCH_TO_USB;
            }
            else if ((__s32)this->root_type == (__s32)RAT_USB)
            {
                my_msg.dwAddData2 = RAT_USB_SWITCH_TO_TF;
            }
            else
            {
                music_menu_destory_dialog();
                my_msg.dwAddData1 = 0;
                my_msg.id = DSK_MSG_HOME;
            }
        }

        GUI_SendNotifyMessage(&my_msg);
        return EPDK_FAIL;
    }

    //__here__;
    this->list_ctrl->total = rat_get_cur_scan_cnt(this->list_ctrl->rat_handle);

    if (this->list_ctrl->total <= 0)
    {
        //__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_EXPLR_UNFOUND_TYPE_FILE};
        //H_WIN h_dialog = NULL;
        __gui_msg_t my_msg;
        __wrn("!!!!!!!!!!!!!this->list_ctrl->total <= 0!!!!!!!!!!!!!!!!!");
        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
        my_msg.h_deswin = GUI_WinGetParent(this->h_framewin);
        my_msg.h_srcwin = this->h_framewin;

        if (disk_status == DSK_MSG_FS_PART_PLUGOUT)
        {
            music_menu_destory_dialog();
            my_msg.dwAddData1 = 0;
            my_msg.id = DSK_MSG_HOME;
        }
        else
        {
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_SWITCH_DISK;

            if ((__s32)this->root_type == (__s32)RAT_TF)
            {
                my_msg.dwAddData2 = RAT_TF_SWITCH_TO_USB;
            }
            else if ((__s32)this->root_type == (__s32)RAT_USB)
            {
                my_msg.dwAddData2 = RAT_USB_SWITCH_TO_TF;
            }
            else
            {
                music_menu_destory_dialog();
                my_msg.dwAddData1 = 0;
                my_msg.id = DSK_MSG_HOME;
            }
        }

        GUI_SendNotifyMessage(&my_msg);
        return EPDK_FAIL;
    }

    __wrn("this->list_ctrl->total:%d", this->list_ctrl->total);
    this->list_ctrl->last_start_id = 0;
    this->list_ctrl->last_focused_id = 0;
    this->list_ctrl->play_index = 0;
    this->list_ctrl->lost_focus = EPDK_FALSE;
    LISTBAR_SetFocusItem(this->list_ctrl->listbar_handle, 0);
    LISTBAR_SetTotalItem(this->list_ctrl->listbar_handle, this->list_ctrl->total);
    pRatList = (__rat_list_t *)this->list_ctrl->rat_handle;
    __wrn("media_type:%d", pRatList->media_type);
    __wrn("str_path:%s", pRatList->str_path);
    __wrn("open_cnt:%d", pRatList->open_cnt);
    __wrn("npl_cur_index:%d", pRatList->npl_cur_index);
    MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, this->play_file_info.filename);
    npl_delete();
    __wrn("this->play_file_info.filename:%s", this->play_file_info.filename);
    rat_set_file_for_play(this->list_ctrl->rat_handle, this->play_file_info.filename);
    npl_create(this->list_ctrl->media_type);
    music_player_set_play_mode(this);
    music_menu_destory_dialog();
    music_play_file(this, this->play_file_info.filename, 0);
    //music_player_set_play_index(this, 0);
    return 0;
}

static __s32 music_player_set_play_index(music_player_ctrl_t *this, __s32 index)
{
    __s32  ret;
    __s32  total_count;
    char  *filename = NULL;
    __listbar_scene_t  scene;
    music_list_ctrl_t *ListCtrl = NULL;
    total_count = robin_npl_get_total_count();
    __wrn("%s", __FUNCTION__);
    __wrn("total_count=%d,index=%d", total_count, index);
    index--;

    if (index < 0 || index >= total_count)
    {
        return EPDK_FAIL;
    }

    ret = robin_npl_index2file(index, this->play_file_info.filename);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    __wrn("this->play_file_info.filename=%s", this->play_file_info.filename);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    robin_set_cmd_stop();
    //DrawMusicPlayNextButton(PRESSED);
    ListCtrl = this->list_ctrl;
    filename = this->play_file_info.filename;
    ListCtrl->play_index = index;
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    //show_music_list(ListCtrl);
    //__msg("------------play next file");
    {
        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
        __wrn("ListCtrl->play_index=%d", ListCtrl->play_index);

        if (ListCtrl->play_index != scene.focus_id)
        {
            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
        }
    }
    music_listbar_on_index((void *)ListCtrl, ListCtrl->play_index);
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    ListCtrl->play_index = scene.focus_id;
    __wrn("scene.focus_id=%d", scene.focus_id);
    __wrn("ListCtrl->play_index=%d", ListCtrl->play_index);
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    get_music_list_focus_item_full_filename(ListCtrl, filename);
    __wrn("filename=%s", filename);
    music_play_file(this, filename, 0);
    {
        __gui_msg_t my_msg;
        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
        my_msg.h_deswin = this->h_framewin;
        my_msg.h_srcwin = NULL;
        my_msg.id = GUI_MSG_COMMAND;
        my_msg.dwAddData1 = MUSIC_REBUILD_SPECTRUM;
        my_msg.dwAddData2 = 0;
        GUI_SendNotifyMessage(&my_msg);
    }
    DrawMusic_playing(this->font);
    gscene_hbar_set_music_state(BG_MUSIC);
    return EPDK_OK;
}


__s32 music_player_delete_digit_layer(H_WIN       music_frame_win)
{
    music_player_ctrl_t *this;
    __wrn("music_player_delete_digit_layer begin...");
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

    if (NULL == this)
    {
        return EPDK_FAIL;
    }

    if (this->hdigit_layer)
    {
        GUI_LyrWinDelete(this->hdigit_layer);
        this->hdigit_layer = NULL;
    }

    if (this->digit_timmer_id)
    {
        if (GUI_IsTimerInstalled(music_frame_win, this->digit_timmer_id))
        {
            GUI_KillTimer(music_frame_win, this->digit_timmer_id);
        }
    }

    __wrn("music_player_delete_digit_layer end...");
    return EPDK_OK;
}
//====================================================


/*
************************************************************************************************************************
*Function   :          static __s32 _music_player_win_cb(__gui_msg_t *msg)
*
*Description    :       framewin 消息处理函数
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 _music_player_win_cb(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    music_player_ctrl_t *this = NULL;
    GUI_RECT clear;
    memset(&clear, 0x00, sizeof(GUI_RECT));
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            __wrn("----explorer frame window GUI_MSG_CREATE begin----");
            music_player_scene_create(msg->h_deswin);                                       //on create message
            return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            music_player_ctrl_t *player_ctrl = NULL;
            player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __wrn("----explorer frame window GUI_MSG_ONPAINT----");

            if (player_ctrl && player_ctrl->list_ctrl && player_ctrl->list_ctrl->total > 0)
            {
                music_player_win_on_paint(msg->h_deswin);
            }
            else
            {
                __log("player_ctrl->list_ctrl = 0x%08x.", (unsigned long)player_ctrl->list_ctrl);
                return EPDK_FAIL;
            }

            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
            __wrn("----explorer frame window GUI_MSG_CLOSE begin----");
            music_player_win_on_close(msg);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
            music_player_win_on_destroy(msg);
            __wrn("----explorer frame window GUI_MSG_DESTROY end----");
            return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            switch (msg->dwAddData1)
            {
                case MUSICPLAYER_MUSIC_PRE:
                {
                    __wrn("+++++++++MUSICPLAYER_MUSIC_PRE++++++++++++++");
                    music_player_set_play_previous(this);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_NEXT:
                {
                    __wrn("+++++++++MUSICPLAYER_MUSIC_NEXT++++++++++++++");
                    music_player_set_play_next(this);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_MENU_EQ:
                {
                    this->EQ_mode = msg->dwAddData2;
                    __wrn("this->EQ_mode = %d", this->EQ_mode);
                    music_player_set_EQ_mode(this);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_MENU_PLAYMODE:
                {
                    this->play_mode = msg->dwAddData2;
                    __wrn("this->play_mode = %d", this->play_mode);
                    music_player_set_play_mode(this);
                    return EPDK_OK;
                }

                case MUSIC_REBUILD_SPECTRUM:
                {
                    __wrn("MUSIC_REBUILD_SPECTRUM");
#ifdef MUSIC_VIEW_CHANGE_20160811
                    DrawMusicMenuBtn(UNFOCUSED);
#endif

                    if (this->showMode == MUSIC_SPECTRUM)
                    {
                        com_memdev_create(this->music_layer);
                        SPCTRM_Clear(0);
                        SPCTRM_Update((msg->h_deswin));
                        com_memdev_delete();
                    }
                    else if (this->showMode == MUSIC_ALBUM)
                    {
                        com_memdev_create(this->music_layer);
                        GUI_SetBkColor(0xFF2B2E36);
                        GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
                        /*current playing*/
                        //__here__;
                        ret = MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, this->play_file_info.filename);
                        __wrn("this->play_file_info.filename:%s", this->play_file_info.filename);

                        if (EPDK_FAIL == ret)
                        {
                            //__here__;
                            com_memdev_delete();
                            return EPDK_FAIL;
                        }

                        GUI_LyrWinSel(this->music_layer);
                        __wrn("DrawMusicAlbum");

                        if (DrawMusicAlbum(this, this->play_file_info.filename, &clear))
                        {
                            __wrn("(This->showMode+1)%MUSIC_LYRICS_MAX = %d", (this->showMode + 1) % MUSIC_LYRICS_MAX);
                        }

                        com_memdev_delete();
                    }
                }
                break;

                case MUSICPLAYER_MUSIC_MENU_SWITCH_DISK:
                {
                    __wrn("+++++++++MUSICPLAYER_MUSIC_MENU_SWITCH_DISK+++++++++++++");

                    if (msg->dwAddData2 == RAT_USB)
                    {
                        DrawMusicPlayerInfo(this, 5);
                        this->root_type = RAT_USB;
                    }
                    else
                    {
                        DrawMusicPlayerInfo(this, 6);
                        this->root_type = RAT_TF;
                    }

                    return music_restore_play_file_list(this, msg->dwReserved);
                }
                break;

                default:
                {
                    music_player_win_on_command(msg);
                    __wrn("----explorer frame window GUI_MSG_COMMAND end----");
                    break;
                }
            }
        }
        break;

        case GUI_MSG_TOUCH:
        {
            return music_player_on_touch(msg);
        }

        case GUI_MSG_KEY:

            //          //__here__;
            if (this->visiable)     //有界面显示才会按键操作
            {
                //  //__here__;
                return music_player_win_key_proc(msg);
            }

            break;

        case DSK_MSG_VOLTAGE_CHANGE:
            music_player_on_power_charge(msg);
            return EPDK_OK;

        case GUI_MSG_TIMER:
            if (MusicSysTimerId == msg->dwAddData1) //定时去更新播放状态)
            {
                music_player_win_on_sys_timer(this);
            }
            else if (MusicCheckFocusTimerId == msg->dwAddData1)
            {
                //music_player_win_check_focus_on_timer(this);
            }
            else if (MusicTimerId == msg->dwAddData1)
            {
                music_player_win_on_timer(msg);
            }
            else if (this->digit_timmer_id && this->digit_timmer_id == msg->dwAddData1)
            {
                //__wrn("timmer has come...");
                //__wrn("this->ndigit_song_num=%d", this->ndigit_song_num);
                //__here__;
                __wrn("music_player_set_play_index");
                music_player_set_play_index(this, this->ndigit_song_num);
                music_player_delete_digit_layer(msg->h_deswin);
            }

            break;

        case DSK_MSG_FS_PART_PLUGOUT:
            __wrn("SD card plugout!");
            //music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
            break;

        default:
            ret = GUI_FrmWinDefaultProc(msg);
            return ret;
    }

    return GUI_FrmWinDefaultProc(msg);
}

//NextKey处理函数
static __s32 music_explorer_on_next_key(__gui_msg_t *msg)
{
    music_player_ctrl_t *player_ctrl = NULL;
    music_list_ctrl_t *ListCtrl = NULL;
    char *filename = NULL;
    __listbar_scene_t scene;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (player_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    ListCtrl = player_ctrl->list_ctrl;

    if (ListCtrl == NULL)
    {
        return EPDK_FAIL;
    }

    filename = player_ctrl->play_file_info.filename;
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
    ListCtrl->OnNextKey((void *)ListCtrl);              //listbar on nextkey
#if HT_1628_LED_EX
    SetMusicNum(APP_MUSIC_ID, ((LISTBAR_GetFocusItem(player_ctrl->list_ctrl->listbar_handle) + 1) & 0x0fff) | 0x2000);
#endif
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
    //show_music_list(ListCtrl);
    //get_music_list_focus_item_full_filename(ListCtrl, filename);
    player_ctrl->CheckFocusItemCnt = 0;     //有按键清除检测焦点条目定时计数器
    return EPDK_OK;
}



static __s32 music_explorer_on_next_key_and_play(__gui_msg_t *msg) //add by chenguofeng
{
    __s32       ret = 0;
    //music_player_ctrl_t *This;
    //music_list_ctrl_t *ListCtrl;
    //char     *filename;
    music_ui_t *ui;
    GUI_RECT    clear;
    //This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    //ListCtrl = player_ctrl->list_ctrl;
    //filename = player_ctrl->play_file_info.filename;
    memset(&clear, 0x00, sizeof(GUI_RECT));
    music_player_ctrl_t *player_ctrl = NULL;
    music_list_ctrl_t   *ListCtrl = NULL;
    char                *filename = NULL;
    __listbar_scene_t    scene;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (player_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    ListCtrl = player_ctrl->list_ctrl;

    if (ListCtrl == NULL)
    {
        return EPDK_FAIL;
    }

    filename = player_ctrl->play_file_info.filename;
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
    ListCtrl->OnNextKey((void *)ListCtrl);              //listbar on nextkey
#if HT_1628_LED_EX
    SetMusicNum(APP_MUSIC_ID, ((LISTBAR_GetFocusItem(player_ctrl->list_ctrl->listbar_handle) + 1) & 0x0fff) | 0x2000);
#endif
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
    //show_music_list(ListCtrl);
    //get_music_list_focus_item_full_filename(ListCtrl, filename);
    player_ctrl->CheckFocusItemCnt = 0;     //有按键清除检测焦点条目定时计数器
    ret = get_music_list_focus_item_full_filename(ListCtrl, filename);
    __msg("filename = %s ", filename);

    if (ret != EPDK_FAIL)
    {
        ListCtrl->play_index = ret;
    }
    else
    {
        ListCtrl->play_index = 0;
        return EPDK_FAIL;
    }

    __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
    {
        //处理当前歌曲的暂停播放
        char cur_playing_file[RAT_MAX_FULL_PATH_LEN];
        //music_player_ctrl_t *player_ctrl;
        //player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
        //if(NULL == player_ctrl)
        //{
        //    return EPDK_FAIL;
        //}
        eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
        ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
        __wrn("ret = %d, cur_playing_file=%s", ret, cur_playing_file);
        __log(".....layer = %08x", player_ctrl->list_ctrl->layer);

        if (EPDK_OK == ret && 0 == eLIBs_stricmp(filename, cur_playing_file))
        {
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

            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            DrawMusicPlayPauseButton(UNFOCUSED);
            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            return EPDK_OK;
        }
        else
        {
            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            music_player_reset_ab_string(player_ctrl);
            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            __wrn(".................");
        }
    }
    __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
    music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
    __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
    music_play_file(player_ctrl, filename, 0);
    {
        __gui_msg_t my_msg;
        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
        my_msg.h_deswin = msg->h_deswin;
        my_msg.h_srcwin = NULL;
        my_msg.id = GUI_MSG_COMMAND;
        my_msg.dwAddData1 = MUSIC_REBUILD_SPECTRUM;
        my_msg.dwAddData2 = 0;
        GUI_SendNotifyMessage(&my_msg);
    }
    return EPDK_OK;
}



//NextKey处理函数
static __s32 music_explorer_on_previous_key(__gui_msg_t *msg)
{
    music_player_ctrl_t *player_ctrl;
    music_list_ctrl_t *ListCtrl;
    //char *filename = NULL;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (player_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    ListCtrl = player_ctrl->list_ctrl;

    if (ListCtrl == NULL)
    {
        return EPDK_FAIL;
    }

    //filename = player_ctrl->play_file_info.filename;
    ListCtrl->OnPreviousKey((void *)ListCtrl);          //listbar on previous key
#if HT_1628_LED_EX
    SetMusicNum(APP_MUSIC_ID, ((LISTBAR_GetFocusItem(player_ctrl->list_ctrl->listbar_handle) + 1) & 0x0fff) | 0x2000);
#endif
    player_ctrl->CheckFocusItemCnt = 0;     //有按键清除检测焦点条目定时计数器
    return EPDK_OK;
}


static __s32 music_explorer_on_previous_key_and_play(__gui_msg_t *msg) //add by chenguofeng
{
    __s32       ret = 0;
    //music_player_ctrl_t *This;
    //music_list_ctrl_t *ListCtrl;
    //char     *filename;
    music_ui_t *ui;
    GUI_RECT    clear;
    //This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    //ListCtrl = player_ctrl->list_ctrl;
    //filename = player_ctrl->play_file_info.filename;
    memset(&clear, 0x00, sizeof(GUI_RECT));
    music_player_ctrl_t *player_ctrl = NULL;
    music_list_ctrl_t   *ListCtrl = NULL;
    char                *filename = NULL;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (player_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    ListCtrl = player_ctrl->list_ctrl;

    if (ListCtrl == NULL)
    {
        return EPDK_FAIL;
    }

    filename = player_ctrl->play_file_info.filename;
    ListCtrl->OnPreviousKey((void *)ListCtrl);          //listbar on previous key
#if HT_1628_LED_EX
    SetMusicNum(APP_MUSIC_ID, ((LISTBAR_GetFocusItem(player_ctrl->list_ctrl->listbar_handle) + 1) & 0x0fff) | 0x2000);
#endif
    player_ctrl->CheckFocusItemCnt = 0;     //有按键清除检测焦点条目定时计数器
    ret = get_music_list_focus_item_full_filename(ListCtrl, filename);
    __msg("filename = %s ", filename);

    if (ret != EPDK_FAIL)
    {
        ListCtrl->play_index = ret;
    }
    else
    {
        ListCtrl->play_index = 0;
        return EPDK_FAIL;
    }

    __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
    {
        //处理当前歌曲的暂停播放
        char cur_playing_file[RAT_MAX_FULL_PATH_LEN];
        //music_player_ctrl_t *player_ctrl;
        //player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
        //if(NULL == player_ctrl)
        //{
        //    return EPDK_FAIL;
        //}
        eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
        ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
        __wrn("ret = %d, cur_playing_file=%s", ret, cur_playing_file);
        __log(".....layer = %08x", player_ctrl->list_ctrl->layer);

        if (EPDK_OK == ret && 0 == eLIBs_stricmp(filename, cur_playing_file))
        {
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

            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            DrawMusicPlayPauseButton(UNFOCUSED);
            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            return EPDK_OK;
        }
        else
        {
            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            music_player_reset_ab_string(player_ctrl);
            __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
            __wrn(".................");
        }
    }
    __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
    music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
    __log(".....layer = %08x", player_ctrl->list_ctrl->layer);
    music_play_file(player_ctrl, filename, 0);
    {
        __gui_msg_t my_msg;
        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
        my_msg.h_deswin = msg->h_deswin;
        my_msg.h_srcwin = NULL;
        my_msg.id = GUI_MSG_COMMAND;
        my_msg.dwAddData1 = MUSIC_REBUILD_SPECTRUM;
        my_msg.dwAddData2 = 0;
        GUI_SendNotifyMessage(&my_msg);
    }
    return EPDK_OK;
}



static __s32 music_explorer_on_menu_key(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (this->menu_mode == MUSIC_EXPLORER_MODE)
    {
#if 0
        {
            music_palyer_ui_t *player_ui;
            GUI_MEMDEV_Handle   draw_mem;
            player_ui = music_get_player_ui_param();

            draw_mem = GUI_MEMDEV_Create(player_ui->contrl_BG_rect.x, player_ui->contrl_BG_rect.y, player_ui->contrl_BG_rect.width, player_ui->contrl_BG_rect.height);

            GUI_MEMDEV_Select(draw_mem);

            this->menu_mode = MUSIC_SET_PLAYER_MODE;

            GUI_UC_SetEncodeUTF8();             //UTF8字体显示

            //GUI_LyrWinCacheOn();

            ClearMusicPlayerContrlButtonBG();
            DrawMusicPlayerContrlButton(this);

            GUI_MEMDEV_CopyToLCD(draw_mem);
            GUI_MEMDEV_Select(NULL);
            GUI_MEMDEV_Delete(draw_mem);

            DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
        }
        //GUI_LyrWinCacheOff();
#else
        this->menu_mode = MUSIC_SET_PLAYER_MODE;
        DrawMusicPlayerInfo(this, 0);
#endif
    }
    else
    {
        //this->menu_mode = MUSIC_EXPLORER_MODE;
        //__msg("error in music explorer");
    }

    return EPDK_OK;
}

static __s32 music_explorer_on_enter_key(__gui_msg_t *msg)
{
    __s32 ret = 0;
    music_player_ctrl_t *This;
    music_list_ctrl_t *ListCtrl;
    char *filename;
    music_ui_t *ui;
    GUI_RECT    clear;
    memset(&clear, 0x00, sizeof(GUI_RECT));
    This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ListCtrl = This->list_ctrl;
    filename = This->play_file_info.filename;
    __log(".....layer = %08x", This->list_ctrl->layer);

    switch (This->menu_index)
    {
#ifndef MUSIC_VIEW_CHANGE_20160811

        case MUSIC_MENU_ICON:
        {
            __wrn("MUSIC_MENU_ICON ");
            ui = music_get_ui_param();
            clear.x0 = ui->album.albumRect.x;
            clear.y0 = ui->album.albumRect.y;
            clear.x1 = clear.x0 + ui->album.albumRect.width;
            clear.y1 = clear.y0 + ui->album.albumRect.height;
            GUI_SetBkColor(0xFF2B2E36);
            GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
            music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_CREATE, 0, this->play_mode);
        }
        break;

        case MUSIC_SHOW_MENU_ICON:
        {
            __wrn("MUSIC_SHOW_MENU_ICON ");

            if (This->showMode == MUSIC_ALBUM)
            {
                This->showMode = MUSIC_SPECTRUM;
                ui = music_get_ui_param();
                clear.x0 = ui->album.albumRect.x;
                clear.y0 = ui->album.albumRect.y;
                clear.x1 = clear.x0 + ui->album.albumRect.width;
                clear.y1 = clear.y0 + ui->album.albumRect.height;
                GUI_LyrWinSel(This->music_layer);
                GUI_SetBkColor(0xFF2B2E36);
                GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
                //eLIBs_FlushCache();
                //esKRNL_TimeDly(1);
                clear.x0 = ui->spectrum.BG_rect.x;
                clear.y0 = ui->spectrum.BG_rect.y;
                clear.x1 = clear.x0 + ui->spectrum.BG_rect.width;
                clear.y1 = clear.y0 + ui->spectrum.BG_rect.height;
                GUI_SetBkColor(0xFF2B2E36);
                GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
                GUI_LyrWinSel(This->music_layer);
                music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_SPCTRM_CREATE, 0, 0);
                //SPCTRM_Clear(0);
                //SPCTRM_Update((H_WIN)(This->h_framewin));
            }
            else if (This->showMode == MUSIC_SPECTRUM)
            {
                This->showMode = MUSIC_ALBUM;
                __wrn("MUSIC_SPECTRUM ");
                ui = music_get_ui_param();
                clear.x0 = ui->spectrum.BG_rect.x;
                clear.y0 = ui->spectrum.BG_rect.y;
                clear.x1 = clear.x0 + ui->spectrum.BG_rect.width;
                clear.y1 = clear.y0 + ui->spectrum.BG_rect.height;
                GUI_LyrWinSel(This->music_layer);
                GUI_SetBkColor(0xFF2B2E36);
                GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
                //eLIBs_FlushCache();
                clear.x0 = ui->album.albumRect.x;
                clear.y0 = ui->album.albumRect.y;
                clear.x1 = clear.x0 + ui->album.albumRect.width;
                clear.y1 = clear.y0 + ui->album.albumRect.height;
                GUI_SetBkColor(0xFF2B2E36);
                GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
                __msg("This->play_file_info.filename = %s", This->play_file_info.filename);
                ret = get_music_list_focus_item_full_filename(This->list_ctrl, This->play_file_info.filename);

                if (EPDK_FAIL == ret)
                {
                    //__here__;
                    return EPDK_FAIL;
                }

                GUI_LyrWinSel(This->music_layer);
                __wrn("DrawMusicAlbum");

                if (DrawMusicAlbum(This, This->play_file_info.filename, &clear))
                {
                    __wrn("(This->showMode+1)%MUSIC_LYRICS_MAX = %d", (This->showMode + 1) % MUSIC_LYRICS_MAX);
                }

                LISTBAR_Resume_dif_task(This->listbar_handle); //chengf
            }
            else
            {
                __wrn("+++++++++++++++++++++++");
            }
        }
        break;

        case MUSIC_AB_ICON:
        {
            music_player_set_ab_mode(This);

            switch (This->AB_mode)
            {
                case MUSIC_AB:
                {
                    DrawABText(STRING_MUSIC_AB);
                }
                break;

                case MUSIC_A:
                {
                    DrawABText(STRING_MUSIC_A);
                }
                break;

                case MUSIC_B:
                {
                    DrawABText(STRING_MUSIC_B);
                }
                break;

                default:
                    break;
            }
        }
        break;
#endif

        case MUSIC_LIST_MENU_ICON:
        {
            ret = get_music_list_focus_item_full_filename(ListCtrl, filename);
            __wrn("filename = %s ", filename);

            if (ret != EPDK_FAIL)
            {
                ListCtrl->play_index = ret;
            }
            else
            {
                ListCtrl->play_index = 0;
                return EPDK_FAIL;
            }

            __log(".....layer = %08x", This->list_ctrl->layer);
            {
                //处理当前歌曲的暂停播放
                char cur_playing_file[RAT_MAX_FULL_PATH_LEN] = {0};
                music_player_ctrl_t *player_ctrl = NULL;
                player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (NULL == player_ctrl)
                {
                    __wrn("NULL == player_ctrl");
                    return EPDK_FAIL;
                }

                ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
                __wrn("ret = %d, cur_playing_file=%s", ret, cur_playing_file);

                if (EPDK_OK == ret && 0 == eLIBs_stricmp(filename, cur_playing_file))
                {
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

                    __log(".....layer = %08x", This->list_ctrl->layer);
                    DrawMusicPlayPauseButton(UNFOCUSED);
                    __log(".....layer = %08x", This->list_ctrl->layer);
                    return EPDK_OK;
                }
                else
                {
                    __log(".....layer = %08x", This->list_ctrl->layer);
                    music_player_reset_ab_string(player_ctrl);
                    __log(".....layer = %08x", This->list_ctrl->layer);
                    __wrn(".................");
                }
            }
            __log(".....layer = %08x", This->list_ctrl->layer);
            __wrn("+++++++++++++++++ MUSIC_LIST_MENU_ICON 5");
            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
            __log(".....layer = %08x", This->list_ctrl->layer);
            __wrn("+++++++++++++++++ MUSIC_LIST_MENU_ICON 6");
            music_play_file(This, filename, 0);

            if (robin_get_fsm_status() == CEDAR_STAT_PLAY)
            {
                __wrn("+++++++++++++++++ MUSIC_LIST_MENU_ICON 7");
                //..robin_set_cmd_pause();
                gscene_hbar_set_music_state(BG_MUSIC);
            }

            {
                __gui_msg_t my_msg;
                eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
                my_msg.h_deswin = msg->h_deswin;
                my_msg.h_srcwin = NULL;
                my_msg.id = GUI_MSG_COMMAND;
                my_msg.dwAddData1 = MUSIC_REBUILD_SPECTRUM;
                my_msg.dwAddData2 = 0;
                GUI_SendNotifyMessage(&my_msg);
            }
        }
        break;

        default:
            break;
    }

    return EPDK_OK;
}

static __s32 music_explorer_on_enter_key_really(__gui_msg_t *msg) //by chenguofeng
{
    __s32 ret = 0;
    music_player_ctrl_t *This;
    music_list_ctrl_t *ListCtrl;
    char *filename;
    music_ui_t *ui;
    GUI_RECT    clear;
    memset(&clear, 0x00, sizeof(GUI_RECT));
    This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    ListCtrl = This->list_ctrl;
    filename = This->play_file_info.filename;
    __wrn("music_explorer_on_enter_key_really ");
    /*if(This->menu_index == MUSIC_MENU_ICON)
    {
        __wrn("+++++++++++++++++ MUSIC_MENU_ICON ");
        ui = music_get_ui_param();
        clear.x0 = ui->album.albumRect.x;
        clear.y0 = ui->album.albumRect.y;
        clear.x1 = clear.x0 + ui->album.albumRect.width;
        clear.y1 = clear.y0 + ui->album.albumRect.height;

        GUI_SetBkColor(0xFF2B2E36);
        GUI_ClearRect(clear.x0,clear.y0,clear.x1,clear.y1);

        music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_CREATE, 0,0);

    }
    else if(This->menu_index == MUSIC_SHOW_MENU_ICON)
    {
        __msg("++++++++++++++++++++++");
        if(This->showMode == MUSIC_ALBUM)
        {
            This->showMode = MUSIC_SPECTRUM;
            ui = music_get_ui_param();

            clear.x0 = ui->album.albumRect.x;
            clear.y0 = ui->album.albumRect.y;
            clear.x1 = clear.x0 + ui->album.albumRect.width;
            clear.y1 = clear.y0 + ui->album.albumRect.height;

            GUI_LyrWinSel(This->music_layer);
            GUI_SetBkColor(0xFF2B2E36);
            GUI_ClearRect(clear.x0,clear.y0,clear.x1,clear.y1);

            //eLIBs_FlushCache();
            //esKRNL_TimeDly(1);

            clear.x0 = ui->spectrum.BG_rect.x;
            clear.y0 = ui->spectrum.BG_rect.y;
            clear.x1 = clear.x0 + ui->spectrum.BG_rect.width;
            clear.y1 = clear.y0 + ui->spectrum.BG_rect.height;

            GUI_SetBkColor(0xFF2B2E36);
            GUI_ClearRect(clear.x0,clear.y0,clear.x1,clear.y1);
            GUI_LyrWinSel(This->music_layer);
            music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_SPCTRM_CREATE, 0,0);
            //SPCTRM_Clear(0);
            //SPCTRM_Update((H_WIN)(This->h_framewin));

        }
        else if(This->showMode == MUSIC_SPECTRUM)
        {
            This->showMode = MUSIC_ALBUM;

            ui = music_get_ui_param();

            clear.x0 = ui->spectrum.BG_rect.x;
            clear.y0 = ui->spectrum.BG_rect.y;
            clear.x1 = clear.x0 + ui->spectrum.BG_rect.width;
            clear.y1 = clear.y0 + ui->spectrum.BG_rect.height;

            GUI_LyrWinSel(This->music_layer);
            GUI_SetBkColor(0xFF2B2E36);
            GUI_ClearRect(clear.x0,clear.y0,clear.x1,clear.y1);

            //eLIBs_FlushCache();

            clear.x0 = ui->album.albumRect.x;
            clear.y0 = ui->album.albumRect.y;
            clear.x1 = clear.x0 + ui->album.albumRect.width;
            clear.y1 = clear.y0 + ui->album.albumRect.height;

            GUI_SetBkColor(0xFF2B2E36);
            GUI_ClearRect(clear.x0,clear.y0,clear.x1,clear.y1);

            __msg("This->play_file_info.filename = %s", This->play_file_info.filename);
            ret = get_music_list_focus_item_full_filename(This->list_ctrl, This->play_file_info.filename);
            if(EPDK_FAIL == ret)
            {
                //__here__;
                return EPDK_FAIL;
            }
            GUI_LyrWinSel(This->music_layer);

            if( DrawMusicAlbum(This, This->play_file_info.filename, &clear) )
            {
                __msg("(This->showMode+1)%MUSIC_LYRICS_MAX = %d", (This->showMode+1)%MUSIC_LYRICS_MAX);
            }
            LISTBAR_Resume_dif_task(This->listbar_handle); //chengf

        }
        else
        {
            __msg("+++++++++++++++++++++++");
        }

    }
    else if(This->menu_index == MUSIC_LIST_MENU_ICON)*/
    {
        ret = get_music_list_focus_item_full_filename(ListCtrl, filename);
        __wrn("filename = %s ", filename);

        if (ret != EPDK_FAIL)
        {
            ListCtrl->play_index = ret;
        }
        else
        {
            ListCtrl->play_index = 0;
            return EPDK_FAIL;
        }

        __wrn(".....layer = %08x", This->list_ctrl->layer);
        {
            //处理当前歌曲的暂停播放
            char cur_playing_file[RAT_MAX_FULL_PATH_LEN];
            music_player_ctrl_t *player_ctrl;
            player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (NULL == player_ctrl)
            {
                return EPDK_FAIL;
            }

            eLIBs_memset(cur_playing_file, 0, sizeof(cur_playing_file));
            ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
            __wrn("ret = %d, cur_playing_file=%s", ret, cur_playing_file);
            __wrn(".....layer = %08x", This->list_ctrl->layer);

            if (EPDK_OK == ret && 0 == eLIBs_stricmp(filename, cur_playing_file))
            {
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

                __wrn(".....layer = %08x", This->list_ctrl->layer);
                DrawMusicPlayPauseButton(UNFOCUSED);
                __wrn(".....layer = %08x", This->list_ctrl->layer);
                return EPDK_OK;
            }
            else
            {
                __wrn(".....layer = %08x", This->list_ctrl->layer);
                music_player_reset_ab_string(player_ctrl);
                __wrn(".....layer = %08x", This->list_ctrl->layer);
                __wrn(".................");
            }
        }
        __wrn(".....layer = %08x", This->list_ctrl->layer);
        music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
        __wrn(".....layer = %08x", This->list_ctrl->layer);
        music_play_file(This, filename, 0);
        {
            __gui_msg_t my_msg;
            eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
            my_msg.h_deswin = msg->h_deswin;
            my_msg.h_srcwin = NULL;
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.dwAddData1 = MUSIC_REBUILD_SPECTRUM;
            my_msg.dwAddData2 = 0;
            GUI_SendNotifyMessage(&my_msg);
        }
    }
    /*else
    {
        __msg("++++++++++++++++++++++");
        __log(".....layer = %08x",This->list_ctrl->layer);

    }*/
    __wrn(".....layer = %8x", This->list_ctrl->layer);
    return EPDK_OK;
}


//================================================
static __s32 music_digit_layer_draw(__gui_msg_t *msg)
{
    __s32    i;
    __s32    j;
    RECT     rect;
    char     text[256] = {0};
    GUI_RECT gui_rect;
    HTHEME   digit_back = 0;
    __s32    total_song_num;
    music_player_ctrl_t *player_ctrl = NULL;
    memset(&rect, 0x00, sizeof(RECT));
    memset(&gui_rect, 0x00, sizeof(GUI_RECT));
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    __wrn("&&&&&&& music_digit_layer_draw &&&&&");

    if (NULL == player_ctrl)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    if (NULL == player_ctrl->hdigit_layer)
    {
        __wrn("digit layer is null...");
        return EPDK_FAIL;
    }

    if (GUI_LYRWIN_STA_ON != GUI_LyrWinGetSta(player_ctrl->hdigit_layer))
    {
        __wrn("digit layer is not on...");
        //  return EPDK_FAIL;
    }

    digit_back = dsk_theme_open(ID_MUSIC_DIGIT_BG_BMP);
    total_song_num = robin_npl_get_total_count();

    if (player_ctrl->ndigit_song_num > total_song_num)
    {
        dsk_langres_get_menu_text(STRING_MUSIC_NO_THIS_SONG, text, sizeof(text));
    }
    else
    {
        eLIBs_memset(text, 0, sizeof(text));
        eLIBs_int2str_dec(player_ctrl->ndigit_song_num, text);
    }

    GUI_LyrWinSel(player_ctrl->hdigit_layer);
    GUI_SetFont(SWFFont);
    GUI_SetColor(GUI_RED);
    GUI_CharSetToEncode(dsk_get_langres_charset());
    GUI_SetBkColor(0x0);
    GUI_Clear();
    GUI_LyrWinGetScnWindow(player_ctrl->hdigit_layer, &rect);
    __wrn("======text=%s======", text);
    gui_rect.x0 = 0;
    gui_rect.y0 = 0;
    gui_rect.x1 = 100;
    gui_rect.y1 = 54;
    GUI_BMP_Draw(dsk_theme_hdl2buf(digit_back), gui_rect.x0, gui_rect.y0);
    GUI_DispStringInRect(text, &gui_rect, GUI_TA_HCENTER | GUI_TA_VCENTER);
    GUI_LyrWinSetSta(player_ctrl->hdigit_layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinSetTop(player_ctrl->hdigit_layer);

    if (digit_back)
    {
        dsk_theme_close(digit_back);
        digit_back = 0;
    }

    return EPDK_OK;
}

//================================================
//1~128
static void __music_ff_rr_set_speed(void)
{
    __u32 total_time = robin_get_total_time() / 1000;
    total_time = total_time / 60;

    if (total_time <= 10)
    {
        robin_set_ff_rr_speed(4);
    }
    else if (total_time <= 30)
    {
        robin_set_ff_rr_speed(8);
    }
    else if (total_time <= 90)
    {
        robin_set_ff_rr_speed(16);
    }
    else if (total_time <= 180)
    {
        robin_set_ff_rr_speed(32);
    }
    else //if(total_time <= 1)
    {
        robin_set_ff_rr_speed(64);
    }
}

static __s32 music_explorer_key_proc(__gui_msg_t *msg)
{
    static __u32 last_key = 0;
    //  //__here__;
    last_key = msg->dwAddData1;

    if ((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        switch (msg->dwAddData1)
        {
#if 0

            case GUI_MSG_KEY_LONGRIGHT:
                //..if (0 == msg->dwReserved)
            {
                __wrn("dsk_volume_inc");
                gscene_hbar_update_text(HBAR_VOLUME_PAINT);
                gscene_hbar_set_state(HBAR_ST_SHOW); //(HBAR_ST_SHOW_WIDTH_VOLUME);
            }
            break;

            case GUI_MSG_KEY_LONGLEFT:
                //..if (0 == msg->dwReserved)
            {
                gscene_hbar_update_text(HBAR_VOLUME_PAINT);
                gscene_hbar_set_state(HBAR_ST_SHOW); //(HBAR_ST_SHOW_WIDTH_VOLUME);
            }
            break;
#endif

            case GUI_MSG_KEY_RR:
            case GUI_MSG_KEY_LONGRR:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_LONGUP:
            {
                __cedar_status_t sta;
                sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == sta)
                {
                    __music_ff_rr_set_speed();
                    robin_set_cmd_rr();
                }

                break;
            }

            case GUI_MSG_KEY_FF:
            case GUI_MSG_KEY_LONGFF:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_LONGDOWN:
            {
                __cedar_status_t sta;
                sta = robin_get_fsm_status();

                if (CEDAR_STAT_PLAY == sta)
                {
                    __music_ff_rr_set_speed();
                    robin_set_cmd_ff();
                }

                break;
            }

            case GUI_MSG_KEY_VDEC:
                last_key = GUI_MSG_KEY_VDEC;
                break;

            case GUI_MSG_KEY_ENTER:
            {
                //__here__;
                return music_explorer_on_enter_key(msg);
            }
            break;

            case GUI_MSG_KEY_RISE:
                break;

            case GUI_MSG_KEY_PLAY_PAUSE:
            {
                //__here__;
                if (msg->dwReserved == 0) //面板做enter键
                {
                    return music_explorer_on_enter_key(msg);
                }
                else if (msg->dwReserved == 1) //IR 当play pause
                {
                    __cedar_status_t sta;
                    //__here__;
                    sta = robin_get_fsm_status();

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
                }

                break;
            }

            case GUI_MSG_KEY_TACK:
            {
                __s32 channel;
                channel = robin_get_channel();
                //channel = (channel + 1) % 3;               //this is abandoned practice
                robin_set_channel(channel);
#if 1
                {
                    __s32 lang_id = STRING_MUSIC_TACK_STEREO;
                    __log("music_player_delete_digit_layer begin... channel = %d", channel);

                    if (0 == channel)
                    {
                        lang_id = STRING_MUSIC_TACK_STEREO;
                    }
                    else if (1 == channel)
                    {
                        lang_id = STRING_MUSIC_TACK_LEFT;
                    }
                    else if (2 == channel)
                    {
                        lang_id = STRING_MUSIC_TACK_RIGHT;
                    }

                    DrawShowTACKText(lang_id);
                }
#endif
                break;
            }

#if 0

            case GUI_MSG_KEY_ACCOMP_ONOFF:
            {
                if (0 == msg->dwReserved)
                {
                    __s32 channel;
                    channel = robin_get_channel();
                    channel = (channel + 1) % 3;                          //this is abandoned practice
                    robin_set_channel(channel);
#if 1
                    {
                        __s32 lang_id = STRING_MUSIC_TACK_STEREO;
                        __log("music_player_delete_digit_layer begin... channel = %d", channel);

                        if (0 == channel)
                        {
                            lang_id = STRING_MUSIC_TACK_STEREO;
                        }
                        else if (1 == channel)
                        {
                            lang_id = STRING_MUSIC_TACK_LEFT;
                        }
                        else if (2 == channel)
                        {
                            lang_id = STRING_MUSIC_TACK_RIGHT;
                        }

                        DrawShowTACKText(lang_id);
                    }
#endif
                }

                break;
            }

#endif

            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_LONGMENU:
            {
                if (last_key == GUI_MSG_KEY_MENU)
                {
                    music_player_ctrl_t *player_ctrl = NULL;
                    __wrn("+++++++++++++++ music play pop menu ");
                    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
#ifndef MUSIC_VIEW_CHANGE_20160811
                    player_ctrl->menu_index = MUSIC_MENU_ICON;
#endif
                    DrawMusicMenuBtn(FOCUSED);
                    DrawMusicShowModeMenu(UNFOCUSED);

                    if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                    {
                        //LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                        //player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                        //  music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                    }

                    music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_CREATE, 0, player_ctrl->play_mode);
                }

                break;
            }

            case GUI_MSG_KEY_ESCAPE:
            case GUI_MSG_KEY_LONGESCAPE:
            {
                if (last_key == GUI_MSG_KEY_ESCAPE)
                {
                    __gui_msg_t my_msg;
                    __wrn("&&&&&&&&&& music_player_delete_digit_layer ");
                    music_player_delete_digit_layer(msg->h_deswin); // 3024384
                    eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.h_srcwin = msg->h_deswin;
                    my_msg.id = DSK_MSG_HOME;
                    my_msg.dwAddData1 = 0;
                    GUI_SendMessage(&my_msg);
                }

                break;
            }

            /*
                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LONGUP:
                {
                    music_player_ctrl_t *player_ctrl = NULL;
                    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                    __msg("----Music list window on DOWN key----");
                    if(player_ctrl->menu_index == 0)
                    {
                        music_explorer_on_previous_key(msg);
                    }
                    break;
                }

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_LONGDOWN:
                {
                    music_player_ctrl_t *player_ctrl = NULL;
                    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                    __msg("----Music list window on UP key----");
                    if(player_ctrl->menu_index == 0)
                    {
                        music_explorer_on_next_key(msg);
                    }
                    break;
                }
            */
            case GUI_MSG_KEY_EQ:
            {
                music_ui_t *ui;
                GUI_RECT clear;
                memset(&clear, 0x00, sizeof(GUI_RECT));
                music_player_ctrl_t *player_ctrl = NULL;
                __msg("GUI_MSG_KEY_RIGHT");
                player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                ui = music_get_ui_param();
                clear.x0 = ui->album.albumRect.x;
                clear.y0 = ui->album.albumRect.y;
                clear.x1 = clear.x0 + ui->album.albumRect.width;
                clear.y1 = clear.y0 + ui->album.albumRect.height;
                GUI_SetBkColor(0xFF2B2E36);
                GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
#ifndef MUSIC_VIEW_CHANGE_20160811
                player_ctrl->menu_index = MUSIC_MENU_ICON;
#endif
                DrawMusicMenuBtn(FOCUSED);
                DrawMusicShowModeMenu(UNFOCUSED);

                if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                {
                    // LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                    //  player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                    //   music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                }

                music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_CREATE, 1, player_ctrl->play_mode);
                break;
            }

            case GUI_MSG_KEY_REPEATE:
            {
                music_ui_t *ui;
                GUI_RECT clear;
                memset(&clear, 0x00, sizeof(GUI_RECT));
                music_player_ctrl_t *player_ctrl = NULL;
                __msg("GUI_MSG_KEY_RIGHT");
                player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                ui = music_get_ui_param();
                clear.x0 = ui->album.albumRect.x;
                clear.y0 = ui->album.albumRect.y;
                clear.x1 = clear.x0 + ui->album.albumRect.width;
                clear.y1 = clear.y0 + ui->album.albumRect.height;
                GUI_SetBkColor(0xFF2B2E36);
                GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
#ifndef MUSIC_VIEW_CHANGE_20160811
                player_ctrl->menu_index = MUSIC_MENU_ICON;
#endif
                DrawMusicMenuBtn(FOCUSED);
                DrawMusicShowModeMenu(UNFOCUSED);

                if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                {
                    //      LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                    //    player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                    //    music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                }

                music_send_parent(msg->h_deswin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_MENU_CREATE, 2, player_ctrl->play_mode);
                break;
            }

            default:
                break;
        }
    }
    else if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
                /*
                            case GUI_MSG_KEY_MENU:
                                if(last_key == GUI_MSG_KEY_MENU)
                                {

                                    #if 1//语音识别点歌
                                        //StartReconize(AiTalkGetStr);
                                        __wrn("StartReconize...");
                                    #else
                                    {
                                        __gui_msg_t my_msg;

                                        eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
                                        my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                                        my_msg.h_srcwin = msg->h_deswin;
                                        my_msg.id = DSK_MSG_HOME;
                                        my_msg.dwAddData1 = 0;

                                        //GUI_SendNotifyMessage(&msg);
                                        GUI_SendMessage(&my_msg);
                                    }
                                    #endif
                                }
                                break;*/
#if 0
            case GUI_MSG_KEY_LONGVADD:
                if (last_key == GUI_MSG_KEY_VADD)
                {
                    music_player_ctrl_t *player_ctrl = NULL;
                    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                    MusicVolumeBarCreate(player_ctrl);
                }

                break;
#endif

            case GUI_MSG_KEY_DOWN:

            //case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_NEXT:
            {
                music_player_ctrl_t *This = NULL;
                This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (This->menu_index != MUSIC_LIST_MENU_ICON)
                {
                    DrawMusicMenuBtn(UNFOCUSED);
                    DrawMusicShowModeMenu(UNFOCUSED);
                    DrawMusicABMenu(UNFOCUSED);

                    if (This->list_ctrl->lost_focus == EPDK_TRUE)
                    {
                        This->list_ctrl->lost_focus = EPDK_FALSE;
                        LISTBAR_ShowPage(This->list_ctrl->listbar_handle);
                        music_list_long_string_start_roll(This->list_ctrl);
                    }

                    This->menu_index = MUSIC_LIST_MENU_ICON;
                }

#ifdef MUSIC_VIEW_CHANGE_20160811
                DrawMusicMenuBtn(UNFOCUSED);
#endif
                music_explorer_on_next_key(msg);
                //music_explorer_on_next_key_and_play(msg);
                gscene_hbar_set_music_state(BG_MUSIC);
                break;
            }

            case GUI_MSG_KEY_UP:

            //case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_PREV:
            {
                music_player_ctrl_t *This = NULL;
                This = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (This->menu_index != MUSIC_LIST_MENU_ICON)
                {
                    DrawMusicMenuBtn(UNFOCUSED);
                    DrawMusicShowModeMenu(UNFOCUSED);
                    DrawMusicABMenu(UNFOCUSED);

                    if (This->list_ctrl->lost_focus == EPDK_TRUE)
                    {
                        This->list_ctrl->lost_focus = EPDK_FALSE;
                        LISTBAR_ShowPage(This->list_ctrl->listbar_handle);
                        music_list_long_string_start_roll(This->list_ctrl);
                    }

                    This->menu_index = MUSIC_LIST_MENU_ICON;
                }

#ifdef MUSIC_VIEW_CHANGE_20160811
                DrawMusicMenuBtn(UNFOCUSED);
#endif
                music_explorer_on_previous_key(msg);
                //music_explorer_on_previous_key_and_play(msg);
                gscene_hbar_set_music_state(BG_MUSIC);
                break;
            }

            //===========================================================
            //处理数字点歌
            case GUI_MSG_KEY_NUM0:
            case GUI_MSG_KEY_NUM1:
            case GUI_MSG_KEY_NUM2:
            case GUI_MSG_KEY_NUM3:
            case GUI_MSG_KEY_NUM4:
            case GUI_MSG_KEY_NUM5:
            case GUI_MSG_KEY_NUM6:
            case GUI_MSG_KEY_NUM7:
            case GUI_MSG_KEY_NUM8:
            case GUI_MSG_KEY_NUM9:
            {
                __s32 cur_val;
                __s32 total_song_num;
                music_player_ctrl_t *player_ctrl = NULL;
                __log("&&&&&&&&&&&&&&&&& player digital sel song:%d &&&&&&&&&&&", msg->dwAddData1);
                player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

                if (NULL == player_ctrl->hdigit_layer)
                {
                    RECT  rect;
                    __s32 width, height;
                    __s32 scn_width, scn_height;
                    __wrn("&&&&&&&&& create hdigit layer &&&&&&&&&");
                    memset(&rect, 0x00, sizeof(rect));
                    dsk_display_get_size(&scn_width, &scn_height);
                    width       = 100; //20*5;
                    height      = 54;
                    rect.x      = 300 + 50;// LISTBAR + 50
                    rect.y      = 46 + 54 * 6; // 7 ITEM OF LISTBAR
                    rect.width  = width;
                    rect.height = height;
                    player_ctrl->hdigit_layer = com_layer_create(&rect, 0, PIXEL_COLOR_ARGB8888, GUI_LYRWIN_STA_SLEEP, "");

                    if (NULL == player_ctrl->hdigit_layer)
                    {
                        __wrn("**********digit layer create fail...");
                    }
                    else
                    {
                        __wrn("digit layer create ok...");
                    }
                }

                __wrn("player_ctrl->digit_timmer_id=%d", player_ctrl->digit_timmer_id);

                //__wrn("msg->h_deswin=0x%x", msg->h_deswin);
                if (!GUI_IsTimerInstalled(msg->h_deswin, player_ctrl->digit_timmer_id))
                {
                    __wrn("===========create timmer...");
                    player_ctrl->ndigit_song_num = 0;
                    GUI_SetTimer(msg->h_deswin, player_ctrl->digit_timmer_id, 300, NULL);
                }
                else
                {
                    __wrn("=============reset timmer...");
                    GUI_ResetTimer(msg->h_deswin, player_ctrl->digit_timmer_id, 300, NULL);
                }

                cur_val = msg->dwAddData1 - GUI_MSG_KEY_NUM0;
                __wrn("&&&&&&&&&&& cur_val=%d ", cur_val);
                total_song_num = robin_npl_get_total_count();
                __wrn("&&&&&&&&&&  total_song_num=%d", total_song_num);

                if (player_ctrl->ndigit_song_num < total_song_num)
                {
                    player_ctrl->ndigit_song_num *= 10;
                    player_ctrl->ndigit_song_num += cur_val;
                }
                else
                {
                    __wrn("=============song num exceed...");
                }

#if HT_1628_LED_EX
                SetMusicNum(APP_MUSIC_ID, ((player_ctrl->ndigit_song_num) & 0x0fff) | 0x2000);
#endif
                __wrn("player_ctrl->ndigit_song_num=%d", player_ctrl->ndigit_song_num);
                music_digit_layer_draw(msg);
                break;
            }

            //===========================================================

            /*case GUI_MSG_KEY_LONGRIGHT:
                robin_set_cmd_play();
                break;

            case GUI_MSG_KEY_LONGLEFT:
                robin_set_cmd_play();
                break;*/

            case GUI_MSG_KEY_RR:
            case GUI_MSG_KEY_LONGRR:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LONGLEFT:
                robin_set_cmd_play();
                break;

            case GUI_MSG_KEY_FF:
            case GUI_MSG_KEY_LONGFF:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_LONGRIGHT:
                robin_set_cmd_play();
                break;

            case GUI_MSG_KEY_RIGHT:
            {
                music_player_ctrl_t *player_ctrl = NULL;
                __msg("GUI_MSG_KEY_RIGHT");
                player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
#ifndef MUSIC_VIEW_CHANGE_20160811

                if (player_ctrl->menu_index >= MUSIC_PLAY_BTN_ICON_MAX - 1)
                {
                    player_ctrl->menu_index = 0;
                }
                else
                {
                    player_ctrl->menu_index++;
                }

                GUI_LyrWinSel(player_ctrl->music_layer);
                GUI_SetFont(player_ctrl->font);
                GUI_UC_SetEncodeUTF8();
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetBkColor(0);
                __msg("player_ctrl->menu_index = %d", player_ctrl->menu_index);

                switch (player_ctrl->menu_index)
                {
                    case MUSIC_MENU_ICON:
                    {
                        //__here__;
                        DrawMusicMenuBtn(FOCUSED);
                        DrawMusicShowModeMenu(UNFOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                        {
                            //LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                            //player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                            //music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    case MUSIC_AB_ICON:
                    {
                        DrawMusicShowModeMenu(UNFOCUSED);
                        DrawMusicABMenu(FOCUSED);
                    }
                    break;

                    case MUSIC_SHOW_MENU_ICON:
                    {
                        //__here__;
                        DrawMusicMenuBtn(UNFOCUSED);
                        DrawMusicShowModeMenu(FOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE) //add by lee
                        {
                            // LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                            //   player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                            //   music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    case MUSIC_LIST_MENU_ICON:
                    {
                        //__here__;
                        DrawMusicMenuBtn(UNFOCUSED);
                        DrawMusicShowModeMenu(UNFOCUSED);
                        DrawMusicABMenu(UNFOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_TRUE)
                        {
                            // player_ctrl->list_ctrl->lost_focus = EPDK_FALSE;
                            //   LISTBAR_ShowPage(player_ctrl->list_ctrl->listbar_handle);
                            //  music_list_long_string_start_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    default:
                        break;
                }

#else
                LISTBAR_NextItem(player_ctrl->listbar_handle);
#endif
            }
            break;

            case GUI_MSG_KEY_LEFT:
            {
                music_player_ctrl_t *player_ctrl = NULL;
                __msg("GUI_MSG_KEY_LEFT");
                player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
#ifndef MUSIC_VIEW_CHANGE_20160811

                if (player_ctrl->menu_index <= 0)
                {
                    player_ctrl->menu_index = MUSIC_PLAY_BTN_ICON_MAX - 1;
                }
                else
                {
                    player_ctrl->menu_index--;
                }

                GUI_LyrWinSel(player_ctrl->music_layer);
                GUI_SetFont(player_ctrl->font);
                GUI_UC_SetEncodeUTF8();
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_SetBkColor(0);
                __msg("player_ctrl->menu_index = %d", player_ctrl->menu_index);

                switch (player_ctrl->menu_index)
                {
                    case MUSIC_MENU_ICON:
                    {
                        //__here__;
                        DrawMusicMenuBtn(FOCUSED);
                        DrawMusicShowModeMenu(UNFOCUSED);
                        DrawMusicABMenu(UNFOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                        {
                            //LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                            // player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                            // music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    case MUSIC_AB_ICON:
                    {
                        DrawMusicShowModeMenu(UNFOCUSED);
                        DrawMusicABMenu(FOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                        {
                            //LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                            //player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                            //music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    case MUSIC_SHOW_MENU_ICON:
                    {
                        //__here__;
                        DrawMusicMenuBtn(UNFOCUSED);
                        DrawMusicShowModeMenu(FOCUSED);
                        DrawMusicABMenu(UNFOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_FALSE)
                        {
                            //LISTBAR_LostFocus(player_ctrl->list_ctrl->listbar_handle);
                            //  player_ctrl->list_ctrl->lost_focus = EPDK_TRUE;
                            // music_list_long_string_stop_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    case MUSIC_LIST_MENU_ICON:
                    {
                        //__here__;
                        DrawMusicMenuBtn(UNFOCUSED);
                        DrawMusicShowModeMenu(UNFOCUSED);
                        DrawMusicABMenu(UNFOCUSED);

                        if (player_ctrl->list_ctrl->lost_focus == EPDK_TRUE)
                        {
                            //    player_ctrl->list_ctrl->lost_focus = EPDK_FALSE;
                            //    LISTBAR_ShowPage(player_ctrl->list_ctrl->listbar_handle);
                            //  music_list_long_string_start_roll(player_ctrl->list_ctrl);
                        }
                    }
                    break;

                    default:
                        break;
                }

#else
                LISTBAR_PrevItem(player_ctrl->listbar_handle);
#endif
            }
            break;

            case GUI_MSG_KEY_ACCOMP_ONOFF:
            {
                if (0 == msg->dwReserved)
                {
                    __s32 channel;
                    channel = robin_get_channel();
                    //channel = (channel + 1) % 3;                          //this is abandoned practice
                    robin_set_channel(channel);
#if 1
                    {
                        __s32 lang_id = STRING_MUSIC_TACK_STEREO;
                        __log("music_player_delete_digit_layer begin... channel = %d", channel);

                        if (0 == channel)
                        {
                            lang_id = STRING_MUSIC_TACK_STEREO;
                        }
                        else if (1 == channel)
                        {
                            lang_id = STRING_MUSIC_TACK_LEFT;
                        }
                        else if (2 == channel)
                        {
                            lang_id = STRING_MUSIC_TACK_RIGHT;
                        }

                        DrawShowTACKText(lang_id);
                    }
#endif
                }

                break;
            }
        }

        last_key = 0;
    }

    return EPDK_OK;
}


//检测播放状态,并根据playmode获得相关的文件,启动播放器
static __s32 music_player_check_play_status_on_timer(music_player_ctrl_t *this)
{
    __s32 index = 0;
    char *filename = NULL;
    __listbar_scene_t scene;
    music_list_ctrl_t *ListCtrl = NULL;
    __cedar_status_t  status = CEDAR_STAT_;
    status = robin_get_fsm_status();

    if (status == CEDAR_STAT_STOP)
    {
        music_player_reset_ab_string(this);

        if (this->play_mode == RAT_PLAY_MODE_ONLY_ONCE)
        {
            if (this->menu_mode == MUSIC_EXPLORER_MODE)
            {
                if (this->h_lyrics != NULL)
                {
                }
                else
                {
                    SPCTRM_Clear(0);        //显示最大值
                }
            }

            return EPDK_OK;
        }

        //if(this->play_mode == RAT_PLAY_MODE_RANDOM)
        {
            //return EPDK_OK;
        }
        index = robin_npl_get_next();
        __msg("--------robin npl get index = %d", index);

        if (index != -1)
        {
#if 0

            if (this->list_ctrl != NULL)
            {
                ListCtrl = this->list_ctrl;
                ListCtrl->play_index = index;
            }

            filename = this->play_file_info.filename;
            robin_npl_index2file(index, filename);
            __msg("--------------------------------robin npl get filename= %s", filename);
            //music_list_set_item_scene(ListCtrl, &(ListCtrl->listbar_play_scene));
            music_play_file(this, filename, 0);

            if (this->play_mode == RAT_PLAY_MODE_RANDOM)
            {
                //music_play_file(this, filename);
                LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
                music_list_set_focus_item(ListCtrl, index);
                LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
                ListCtrl->play_index = scene.focus_id;
                return EPDK_OK;
            }

            if (this->visiable)     //背景播放则不显示listbar
            {
                if ((this->play_mode == RAT_PLAY_MODE_ROTATE_ALL)
                    || (this->play_mode == RAT_PLAY_MODE_SEQUENCE))
                {
                    {
                        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                        __msg("-----------play index = %d", ListCtrl->play_index);
                        __msg("------------------listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);

                        if (ListCtrl->play_index != scene.focus_id + 1)
                        {
                            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
                        }
                        else
                        {
                            ListCtrl->OnNextKey((void *)ListCtrl);  //listbar on previous key
                        }

                        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                        __msg("------------------listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
                        ListCtrl->play_index = scene.focus_id;
                        __msg("-----------play index = %d", ListCtrl->play_index);
                    }
                }
            }

#else

            if (this->list_ctrl != NULL)
            {
                //__here__;
                ListCtrl = this->list_ctrl;
                ListCtrl->play_index = index;
            }
            else
            {
                //__here__;
                this->play_index = index;
            }

            __msg("++++++++++++++++++++++++++++auto play to next+++++++++++++++++++++++++++++++++++");
            __msg("this->play_file_info.filename = %s", this->play_file_info.filename);
            filename = this->play_file_info.filename;
            robin_npl_index2file(index, filename);
            __msg(" robin npl get filename= %s", filename);
            //music_list_set_item_scene(ListCtrl, &(ListCtrl->listbar_play_scene));
            music_play_file(this, filename, 0);
            //================chengf2 add begin
            __wrn("@@@@@ showMode = %d,menu_mode = %d", this->showMode, this->menu_mode);

            if ((MUSIC_SPECTRUM == this->showMode) && (MUSIC_EXPLORER_MODE == this->menu_mode))
            {
                __gui_msg_t my_msg;
                __wrn("@@@@@ update the spectrum ");
                eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
                my_msg.h_deswin = this->h_framewin;
                my_msg.h_srcwin = NULL;
                my_msg.id = GUI_MSG_COMMAND;
                my_msg.dwAddData1 = MUSIC_REBUILD_SPECTRUM;
                my_msg.dwAddData2 = 0;
                GUI_SendNotifyMessage(&my_msg);
            }

            //================chengf2 add end
            __msg("this->play_mode = %d", this->play_mode);
            __msg("this->visiable  = %d", this->visiable);

            if (this->play_mode == RAT_PLAY_MODE_RANDOM && ListCtrl != NULL)
            {
                LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
                music_list_set_focus_item(ListCtrl, index);
                LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
                ListCtrl->play_index = scene.focus_id;

                if (ListCtrl->lost_focus == EPDK_TRUE)
                {
                    // LISTBAR_LostFocus(ListCtrl->listbar_handle);
                }

                return EPDK_OK;
            }

            if (this->visiable)     //背景播放则不显示listbar
            {
                if ((this->play_mode == RAT_PLAY_MODE_ROTATE_ALL)
                    || (this->play_mode == RAT_PLAY_MODE_SEQUENCE))
                {
                    {
                        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                        __msg("-----------play index = %d", ListCtrl->play_index);
                        __msg("------------------listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);

                        if (ListCtrl->play_index != scene.focus_id + 1)
                        {
                            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
                        }
                        else
                        {
                            ListCtrl->OnNextKey((void *)ListCtrl);  //listbar on previous key
                        }

                        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                        __msg("------------------listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
                        ListCtrl->play_index = scene.focus_id;
                        __msg("-----------play index = %d", ListCtrl->play_index);

                        if (ListCtrl->lost_focus == EPDK_TRUE)
                        {
                            //  LISTBAR_LostFocus(ListCtrl->listbar_handle);
                        }
                    }
                }
            }

#endif
        }
    }

    return EPDK_OK;
}
__s32 music_player_get_last_para(music_player_ctrl_t *This)
{
    __s32 ret = 0;
    reg_music_para_t *music_last_para = NULL;
    reg_system_para_t *system_last_para = NULL;
    //__wait__;
    music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);
    system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    //This->play_index = music_last_para->play_index;       //获得实际播放的索引号
    This->EQ_mode = music_last_para->EQ_mode;
    This->play_mode = music_last_para->play_mode;
    __wrn("music_last_para->play_mode:%d", music_last_para->play_mode);
    //This->BL_time = music_last_para->BL_time;

    switch (This->play_mode)
    {
        case RAT_PLAY_MODE_SEQUENCE:
            break;

        case RAT_PLAY_MODE_ROTATE_ONE:
            break;

        case RAT_PLAY_MODE_ROTATE_ALL:
            break;

        case RAT_PLAY_MODE_ONLY_ONCE:
            break;

        case RAT_PLAY_MODE_RANDOM:
            break;

        default:
            This->play_mode = RAT_PLAY_MODE_SEQUENCE;
            break;
    }

    switch (This->EQ_mode)
    {
        case MUSIC_EQ_NORMAL:
            break;

        case MUSIC_EQ_ROCK:
            break;

        case MUSIC_EQ_CLASSIC:
            break;

        case MUSIC_EQ_JAZZ:
            break;

        case MUSIC_EQ_POP:
            break;

        default:
            This->EQ_mode = MUSIC_EQ_NORMAL;
            break;
    }

    switch (system_last_para->backlightoff)
    {
        case 0:
            This->BL_time = MUSIC_BL_ON;
            break;

        case 10:
            This->BL_time = MUSIC_BL_LAST_10S;
            break;

        case 30:
            This->BL_time = MUSIC_BL_LAST_30S;
            break;

        case 60:
            This->BL_time = MUSIC_BL_LAST_60S;
            break;

        case 90:
            This->BL_time = MUSIC_BL_LAST_90S;
            break;

        default:
            This->BL_time = MUSIC_BL_ON;
            break;
    }

    return EPDK_OK;
}

//保存注册表信息直接操作结构体
__s32 music_player_save_last_para(music_player_ctrl_t *This)
{
    __s32 ret = 0;
    reg_music_para_t *music_last_para;
    reg_system_para_t *system_last_para = NULL;
    system_last_para = dsk_reg_get_para_by_app(REG_APP_SYSTEM);
    music_last_para = dsk_reg_get_para_by_app(REG_APP_MUSIC);
    music_last_para->EQ_mode = This->EQ_mode;
    music_last_para->play_mode = This->play_mode;
    //music_last_para->BL_time = ;
#if 0

    switch (This->BL_time)
    {
        case MUSIC_BL_ON:
            system_last_para->backlightoff = 0;
            break;

        case MUSIC_BL_LAST_10S:
            system_last_para->backlightoff = 10;
            break;

        case MUSIC_BL_LAST_30S:
            system_last_para->backlightoff = 30;
            break;

        case MUSIC_BL_LAST_60S:
            system_last_para->backlightoff = 60;
            break;

        case MUSIC_BL_LAST_90S:
            system_last_para->backlightoff = 90;
            break;

        default:
            system_last_para->backlightoff = 0;
            break;
    }

#endif
    return EPDK_OK;
}
static void music_player_set_ab_mode(music_player_ctrl_t *this)
{
    switch (this->AB_mode)
    {
        case MUSIC_AB:
        {
            this->AB_mode = MUSIC_A;
            robin_set_ab_a_v2();
            robin_set_ab_loop_count_v2(1000);
        }
        break;

        case MUSIC_A:
        {
            this->AB_mode = MUSIC_B;
            robin_set_ab_b_v2();
            robin_enable_ab();
        }
        break;

        case MUSIC_B:
        {
            this->AB_mode = MUSIC_AB;
            robin_cancle_ab_v2();
        }
        break;

        default:
            break;
    }
}
static __s32 music_player_set_play_mode(music_player_ctrl_t *this)
{
#if 0

    switch (this->play_mode)
    {
        case RAT_PLAY_MODE_SEQUENCE:
            this->play_mode = RAT_PLAY_MODE_ROTATE_ONE;
            break;

        case RAT_PLAY_MODE_ROTATE_ONE:
            this->play_mode = RAT_PLAY_MODE_ROTATE_ALL;
            break;

        case RAT_PLAY_MODE_ROTATE_ALL:
            this->play_mode = RAT_PLAY_MODE_ONLY_ONCE;
            break;

        case RAT_PLAY_MODE_ONLY_ONCE:
            this->play_mode = RAT_PLAY_MODE_RANDOM;
            //this->play_mode = RAT_PLAY_MODE_SEQUENCE;
            break;

        case RAT_PLAY_MODE_RANDOM:
            this->play_mode = RAT_PLAY_MODE_SEQUENCE;
            break;

        default:
            break;
    }

    DrawMusicPlayModeStatus(this->play_mode);
#endif

    switch (this->play_mode)
    {
        case RAT_PLAY_MODE_SEQUENCE:
            this->play_mode = RAT_PLAY_MODE_SEQUENCE;
            break;

        case RAT_PLAY_MODE_ROTATE_ONE:
            this->play_mode = RAT_PLAY_MODE_ROTATE_ONE;
            break;

        case RAT_PLAY_MODE_ROTATE_ALL:
            this->play_mode = RAT_PLAY_MODE_ROTATE_ALL;
            break;

        case RAT_PLAY_MODE_ONLY_ONCE:
            this->play_mode = RAT_PLAY_MODE_ONLY_ONCE;
            break;

        case RAT_PLAY_MODE_RANDOM:
            this->play_mode = RAT_PLAY_MODE_RANDOM;
            break;

        default:
            this->play_mode = RAT_PLAY_MODE_SEQUENCE;
            break;
    }

    robin_set_play_mode(this->play_mode);
    return EPDK_OK;
}

static __s32 music_player_set_EQ_mode(music_player_ctrl_t *this)
{
    __cedar_audio_eq_t eq_mode;
#if 0

    switch (this->EQ_mode)
    {
        case MUSIC_EQ_NORMAL:
            this->EQ_mode = MUSIC_EQ_DBB;
            eq_mode = CEDAR_AUD_EQ_TYPE_DBB;
            break;

        case MUSIC_EQ_DBB:
            this->EQ_mode = MUSIC_EQ_POP;
            eq_mode = CEDAR_AUD_EQ_TYPE_POP;
            break;

        case MUSIC_EQ_POP:
            this->EQ_mode = MUSIC_EQ_ROCK;
            eq_mode = CEDAR_AUD_EQ_TYPE_ROCK;
            break;

        case MUSIC_EQ_ROCK:
            this->EQ_mode = MUSIC_EQ_CLASSIC;
            eq_mode = CEDAR_AUD_EQ_TYPE_CLASSIC;
            break;

        case MUSIC_EQ_CLASSIC:
            this->EQ_mode = MUSIC_EQ_JAZZ;
            eq_mode = CEDAR_AUD_EQ_TYPE_JAZZ;
            break;

        case MUSIC_EQ_JAZZ:
            this->EQ_mode = MUSIC_EQ_VOCAL;
            eq_mode = CEDAR_AUD_EQ_TYPE_VOCAL;
            break;

        case MUSIC_EQ_VOCAL:
            this->EQ_mode = MUSIC_EQ_DANCE;
            eq_mode = CEDAR_AUD_EQ_TYPE_DANCE;
            break;

        case MUSIC_EQ_DANCE:
            this->EQ_mode = MUSIC_EQ_SOFT;
            eq_mode = CEDAR_AUD_EQ_TYPE_SOFT;
            break;

        case MUSIC_EQ_SOFT:
            this->EQ_mode = MUSIC_EQ_NORMAL;
            eq_mode = CEDAR_AUD_EQ_TYPE_NORMAL;
            break;
    }

    DrawMusicEQModeStatus(this);
#endif

    switch (this->EQ_mode)
    {
        case MUSIC_EQ_NORMAL:
            this->EQ_mode = MUSIC_EQ_NORMAL;
            eq_mode = CEDAR_AUD_EQ_TYPE_NORMAL;
            break;

        case MUSIC_EQ_DBB:
            this->EQ_mode = MUSIC_EQ_DBB;
            eq_mode = CEDAR_AUD_EQ_TYPE_DBB;
            break;

        case MUSIC_EQ_POP:
            this->EQ_mode = MUSIC_EQ_POP;
            eq_mode = CEDAR_AUD_EQ_TYPE_POP;
            break;

        case MUSIC_EQ_ROCK:
            this->EQ_mode = MUSIC_EQ_ROCK;
            eq_mode = CEDAR_AUD_EQ_TYPE_ROCK;
            break;

        case MUSIC_EQ_CLASSIC:
            this->EQ_mode = MUSIC_EQ_CLASSIC;
            eq_mode = CEDAR_AUD_EQ_TYPE_CLASSIC;
            break;

        case MUSIC_EQ_JAZZ:
            this->EQ_mode = MUSIC_EQ_JAZZ;
            eq_mode = CEDAR_AUD_EQ_TYPE_JAZZ;
            break;

        case MUSIC_EQ_VOCAL:
            this->EQ_mode = MUSIC_EQ_DANCE;
            eq_mode = CEDAR_AUD_EQ_TYPE_DANCE;
            break;

        case MUSIC_EQ_DANCE:
            this->EQ_mode = MUSIC_EQ_DANCE;
            eq_mode = CEDAR_AUD_EQ_TYPE_DANCE;
            break;

        case MUSIC_EQ_SOFT:
            this->EQ_mode = MUSIC_EQ_SOFT;
            eq_mode = CEDAR_AUD_EQ_TYPE_SOFT;
            break;

        default :
            this->EQ_mode = MUSIC_EQ_NORMAL;
            eq_mode = CEDAR_AUD_EQ_TYPE_NORMAL;
    }

    robin_set_eq_mode(eq_mode);
    return EPDK_OK;
}
static __s32 music_player_set_BL_time(music_player_ctrl_t *this)
{
    __u32 time = 0;

    switch (this->BL_time)
    {
        case MUSIC_BL_ON:
            this->BL_time = MUSIC_BL_LAST_10S;
            time = 10;
            break;

        case MUSIC_BL_LAST_10S:
            this->BL_time = MUSIC_BL_LAST_30S;
            time = 30;
            break;

        case MUSIC_BL_LAST_30S:
            this->BL_time = MUSIC_BL_LAST_60S;
            time = 60;
            break;

        case MUSIC_BL_LAST_60S:
            this->BL_time = MUSIC_BL_LAST_90S;
            time = 90;
            break;

        case MUSIC_BL_LAST_90S:
            this->BL_time = MUSIC_BL_ON;
            time = 0;
            break;

        default:
            time = 0;
            break;
    }

    g_set_close_scn_time(time);
    DrawMusicBLTimeButton(this->BL_time, PRESSED);
#if BEETLES_RTC_EXIST
#else
    DrawMusicBLLastTimeStatus(this->BL_time);           //RTC 不存在显示背光时间
#endif
    return EPDK_OK;
}

static void  music_player_reset_ab_string(music_player_ctrl_t *this)
{
#if 0
    music_ui_t *ui;
    __u32 x0, y0, x1, y1;
    char text[128];
#endif
#ifndef MUSIC_VIEW_CHANGE_20160811

    if (MUSIC_AB != this->AB_mode)
    {
        this->AB_mode = MUSIC_AB;
        robin_cancle_ab_v2();
        DrawABText(STRING_MUSIC_AB);
#else
    extern __s32 music_get_ab_status(void);
    extern void music_set_ab_status(__s32 ab_mode);

    if (MUSIC_AB != music_get_ab_status())
    {
        music_set_ab_status(MUSIC_AB);
        robin_cancle_ab_v2();
#endif
#if 0
        ui = music_get_ui_param();
        x0 = ui->bottom.AB_rect.x + 35;
        y0 = ui->bottom.AB_rect.y - 5;
        x1 = x0 + 42;
        y1 = y0 + 32;
        GUI_ClearRect(x0, y0, x1, y1);
        GUI_SetColor(GUI_WHITE);
        dsk_langres_get_menu_text(STRING_MUSIC_AB, text, sizeof(text));
        GUI_DispStringAt(text, ui->bottom.AB_rect.x + 35, ui->bottom.AB_rect.y + (28 - 16) / 2 - 2);
#endif
    }
}

static __s32 music_player_set_play_previous(music_player_ctrl_t *this)
{
    __s32 ret;
    music_list_ctrl_t *ListCtrl;
    char *filename;
    __listbar_scene_t scene;
    music_player_reset_ab_string(this);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    robin_set_cmd_stop();
    DrawMusicPlayPreButton(PRESSED);
    ListCtrl = this->list_ctrl;
    filename = this->play_file_info.filename;
    __msg("------------play previous file");
    //__wait__;
    {
        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);

        if (ListCtrl->play_index != scene.focus_id)
        {
            __msg("------------------current focus item has been moved!");
            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
        }
    }                   //
    ListCtrl->OnPreviousKey((void *)ListCtrl);                  //listbar on previous key
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    ListCtrl->play_index = scene.focus_id;
    ret = get_music_list_focus_item_full_filename(ListCtrl, filename);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    music_play_file(this, filename, 0);

    if (robin_get_fsm_status() == CEDAR_STAT_PLAY)
    {
        __wrn("+++++++++++++++++ MUSIC_LIST_MENU_ICON 7");
        //..robin_set_cmd_pause();
        gscene_hbar_set_music_state(BG_MUSIC);
    }//..

    if (ListCtrl->lost_focus == EPDK_TRUE)
    {
        //  LISTBAR_LostFocus(ListCtrl->listbar_handle);
    }

    return EPDK_OK;
}
static __s32 music_player_set_play_next(music_player_ctrl_t *this)
{
    __s32 ret;
    music_list_ctrl_t *ListCtrl;
    char *filename;
    __listbar_scene_t scene;
    music_player_reset_ab_string(this);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    robin_set_cmd_stop();
    DrawMusicPlayNextButton(PRESSED);
    ListCtrl = this->list_ctrl;
    filename = this->play_file_info.filename;
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    //show_music_list(ListCtrl);
    __msg("------------play next file");
    {
        LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);

        if (ListCtrl->play_index != scene.focus_id)
        {
            music_list_set_focus_item(ListCtrl, ListCtrl->play_index);
        }
    }
    ListCtrl->OnNextKey((void *)ListCtrl);                  //listbar on previous key
    LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
    ListCtrl->play_index = scene.focus_id;
    //music_list_set_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));
    ret = get_music_list_focus_item_full_filename(ListCtrl, filename);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    music_play_file(this, filename, 0);

    if (robin_get_fsm_status() == CEDAR_STAT_PLAY)
    {
        __wrn("+++++++++++++++++ MUSIC_LIST_MENU_ICON 7");
        //..robin_set_cmd_pause();
        gscene_hbar_set_music_state(BG_MUSIC);
    }

    if (ListCtrl->lost_focus == EPDK_TRUE)
    {
        //LISTBAR_LostFocus(ListCtrl->listbar_handle);
    }

    return EPDK_OK;
}

//reg_root_type，0:sd 1:u盘
static __s32 music_player_switch_disk(music_player_ctrl_t *this, __u32 reg_root_type)
{
    __s32 ret;
    music_list_ctrl_t *ListCtrl;
    char *filename;
    __listbar_scene_t scene;
    __wrn("music_player_switch_disk, reg_root_type=%d", reg_root_type);

    if (NULL == this || NULL == this->h_framewin)
    {
        return EPDK_FAIL;
    }

    music_send_command(this->h_framewin, GUI_MSG_COMMAND, SWITCH_DISK, reg_root_type);
    return EPDK_OK;
}

static __s32 music_player_set_play_pause(music_player_ctrl_t *this, music_btn_status_e status)
{
    DrawMusicPlayPauseButton(status);

    if (robin_get_fsm_status() == CEDAR_STAT_PLAY)
    {
        robin_set_cmd_pause();
        DrawMusicPlayStatusButton(MUSIC_PAUSE);
        gscene_hbar_set_music_state(BG_PAUSE);
    }
    else
    {
        robin_set_cmd_play();
        DrawMusicPlayStatusButton(MUSIC_PLAYING);
        gscene_hbar_set_music_state(BG_MUSIC);
    }

    return EPDK_OK;
}

//不管在什么模式，对是否存在歌词进行检测
__s32  music_player_check_lyrics(music_player_ctrl_t *this, __bool bUpdate)
{
    if (this == NULL)
    {
        return EPDK_FAIL;
    }

    delete_music_lyrics_scene(this);
    {
        if (music_lyrics_exist())       //检查是否有歌词
        {
            //有歌词则不显示频谱
            if (create_music_lyrics_scene(this) != EPDK_FAIL)
            {
                __msg("---------------create music lyrics scene successfully!");
            }
            else
            {
                __wrn("---------------create music lyrics scene failed!");
            }

            //__here__;
            if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
            {
                __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                __wrn("g_set_close_scn_time=%d", 0);
                g_set_close_scn_time(0);
            }
        }
        else
        {
            //__here__;
            if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
            {
                __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                {
                    //恢复系统自动关屏时间
                    reg_system_para_t *para;
                    para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                    if (para)
                    {
                        __wrn("g_set_close_scn_time=%d", para->backlightoff);
                        g_set_close_scn_time(para->backlightoff);
                    }
                }
            }
        }
    }
    return EPDK_OK;
}
static __s32 music_player_set_play_backward(music_player_ctrl_t *this)
{
    __s32 ret = 0;
    __u32 total_time = 0, cur_time = 0;
    __cedar_status_t play_status;
    DrawMusicPlayBackwardButton(PRESSED);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    //__here__;
    total_time = robin_get_total_time();
    play_status = robin_get_fsm_status();

    if (play_status == CEDAR_STAT_PAUSE)
    {
        robin_set_cmd_play();           //后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        cur_time = robin_get_cur_time();

        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }
    else if (play_status == CEDAR_STAT_STOP)
    {
        //__here__;
        this->CheckFocusItemCnt = 0;
        __msg("2------------check focus item on timer");
        music_player_win_check_focus_on_timer(this);    //检测当前焦点是否播放条目
        robin_set_cmd_play();           //后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        cur_time = robin_get_cur_time();

        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        DrawMusicPlayingTime(cur_time);
        DrawMusicProgressBar(total_time, cur_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }
    else if (play_status == CEDAR_STAT_RR)
    {
        cur_time = robin_get_cur_time();

        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        DrawMusicPlayingTimeState(cur_time, total_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }

    robin_set_ff_rr_speed(MUSIC_RR_SPEED);
    //robin_set_cmd_play();
    ret = robin_set_cmd_rr();

    if (ret == -1)
    {
        robin_set_cmd_play();           //后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        DrawMusicProgressBar(0, 0);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }

    this->setting_status = CEDAR_STAT_RR;
    cur_time = robin_get_cur_time();

    if (cur_time > total_time)
    {
        cur_time = total_time;
    }

    DrawMusicPlayingTime(cur_time);
    DrawMusicProgressBar(total_time, cur_time);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    return EPDK_OK;
}

static __s32 music_player_set_play_forward(music_player_ctrl_t *this)
{
    __s32 ret = 0;
    __u32 total_time = 0, cur_time = 0;
    __cedar_status_t play_status;
    DrawMusicPlayForwardButton(PRESSED);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    total_time = robin_get_total_time();
    //__here__;
    play_status = robin_get_fsm_status();

    if (play_status == CEDAR_STAT_PAUSE)
    {
        robin_set_cmd_play();
        music_player_check_lyrics(this, EPDK_FALSE);
        cur_time = robin_get_cur_time();

        //__here__;
        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        //DrawMusicPlayingTime(cur_time);
        //DrawMusicProgressBar(total_time, cur_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }
    else if (play_status == CEDAR_STAT_STOP)
    {
        //__here__;
        robin_set_cmd_stop();
        this->CheckFocusItemCnt = 0;
        __msg("2------------check focus item on timer");
        music_player_win_check_focus_on_timer(this);    //检测当前焦点是否播放条目
        music_player_check_lyrics(this, EPDK_FALSE);
        cur_time = robin_get_cur_time();

        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        //DrawMusicPlayingTime(cur_time);
        //DrawMusicProgressBar(total_time, cur_time);
        this->setting_status = CEDAR_STAT_PLAY;
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }
    else if (play_status == CEDAR_STAT_FF)
    {
        cur_time = robin_get_cur_time();

        //__here__;
        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        DrawMusicPlayingTimeState(cur_time, total_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }

    robin_set_ff_rr_speed(MUSIC_FF_SPEED);
    //__here__;
    ret = robin_set_cmd_ff();

    //__here__;
    if (ret == -1)
    {
        robin_set_cmd_play();           //后退失败,则继续播放
        music_player_check_lyrics(this, EPDK_FALSE);
        DrawMusicProgressBar(total_time, total_time);
        GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
        return EPDK_OK;
    }

    //__here__;
    this->setting_status = CEDAR_STAT_FF;
    cur_time = robin_get_cur_time();

    //__here__;
    if (cur_time > total_time)
    {
        cur_time = total_time;
    }

    DrawMusicProgressBar(total_time, cur_time);
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    return EPDK_OK;
}

static __s32 music_player_on_setting(music_player_ctrl_t *this)
{
    switch (this->menu_id)
    {
        case SET_PLAY_MODE:
            music_player_set_play_mode(this);
            break;

        case SET_EQ_MODE:
            music_player_set_EQ_mode(this);
            break;

        case SET_BL_TIME:
            music_player_set_BL_time(this);
            break;

        case SET_PLAY_PREVIOUS:
            music_player_set_play_previous(this);
            break;

        case SET_PLAY_NEXT:
            music_player_set_play_next(this);
            break;

        case SET_PLAY_PAUSE:
            music_player_set_play_pause(this, PRESSED);
            break;

        case SET_PLAY_BACKWARD:
            music_player_set_play_backward(this);
            break;

        case SET_PLAY_FORWARD:
            music_player_set_play_forward(this);
            break;
    }

    return EPDK_OK;
}

static __s32 music_player_on_setting_touch(music_player_ctrl_t *this)
{
    switch (this->menu_touch_id)
    {
        case SET_PLAY_PAUSE_TOUCH:
            music_player_set_play_pause(this, PRESSED);
            break;

        case SET_PLAY_PREVIOUS_TOUCH:
            music_player_set_play_previous(this);
            break;

        case SET_PLAY_NEXT_TOUCH:
            music_player_set_play_next(this);
            break;

        case SET_PLAY_BACKWARD_TOUCH:
            music_player_set_play_backward(this);
            break;

        case SET_PLAY_FORWARD_TOUCH:
            music_player_set_play_forward(this);
            break;
    }

    return EPDK_OK;
}
//设置播放器按键处理函数
static __s32 music_setting_player_key_proc(__gui_msg_t *msg)
{
    static __u32 last_key = 0;
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if ((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        this->AutoExitMenuCnt = 0;

        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
#if 0
                DrawMusicPlayerContrlButtonByID(this, this->menu_id, UNFOCUSED);

                if (this->menu_id == SET_PLAY_FORWARD)
                {
                    this->menu_id = SET_PLAY_MODE;
                }
                else
                {
                    this->menu_id++;
                }

                DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
#else

                if (this->menu_touch_id == SET_PLAY_NEXT_TOUCH)
                {
                    this->menu_touch_id = SET_PLAY_PAUSE_TOUCH;
                }
                else
                {
                    this->menu_touch_id++;
                }

                DrawMusicPlayerInfo(this, this->menu_touch_id);
#endif
                break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
#if 0
                DrawMusicPlayerContrlButtonByID(this, this->menu_id, UNFOCUSED);

                if (this->menu_id == SET_PLAY_MODE)
                {
                    this->menu_id = SET_PLAY_FORWARD;
                }
                else
                {
                    this->menu_id--;
                }

                DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);
#else

                if (this->menu_touch_id == SET_PLAY_PAUSE_TOUCH)
                {
                    this->menu_touch_id = SET_PLAY_NEXT_TOUCH;
                }
                else
                {
                    this->menu_touch_id--;
                }

                DrawMusicPlayerInfo(this, this->menu_touch_id);
#endif
                break;

            case GUI_MSG_KEY_VDEC:
                break;

            //case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_MENU:
                last_key = GUI_MSG_KEY_MENU;
                break;
#if 0

            case GUI_MSG_KEY_VADD:
                last_key = GUI_MSG_KEY_VADD;
                break;
#endif

            case GUI_MSG_KEY_ENTER:
                last_key = GUI_MSG_KEY_ENTER;

                if (KEY_DOWN_ACTION == msg->dwAddData2)
                {
#if 0
                    music_player_on_setting(this);
#else
                    __log(".....layer = %8x", this->list_ctrl->layer);
                    DrawMusicPlayerInfo(this, -1);
                    __log(".....layer = %8x", this->list_ctrl->layer);
                    music_player_on_setting_touch(this);
                    __log(".....layer = %8x", this->list_ctrl->layer);
#endif
                }

                break;

            //case GUI_MSG_KEY_VADD:
            case GUI_MSG_KEY_ESCAPE:
#if 0
                music_send_command(msg->h_deswin, 0, SWITCH_TO_EXPLORER, 0);
#else
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.h_srcwin = msg->h_deswin;
                    my_msg.id = DSK_MSG_HOME;
                    my_msg.dwAddData1 = 0;
                    GUI_SendNotifyMessage(&my_msg);
                    //GUI_SendMessage(&my_msg);
                }
#endif
                break;

            case GUI_MSG_KEY_RISE:
                break;

            default:
                break;
        }
    }
    else if (KEY_UP_ACTION == msg->dwAddData2)
    {
        switch (msg->dwAddData1)
        {
            //case GUI_MSG_KEY_ENTER:
            case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_LONGMENU:
                if (last_key == GUI_MSG_KEY_MENU)
                {
#if 0

                    if (this->menu_mode == MUSIC_SET_PLAYER_MODE)
                    {
                        //DrawMusicPlayerContrlButton(this);
                        ClearMusicPlayerContrlButtonBG();

                        if (this->h_lyrics != NULL)
                        {
                            //if(this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
                            {
                                this->RefreshLyricsTimerCnt = 0;
                                update_music_lyrics(this->h_lyrics);
                            }
                        }
                        else
                        {
                            SPCTRM_Clear(0);
                            SPCTRM_Update((H_WIN)(this->h_framewin));
                            //draw spectrum();//画频谱图
                        }

                        this->menu_mode = MUSIC_EXPLORER_MODE;
                    }

#else
                    DrawMusicPlayerInfo(this, -1);
                    this->menu_mode = MUSIC_EXPLORER_MODE;
#endif
                }

                break;

            case GUI_MSG_KEY_ENTER:
                if (last_key == GUI_MSG_KEY_ENTER)
                {
#if 0
                    DrawMusicPlayerContrlButtonByID(this, this->menu_id, FOCUSED);  //短按键抬起
#else
                    __log(".....layer = %8x", this->list_ctrl->layer);
                    com_memdev_create(this->music_layer);
                    DrawMusicPlayerInfo(this, this->menu_touch_id);
                    com_memdev_delete();
                    __log(".....layer = %8x", this->list_ctrl->layer);
#endif
                }

                break;
        }

        last_key = 0;
    }

    return EPDK_OK;
}

//按键消息处理函数
static __s32 music_player_win_key_proc(__gui_msg_t *msg)
{
    music_player_ctrl_t *this = NULL;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    //  //__here__;
    __wrn("this->menu_mode:%d", this->menu_mode);

    switch (this->menu_mode)
    {
        case MUSIC_EXPLORER_MODE:
            music_explorer_key_proc(msg);           //浏览器on key process
            break;

        case MUSIC_SET_PLAYER_MODE:
            music_setting_player_key_proc(msg);     //setting player on key process
            break;

        default:
            break;
    }

    return EPDK_OK;
}

__s32 music_player_win_check_focus_on_timer(music_player_ctrl_t *this)
{
    __listbar_scene_t scene;

    if (this == NULL)
    {
        return EPDK_FAIL;
    }

    if (!this->visiable)
    {
        return EPDK_FAIL;
    }

    if (this->list_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    LISTBAR_GetScene(this->list_ctrl->listbar_handle, &scene);
    __msg("listbar get scene focusid = %d, start id = %d", scene.focus_id, scene.start_id);
    __msg("this->list_ctrl->play_index = %d", this->list_ctrl->play_index);

    if (this->list_ctrl->play_index != scene.focus_id)
    {
        music_list_set_focus_item(this->list_ctrl, this->list_ctrl->play_index);
        __msg("++++++++set focus item to play index+++++++++++");
    }

    return EPDK_OK;
}


__s32 music_player_win_on_sys_timer(music_player_ctrl_t *this)
{
#if BEETLES_RTC_EXIST
    {
        //更新系统时间
        __awos_time_t sys_time = {0, 0, 0};

        if (this == NULL)
        {
            return EPDK_FAIL;
        }

        if (this->visiable)
        {
            this->RefreshSystimeCnt++;

            //__msg("1-----music on systime....tick = %d", this->RefreshSystimeCnt);
            if (this->RefreshSystimeCnt > MUSIC_REFRESH_SYSTIME_CNT)
            {
                this->RefreshSystimeCnt = 0;
                esTIME_GetTime(&sys_time);
                //__msg("2------------------------------music display systime, minute = %d",sys_time.minute);
                DrawMusicSystemTime(&sys_time);
            }
        }
    }
#endif
    {
        if (!this->visiable)
        {
            return EPDK_FAIL;
        }

        this->CheckFocusItemCnt++;
        //__msg("1--------------check focus item on timer, tick = %d", this->CheckFocusItemCnt);
        if (this->CheckFocusItemCnt > MUSIC_CHECK_FOCUS_TIMER_CNT)
        {
            this->CheckFocusItemCnt = 0;
            //__msg("2------------check focus item on timer");
            music_player_win_check_focus_on_timer(this);
        }
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function   :           static __s32 music_player_win_on_timer(msg)
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 music_player_win_on_timer(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    __u32 total_time = 0, cur_time = 0;
    __cedar_status_t play_status;
    GUI_MEMDEV_Handle   draw_mem;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (this->alarmComing)
    {
        return EPDK_TRUE;
    }

    //当后退超过0时，会进入stop状态，此时应重新play

    if ((this->setting_status == CEDAR_STAT_FF) || (this->setting_status == CEDAR_STAT_RR))
    {
        play_status = robin_get_fsm_status();

        if ((play_status == CEDAR_STAT_FF)
            || (play_status == CEDAR_STAT_RR))
        {
            robin_set_cmd_play();
        }

        if (play_status == CEDAR_STAT_STOP)
        {
            total_time = robin_get_total_time();

            if (this->visiable)
            {
                DrawMusicProgressBar(total_time, total_time);
            }

            if (this->setting_status == CEDAR_STAT_RR)
            {
                robin_set_cmd_play();
            }

            this->setting_status = CEDAR_STAT_PLAY;
            return EPDK_OK;
        }

        this->setting_status = CEDAR_STAT_PLAY;
        return EPDK_OK;
    }
    else
    {
        music_player_check_play_status_on_timer(this);  //check status for play mode
    }

    if (this->visiable)
    {
        //__msg("--------------------music player on timer");
        if (this->list_ctrl != NULL)
        {
            if (this->list_ctrl->total == 0)
            {
                return EPDK_FAIL;
            }
        }
        else
        {
            return EPDK_FAIL;
        }

        GUI_LyrWinSel(this->music_layer);

        if (this->RefreshPlayTimeCnt++ >= MUSIC_REFRESH_PLAYTIME_CNT)
        {
#if BEETLES_RTC_EXIST
            {
                //__awos_time_t sys_time;
                //esTIME_GetTime(&sys_time);
                //DrawMusicSystemTime(&sys_time);
            }
#endif
            total_time = robin_get_total_time();

            if (this->isTouchPlayingBar)
            {
                cur_time = this->curTouchPlayingBarTime;
            }
            else
            {
                cur_time = robin_get_cur_time();
            }

            DrawMusicPlayingTimeState(cur_time, total_time);
        }

        //  __msg("this->showMode = %d", this->showMode);
        play_status = robin_get_fsm_status();//add by Kingvan

        if (this->menu_mode == MUSIC_EXPLORER_MODE)
        {
            if (this->h_lyrics != NULL)
            {
                if (this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
                {
                    this->RefreshLyricsTimerCnt = 0;
                    update_music_lyrics(this->h_lyrics);
                }
            }

            if (this->showMode == MUSIC_SPECTRUM)
            {
                if (CEDAR_STAT_PLAY == play_status) //add by Kingvan
                {
                    SPCTRM_Update((H_WIN)(this->h_framewin));
                }
            }
            else
            {
                //  __msg("+++++++++++++++++++++++++++++");
            }
        }
        else
        {
#if 0

            //add by Kingvan
            if (this->h_lyrics != NULL && this->showMode == MUSIC_LYRICS)
            {
                if (this->RefreshLyricsTimerCnt++ > MUSIC_REFRESH_LYRICS_CNT)
                {
                    this->RefreshLyricsTimerCnt = 0;
                    update_music_lyrics(this->h_lyrics);
                }
            }
            else if (this->showMode == MUSIC_SPECTRUM)
            {
                if (CEDAR_STAT_PLAY == play_status)
                {
                    SPCTRM_Update((H_WIN)(this->h_framewin));
                }
            }
            else
            {
                __msg("+++++++++++++++++++++++");
            }

            this->AutoExitMenuCnt++;

            if (this->AutoExitMenuCnt > C_AUTO_EXIT_MENU_TIME)
            {
                this->menu_mode = MUSIC_EXPLORER_MODE;
                this->AutoExitMenuCnt = 0;
                DrawMusicPlayerInfo(this, -1);
            }

#endif
        }
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :           static __s32 music_player_win_on_close(__gui_msg_t *msg)
*
*Description    :               OnClose处理函数，释放FrameWin
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 music_player_win_on_close(__gui_msg_t *msg)
{
    return GUI_FrmWinDelete(msg->h_deswin);         //send destroy message
}

/*
************************************************************************************************************************
*Function   :           static __s32  music_player_win_on_destroy(__gui_msg_t *msg)
*
*Description    :               OnDestroy处理函数，释放framewin 所有申请的资源
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32  music_player_win_on_destroy(__gui_msg_t *msg)
{
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (this == NULL)
    {
        return EPDK_FAIL;
    }

    if (NULL == this->h_framewin)
    {
        return EPDK_FAIL;
    }

    if (GUI_IsTimerInstalled(this->h_framewin, MusicSysTimerId))
    {
        GUI_KillTimer(this->h_framewin, MusicSysTimerId);
        __msg("--------system time timer has been killed");
    }

    if (GUI_IsTimerInstalled(this->h_framewin, MusicCheckFocusTimerId))
    {
        GUI_KillTimer(this->h_framewin, MusicCheckFocusTimerId);
        __msg("--------check focus item timer has been killed");
    }

    //app_print_memory_usage("music player on destroy", NULL);
    music_player_delete_digit_layer(msg->h_deswin);
    delete_music_lyrics_scene(this);
    music_player_save_last_para(this);

    if (this->list_ctrl)
    {
        //__here__;
        delete_music_list(this->list_ctrl);
        this->list_ctrl = NULL;        //ADD XC
    }
    else
    {
        //__here__;
        dsk_reg_save_cur_play_info(REG_APP_MUSIC, this->play_index, this->play_file_info.filename, this->root_type);
    }

    //__here__;
    __music_save_breakpoint_info(this->play_index);
    //__here__;
    music_player_device_uninit(this);
    //__here__;
    eLIBs_memset(this, 0, sizeof(music_player_ctrl_t));
    esMEMS_Mfree(0, this);
    //__here__;
    music_free_icon_res();
    //__here__;
    //app_print_memory_usage("music player on destroy finished", NULL);
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :          static __s32 music_player_scene_create(H_WIN  music_frame_win)
*
*Description    :       music player 场景创建
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 music_player_scene_create(H_WIN  music_frame_win)
{
    music_player_ctrl_t *this;
    music_list_para_t list_para;
    __bool ret = 0;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);
    this->h_framewin = music_frame_win;                                     //保存窗口句柄
    __wrn("music player is initializing!");
    list_para.framewin = this->h_framewin;
    list_para.layer = this->music_layer;
    list_para.visiable = this->visiable;
    list_para.root_type = this->root_type;
    list_para.media_type = this->media_type;
    list_para.font = this->font;
    list_para.explorer_play_music = this->explorer_play_music;
    list_para.music_file_name = this->music_file_name;
    //app_print_memory_usage("new music list", NULL);
    __wrn("--------list_para.music_file_name = %s-------", list_para.music_file_name);

    if (list_para.visiable == EPDK_TRUE)
    {
        this->list_ctrl = new_music_list(&list_para);

        if (NULL == this->list_ctrl)
        {
            __wrn("-----------list ctrl is null----------");
            return EPDK_FAIL;
        }
    }
    else
    {
        this->list_ctrl = NULL;
        GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_SUSPEND);
    }

    music_player_get_last_para(this);

    if (this->list_ctrl != NULL)
    {
        char *filename = this->play_file_info.filename;
        __wrn("filename %s", filename);
        //get_music_list_focus_item_full_filename(this->list_ctrl, filename);
        ret = MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);
        __wrn("filename = %s", filename);

        if (EPDK_FAIL == ret)
        {
            __wrn("get file path fail ....");
            return EPDK_FAIL;
        }

        rat_set_file_for_play(this->list_ctrl->rat_handle, filename);
        this->play_index = this->list_ctrl->play_index;
    }
    else
    {
        music_list_ctrl_t list_ctrl;
        char *filename = this->play_file_info.filename;
        __wrn("filename %s", filename);

        if (init_bg_music_list(&list_ctrl, &list_para) == EPDK_FAIL)
        {
            __log("init_bg_music_list = %d", ret);
            return EPDK_FAIL;
        }

        ret = MusicGetListItemFileFullPath(list_ctrl.rat_handle, list_ctrl.play_index, filename);
        __wrn("filename %s", filename);

        if (EPDK_FAIL == ret)
        {
            __wrn("get file full path ret = %d", ret);
            return EPDK_FAIL;
        }

        rat_set_file_for_play(list_ctrl.rat_handle, filename);
        this->play_index = list_ctrl.play_index;
    }

    //this->play_mode = RAT_PLAY_MODE_SEQUENCE;
    //this->EQ_mode = MUSIC_EQ_NORMAL;
    music_player_device_init(this);

    //app_print_memory_usage("music player win on create message end", NULL);

    /*
        {
            char *filename = player_ctrl->play_file_info.filename;
            get_music_list_focus_item_full_filename(player_ctrl->list_ctrl, filename);
            music_play_file(player_ctrl, filename);
        }
    */

    if (this->visiable)
    {
        ret = GUI_SetTimer(music_frame_win, MusicSysTimerId, MUSIC_SYSTIMER_INTERVAL, NULL);                        //系统时间刷新定时器

        if (!ret)
        {
            __wrn("creat timer for system time failed");
        }
        else
        {
            __wrn("creat timer for system time successfully");
        }

        /*
        ret = GUI_SetTimer(music_frame_win, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);        //检测是否foucus item 与当前播放index一致
        if(!ret)
        {
            __wrn("creat timer for check focus item failed");
        }
        else
        {
            __msg("creat timer for check focus item successfully");
        }
        */
#ifdef MUSIC_VIEW_CHANGE_20160811

        if (this->showMode == MUSIC_SPECTRUM)
        {
            music_send_parent(this->h_framewin, GUI_MSG_COMMAND, MUSICPLAYER_MUSIC_SPCTRM_CREATE, 0, 0);
        }

#endif
    }

    return EPDK_OK;
}


/*
************************************************************************************************************************
*Function   :          static __s32 music_player_win_on_paint(H_WIN  music_frame_win)
*
*Description    :       on paint message
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/


static __s32 music_player_win_on_paint(H_WIN  music_frame_win)
{
    __s32 ret = 0;
    music_player_ctrl_t *player_ctrl = NULL;
    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);

    if (GUI_LyrWinGetSta(player_ctrl->music_layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    __msg("music player on paint message");

    //__wait__;

    //   __wrn("player_ctrl->visiable=%d", player_ctrl->visiable);
    if (player_ctrl->visiable)
    {
        GUI_LyrWinSetTop(player_ctrl->music_layer);
        MusicSetTransColor();
        __wrn("player_ctrl->visiable=%d", player_ctrl->visiable);
        GUI_LyrWinSel(player_ctrl->music_layer);
        DrawMusicBg();
        DrawMusicMenuAll(player_ctrl);
        //DrawMusicInfoBg();
        //DrawMusicPlayBar();
        //DrawMusicBottomBg();
        //DrawMusicBottom(player_ctrl,-1);
        show_music_list(player_ctrl->list_ctrl);
        //DrawMusicHeadbar(player_ctrl);
    }

    //app_print_memory_usage("music player on paint message, & ready to play mp3 file", NULL);

    if (player_ctrl->list_ctrl != NULL)
    {
        music_list_get_item_scene(player_ctrl->list_ctrl, &(player_ctrl->list_ctrl->listbar_play_scene));
        __wrn("player_ctrl->list_ctrl->listbar_play_scene.focus_id = %d", player_ctrl->list_ctrl->listbar_play_scene.focus_id);
        __wrn("player_ctrl->list_ctrl->listbar_play_scene.start_id = %d", player_ctrl->list_ctrl->listbar_play_scene.start_id);
    }

    __wrn("player_ctrl->visiable=%d", player_ctrl->visiable);
    //DrawMusicPlayer(player_ctrl);
    {
        char *filename = player_ctrl->play_file_info.filename;

        if (player_ctrl->list_ctrl != NULL)
        {
            ret = MusicGetListItemFileFullPath(player_ctrl->list_ctrl->rat_handle, player_ctrl->list_ctrl->play_index, filename);

            if (EPDK_FAIL == ret)
            {
                return EPDK_FAIL;
            }
        }

        __wrn("filename=%s", filename);
        //if(robin_get_fsm_status() != CEDAR_STAT_PLAY)
        {
#ifdef C_MUSIC_DEBUG
#else
            ret = music_play_file(player_ctrl, filename, 1);
            __wrn("player_ctrl->visiable=%d", player_ctrl->visiable);

            if (player_ctrl->visiable)
            {
                __wrn("before DrawMusicPlayerInfo");

                if (player_ctrl->root_type == RAT_TF)
                {
                    DrawMusicPlayerInfo(player_ctrl, 6);
                }
                else if (player_ctrl->root_type == RAT_USB)
                {
                    DrawMusicPlayerInfo(player_ctrl, 5);
                }

#if 0
                {
                    //后台恢复后，重设当前焦点
                    music_list_ctrl_t *ListCtrl = NULL;
                    __listbar_scene_t scene;
                    __s32 index;

                    //__here__;
                    if (player_ctrl)
                    {
                        //__here__;
                        ListCtrl = player_ctrl->list_ctrl;

                        if (ListCtrl)
                        {
                            //__here__;
                            LISTBAR_GetScene(ListCtrl->listbar_handle, &scene);
                            __wrn("scene.focus_id = %d, scene.start_id = %d", scene.focus_id, scene.start_id);
                            index = robin_npl_get_cur();
                            __wrn("index=%d", index);

                            if (-1 != index)
                            {
                                //music_list_set_focus_item(ListCtrl, index);
                            }
                        }
                    }
                }
#endif
            }

#endif
        }
#ifdef C_MUSIC_DEBUG
#else
        GUI_SetTimer(music_frame_win, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);//play之后才可以打开timer
#endif
    }
    //app_print_memory_usage("player is playing", NULL);
    return EPDK_OK;
}

//extern __s32 music_list_long_string_stop_roll(music_list_ctrl_t *this);
__s32 music_player_scene_delete(H_WIN  music_frame_win)
{
    __s32 ret = 0;
    music_player_ctrl_t *this;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);
    __wrn("*****begin music_player_scene_delete********");

    if (!this->visiable)
    {
        __wrn("this->visiable is false");
        return EPDK_FAIL;
    }

    this->visiable = EPDK_FALSE;
    /*
    if (GUI_LyrWinGetSta(this->music_layer) == GUI_LYRWIN_STA_ON)
    {
        GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_OFF);
    }
    else
    {
        //return EPDK_OK;
    }
    */

    if (GUI_IsTimerInstalled(music_frame_win, MusicSysTimerId))
    {
        GUI_KillTimer(music_frame_win, MusicSysTimerId);
        __msg("----------system time timer has been killed");
    }

    if (GUI_IsTimerInstalled(music_frame_win, MusicCheckFocusTimerId))
    {
        GUI_KillTimer(music_frame_win, MusicCheckFocusTimerId);
        __msg("----------check focus item timer has been killed");
    }

#if 0
    delete_music_lyrics_scene(this);
    music_player_save_last_para(this);
    //clear_music_list_area();
    //delete_music_list(this->list_ctrl);
    //this->list_ctrl = NULL;
    music_list_long_string_stop(this->list_ctrl);
    //music_list_long_string_stop_roll(this->list_ctrl);
    //esMEMS_Bfree(this, sizeof(music_player_ctrl_t));
    music_free_icon_res();
#else
    delete_music_lyrics_scene(this);
    music_player_save_last_para(this);
    //__here__;
    delete_music_list(this->list_ctrl);
    this->list_ctrl = NULL;
    music_free_icon_res();
#endif
    __wrn("******after music_player_scene_delete*******");
    return EPDK_OK;
}

__s32  musicplayer_rat_get_search_path(__s32 root_type, char *search_path)
{
    __s32 ret = 0;
    char s_root_type[50];
    char disk_name[RAT_MAX_PARTITION][4];
    eLIBs_memset(s_root_type, 0, sizeof(s_root_type));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));
    //eLIBs_memset(list_ctrl->search_path, 0, sizeof(list_ctrl->search_path));
    __msg("root_type=%d", root_type);

    switch (root_type)
    {
        case RAT_LOCAL:
            eLIBs_strcpy(s_root_type, RAT_LOCAL_DISK);
            break;

        case RAT_TF:
            eLIBs_strcpy(s_root_type, RAT_SD_CARD);
            break;

        case RAT_USB:
            eLIBs_strcpy(s_root_type, RAT_USB_DISK);
            break;

        default:
            eLIBs_strcpy(s_root_type, RAT_SD_CARD);
            break;
    }

    ret = rat_get_partition_name(s_root_type, disk_name, 0);
    eLIBs_strcpy(search_path, disk_name[0]);
    __msg("==============disk_name[0]=%s", disk_name[0]);
    return ret;
}

__s32 music_player_restore(H_WIN  music_frame_win, __s32 root_type)
{
    music_player_ctrl_t *this;
    music_list_para_t list_para;
    __s32 ret = 0;
    GUI_MEMDEV_Handle   draw_mem;

    if (music_frame_win == NULL)
    {
        return EPDK_FAIL;
    }

    //__wait__;
    this = (music_player_ctrl_t *)GUI_WinGetAttr(music_frame_win);
    this->menu_index = MUSIC_LIST_MENU_ICON;
    __msg("++++++++++++++++music_player_restore++++++++++++++++++++++");

    if (robin_get_fsm_status() == CEDAR_STAT_STOP)
    {
        //__here__;
        //return EPDK_FAIL;
    }

    this->visiable = EPDK_TRUE;

    if (GUI_LyrWinGetSta(this->music_layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        //__here__;
        return EPDK_FAIL;
    }

    GUI_LyrWinSel(this->music_layer);
    GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_SLEEP);
    DrawMusicBg();
    DrawMusicMenuAll(this);
    //DrawMusicInfoBg();
    // DrawMusicPlayBar();
    // DrawMusicBottomBg();
    // DrawMusicBottom(this,-1);
    __wrn("%s", __FUNCTION__);

    if (this->root_type == RAT_TF)
    {
        DrawMusicPlayerInfo(this, 6);
    }
    else if (this->root_type == RAT_USB)
    {
        DrawMusicPlayerInfo(this, 5);
    }

    if (ret != EPDK_OK)
    {
        __msg("Set music layer as top failed");
        return EPDK_FAIL;
    }
    else
    {
        __msg("--------------music player set layer status on, ret = %d", ret);
    }

    this->visiable = EPDK_TRUE;

    if (this->list_ctrl != NULL)
    {
        this->list_ctrl->visiable = EPDK_TRUE;
    }

    music_player_get_last_para(this);
    __msg("this->music_file_name = %s", this->music_file_name);
    __msg("this->play_file_info.filename = %s", this->play_file_info.filename);

    if (this->list_ctrl != NULL)                //先删除列表，防止闪烁
    {
        //__here__;
        delete_music_list(this->list_ctrl);
        this->list_ctrl = NULL;
    }
    else
    {
        //__here__;
        ;//NOTE
    }

    {
        char filename[RAT_MAX_FULL_PATH_LEN];
        char search_path[RAT_MAX_FULL_PATH_LEN];
        int index = 0;
        eLIBs_memset(filename, 0, sizeof(filename));
        musicplayer_rat_get_search_path(root_type, search_path);

        if (robin_get_fsm_cur_file(filename, sizeof(filename)) != -1)
        {
            __msg("robin get cur play file = %s!", filename);

            if (this->list_ctrl->rat_handle != 0)
            {
                this->list_ctrl->rat_handle = rat_open_ex(root_type, RAT_MEDIA_TYPE_AUDIO, 0);
                __msg("this->list_ctrl->rat_handle = %d!", this->list_ctrl->rat_handle);

                if (rat_set_file_for_play(this->list_ctrl->rat_handle, filename) == -1)
                {
                    return EPDK_FAIL;
                }

                index = robin_npl_get_cur();
                __msg("=robin npl get cur index = %d!", index);
            }
            else
            {
                return EPDK_FAIL;
            }
        }
        else
        {
            __wrn("robin get cur play file failed!");
            return EPDK_FAIL;
        }
    }

    {
        music_list_para_t list_para;
        __msg("music list is initializing!");
        this->root_type = root_type;
        list_para.framewin = this->h_framewin;
        list_para.layer = this->music_layer;
        list_para.visiable = this->visiable;
        list_para.root_type = this->root_type;
        list_para.media_type = this->media_type;
        list_para.font = this->font;
        //赋值，用于创建list时检测是否从文件管理恢复后台播放
        list_para.explorer_play_music = this->explorer_play_music;
        list_para.music_file_name = this->music_file_name;
        __msg("list_para.explorer_play_music = %d", list_para.explorer_play_music);
        __msg("this->music_file_name = %s", this->music_file_name);
        this->list_ctrl = new_music_list(&list_para);

        if (this->list_ctrl != NULL)
        {
            //__here__;
            show_music_list(this->list_ctrl);
        }
        else
        {
            //__here__;
            return EPDK_FAIL;
        }
    }

    MusicSetTransColor();
    //DrawMusicHeadbar(this);
    ret = music_restore_play_file_info(this);
    __msg("ret = %d", ret);
    GUI_LyrWinCacheOn();
    ret = GUI_LyrWinSetSta(this->music_layer, GUI_LYRWIN_STA_ON);
    ret = GUI_LyrWinSetTop(this->music_layer);
    GUI_LyrWinFlushFB(this->music_layer);
    GUI_LyrWinCacheOff();

    if (EPDK_FAIL == ret)
    {
        //__here__;
        return EPDK_FAIL;
    }

    {
        __bool timer_ret;

        if (!GUI_IsTimerInstalled(this->h_framewin, MusicSysTimerId))
        {
            timer_ret = GUI_SetTimer(this->h_framewin, MusicSysTimerId, MUSIC_SYSTIMER_INTERVAL, NULL);                     //系统时间刷新定时器

            if (!timer_ret)
            {
                __wrn("creat timer for system time failed");
            }
            else
            {
                __msg("---creat timer for system time successfully");
            }
        }

        /*
        if(!GUI_IsTimerInstalled(this->h_framewin, MusicCheckFocusTimerId))
        {
            timer_ret = GUI_SetTimer(this->h_framewin, MusicCheckFocusTimerId, MUSIC_CHECK_FOCUS_TIMER_INTERVAL, NULL);     //检测是否foucus item 与当前播放index一致
            if(!timer_ret)
            {
                __wrn("creat timer for check focus item failed");
            }
            else
            {
                __msg("----------------creat timer for check focus item successfully");
            }
        }
        */
    }

    if (EPDK_TRUE == robin_lyric_exist())
    {
        g_set_close_scn_time(0);
    }
    else
    {
        {
            //恢复系统自动关屏时间
            reg_system_para_t *para;
            para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

            if (para)
            {
                __wrn("g_set_close_scn_time=%d", para->backlightoff);
                g_set_close_scn_time(para->backlightoff);
            }
        }
    }

    {
        char title[32] = {0};
        dsk_langres_get_menu_text(STRING_MUSIC_TITLE, title, sizeof(title));
        gscene_hbar_set_title(title, sizeof(title));
        return EPDK_OK;
    }

    gscene_hbar_set_state(HBAR_ST_SHOW);
    return EPDK_OK;
}



//获得媒体文件信息
static __s32 music_get_audio_file_info(music_player_ctrl_t *this, char *filename)
{
    robin_audio_info_t audio_file_info;
    eLIBs_memset(&audio_file_info, 0, sizeof(robin_audio_info_t));
    robin_get_audio_info(filename, &audio_file_info);
    eLIBs_strcpy(this->play_file_info.artist, audio_file_info.artist);
    eLIBs_strcpy(this->play_file_info.album, audio_file_info.album);
    this->play_file_info.bitrate = audio_file_info.bps;
    this->play_file_info.filesize = eLIBs_GetFileSize(filename);
    this->play_file_info.artist_charset = audio_file_info.artist_charset ;
    this->play_file_info.album_charset = audio_file_info.album_charset ;
    return EPDK_OK;
}

__s32 __music_load_breakpoint_info(__s32 index, __cedar_tag_inf_t *tag_inf)
{
    __s32 ret;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
#if defined(CONFIG_SUNXI_QA_TEST)
	return -1;//qa测试不使用断点播放?
#endif
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return -1;
    }

    if (!fifo_db_has_exist(NAME_MUSIC_BKPOINT))
    {
        return -1;
    }

    //__here__;
    if (get_item_content_from_fifo_db(NAME_MUSIC_BKPOINT, filename, tag_inf))
    {
        return -1;
    }

    //__here__;
    return 0;
}

__s32 __music_save_breakpoint_info(__s32 index)
{
    __cedar_tag_inf_t tag_inf;
    char filename[RAT_MAX_FULL_PATH_LEN] = {0};
    __s32 ret;

    /*get the tag*/
    if (EPDK_FALSE == robin_is_open())
    {
        //__here__;
        return EPDK_FAIL;
    }

    // //__here__;
    ret = robin_get_tag(&tag_inf);

    if (0 != ret)
    {
        __msg("robin_get_tag fail...");
        return -1;
    }

    // //__here__;
    ret = robin_npl_index2file(index, filename);

    if (-1 == ret)
    {
        __msg("robin_npl_index2file fail...");
        return -1;
    }

    __msg("begin save breakpoint, index = %d, filename=%s", index, filename);

    if (!fifo_db_has_exist(NAME_MUSIC_BKPOINT))
    {
        ret = create_fifo_db(NAME_MUSIC_BKPOINT, 4, sizeof(__cedar_tag_inf_t));

        if (0 != ret)
        {
            __msg("create_fifo_db fail...");
            return -1;
        }
    }

    // //__here__;
    ret = add_item_to_fifo_db(NAME_MUSIC_BKPOINT, filename, &tag_inf);
    __msg("add_item_to_fifo_db, ret=%d", ret);
    return ret;
}


static __s32 music_play_file(music_player_ctrl_t *this, char *filename, __bool breakplay)
{
    __u32 total_time = 0, cur_time = 0;
    __s32 ret = 0;
    __s32 ret1;
    GUI_RECT    clear;
    music_ui_t  *ui_para = NULL;
    GUI_RECT    spctr_rect;
    memset(&spctr_rect, 0x00, sizeof(GUI_RECT));
    memset(&clear, 0x00, sizeof(GUI_RECT));

    if (this  == NULL)
    {
        __wrn("invalid para ....");
        return EPDK_FAIL;
    }

    //app_print_memory_usage("check memory in function of music_play_file", NULL);

    if (this->list_ctrl != NULL)
    {
        if ((eLIBs_strlen(filename) == 0) || (this->list_ctrl->total == 0))
        {
            if (this->visiable)
            {
                //DrawMusicPlayingTime(0);
                //CleanMusicTotalTime();
                //DrawMusicTotalTime(0);
                //DrawMusicProgressBar(0, 0);                   //显示进度条
                //eLIBs_memset(&(this->play_file_info), 0, sizeof(audio_file_info_t));
                //DrawMusicPlayerFileInfo(&(this->play_file_info));                 //显示文件信息
                //DrawMusicPlayStatusButton(MUSIC_PLAYING);
                //if(this->menu_mode == MUSIC_EXPLORER_MODE)
                //{
                //  SPCTRM_Clear((__u32)0x000f);        //显示最大值
                //}
            }

            return EPDK_FAIL;
        }
    }

    this->CheckFocusItemCnt = 0;        //有按键清除检测焦点条目定时计数器
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);

    if (this->list_ctrl != NULL)
    {
        music_list_get_item_scene(this->list_ctrl, &(this->list_ctrl->listbar_play_scene));//获得当前listbar场景
        //__wrn("this->list_ctrl->listbar_play_scene.focus_id = %d", this->list_ctrl->listbar_play_scene.focus_id);
        //__wrn("this->list_ctrl->listbar_play_scene.start_id = %d", this->list_ctrl->listbar_play_scene.start_id);
        robin_npl_set_cur(this->list_ctrl->play_index);
        this->play_index = this->list_ctrl->play_index;
        __wrn("this->play_index=%d", this->play_index);
    }
    else
    {
        robin_npl_set_cur(this->play_index);
        __wrn("this->play_index=%d", this->play_index);
    }

    //app_print_memory_usage("robin start to play file", NULL);
    if (1 == breakplay)
    {
        __cedar_tag_inf_t tag_info;
        __cedar_status_t cedar_status;
        char cur_playing_file[RAT_MAX_FULL_PATH_LEN] = {0};
        //__log(".....layer = %08x",this->list_ctrl->layer);
        ret = robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
        __wrn("---------ret = %d, cur_playing_file=%s---------", ret, cur_playing_file);
        cedar_status = robin_get_fsm_status();
        __wrn("cedar_status=%d", cedar_status);

        if ((CEDAR_STAT_PLAY == cedar_status || CEDAR_STAT_FF == cedar_status || CEDAR_STAT_RR == cedar_status
             || CEDAR_STAT_JUMP == cedar_status) && EPDK_OK == ret && 0 == eLIBs_stricmp(filename, cur_playing_file))
        {
            __wrn("cur music is playing, not need replay...");
        }
        else
        {
            ret = __music_load_breakpoint_info(this->play_index, &tag_info);

            if (EPDK_OK == ret)
            {
                ret = robin_play_file(filename, &tag_info);
                __wrn("roin play file = %d", ret);
                gscene_hbar_set_music_state(BG_MUSIC); //chengf  add
            }
            else
            {
                ret = robin_play_file(filename, NULL);
                __wrn("roin play file = %d", ret);
                gscene_hbar_set_music_state(BG_MUSIC); //chengf  add
            }

            if (ret == EPDK_OK)
            {
                __wrn("index: %d", this->play_index);
            }
        }

        //__log(".....layer = %08x",this->list_ctrl->layer);
    }
    else
    {
        ret = robin_play_file(filename, NULL);
        __wrn("robin play file = %d", ret);
        gscene_hbar_set_music_state(BG_MUSIC); //chengf  add
    }

    __wrn("music  filename is %s", filename);

    //app_print_memory_usage("robin start to play file finished", NULL);
    if (this->visiable)
    {
        //__log(".....layer = %08x",this->list_ctrl->layer);
        music_get_audio_file_info(this, filename);
        //__log(".....layer = %08x",this->list_ctrl->layer);
        total_time = robin_get_total_time();
        //__log(".....layer = %08x",this->list_ctrl->layer);
        cur_time = robin_get_cur_time();
        //__log(".....layer = %08x",this->list_ctrl->layer);
        //__wrn("filename = %s", filename);
        __wrn("----------current time = %d, total time = %d---------", cur_time, total_time);

        if (cur_time > total_time)
        {
            cur_time = total_time;
        }

        //__log(".....layer = %08x",this->list_ctrl->layer);
        ui_para = music_get_ui_param();
        //__log(".....layer = %08x",this->list_ctrl->layer);
        clear.x0 = ui_para->album.albumRect.x;
        clear.y0 = ui_para->album.albumRect.y;
        clear.x1 = clear.x0 + ui_para->album.albumRect.width;
        clear.y1 = clear.y0 + ui_para->album.albumRect.height;
        GUI_SetBkColor(0xFF2B2E36);
        GUI_ClearRect(clear.x0, clear.y0, clear.x1, clear.y1);
        __wrn("this->play_file_info.filename = %s", this->play_file_info.filename);
        __wrn("filename = %s", filename);
        //__log(".....layer = %08x",this->list_ctrl->layer);
        spctr_rect.x0 = ui_para->spectrum.BG_rect.x;
        spctr_rect.y0 = ui_para->spectrum.BG_rect.y;
        spctr_rect.x1 = spctr_rect.x0 + ui_para->spectrum.BG_rect.width;
        spctr_rect.y1 = spctr_rect.y1 + ui_para->spectrum.BG_rect.height;
        GUI_SetBkColor(0xFF2B2E36);
        GUI_ClearRect(spctr_rect.x0, spctr_rect.y0, spctr_rect.x1, spctr_rect.y1);
        GUI_LyrWinSel(this->music_layer);

        if (this->showMode == MUSIC_ALBUM)
        {
            ret1 = DrawMusicAlbum(this, filename, &clear);
        }

        __wrn("ret1 = %d", ret1);
    }

    __wrn("this->visiable = %d", this->visiable);
    __wrn("this->menu_mode = %d", this->menu_mode);
    __wrn("this->showMode = %d", this->showMode);

    if (this->visiable)
    {
        //__log(".....layer = %08x",this->list_ctrl->layer);
        delete_music_lyrics_scene(this);

        //__log(".....layer = %08x",this->list_ctrl->layer);
        if (this->menu_mode == MUSIC_EXPLORER_MODE)
        {
            __wrn("music_lyrics_exist() = %d", music_lyrics_exist());

            if (music_lyrics_exist())       //检查是否有歌词
            {
                //有歌词则不显示频谱
                if (create_music_lyrics_scene(this) != EPDK_FAIL)
                {
                    __wrn("this->play_file_info.filename = %s", this->play_file_info.filename);
                    __wrn("filename = %s", filename);
                    update_music_lyrics(this->h_lyrics);

                    if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
                    {
                        __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                        g_set_close_scn_time(0);
                    }
                }

                //__log(".....layer = %08x",this->list_ctrl->layer);
            }
            else
            {
                /*
                this->showMode = MUSIC_SPECTRUM;
                SPCTRM_Clear(0);        //显示最大值
                SPCTRM_Update((H_WIN)(this->h_framewin));

                //__here__;
                if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
                {
                    __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");

                    {//恢复系统自动关屏时间
                        reg_system_para_t* para;
                        para = (reg_system_para_t*)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
                        if(para)
                        {
                            __wrn("g_set_close_scn_time=%d", para->backlightoff);
                            g_set_close_scn_time(para->backlightoff);
                        }
                    }
                }
                */
            }
        }

#if 0
        else if (this->menu_mode == MUSIC_SET_PLAYER_MODE)
        {
            if (music_lyrics_exist())       //检查是否有歌词
            {
                //有歌词则不显示频谱
                this->showMode = MUSIC_LYRICS;

                if (create_music_lyrics_scene(this) != EPDK_FAIL)       //只创建，不显示
                {
                    //update_music_lyrics(this->h_lyrics);
                    __msg("create lyrics successed!");
                }

                //__here__;
                if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
                {
                    __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                    __wrn("g_set_close_scn_time=%d", 0);
                    g_set_close_scn_time(0);
                }
            }
            else
            {
                this->showMode = MUSIC_SPECTRUM;
                SPCTRM_Clear(0);        //显示最大值
                SPCTRM_Update((H_WIN)(this->h_framewin));

                //__here__;
                if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
                {
                    __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                    {
                        //恢复系统自动关屏时间
                        reg_system_para_t *para;
                        para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                        if (para)
                        {
                            __wrn("g_set_close_scn_time=%d", para->backlightoff);
                            g_set_close_scn_time(para->backlightoff);
                        }
                    }
                }
            }
        }

#endif
    }

    //__log(".....layer = %08x",this->list_ctrl->layer);
#if OUTAGE_SAVE_CUR_FILE
    dsk_reg_save_cur_play_info(REG_APP_MUSIC, this->play_index, this->play_file_info.filename, this->root_type);
    //__music_save_breakpoint_info(this->play_index);
    dsk_reg_flush();
#endif
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    //app_print_memory_usage("check memory in function of music_play_file finished", NULL);
    return EPDK_OK;
}


__s32 music_restore_play_file_info(music_player_ctrl_t *this)
{
    __s32 ret;
    __u32 total_time = 0, cur_time = 0;
    char *filename;

    if (this == NULL)
    {
        return EPDK_FAIL;
    }

    filename = this->play_file_info.filename;
    ret = MusicGetListItemFileFullPath(this->list_ctrl->rat_handle, this->list_ctrl->play_index, filename);

    if (EPDK_FAIL == ret)
    {
        //__here__;
        return EPDK_FAIL;
    }

    music_get_audio_file_info(this, filename);
    total_time = robin_get_total_time();
    cur_time = robin_get_cur_time();

    if (cur_time > total_time)
    {
        cur_time = total_time;
    }

    //DrawMusicPlayingTime(cur_time);

    //CleanMusicTotalTime();
    //DrawMusicTotalTime(total_time);

    //DrawMusicProgressBar(total_time, cur_time);                   //显示进度条

    //DrawMusicPlayerFileInfo(&(this->play_file_info));                 //显示文件信息
    //DrawMusicPlayStatusButton(MUSIC_PLAYING);
    if (this->showMode == MUSIC_ALBUM)
    {
        __s32 ret1 = EPDK_OK;
        GUI_RECT        clear;
        memset(&clear, 0x00, sizeof(GUI_RECT));
        music_ui_t *ui_para;
        ui_para = music_get_ui_param();
        clear.x0 = ui_para->album.albumRect.x;
        clear.y0 = ui_para->album.albumRect.y;
        clear.x1 = clear.x0 + ui_para->album.albumRect.width;
        clear.y1 = clear.y0 + ui_para->album.albumRect.height;

        if (EPDK_FAIL == ret1)
        {
            //__here__;
            return EPDK_FAIL;
        }

        //__here__;
        __msg("this->play_file_info.filename = %s", this->play_file_info.filename);
        GUI_LyrWinSel(this->music_layer);
        ret1 = DrawMusicAlbum(this, this->play_file_info.filename, &clear);
        __msg("ret1 = %d", ret1);
        //__here__;
    }
    else
    {
        com_memdev_create(this->music_layer);
        SPCTRM_Clear(0);
        SPCTRM_Update((H_WIN)(this->h_framewin));
        com_memdev_delete();
    }

    delete_music_lyrics_scene(this);
    __msg("this->showMode = %d", this->showMode);
    __msg("this->menu_mode = %d", this->menu_mode);

    if (this->menu_mode == MUSIC_EXPLORER_MODE)
    {
        //ClearMusicPlayerContrlButtonBG();
        __msg("music_lyrics_exist() = %d", music_lyrics_exist());

        if (music_lyrics_exist())       //检查是否有歌词
        {
            //有歌词则不显示频谱
            //__here__;
            if (create_music_lyrics_scene(this) != EPDK_FAIL)
            {
                __msg("update music lyrics ");
                update_music_lyrics(this->h_lyrics);
            }

            //__here__;
            if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
            {
                __wrn("g_set_close_scn_time=%d", 0);
                g_set_close_scn_time(0);
            }
        }
        else
        {
            __msg("music lyrics is not exist");
            /*SPCTRM_Clear(0);      //显示最大值
            SPCTRM_Update((H_WIN)(this->h_framewin));

            //__here__;
            if(GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
            {
                __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");

                {//恢复系统自动关屏时间
                    reg_system_para_t* para;
                    para = (reg_system_para_t*)dsk_reg_get_para_by_app(REG_APP_SYSTEM);
                    if(para)
                    {
                        __wrn("g_set_close_scn_time=%d", para->backlightoff);
                        g_set_close_scn_time(para->backlightoff);
                    }
                }
            }*/
        }

        if (this->showMode == MUSIC_SPECTRUM)
        {
            GUI_ClearRect(this->h_lyrics->UI.area.x, this->h_lyrics->UI.area.y - 14,
                          this->h_lyrics->UI.area.x + this->h_lyrics->UI.area.width,
                          this->h_lyrics->UI.area.y + this->h_lyrics->UI.area.height);
            SPCTRM_Clear(0);        //显示最大值
            SPCTRM_Update((H_WIN)(this->h_framewin));
        }
    }

#if 0
    else if (this->menu_mode == MUSIC_SET_PLAYER_MODE)
    {
        if (music_lyrics_exist())       //检查是否有歌词
        {
            //有歌词则不显示频谱
            if (create_music_lyrics_scene(this) != EPDK_FAIL)       //只创建，不显示
            {
                //update_music_lyrics(this->h_lyrics);
                __msg("create lyrics successed!");
            }

            //__here__;
            if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
            {
                __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                __wrn("g_set_close_scn_time=%d", 0);
                g_set_close_scn_time(0);
            }
        }
        else
        {
            //__here__;
            if (GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer))
            {
                __wrn("GUI_LYRWIN_STA_ON == GUI_LyrWinGetSta(this->music_layer)");
                {
                    //恢复系统自动关屏时间
                    reg_system_para_t *para;
                    para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                    if (para)
                    {
                        __wrn("g_set_close_scn_time=%d", para->backlightoff);
                        g_set_close_scn_time(para->backlightoff);
                    }
                }
            }
        }
    }

#endif
    GUI_ResetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    return EPDK_OK;
}

//初始化音频设备
static __s32 music_player_device_init(music_player_ctrl_t *this)
{
    __s32 ret;
    //__bool ret;
    __wrn("music_player_device_init ");
    //app_print_memory_usage("robin create", NULL);
    /*open display*/
    //ADE_Open();
    ret = robin_open(ROBIN_MODE_AUDIO_MIN, NULL);
    __wrn("=========music robin open ret = %d=============", ret);
    robin_enable_spectrum();
    robin_set_eq_mode(this->EQ_mode);
    //music_player_set_robin_EQ_mode(this);
    robin_set_ff_rr_speed(MUSIC_FF_SPEED);
    //this->play_mode = RAT_PLAY_MODE_SEQUENCE;
    robin_set_play_mode(this->play_mode);
    //GUI_SetTimer(this->h_framewin, MusicTimerId, MUSIC_TIMER_INTERVAL, NULL);
    //app_print_memory_usage("robin create finished", NULL);
    return EPDK_OK;
}


//卸载音频设备
static __s32 music_player_device_uninit(music_player_ctrl_t *player_ctrl)
{
    //app_print_memory_usage("delete robin ", NULL);
    if (NULL == player_ctrl || NULL == player_ctrl->h_framewin)
    {
        return EPDK_FAIL;
    }

    //__here__;
    if (GUI_IsTimerInstalled(player_ctrl->h_framewin, MusicTimerId))
    {
        GUI_KillTimer(player_ctrl->h_framewin, MusicTimerId);
    }

    if (EPDK_FALSE == robin_is_open())
    {
        //__here__;
        return EPDK_FALSE;
    }

    //__here__;
    robin_set_cmd_stop();
    //__here__;
    //GUI_KillTimer(player_ctrl->h_framewin, MusicSpectrumTimerId);
    /*close robin*/
    robin_close();
    //__here__;
    /*close de*/
    //ADE_Close();
    //app_print_memory_usage("delete robin finished", NULL);
    return EPDK_OK;
}

//


//发送一个命令参数给主界面
__s32 music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128] = {0};
    H_WIN hDstWin;
    H_WIN hManagerWin;
    //robin_set_cmd_stop();
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
    //GUI_SendMessage(&msg);
    return EPDK_OK;
}

//movie_cmd2parent(H_WIN hwin, __s32 id, __s32 data2, __s32 reserved);
__s32 music_send_parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2, __s32 reserved)
{
    H_WIN hparent;
    __gui_msg_t msg;
    hparent = GUI_WinGetParent(hwin);

    if (!hparent)
    {
        __err("hparent is null...");
        return EPDK_FAIL;
    }

    msg.h_deswin = hparent;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    //msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData1 = data1;
    msg.dwAddData2 = data2;
    msg.dwReserved = reserved;
    GUI_SendNotifyMessage(&msg);
    return EPDK_OK;
}

__s32 create_music_lyrics_scene(music_player_ctrl_t *This)
{
    music_lyrics_para_t para;
    RECT rect;
    memset(&rect, 0x00, sizeof(rect));
    para.layer = This->music_layer;

    if (This->h_lyrics != NULL)
    {
        delete_music_lyrics_scene(This);
    }

    //app_print_memory_usage("new music lyrics", NULL);
    dsk_display_get_size(&rect.width, &rect.height);

    if (rect.width == 800 && rect.height == 480)
    {
        para.area.x = 350;
        para.area.y = rect.height - 74; //35;
    }
    else
    {
        para.area.x = 350 - 80;
        para.area.y = 480 - 74; //35;
    }

    //para.area.y = rect.height-74; //35;
    para.area.width = 219 + 150;
    para.area.height = 74;  //125 - 35;     //显示4行歌词
    para.font = This->font;
    para.font_color = GUI_WHITE;
    para.hight_light_color = GUI_LIGHTRED;  //BEETLES_GRAY_COLOR;
    This->h_lyrics = new_music_lyrics(&para);

    if (This->h_lyrics == NULL)
    {
        return EPDK_FAIL;
    }

    g_set_close_scn_time(0);
    //app_print_memory_usage("new music lyrics finished", NULL);
    return EPDK_OK;
}

__s32 delete_music_lyrics_scene(music_player_ctrl_t *This)
{
    //app_print_memory_usage("delete music lyrics", NULL);
    if (This->h_lyrics != NULL)
    {
        delete_music_lyrics(This->h_lyrics);
        This->h_lyrics = NULL;
    }

    //app_print_memory_usage("delete music lyrics finished", NULL);
    return EPDK_OK;
}

__s32 delete_music_player_volume_scene(H_WIN hfrm)
{
#if 0
    music_player_ctrl_t *player_ctrl = NULL;

    if (!hfrm)
    {
        return EPDK_FAIL;
    }

    player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(hfrm);

    if (player_ctrl && player_ctrl->hVolumeBar)
    {
        VolumeBarDelete(player_ctrl->hVolumeBar);
        player_ctrl->hVolumeBar = NULL;
    }

#endif
    return EPDK_TRUE;
}

#ifdef BEETLES_SUPPORT_KARAOKE

static __s32 AiTalkGetStr(char str[AITALK_MAX_STR][AITALK_MAX_STR_SIZE])
{
    __s32 i;
    __bool bsuccess;
    bsuccess = 0;
    __wrn("begin dump str...");

    for (i = 0 ; i < AITALK_MAX_STR ; i++)
    {
        if (str[i][0])
        {
            __inf("%s, ", str[i][0]);
            bsuccess = 1;
        }
    }

    __wrn("end dump str...");

    if (1 == bsuccess)
    {
        robin_set_cmd_play();
        gscene_hbar_set_music_state(BG_MUSIC);
    }
    else
    {
        robin_set_cmd_pause();
        gscene_hbar_set_music_state(BG_PAUSE);
    }

    return EPDK_OK;
}
#endif
