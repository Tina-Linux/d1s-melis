/*
**************************************************************************************************************
*                                                    ePDK
*                                   the Easy Portable/Player Develop Kits
*                                              desktop system
*
*                                    (c) Copyright 2007-2011, CHIPHD, China
*                                            All Rights Reserved
*
* File      : App_photo.c
* By        : CQQ
* Func      :
* Version   : v1.0
* ============================================================================================================
* 2011/06/07 15:42  create this file, implements the fundamental interface;
**************************************************************************************************************
*/
#include "App_photo_i.h"
#include "photo_sset.h"
#include <log.h>

H_WIN   g_PhotoMainWnd = NULL;
static int32_t  app_photo_notify_delete_sub_scene2(H_WIN hmanwin);
static int32_t  on_photo_frm_create(__gui_msg_t *msg);
static int32_t  photo_show_dialog(H_WIN photo_manager, int32_t win_id, int32_t item_id);

//////////////////////////////////////////////////////////////////////////
// 建立图层
//#define photo_layer_create(_lyr, _prt, _name)   app_photo_layer_create_default(_lyr, _prt, _name)

//打开背景音乐
#define OpenBgMusic(_hwnd)   do \
    { \
        NOTIFY_MSG(GUI_MSG_COMMAND, _hwnd, GUI_WinGetParent(GetPhotoManWnd()), MAKELONG(GUI_WinGetItemId(GetPhotoManWnd()), OPEN_BG_MUSIC), 0); \
    } while(0)

#define CloseBgMusic(_hwnd)   do \
    { \
        NOTIFY_MSG(GUI_MSG_COMMAND, _hwnd, GUI_WinGetParent(GetPhotoManWnd()), MAKELONG(GUI_WinGetItemId(GetPhotoManWnd()), CLOSE_BG_MUSIC), 0); \
    } while(0)

//播放 暂停窗口ID
#define APP_PHOTO_ID        0x5001
#define PHOTO_PLAY_ID       (APP_PHOTO_ID+1)
#define PHOTO_PAUSE_ID      (APP_PHOTO_ID+2)

/************************************************************************/
// 销毁sset
/************************************************************************/
static int32_t photo_destroy_sset(PHOTO_WND_T *wnd_para, int32_t val)
{
    ASSERT(wnd_para);

    //CheckAPPMEM();
    if (val & 0x2)
    {
        if (wnd_para->h_sset)
        {
            SSET_Destroy(wnd_para->h_sset);
            wnd_para->h_sset = NULL;
        }
    }

    if (val & 0x1)
    {
        if (wnd_para->sset_create_para)
        {
            Photo_SSET_Destroy_CreatePara(wnd_para->sset_create_para);
            wnd_para->sset_create_para = NULL;
        }
    }

    //  CheckAPPMEM();
    return EPDK_OK;
}

/************************************************************************/
// 创建sset
/************************************************************************/
static int32_t photo_create_sset(PHOTO_WND_T *wnd_para, int32_t val)
{
    ASSERT(wnd_para);

    //  CheckAPPMEM();
    if (val & 0x1)
    {
        if (wnd_para->sset_create_para == NULL)
        {
            wnd_para->sset_create_para = Photo_SSET_Build_CreatePara(GetPhotoManWnd());
        }
    }

    if (val & 0x2)
    {
        if (wnd_para->h_sset == NULL)
        {
            ASSERT(wnd_para->sset_create_para);
            wnd_para->h_sset = SSET_Create(wnd_para->sset_create_para);
        }
    }

    if (val & 0x4)
    {
        SSET_Set_Nodelist("photo", NODELIST_ON);
    }

    //  CheckAPPMEM();
    return EPDK_OK;
}

/************************************************************************/
// 处理sset命令
/************************************************************************/
static int32_t on_photo_sset_cmd(H_WIN hwnd, int32_t cmd, int32_t para)
{
    PHOTO_WND_T     *wnd_para;

    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, hwnd);

    __msg("on_photo_sset_cmd(%x, %d, %d)", hwnd, cmd, para);

    if (PHOTO_SSET_NC_ID_UNKNOW == para)
    {
        __wrn("PHOTO_SSET_NC_ID_UNKNOW == para");
        return EPDK_OK;
    }

    switch (cmd)
    {
        case PHOTO_SSET_NC_EXIT:
        {
            //notify_to_close_photo(hwnd, SWITCH_TO_EXPLORER);
            todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0); //sset命令是用send发的，因此不直接调photo_destroy_sset
            do_photo_command(hwnd, ID_PHOTO_CMD_EXIT, 0);
        }
        break;

        case PHOTO_SSET_NC_BGMUSIC:
        {
            //todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0);
            todo_photo_command(hwnd, ID_PHOTO_CMD_BG_MUSIC, 0/*photo_get_bgmusic()*/);
        }
        break;

        case PHOTO_SSET_NC_ZOOM:
        {
            todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0);
            todo_photo_command(hwnd, ID_PHOTO_CMD_SCALE, SHOW_UI);
        }
        break;

        case PHOTO_SSET_NC_SLIDESHOW:
        {
            if (para != PHOTO_SSET_NC_ID_UNKNOW && para != ROOM_EXIT)
            {
                photo_show_dialog(g_PhotoMainWnd, PHOTO_SLIDE_SHOW_ID, para);
                do_anole_command(hwnd, ID_ANOLE_CMD_SLIDESHOW, para);
            }
            else if (para == ROOM_EXIT) //退出
            {
                app_photo_notify_delete_sub_scene2(hwnd);
            }
        }
        break;

        case PHOTO_SSET_NC_INTERVAL:
        {
            if (para == PHOTO_SPEEDFLAG_EXIT) //退出
            {
                app_photo_notify_delete_sub_scene2(hwnd);
            }
            else
            {
                photo_show_dialog(g_PhotoMainWnd, PHOTO_PLAY_SPEED_ID, para);
                do_photo_command(hwnd, ID_PHOTO_CMD_INTERVAL, para);
                do_anole_command(hwnd, ID_ANOLE_CMD_INTERVAL, para);
            }
        }
        break;

        case PHOTO_SSET_NC_AUTOPLAY:
        {
            todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0);
            {
                H_WIN       dlg;
                int32_t     lang_id[] = {STRING_PHOTO_TIPS, STRING_PHOTO_PPLAY};

                default_dialog(dlg, hwnd, PHOTO_PLAY_ID, ADLG_OK, lang_id);
                esKRNL_TimeDly(100);
                app_dialog_destroy(dlg);
            }
            todo_anole_command(hwnd, ID_ANOLE_CMD_AUTOPLAY, 0);
            do_anole_command(hwnd, ID_PHOTO_CMD_AUTOPLAY, SHOW_UI);
        }
        break;

        case PHOTO_SSET_NC_FILEINFO:
        {
            todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0);
            todo_anole_command(hwnd, ID_ANOLE_CMD_FILEINFO, SHOW_UI);
        }
        break;

        case PHOTO_SSET_NC_SAVE_BG:
        {
            todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0);
            todo_photo_command(hwnd, ID_PHOTO_CMD_POP_SAVE_DIALOG, 0);
        }
        break;

        case PHOTO_SSET_NC_INTERVAL_USER:
        {
            do_photo_command(hwnd, ID_PHOTO_CMD_INTERVAL, PHOTO_SPEEDFLAG_MANNUAL);
            do_anole_command(hwnd, ID_ANOLE_CMD_INTERVAL, para);
        }
        break;

        case SSET_NC_TIMER:
            todo_photo_command(hwnd, ID_PHOTO_CMD_HIDE_MENU, 0);
            break;
    }

    return EPDK_OK;
}

/************************************************************************/
//                       获取背景名
/************************************************************************/
#define DEF_SAVE_BG_FILE_NAME_PATH          ""
#define DEF_SAVE_BG_FILE_NAME_EXT           ".bin"
static char *get_bg_name(char file_name[])
{
    char    name[MAX_FILE_NAME_LEN] = {0};
    char    *pName;

    APP_ANOLE_NPL_Index2File(APP_ANOLE_GetCurPicIndex(), name);

    eLIBs_strcpy(file_name, name);

    pName = eLIBs_strchr(file_name, ':');
    ASSERT(pName);

    eLIBs_strcpy(pName + 1, DEF_SAVE_BG_FILE_NAME_PATH);

    pName = eLIBs_strchrlast(name, '\\');
    ASSERT(pName);

    eLIBs_strcat(file_name, pName);

    pName = eLIBs_strchrlast(file_name, '.');

    eLIBs_strcpy(pName, DEF_SAVE_BG_FILE_NAME_EXT);
    return file_name;
}

static __s32 photo_show_dialog(H_WIN photo_manager, __s32 win_id, __s32  item_id)
{
    H_WIN h_dialog = NULL;
    __s32 lang_id[] = {STRING_PHOTO_TIPS, NULL};

    switch (win_id)
    {
        case PHOTO_SLIDE_SHOW_ID:
        {
            switch (item_id)
            {
                case DEFAULT_SWITCH:
                    lang_id[1] = STRING_PHOTO_DEFAULT;
                    break;

                case FADE_IN_OUT:
                    lang_id[1] = STRING_PHOTO_FADE_IN_FADE_OUT;
                    break;

                case PERSIAN_BLIND_H:
                    lang_id[1] = STRING_PHOTO_PERSIAN_BLINDS_H;
                    break;

                case PERSIAN_BLIND_V:
                    lang_id[1] = STRING_PHOTO_PERSIAN_BLINDS_V;
                    break;

                case SLID_UP:
                    lang_id[1] = STRING_PHOTO_SLIDE_UP;
                    break;

                case SLID_DOWN:
                    lang_id[1] = STRING_PHOTO_SLIDE_DOWN;
                    break;

                case SLID_LEFT:
                    lang_id[1] = STRING_PHOTO_SLIDE_LEFT;
                    break;

                case SLID_RIGHT:
                    lang_id[1] = STRING_PHOTO_SLIDE_RIGHT;
                    break;

                case STRETCH_UP:
                    lang_id[1] = STRING_PHOTO_STRETCH_UP;
                    break;

                case STRETCH_DOWN:
                    lang_id[1] = STRING_PHOTO_STRETCH_DOWN;
                    break;

                case STRETCH_LEFT:
                    lang_id[1] = STRING_PHOTO_STRETCH_LEFT;
                    break;

                case STRETCH_RIGHT:
                    lang_id[1] = STRING_PHOTO_STRETCH_RIGHT;
                    break;

                case MOSAIC:
                    lang_id[1] = STRING_PHOTO_MOSAIC;
                    break;

                case ROOM_IN:
                    lang_id[1] = STRING_PHOTO_ZOOM_IN;
                    break;

                case ROOM_OUT:
                    lang_id[1] = STRING_PHOTO_ZOOM_OUT;
                    break;

                case ROOM_RANDOM:
                    lang_id[1] = STRING_PHOTO_RANDOM_SEL;
                    break;

                default:
                    lang_id[1] = NULL;
                    break;
            }
        }
        break;

        case PHOTO_PLAY_SPEED_ID:
        {
            switch (item_id)
            {
                case PHOTO_SPEEDFLAG_FAST:
                    lang_id[1] = STRING_PHOTO_FAST;
                    break;

                case PHOTO_SPEEDFLAG_MIDDLE:
                    lang_id[1] = STRING_PHOTO_MIDDLE;
                    break;

                case PHOTO_SPEEDFLAG_SLOW:
                    lang_id[1] = STRING_PHOTO_SLOW;
                    break;

                default:
                    lang_id[1] = NULL;
                    break;
            }
        }
        break;

        default:
        {
            lang_id[1] = NULL;
        }
        break;
    }

    default_dialog(h_dialog, photo_manager, win_id, ADLG_OK, lang_id);
    esKRNL_TimeDly(50);

    if (h_dialog)
    {
        app_dialog_destroy(h_dialog);
        h_dialog = NULL;
    }

    return EPDK_OK;
}


/************************************************************************/
//                        保存背景为桌面
/************************************************************************/
static __s32 photo_save_bg(void)
{
    FB fb;
    char name[MAX_FILE_NAME_LEN] = {0};
    DEBUG_photo("..........set bg file");
    get_bg_name(name);
    DEBUG_photo("save file bg is %s", name);
    ZeroMemory(&fb, sizeof(fb));
    fb.fmt.type = FB_TYPE_YUV;
    anole_set_bg_pic(&fb);
    gscene_bgd_save_fb(&fb, name);
    return EPDK_OK;
}

/************************************************************************/
// 通知桌面退出应用
/************************************************************************/
static __s32 notify_to_close_photo(H_WIN hwnd, __s32 swith_wnd_cmd)
{
    __gui_msg_t msg_send;
    msg_send.h_deswin   = GUI_WinGetParent(GetPhotoManWnd());
    msg_send.h_srcwin   = hwnd;
    msg_send.id         = GUI_MSG_COMMAND;
    msg_send.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwnd), swith_wnd_cmd);
    msg_send.dwAddData2 = 0;
    msg_send.dwReserved = 0;
    return GUI_SendNotifyMessage(&msg_send);
}

/************************************************************************/
//按键消息处理，因为当SSET 窗口存在的时候photo frm 收不到按键消息
//这里要单独处理一下GUI_MSG_KEY_MENU,GUI_MSG_KEY_LONGMENU这两消息,返回EPDK_TRUE 表示不需要先下子窗口传，否则需要
/************************************************************************/
static __bool photo_key_proc(__gui_msg_t *msg)
{
    static __u32 lastkey = VK_NULL;
    __bool  b_ret = EPDK_FALSE;
    PHOTO_WND_T *wnd_para = NULL;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);

    if (wnd_para->h_dialog != NULL || wnd_para->h_frm_alarm_coming != NULL)
    {
        return EPDK_FALSE;
    }

    //__here__;
    if (msg->dwAddData2 == KEY_UP_ACTION)
    {
        if (wnd_para->photo_frm_para->ctrl_para.show_para == PHOTO_SHOW_NULL)
        {
            switch (lastkey)
            {
                case GUI_MSG_KEY_ESCAPE://..
                {
                    notify_to_close_photo(msg->h_deswin, SWITCH_TO_EXPLORER);
                }
                break;

                case GUI_MSG_KEY_MENU://..
                {
                    //break; //need to fixed
                    APP_ANOLE_EnableTouch(EPDK_FALSE);
                    gscene_hbar_set_state(HBAR_ST_SHOW);
                    do_anole_command(msg->h_deswin, ID_ANOLE_CMD_PAUSE, 0);
                    //                      do_anole_command(msg->h_deswin, ID_ANOLE_CMD_FILEINFO, HIDE_UI);
                    //创建sset
                    photo_create_sset(wnd_para, 0xff);
                    wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_MENU;
                }
                break;

                case GUI_MSG_KEY_UP:
                case GUI_MSG_KEY_LONGLEFT:
                case GUI_MSG_KEY_LEFT:
                case GUI_MSG_KEY_PREV:
                case GUI_MSG_KEY_LONGPREV:
                {
                    APP_ANOLE_Pause();
                    anole_play_prev();
#ifdef  NEED_SHOW_ANOLE_SATUT
                    wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_PLAY_PREV;
                    /*需要创建photo frm win 显示图片*/
                    on_photo_frm_create(msg);
#endif
                }
                break;

                case GUI_MSG_KEY_DOWN:
                case GUI_MSG_KEY_RIGHT:
                case GUI_MSG_KEY_LONGRIGHT:
                case GUI_MSG_KEY_NEXT:
                case GUI_MSG_KEY_LONGNEXT:
                {
                    APP_ANOLE_Pause();
                    anole_play_next();
#ifdef      NEED_SHOW_ANOLE_SATUT
                    wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_PLAY_NEXT;
                    /*需要创建photo frm win 显示图片*/
                    on_photo_frm_create(msg);
#endif
                }
                break;

                case GUI_MSG_KEY_ENTER:
                    //case GUI_MSG_KEY_PLAY_PAUSE:
                {
                    if (ANOLE_MODE_PLAY == anole_get_sta())
                    {
                        APP_ANOLE_Pause();
#ifdef      NEED_SHOW_ANOLE_SATUT
                        wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_PAUSE;
                        /*需要创建photo frm win 显示图片*/
                        on_photo_frm_create(msg);
#endif
                    }
                    else if (ANOLE_MODE_ZOOM == anole_get_sta())
                    {
                        APP_ANOLE_Pause();
                        APP_ANOLE_AutoPlay();
#ifdef      NEED_SHOW_ANOLE_SATUT
                        wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_AUTO_PLAY;
                        /*需要创建photo frm win 显示图片*/
                        on_photo_frm_create(msg);
#endif
                    }
                    else
                    {
                        APP_ANOLE_AutoPlay();
#ifdef      NEED_SHOW_ANOLE_SATUT
                        wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_AUTO_PLAY;
                        /*需要创建photo frm win 显示图片*/
                        on_photo_frm_create(msg);
#endif
                    }
                }
                break;

                case GUI_MSG_KEY_PLAY_PAUSE:
                {
                    __wrn("GUI_MSG_KEY_PLAY_PAUSE11");

                    if (ANOLE_MODE_PLAY == anole_get_sta())
                    {
                        H_WIN dlg;
                        __s32 lang_id[] = {STRING_PHOTO_TIPS, STRING_PHOTO_PPAUSE};
                        default_dialog(dlg, msg->h_deswin, PHOTO_PLAY_ID, ADLG_OK, lang_id);
                        esKRNL_TimeDly(100);
                        app_dialog_destroy(dlg);
                        APP_ANOLE_Pause();
#ifdef      NEED_SHOW_ANOLE_SATUT
                        wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_PAUSE;
                        /*需要创建photo frm win 显示图片*/
                        on_photo_frm_create(msg);
#endif
                    }
                    else
                    {
                        H_WIN dlg;
                        __s32 lang_id[] = {STRING_PHOTO_TIPS, STRING_PHOTO_PPLAY};
                        default_dialog(dlg, msg->h_deswin, PHOTO_PLAY_ID, ADLG_OK, lang_id);
                        esKRNL_TimeDly(100);
                        app_dialog_destroy(dlg);
                        todo_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, 0);
                        todo_anole_command(msg->h_deswin, ID_ANOLE_CMD_AUTOPLAY, 0);
                        do_anole_command(msg->h_deswin, ID_PHOTO_CMD_AUTOPLAY, SHOW_UI);
                    }
                }
                break;

                default:
                    break;
            }

            b_ret = EPDK_TRUE;
        }
        else if (wnd_para->photo_frm_para->ctrl_para.show_para == PHOTO_SHOW_MENU)
        {
            if (lastkey == GUI_MSG_KEY_LONGMENU || lastkey == GUI_MSG_KEY_MENU || lastkey == GUI_MSG_KEY_ESCAPE || lastkey == GUI_MSG_KEY_LONGESCAPE) //..
            {
                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, HIDE_UI);
                wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_NULL;
                b_ret = EPDK_TRUE;
            }
            else
            {
                b_ret = EPDK_FALSE;
            }
        }
        else
        {
            b_ret = EPDK_FALSE;
        }

        lastkey = VK_NULL;
    }
    else
    {
        if (wnd_para->photo_frm_para->ctrl_para.show_para == PHOTO_SHOW_NULL)
        {
            switch (msg->dwAddData1)
            {
                case GUI_MSG_KEY_LONGRIGHT:
                case GUI_MSG_KEY_LONGNEXT:
                {
                    APP_ANOLE_Pause();
                    anole_play_next();
#ifdef      NEED_SHOW_ANOLE_SATUT
                    wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_PLAY_NEXT;
                    /*需要创建photo frm win 显示图片*/
                    on_photo_frm_create(msg);
#endif
                }
                break;

                case  GUI_MSG_KEY_LONGLEFT:
                case GUI_MSG_KEY_LONGPREV:
                {
                    APP_ANOLE_Pause();
                    anole_play_prev();
#ifdef      NEED_SHOW_ANOLE_SATUT
                    wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_PLAY_PREV;
                    /*需要创建photo frm win 显示图片*/
                    on_photo_frm_create(msg);
#endif
                }
                break;

                default:
                    break;
            }

            lastkey = msg->dwAddData1;
            b_ret = EPDK_TRUE;
        }
        else
        {
            b_ret = EPDK_FALSE;
            lastkey = msg->dwAddData1;
        }
    }

    //__here__;
    return b_ret;
}


/************************************************************************/
//命令消息处理
/************************************************************************/
static __s32 on_windos_command(__gui_msg_t *msg)
{
    PHOTO_WND_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);

    switch (LOWORD(msg->dwAddData1))
    {
        case ID_PHOTO_SCENE_SSET:
        {
            on_photo_sset_cmd(msg->h_deswin, HIWORD(msg->dwAddData1), msg->dwAddData2);
        }
        break;

        case PHOTO_SAVE_BG_DIALOG_ID:
        {
            //__here__;
            switch (HIWORD(msg->dwAddData1))
            {
                case ADLG_CMD_EXIT:
                {
                    DEBUG_photo("dialog ret = %d ", msg->dwAddData2);
                    app_dialog_destroy(msg->h_srcwin);
                    wnd_para->h_dialog = NULL;

                    if (msg->dwAddData2 == ADLG_IDYES)
                    {
                        __s32 str[] = {STRING_HOME_PHOTO, STRING_PHOTO_BG_SAVED};
                        photo_save_bg();
                        default_dialog(wnd_para->h_dialog, msg->h_deswin, PHOTO_BG_OK_DIALOG_ID, ADLG_OK, str);
                        return EPDK_OK;
                    }
                }

                return EPDK_OK;

                case ADLG_CMD_SET_UI:
                {
                    //set dialog ui para
                    app_dialog_set_ui(msg->h_srcwin, ADLG_UI_OK, 0, 0);
                }
                break;
            }
        }
        break;

        case PHOTO_BG_OK_DIALOG_ID:
        {
            if (HIWORD(msg->dwAddData1) == ADLG_CMD_EXIT)
            {
                app_dialog_destroy(msg->h_srcwin);
                wnd_para->h_dialog = NULL;
            }
        }
        break;
    }

    return EPDK_OK;
}

/************************************************************************/
//自定义命令消息处理(MSG_PHOTO_COMMAND)
/************************************************************************/
static __s32 on_photo_command(__gui_msg_t *msg)
{
    H_WIN hwnd = msg->h_deswin;
    PHOTO_WND_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);
    APP_DEBUG_CBMSGEx;

    switch (msg->dwAddData1)
    {
        case ID_PHOTO_CMD_EXIT:
        {
            notify_to_close_photo(hwnd, SWITCH_TO_EXPLORER);
        }
        break;

        case ID_PHOTO_CMD_BG_MUSIC:
        {
            //          if (msg->dwAddData2 == 0)
            if (photo_get_bgmusic() == 0)
            {
                OpenBgMusic(hwnd);
            }
            else
            {
                CloseBgMusic(hwnd);
            }
        }
        break;

        case ID_PHOTO_CMD_POP_MENU:
        {
            //__log("%s %d--------收到创建SSET", __FILE__,__LINE__);
            /*删除phot frm win */
            do_anole_command(hwnd, ID_ANOLE_CMD_PAUSE, 0);

            app_com_layer_destroy(wnd_para->h_main_lyr);

            wnd_para->h_main_lyr = NULL;
            wnd_para->h_main_frw = NULL;

            APP_ANOLE_EnableTouch(EPDK_FALSE);
            gscene_hbar_set_state(HBAR_ST_SHOW);
            //          do_anole_command(hwnd, ID_ANOLE_CMD_FILEINFO, HIDE_UI);
            //创建sset
            photo_create_sset(wnd_para, 0xff);
            wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_MENU;
        }
        break;

        case ID_PHOTO_CMD_HIDE_MENU:
        {
            //__log("%s %d--------收到删除SSET", __FILE__,__LINE__);
            photo_destroy_sset(wnd_para, 0xff);
            gscene_hbar_set_state(HBAR_ST_HIDE);
            wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_NULL;
            APP_ANOLE_EnableTouch(EPDK_TRUE);
            //CheckAPPMEM();
        }
        break;

        case ID_PHOTO_CMD_TOUCH_UP:
        {
            //          __s32 x, y;
            //
            //          x = LOSWORD(msg->dwAddData2);
            //          y = HISWORD(msg->dwAddData2);
            if (wnd_para->h_sset)
            {
                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, 0);
            }
            else
            {
                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_POP_MENU, 0);
            }
        }
        break;

        case ID_PHOTO_CMD_KILL_PHOTO_FRMWIN:
        {
            __wrn("%s %d--------删除photo frm win", __FILE__, __LINE__);
            app_com_layer_destroy(wnd_para->h_main_lyr);
            wnd_para->h_main_lyr = NULL;
            wnd_para->h_main_frw = NULL;
            wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_NULL;
            APP_ANOLE_EnableTouch(EPDK_TRUE);
        }
        break;

        case ID_PHOTO_CMD_POP_SAVE_DIALOG:
        {
            __s32 str[] = {STRING_HOME_PHOTO, STRING_CONFIRM_BG_SET};
            //__here__;
            default_dialog(wnd_para->h_dialog, msg->h_deswin, PHOTO_SAVE_BG_DIALOG_ID, ADLG_YESNO, str);
            //__here__;
        }
        break;

        case ID_PHOTO_CMD_INTERVAL:
        {
            __wrn("msg->dwAddData2=%d", msg->dwAddData2);
            set_photo_value(msg->dwAddData2, photo_set_speedflag);
        }
        break;

        case ID_PHOTO_CMD_SCALE:
        {
            if (wnd_para && wnd_para->h_main_lyr)
            {
                SEND_MSG(MSG_PHOTO_SCALE, hwnd, wnd_para->h_main_frw, msg->dwAddData2, msg->dwAddData2);
            }
            else
            {
                wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_SCALE;
                APP_ANOLE_EnableTouch(EPDK_FALSE);
                //__here__;
                __log("...........ID_PHOTO_CMD_SCALE");
                on_photo_frm_create(msg);
            }
        }
        break;

        //////////////////////////////anole cmd///////////////////////////
        case ID_ANOLE_CMD_SLIDESHOW:
        {
            APP_ANOLE_SetSwitchMode(msg->dwAddData2);
        }
        break;

        case ID_ANOLE_CMD_INTERVAL:
        {
            APP_ANOLE_SetPlayInterval(msg->dwAddData2, msg->dwAddData2);
        }
        break;

        case ID_ANOLE_CMD_AUTOPLAY:
        {
            __wrn("*********ID_ANOLE_CMD_AUTOPLAY***********");
            APP_ANOLE_SetSwitchMode(photo_get_effect());
            APP_ANOLE_AutoPlay();
            dsk_set_auto_off_time(0);
        }
        break;

        case ID_ANOLE_CMD_PAUSE:
        {
            __wrn("*********ID_ANOLE_CMD_PAUSE***********");
            APP_ANOLE_Pause();
        }
        break;

        case ID_ANOLE_CMD_FILEINFO:
        {
            if (wnd_para && wnd_para->h_main_lyr)
            {
                __wrn("GUI_LyrWinGetSta(wnd_para->h_main_lyr) is %d %x", GUI_LyrWinGetSta(wnd_para->h_main_lyr), wnd_para->h_main_lyr);
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, wnd_para->h_main_frw, ID_ANOLE_CMD_FILEINFO, msg->dwAddData2);
            }
            else
            {
                __wrn("GUI_LyrWinGetSta(wnd_para->h_main_lyr) is %d %x", GUI_LyrWinGetSta(wnd_para->h_main_lyr), wnd_para->h_main_lyr);
                wnd_para->photo_frm_para->ctrl_para.show_para = PHOTO_SHOW_FILEINFO;
                //__here__;
                on_photo_frm_create(msg);
            }
        }
        break;

        case  ID_PHOTO_CMD_PAUSE:
        {
            if (wnd_para && wnd_para->h_main_lyr)
            {
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, wnd_para->h_main_frw, ID_PHOTO_CMD_PAUSE, msg->dwAddData2);
            }

            __wrn("********ID_PHOTO_CMD_PAUSE*********");
        }
        break;

        case ID_PHOTO_CMD_AUTOPLAY:
        {
            if (wnd_para && wnd_para->h_main_lyr)
            {
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, wnd_para->h_main_frw, ID_PHOTO_CMD_AUTOPLAY, msg->dwAddData2);
            }

            __wrn("********ID_PHOTO_CMD_AUTOPLAY*********");
        }
        break;

        case ID_PHOTO_CMD_PLAYPREV:
        {
            if (wnd_para && wnd_para->h_main_lyr)
            {
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, wnd_para->h_main_frw, ID_PHOTO_CMD_PLAYPREV, msg->dwAddData2);
            }
        }
        break;

        case  ID_PHOTO_CMD_PLAYNEXT:
        {
            if (wnd_para && wnd_para->h_main_lyr)
            {
                SEND_MSG(MSG_PHOTO_SHOW_HIDE_PARA, hwnd, wnd_para->h_main_frw, ID_PHOTO_CMD_PLAYNEXT, msg->dwAddData2);
            }
        }
        break;

        case ID_ANOLE_CMD_SCALE:
        {
            switch (msg->dwAddData2)
            {
                case PHOTO_SCALE_ITEM_ROTATELEFT:
                {
                    APP_ANOLE_RotateLeft();
                }
                break;

                case PHOTO_SCALE_ITEM_MOVE_UP:
                {
                    APP_ANOLE_ScaleMove(ANOLE_MOVE_UP);
                }
                break;

                case PHOTO_SCALE_ITEM_RPTATERIGHT:
                {
                    APP_ANOLE_RotateRight();
                }
                break;

                case PHOTO_SCALE_ITEM_MOVE_LEFT:
                {
                    APP_ANOLE_ScaleMove(ANOLE_MOVE_LEFT);
                }
                break;

                case PHOTO_SCALE_ITEM_EXIT:
                {
                    if (wnd_para && wnd_para->h_main_lyr)
                    {
                        wnd_para->photo_frm_para->ctrl_para.scale_new_item = wnd_para->photo_frm_para->ctrl_para.scale_old_item = PHOTO_SCALE_ITEM_ALL;
                        SEND_MSG(MSG_PHOTO_SCALE, hwnd, wnd_para->h_main_frw, HIDE_UI, HIDE_UI);
                    }
                }
                break;

                case PHOTO_SCALE_ITEM_MOVE_RIGHT:
                {
                    APP_ANOLE_ScaleMove(ANOLE_MOVE_RIGHT);
                }
                break;

                case PHOTO_SCALE_ITEM_ZOOMDEC:
                {
                    APP_ANOLE_ZoomDec();
                }
                break;

                case PHOTO_SCALE_ITEM_MOVE_DOWN:
                {
                    APP_ANOLE_ScaleMove(ANOLE_MOVE_DOWN);
                }
                break;

                case PHOTO_SCALE_ITEM_ZOOMINC:
                {
                    APP_ANOLE_ZoomInc();
                }
                break;
            }
        }
        break;

        default:
            //__log("%s  %d---收到的是%d  没做什么",__FILE__,__LINE__,msg->dwAddData1);
            break;
    }

    return EPDK_OK;
}

/************************************************************************/
//拔卡处理
/************************************************************************/
static __s32 on_photo_sdcrad_plugout(__gui_msg_t *msg)
{
    __bool to_exist;//是否退出
    __s32 ret;
    char diskname[2] = {0};
    char cur_play_file[RAT_MAX_FULL_PATH_LEN] = {0};
    __msg("************DSK_MSG_FS_PART_PLUGOUT*******");
    diskname[0] = (char)(msg->dwAddData2);
    diskname[1] = 0;
    to_exist = 1;//默认需要退出
    ret = anole_npl_get_cur();

    if (-1 != ret)
    {
        ret = anole_npl_index2file(ret, cur_play_file);

        if (EPDK_OK == ret)
        {
            if (0 != cur_play_file[0]
                && 0 != diskname[0]
                && 0 != eLIBs_strnicmp(cur_play_file, diskname, 1))
            {
                to_exist = 0;//拔的卡不是当前播放文件所在卡，不需要退出
            }
        }
    }

    if (1 == to_exist)
    {
        //退到桌面
        notify_to_close_photo(msg->h_deswin, SWITCH_TO_MMENU);
    }

    return EPDK_OK;
}

//显示下一图片时的回调
static __s32 photo_cb_show_next_pic(void *arg_p)
{
    //  static __s32 OldIndex = -1, OldNum = -1;
    H_WIN hwnd;
    PHOTO_WND_T *wnd_para;
    __gui_msg_t  setmsg = {0};
    hwnd = (H_WIN)arg_p;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, hwnd);
    ASSERT(wnd_para);

    if (wnd_para->h_main_lyr && wnd_para->h_main_frw && APP_ANOLE_GetSta() == ANOLE_MODE_PLAY)
    {
        setmsg.h_deswin   = wnd_para->h_main_frw;
        setmsg.id         = MSG_PHOTO_NEW_PLAY;
        setmsg.dwAddData1 = APP_ANOLE_GetCurPicIndex();
        setmsg.dwAddData2 = APP_ANOLE_GetPicNum();
        //      if (OldIndex != setmsg.dwAddData1 || OldNum != setmsg.dwAddData2)
        //      {
        GUI_PostMessage(&setmsg);
        //          OldIndex = setmsg.dwAddData1;
        //          OldNum   = setmsg.dwAddData2;
        //      }
    }

    return EPDK_OK;
}

//显示图片出错时的回调
static __s32 photo_cb_anole_show_error(void *arg_p)
{
    if (arg_p)
    {
        PHOTO_WND_T *wnd_para;
        PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, (H_WIN)arg_p);

        if (wnd_para)
        {
            APP_ANOLE_ShowErrorPic();
        }
    }

    __msg("photo_cb_anole_show_error...");
    return EPDK_OK;
}

/************************************************************************/
//中间件模块卸载
/************************************************************************/
static int32_t photo_uninit_module(H_WIN hwnd, PHOTO_WND_T *wnd_para)
{
    APP_ANOLE_Exit();
    return EPDK_OK;
}

/************************************************************************/
//中间件模块初始化
/************************************************************************/
static int32_t photo_init_module(H_WIN hwnd, PHOTO_WND_T *wnd_para)
{
    load_bmp_res_t      err_pic, decoding_pic;
    APP_ANOLE_INIT_T    InitData;
    int32_t             ret;

    create_bmp_res(ID_PHOTO_ERROR_BMP,      err_pic);
    create_bmp_res(ID_PHOTO_DECODING_BMP,   decoding_pic);

    InitData.pErrFileData   = ResHandle2Data(err_pic);
    InitData.pDecodingData  = ResHandle2Data(decoding_pic);
    InitData.pUserPara      = (void *)hwnd;
    InitData.cb_err         = photo_cb_anole_show_error;
    InitData.cb_pic_change  = photo_cb_show_next_pic;

    ret = APP_ANOLE_Init(&InitData);

    if (ret != 0)
    {
        __msg("APP_ANOLE_Init failed");
    }
    else
    {
        ;//APP_ANOLE_SetShowMode(photo_get_ratio());
    }

    destroy_bmp_res(err_pic);
    destroy_bmp_res(decoding_pic);
    return ret;
}

/************************************************************************/
//主窗口销毁处理
/************************************************************************/
static __s32 on_photo_destory(__gui_msg_t *msg)
{
    PHOTO_WND_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);
    //TODO:release resource
    app_com_layer_destroy(wnd_para->h_main_lyr);
    //刷新注册表数据
    //update_to_reg();
    //保存断点
    photo_save_play_info((rat_type_t)wnd_para->root_type);
    //允许自动关屏
    g_enable_close_scn();

    if (wnd_para->h_frm_alarm_coming)
    {
        app_alarm_frm_destory(wnd_para->h_frm_alarm_coming);
        wnd_para->h_frm_alarm_coming = NULL ;
    }

    if (wnd_para->h_dialog)
    {
        app_dialog_destroy(wnd_para->h_dialog);
        wnd_para->h_dialog = NULL;
    }

    photo_destroy_sset(wnd_para, 0xff);
    photo_uninit_module(msg->h_deswin, wnd_para);
    BFreeType(wnd_para->photo_frm_para, PHOTO_MAIN_FRW_T);
    BFreeType(wnd_para, PHOTO_WND_T);
    //EndOfDebugAPPMEM();
    __msg("!!----on_photo_destory end");
    return 0;
}

/************************************************************************/
//主窗口创建处理
/************************************************************************/
static int32_t on_photo_create(__gui_msg_t *msg)
{
    //  PHOTO_UI_PARA_T *ui;
    PHOTO_WND_T     *wnd_para;

    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);

    //不显示headbar
    gscene_hbar_set_state(HBAR_ST_HIDE);

    //关背景
    gscene_bgd_set_state(BGD_STATUS_HIDE);

    //禁止自动关屏
    g_disable_close_scn();

    //设置调色板
    //com_set_palette_by_id(ID_PHOTO_COLOURTABLE_BMP);
    {
        //背景色，文字编辑底色
        //__u32 color[]={COLOUR_BG_8BPP, COLOUR_EDIT_BG_8BPP};
        //com_set_palette_color_by_index(COLOUR_8BPP_INDEX_0, color, BEETLES_TBL_SIZE(color));
    }

    /*为了节省内存暂时不创建图层和窗口,只初始化一下窗口的属性，当学要的时候在创建*/
    BallocType(wnd_para->photo_frm_para, PHOTO_MAIN_FRW_T);
    ASSERT(wnd_para->photo_frm_para != NULL);

    ZeroTypeMem(wnd_para->photo_frm_para, PHOTO_MAIN_FRW_T);

    wnd_para->photo_frm_para->ctrl_para.show_para   = PHOTO_SHOW_NULL;
#if 0
    //TODO:init wnd_para
    ui = get_photo_uipara();
    photo_layer_create(wnd_para->h_main_lyr, &ui->rt, "photo_main");
    wnd_para->h_main_frw = photo_main_frw_create(GetPhotoManWnd(), wnd_para->h_main_lyr, wnd_para);
    _app_show_focus_wnd(wnd_para->h_main_frw, wnd_para->h_main_lyr);
    GUI_LyrWinSetTop(wnd_para->h_main_lyr);
#endif
    photo_init_module(msg->h_deswin, wnd_para);
    return 0;
}


static H_LYR photo_layer_create(RECT  *rect)
{
    H_LYR layer = NULL;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        //.. {FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        //    DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        0,                                              /* ck_mode   */
        1,                                              /* alpha_en  */
        0XAA,                                              /* alpha_val */
        1,                                              /* pipe      */
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "photo info layer",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    //uipara = (setting_uipara_t*)setting_get_uipara();
    fb.size.width       = rect->width;
    fb.size.height      = rect->height;
    lstlyr.src_win.x        = 0;
    lstlyr.src_win.y        = 0;
    lstlyr.src_win.width        = rect->width;
    lstlyr.src_win.height   = rect->height;
    lstlyr.scn_win.x        = rect->x;
    lstlyr.scn_win.y        = rect->y;
    lstlyr.scn_win.width    = rect->width;
    lstlyr.scn_win.height   = rect->height;
    lstlyr.pipe = 1;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}


/************************************************************************/
//主实体窗口创建处理
/************************************************************************/
static __s32 on_photo_frm_create(__gui_msg_t *msg)
{
    PHOTO_UI_PARA_T *ui = NULL;
    PHOTO_WND_T *wnd_para = NULL;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);
    //TODO:init wnd_para
    __log("sfaagffe.....on_photo_frm_create");
    ui = get_photo_uipara();
    wnd_para->h_main_lyr = photo_layer_create(&ui->rt);
    wnd_para->h_main_frw = photo_main_frw_create(GetPhotoManWnd(), wnd_para->h_main_lyr, wnd_para);
    _app_show_focus_wnd(wnd_para->h_main_frw, wnd_para->h_main_lyr);
    GUI_LyrWinSetTop(wnd_para->h_main_lyr);
    return 0;
}





/************************************************************************/
//触摸消息处理
/************************************************************************/
static __bool  photo_main_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __bool b_ret = EPDK_FALSE;
    PHOTO_WND_T *wnd_para;
    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);

    //__log(" %s  %d----------photo MAN win 触摸到(%d,%d) show_para = %d",__FILE__,__LINE__,x,y,
    //  wnd_para->photo_frm_para->ctrl_para.show_para);

    if (wnd_para->photo_frm_para->ctrl_para.show_para == PHOTO_SHOW_NULL)
    {
        if (APP_ANOLE_IsEnableTouch())
        {
            __u32       dwAddData1 = 0;

            //__log("%s  %d-----%d",__FILE__,__LINE__,msg->dwAddData1);
            switch (msg->dwAddData1)
            {
                case GUI_MSG_TOUCH_OVERDOWN:
                    dwAddData1 = GUI_MSG_TOUCH_DOWN;
                    break;

                case GUI_MSG_TOUCH_OVERLONGDOWN:
                    dwAddData1 = GUI_MSG_TOUCH_LONGDOWN;
                    break;

                case GUI_MSG_TOUCH_OVERUP:
                    dwAddData1 = GUI_MSG_TOUCH_UP;
                    break;

                case GUI_MSG_TOUCH_OVERMOVE:
                    dwAddData1 = GUI_MSG_TOUCH_MOVE;
                    break;

                case GUI_MSG_TOUCH_OVERZOOMIN:
                    dwAddData1 = GUI_MSG_TOUCH_ZOOMIN;
                    break;

                case GUI_MSG_TOUCH_OVERZOOMOUT:
                    dwAddData1 = GUI_MSG_TOUCH_ZOOMOUT;
                    break;

                default:
                    break;
            }

            if (APP_ANOLE_Touch(x, y, dwAddData1, LOSWORD(msg->dwReserved)))
            {
                //__here__;
                return EPDK_TRUE;
            }
        }

        //__log("%s  %d--------------APP_ANOLE_Touch do nothing,  msg->dwAddData1= %d", __FILE__,__LINE__,msg->dwAddData1);
        {
            PHOTO_MAIN_FRW_CTRL_PARA_T *ctrl_para = &wnd_para->photo_frm_para->ctrl_para;

            if (msg->dwAddData1 == GUI_MSG_TOUCH_UP || msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP)
            {
                {
                    if (ctrl_para->last_action == GUI_MSG_TOUCH_DOWN || ctrl_para->last_action == GUI_MSG_TOUCH_LONGDOWN
                        || ctrl_para->last_action == GUI_MSG_TOUCH_OVERDOWN || ctrl_para->last_action == GUI_MSG_TOUCH_OVERLONGDOWN)
                    {
                        //无移动操作
                        __s32 d1, d2;
                        d1 = ABS(x - ctrl_para->last_pos.x);
                        d2 = ABS(y - ctrl_para->last_pos.y);

                        if (d1 <= TouchMinDist() && d2 <= TouchMinDist())
                        {
                            if (wnd_para->h_sset)
                            {
                                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, 0);
                            }
                            else
                            {
                                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_POP_MENU, 0);
                            }
                        }
                    }
                }
                //为下一次动作清空数据
                ctrl_para->last_pos.x = 0;
                ctrl_para->last_pos.y = 0;
            }
            else
            {
                ctrl_para->last_pos.x = x;
                ctrl_para->last_pos.y = y;
                ctrl_para->last_action = msg->dwAddData1;
            }

            b_ret = EPDK_TRUE;
        }
    }
    else  if (wnd_para->photo_frm_para->ctrl_para.show_para == PHOTO_SHOW_MENU)
    {
        if (SSET_court_is_tbar() == EPDK_TRUE)
        {
            if (y < 480 - SSET_TBAR_WIN_SRC_HEIGHT)
            {
                /*触摸区域没有点钟SSET TBAR*/
                if (msg->dwAddData1 == GUI_MSG_TOUCH_UP || msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP)
                {
                    do_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, 0);
                }

                b_ret = EPDK_TRUE;
            }
            else
            {
                b_ret = EPDK_FALSE;
            }
        }
        else
        {
            /*触摸区域没有点钟SSET TBAR*/
            if (msg->dwAddData1 == GUI_MSG_TOUCH_UP || msg->dwAddData1 == GUI_MSG_TOUCH_OVERUP)
            {
                do_photo_command(msg->h_deswin, ID_PHOTO_CMD_HIDE_MENU, 0);
            }

            b_ret = EPDK_TRUE;
        }
    }
    else
    {
        b_ret = EPDK_FALSE;
    }

    return b_ret;
}

//主消息处理函数
static int32_t _app_photo_Proc(__gui_msg_t *msg)
{
    APP_DEBUG_CBMSGEx;

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            g_PhotoMainWnd  = msg->h_deswin;
            return on_photo_create(msg);
        }

        case GUI_MSG_DESTROY:
        {
            on_photo_destory(msg);

            g_PhotoMainWnd  = NULL;

            {
                reg_root_para_t *root_reg_para;

                root_reg_para   = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

                if (root_reg_para)
                {
                    root_reg_para->cur_play_storage = -1;
                }
                else
                {
                    __wrn("para is null...");
                }
            }

            //如果没有后台音频，则允许自动关机
            if (EPDK_FALSE == is_app_exist(APP_MUSIC))
            {
                reg_system_para_t   *para;

                para = (reg_system_para_t *)dsk_reg_get_para_by_app(REG_APP_SYSTEM);

                if (para)
                {
                    dsk_set_auto_off_time(para->poweroff);
                    __wrn("para->poweroff=%d", para->poweroff);
                }
            }

            return EPDK_OK;
        }

        case DSK_MSG_HOME:
        {
            notify_to_close_photo(msg->h_deswin, SWITCH_TO_MMENU);
        }

        return EPDK_OK;

        case DSK_MSG_KILL:
        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
        }
        break;

        case GUI_MSG_COMMAND:
        {
            return on_windos_command(msg);
        }

        case MSG_PHOTO_COMMAND:
        {
            //esKSRV_SysInfo();
            return on_photo_command(msg);
        }

        case GUI_MSG_KEY:
        {
            if (photo_key_proc(msg) == EPDK_TRUE)
            {
                return EPDK_OK;
            }
        }
        break;

        case GUI_MSG_TOUCH:
        {
            if (photo_main_touch_proc(msg) == EPDK_TRUE)
            {
                return EPDK_OK;
            }
        }
        break;

        case DSK_MSG_FS_PART_PLUGOUT:
            return on_photo_sdcrad_plugout(msg);

        case DSK_MSG_ALARM:
        {
            PHOTO_WND_T     *wnd_para;

            PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);

            if (wnd_para->h_dialog)
            {
                app_dialog_destroy(wnd_para->h_dialog);
                wnd_para->h_dialog = NULL ;
            }

            if (wnd_para->h_sset)
            {
                SSET_Destroy(wnd_para->h_sset);

                wnd_para->h_sset    = NULL ;

                gscene_hbar_set_state(HBAR_ST_HIDE);    //弹出sset时，headbar 也处于show状态，故也隐藏
            }

            wnd_para->h_frm_alarm_coming    = app_alarm_frm_create(msg->h_deswin);

            return EPDK_OK ;
        }

        case ALARM_MSG_CLOSE:
        {
            PHOTO_WND_T     *wnd_para;

            PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, msg->h_deswin);

            if (wnd_para->h_frm_alarm_coming)
            {
                wnd_para->h_frm_alarm_coming    = NULL ;
            }

            GUI_WinSetFocusChild(msg->h_deswin);
            return EPDK_OK ;
        }

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}


H_WIN app_photo_create(root_para_t *para)
{
    __gui_manwincreate_para_t   create_info;
    PHOTO_WND_T                 *wnd_para = NULL;
    H_WIN                       hManWin = NULL;

    //StartDebugAPPMEM();
    //esKSRV_SysInfo();
    GUI_Memdev_FrameBuffer_Release(1);

    __inf("*****************************************");
    __inf("********  enter photo app  **************");
    __inf("*****************************************");

    app_set_title(STRING_HOME_PHOTO);

    BallocType(wnd_para, PHOTO_WND_T);

    ZeroTypeMem(wnd_para, PHOTO_WND_T);

    wnd_para->font      = para->font;
    wnd_para->root_type = (H_WIN)para->root_type;

    ZeroTypeMem(&create_info, __gui_manwincreate_para_t);

    create_info.name            = APP_PHOTO;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_app_photo_Proc);
    create_info.attr            = (void *)wnd_para;
    create_info.id              = APP_PHOTO_ID;
    create_info.hHosting        = NULL;

    hManWin     = GUI_ManWinCreate(&create_info);

    if (hManWin)
    {
        reg_root_para_t *root_reg_para;
        int32_t         reg_storage_type= 0;

        if (para)
        {
            if (RAT_TF == para->root_type)
            {
                reg_storage_type = 0;
            }
            else if (RAT_USB == para->root_type)
            {
                reg_storage_type = 1;
            }
            else
            {
                __wrn("para->root_para->root_type invalid...");
            }
        }

        root_reg_para   = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

        if (root_reg_para)
        {
            int32_t reg_app_type = 3;

            root_reg_para->cur_play_storage     = reg_storage_type;
            root_reg_para->last_play_app        = reg_app_type;

            root_reg_para->last_app_play_storage[reg_app_type]      = reg_storage_type;
            root_reg_para->last_storage_play_app[reg_storage_type]  = reg_app_type;
        }
        else
        {
            __wrn("para is null...");
        }
    }

    return hManWin;
}

static int32_t app_photo_notify_delete_sub_scene2(H_WIN hmanwin)
{
    PHOTO_WND_T     *wnd_para;

    if (NULL == hmanwin)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, hmanwin);

    if (wnd_para->h_sset)
    {
        todo_photo_command(hmanwin, ID_PHOTO_CMD_HIDE_MENU, 0);
    }

    return EPDK_OK;
}

int32_t app_photo_notify_delete_sub_scene(H_WIN hmanwin)
{
    PHOTO_WND_T     *wnd_para;

    if (NULL == hmanwin)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    PHOTO_GetWndPara(wnd_para, PHOTO_WND_T, hmanwin);

    if (wnd_para->h_sset)
    {
        do_photo_command(hmanwin, ID_PHOTO_CMD_HIDE_MENU, 0);
    }

    return EPDK_OK;
}
