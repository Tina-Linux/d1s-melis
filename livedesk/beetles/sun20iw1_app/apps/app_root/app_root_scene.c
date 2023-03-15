/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <log.h>
#include "beetles_app.h"
#include "app_root.h"
#include "app_root_scene.h"
#include "app_root_init.h"
#include "multi_screen_home/app_multi_screen_home_i.h"
#include "explorer/Explorer.h"
#include "movie/app_movie.h"
#include "music/music.h"
#include "photo/App_photo.h"
#include "setting/app_setting.h"
#include "fm/app_fm.h"
#include "ebook/app_ebook.h"
#include "record/app_record.h"
#include "record_video/record_video.h"
#include "desktop_scene.h"
#include "app_multi_screen_home.h"
#include "dtv.h"

//#define ENABLE_MTP_FUNCTION
#ifdef CONFIG_DRIVERS_USB_GADGET_MTP
#include "mtp_app.h"
#endif
typedef struct tag_root_ctrl
{
    H_WIN   h_app_home;
    H_WIN   h_app_dtv;
    H_WIN   h_app_explorer;
    H_WIN   h_app_fm;
    H_WIN   h_app_movie;
    H_WIN   h_app_music;
    H_WIN   h_app_photo;
    H_WIN   h_app_ebook;
    H_WIN   h_app_usb_audio;
    H_WIN   h_app_setting;
    H_WIN   h_app_calendar;
    H_WIN   h_app_record;
#ifdef APP_ADD_RECORD_VEDIO
    H_WIN   h_app_record_video;
#endif
    root_para_t *root_para;
} root_ctrl_t;


typedef enum
{
    app_root_prob_ok = 0,
    app_root_prob_fail,
    app_root_prob_already_running,
    app_root_prob_not_running,
    app_root_prob_no_correspond_storage,
    app_root_prob_no_meida_file,
    app_root_prob_storage_switch,
    app_root_prob_media_switch,
    app_root_prob_storage_plugined,
} app_root_prob_result_t;


typedef enum
{
    app_root_prob_intent_switch_media = 0,
    app_root_prob_intent_switch_storage,
    app_root_prob_intent_plugin_for_play,
    app_root_prob_intent_first_play,
    app_root_prob_intent_max,
    app_root_prob_intent_unknown,
} app_root_prob_intent_t;

/**********************************************************************************************************************/

typedef enum
{
    APP_ROOT_MSG_DO_ASYNC = GUI_MSG_ + 1,
} app_root_msg_id_t;

typedef __s32(*p_app_root_fun)(void *arg);

#define APP_COM_DLG_ID           (APP_ROOT_ID+1)
#define APP_COM_FLUSH_ID         (APP_ROOT_ID+2)


//向hwin发送异步消息，执行pfun函数
static __s32 __app_do_async(H_WIN hwin, __u32 msg_id, p_app_root_fun pfun, void *arg)
{
    if (0 == hwin)
    {
        return EPDK_FAIL;
    }

    {
        __gui_msg_t setmsg;
        setmsg.id           =  msg_id;
        setmsg.h_srcwin     =  NULL;
        setmsg.h_deswin     =  hwin;
        setmsg.dwAddData1   = (unsigned long)pfun;
        setmsg.dwAddData2   = (unsigned long)arg;
        setmsg.p_arg        =  NULL;
        __log("setmsg.id=%x, setmsg.h_deswin=%x, setmsg.dwAddData1=%x, setmsg.dwAddData2=%x",
              setmsg.id, setmsg.h_deswin, setmsg.dwAddData1, setmsg.dwAddData2);
        GUI_SendNotifyMessage(&setmsg);
    }

    return EPDK_OK;
}

//向hwin发送setfocus消息，syn 为1代表需要直接调用GUI_WinSetFocusChild。
static void __app_set_focus_child(H_WIN hwin, __s32 syn)
{
    if (0 == hwin)
    {
        return;
    }

    if (1 == syn)
    {
        GUI_WinSetFocusChild(hwin);
    }
    else
    {
        __gui_msg_t setmsg;
        setmsg.id           =  GUI_MSG_SET_FOCUS;
        setmsg.h_srcwin     =  NULL;
        setmsg.h_deswin     =  hwin;
        setmsg.dwAddData1   =  NULL;
        setmsg.dwAddData2   =  NULL;
        setmsg.p_arg        =  NULL;
        GUI_SendNotifyMessage(&setmsg);
    }
}


int32_t __app_root_get_bg_music_index(__u32 *index)
{
    H_WIN   root, child;
    char    winname[256];
    H_WIN   hMusic = NULL;

    root    = GUI_WinGetHandFromName(APP_ROOT);
    child   = GUI_WinGetFirstChild(root);
    hMusic  = 0;

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        if (0 == eLIBs_strcmp(APP_MUSIC, winname))
        {
            hMusic = child;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    return app_music_get_index(hMusic, index);
}



uint32_t SetCurrentDevTypeEx(uint32_t curdev) //curdev = 3: USB, curdev = 2:SD
{
    reg_root_para_t *para;

    para    = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);
    //para->last_type=curdev;
    para->cur_storage   = curdev;
    return EPDK_OK;
}

uint32_t GetCurrentDevTypeEx(void)
{
    reg_root_para_t     *para;

    para    = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);
    //return(para->last_type);
    return (para->cur_storage);
}


static void __app_home_restore(__gui_msg_t *msg)
{
    root_ctrl_t   *root_ctrl;

    root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

    __log("__app_home_restore");
    if (root_ctrl->h_app_home)
    {
        __gui_msg_t mymsg;

        mymsg.h_deswin      = root_ctrl->h_app_home;
        mymsg.id            = DSK_APP_RESTORE;
        mymsg.dwAddData1    = 0;
        mymsg.dwAddData2    = 0;

        GUI_SendNotifyMessage(&mymsg);

        mymsg.h_deswin      = root_ctrl->h_app_home;
        mymsg.id            = DSK_MSG_FS_PART_PLUGOUT;
        mymsg.dwAddData1    = msg->dwAddData1;
        mymsg.dwAddData2    = msg->dwAddData2;

        GUI_SendNotifyMessage(&mymsg);

        __app_set_focus_child(root_ctrl->h_app_home, 0);
        gscene_hbar_set_state(HBAR_ST_SHOW);
        gscene_bgd_set_state(BGD_STATUS_SHOW);
    }
    else
    {
        root_ctrl->h_app_home   = app_home_create(root_ctrl->root_para);
        __app_set_focus_child(root_ctrl->h_app_home, 0);
    }
}

static int32_t __app_root_is_bg_music_exist(__gui_msg_t *msg, H_WIN *hMusic)
{
    H_WIN   root, child;
    char    winname[256];
    __bool  exist;

    root    = GUI_WinGetHandFromName(APP_ROOT);
    child   = GUI_WinGetFirstChild(root);
    exist   = 0;
    *hMusic = 0;

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);
        __log("winname = %s ", winname);

        if (0 == eLIBs_strcmp(APP_MUSIC, winname))
        {
            *hMusic = child;
            exist = 1;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    return exist;
}


static void __app_root_delete_bg_music(__gui_msg_t *msg)
{
    H_WIN       root, child, next_child;
    char        winname[256];
    root_para_t *root_para;
    root_ctrl_t *root_ctrl;

    root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    root_para   = root_ctrl->root_para;
    root        = GUI_WinGetHandFromName(APP_ROOT);

    if (!root)
    {
        __err("GUI_WinGetHandFromName fail...");
        return ;
    }

    child = GUI_WinGetFirstChild(root);

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);
        next_child = GUI_WinGetNextBro(child);
        __log("winname = %s ", winname);

        if (0 == eLIBs_strcmp(APP_MUSIC, winname))
        {
            GUI_ManWinDelete(child);
            root_ctrl->h_app_music = NULL;
            break;
        }

        child = next_child;
    }
}

static void __app_root_create_bg_music(__gui_msg_t *msg)
{
    H_WIN       root, child;
    char        winname[256];
    __bool      exist;
    root_para_t *root_para;
    root_ctrl_t *root_ctrl;

    root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    root_para   = root_ctrl->root_para;
    root        = GUI_WinGetHandFromName(APP_ROOT);
    child       = GUI_WinGetFirstChild(root);
    exist       = 0;

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);
        __log("winname = %s ", winname);

        if (0 == eLIBs_strcmp(APP_MUSIC, winname))
        {
            exist = 1;
            break;
        }

        child = GUI_WinGetNextBro(child);
    }

    if (0 == exist)
    {
        root_para->data = MUSICPLAYER_BACKGROUND;
        root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
        root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
    }
}


//进入应用之前把背景设置为默认背景
static void __app_root_change_to_default_bg(void)
{
    reg_init_para_t *para;

    para = dsk_reg_get_para_by_app(REG_APP_INIT);

    if (para)
    {
        para->bgd_flag_in_main  = gscene_bgd_get_flag();
        if (EPDK_TRUE == para->bgd_flag_in_main) //当前是默认背景，则不做任何事情
        {
        }
        else//当前不是默认背景，修改为默认背景
        {
            gscene_bgd_set_flag(EPDK_TRUE);
            gscene_bgd_refresh();
        }
    }
    else
    {
        __log("dsk_reg_get_para_by_app fail...");
    }
}

static int32_t app_root_command_proc(__gui_msg_t *msg)
{
    root_para_t *root_para;
    root_ctrl_t *root_ctrl;
    __gui_msg_t mymsg;

    root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    root_para   = root_ctrl->root_para;

    __log("*****************app_root_command_proc**************");

    switch (LOWORD(msg->dwAddData1))
    {
        case APP_HOME_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_OTHER_APP:
                {
                    GUI_Memdev_FrameBuffer_Release(1);
                    GUI_ManWinDelete(root_ctrl->h_app_home);
                    root_ctrl->h_app_home = NULL;

                    switch (msg->dwAddData2)
                    {
                    	case ID_HOME_EXPLORER:
                        {
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            root_para->root_type = msg->dwReserved;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = ID_EXPLORER_ALL;
                            root_ctrl->root_para->h_parent = msg->h_deswin;
                            root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
							if(root_ctrl->h_app_explorer)
							{
                            	GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
							}
							
                        }
                        break;
                        case ID_HOME_DTV:
                        case ID_DTV_SHORTCUT:
                        {
                            __app_root_delete_bg_music(msg);
                            root_ctrl->h_app_dtv = (H_WIN)app_dtv_create(root_para);

                            if (root_ctrl->h_app_dtv)
                            {
                                __log("root_ctrl->h_app_dtv = %x", root_ctrl->h_app_dtv);
                                GUI_WinSetFocusChild(root_ctrl->h_app_dtv);
                            }
                        }
                        break;

                        case ID_HOME_FM:
                        case ID_FM_SHORTCUT:
                        {
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            //删除后台音乐
                            __app_root_delete_bg_music(msg);
                            __log("**********enter fm**********");
                            root_ctrl->root_para->h_parent = msg->h_deswin;
                            root_ctrl->h_app_fm =  app_fm_create(root_para);
                            __log("root_ctrl->h_app_fm=%08x", root_ctrl->h_app_fm);
                            GUI_WinSetFocusChild(root_ctrl->h_app_fm);
                            break;
                        }
                        break;

                        case ID_HOME_MOVIE:
                        {
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            __log("**********enter movie explorer**********");
                            __log("root type=%d", msg->dwReserved);
                            root_para->root_type = msg->dwReserved;//RAT_TF;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = ID_EXPLORER_MOVIE;
                            root_ctrl->root_para->h_parent = msg->h_deswin;
                            root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
                            __log("root_ctrl->h_app_explorer = %x", root_ctrl->h_app_explorer);
                            GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                            break;
                        }
                        break;

                        case ID_HOME_PICTURE:
                        {
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            __log("**********enter photo explorer**********");
                            __log("root type=%d", msg->dwReserved);
                            root_para->root_type = msg->dwReserved;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = ID_EXPLORER_PHOTO;
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
                            __log("root_ctrl->h_app_explorer = %x", root_ctrl->h_app_explorer);
                            GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                                            //while(1)esKRNL_TimeDly(111);

                            break;
                        }
                        break;

                        case ID_HOME_MUSIC:
                        {
                            __s32 ret;
                            __s32 exist;
                            H_WIN hmusic;
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            root_para->explorer_play_music = 0;
                            root_para->root_type = msg->dwReserved;
                            root_para->data = MUSICPLAYER_NORMAL;
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            __log("root_para->root_type=%d \n", root_para->root_type);
                            //__getc();
                            exist = __app_root_is_bg_music_exist(msg, &hmusic);
                            __log("exist = %d",exist);

                            if (1 == exist) //重建后台音乐
                            {
                                //ret = app_music_restore(hmusic, root_para->root_type);
                                ret = app_music_restore(hmusic, root_para);

                                if (EPDK_OK == ret)
                                {
                                    if (root_ctrl->h_app_music != hmusic)
                                    {
                                        __log("*******root_ctrl->h_app_music != hmusic********");
                                    }

                                    root_ctrl->h_app_music = hmusic;
                                }
                                else
                                {
                                    __log("*****app_music_restore fail*******");
                                    __app_root_delete_bg_music(msg);
                                    //__here__;
                                    root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
                                    //__here__;
                                }
                            }
                            else
                            {
                                root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
                            }

                            GUI_WinSetFocusChild(root_ctrl->h_app_music);
                            // gscene_bgd_set_state(BGD_STATUS_HIDE);
                        }
                        break;

                        case ID_HOME_EBOOK:
                        {
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            root_para->root_type = msg->dwReserved;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = ID_EXPLORER_EBOOK;
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                        }
                        break;

                        case ID_HOME_RECORD:
                        {
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            //删除后台音乐
                            __app_root_delete_bg_music(msg);
                            __log("**********ID_HOME_RECORD**********");
                            root_ctrl->root_para->root_type = msg->dwReserved;//RAT_USB或者RAT_TF
                            root_ctrl->root_para->record_opt = 0;//正常录音
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            root_ctrl->h_app_record =  app_record_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_record);
                            break;
                        }

                        case ID_HOME_SETTING:
                        case ID_HOME_OTHERS:
                        {
                            
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            //删除后台音乐
                            //__app_root_delete_bg_music(msg);
                            __log("**********enter other apps**********");
                            {
                                __app_root_change_to_default_bg();
                                __log("**********enter setting**********");
                                root_ctrl->root_para->setting_opt = 0;//通用设置
                                root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                                root_ctrl->h_app_setting =  app_setting_create(root_para);
                                GUI_WinSetFocusChild(root_ctrl->h_app_setting);
                                break;
                            }
                        }

                        case ID_MOVIE_SHORTCUT:
                        {
                            __app_root_delete_bg_music(msg);//删除后台音乐
                            root_para->root_type = msg->dwReserved;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = 0;
                            root_para->tv_mode = MOVIE_MODE_NORMAL;
                            root_para->return_to_explorer_file_list = 1 ;
                            root_ctrl->h_app_movie = app_movie_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_movie);
                            break;
                        }

                        case ID_MUSIC_SHORTCUT:
                        {
                            __s32 ret;
                            __s32 exist;
                            H_WIN hmusic;
                            root_para->explorer_play_music = 0;
                            root_para->root_type = msg->dwReserved;
                            root_para->data = MUSICPLAYER_NORMAL;
                            root_para->return_to_explorer_file_list = 0 ;
                            exist = __app_root_is_bg_music_exist(msg, &hmusic);

                            if (1 == exist) //重建后台音乐
                            {
                                ret = app_music_get_root_type(hmusic, &root_para->root_type);

                                if (EPDK_FAIL == ret)
                                {
                                    root_para->root_type = RAT_TF;
                                }

                                ret = app_music_restore(hmusic, root_para);

                                if (EPDK_OK == ret)
                                {
                                    if (root_ctrl->h_app_music != hmusic)
                                    {
                                        __log("*******root_ctrl->h_app_music != hmusic********");
                                    }

                                    root_ctrl->h_app_music = hmusic;
                                }
                                else
                                {
                                    __log("*****app_music_restore fail*******");
                                    __app_root_delete_bg_music(msg);
                                    //__here__;
                                    root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
                                    //app_root_check_infinite_long_aitalk(root_ctrl);
                                    //__here__;
                                }
                            }
                            else
                            {
                                root_para->root_type = msg->dwReserved;
                                root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
                            }

                            GUI_WinSetFocusChild(root_ctrl->h_app_music);
                            break;
                        }

                        case ID_PHOTO_SHORTCUT:
                        {
                            __log("root type=%d", msg->dwReserved);
                            root_para->root_type = msg->dwReserved;//RAT_TF;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = 0;
                            root_para->return_to_explorer_file_list = 1 ;
                            root_ctrl->h_app_photo = app_photo_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_photo);
                            break;
                        }

                        case ID_EBOOK_SHORTCUT:
                        {
#if  1
                            root_para->root_type = msg->dwReserved;//RAT_TF;
                            root_para->explr_root = msg->dwReserved;
                            root_para->data = 0;
                            root_para->return_to_explorer_file_list = 1 ;
                            root_ctrl->h_app_ebook = (H_WIN)app_ebook_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_ebook);
#endif
                        }
                        break;

                        default:
                            break;
                    }
                }
                break;

                default:
                    break;
            }
        }
        break;

        case APP_DTV_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_MMENU:
                {
                    GUI_ManWinDelete(root_ctrl->h_app_dtv);
                    root_ctrl->h_app_dtv = 0;
                    //__here__;
                    __app_home_restore(msg);
                }
                break;
#ifdef  APP_ADD_RECORD_VEDIO

                case SWITCH_TO_OTHER_APP:
                {
                    GUI_ManWinDelete(root_ctrl->h_app_dtv);
                    root_ctrl->h_app_dtv = 0;
                    root_ctrl->h_app_record_video = app_record_video_create(root_para->h_parent, msg->dwAddData2);
                    GUI_WinSetFocusChild(root_ctrl->h_app_record_video);
                }
                break;
#endif

                default:
                    break;
            }
        }
        break;
#ifdef APP_ADD_RECORD_VEDIO

        case APP_RECORD_VEDIO_ID:
        {
            //eLIBs_printf("%s %d APP_RECORD_VEDIO_ID!", __FILE__, __LINE__);
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_OTHER_APP:
                {
                    //eLIBs_printf("%s %d SWITCH_TO_OTHER_APP!", __FILE__, __LINE__);
                    if (msg->dwAddData2 == 0x9001)
                    {
                        //eLIBs_printf("**********SETTING_SW_TO_MAIN to home**********");
                        GUI_ManWinDelete(root_ctrl->h_app_record_video);
                        root_ctrl->h_app_record_video = 0;
                        root_ctrl->h_app_dtv = (H_WIN)app_dtv_create(root_para);

                        if (root_ctrl->h_app_dtv)
                        {
                            __log("root_ctrl->h_app_dtv = %x", root_ctrl->h_app_dtv);
                            GUI_WinSetFocusChild(root_ctrl->h_app_dtv);
                        }
                    }
                }
                break;

                default:
                    break;
            }
        }
        break;
#endif

        case APP_EXPLORER_ID:
        {
            //__log("**********APP_EXPLORER_ID**********");
            //__log("msg->dwAddData1=%d", msg->dwAddData1);
            //__log("msg->dwAddData2=%d", msg->dwAddData2);
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_OTHER_APP:
                {
                    switch (msg->dwAddData2)
                    {
                        case EXPLR_SW_TO_MAIN:
                        {
                            if (root_ctrl->h_app_explorer)
                            {
                                GUI_ManWinDelete(root_ctrl->h_app_explorer);
                                root_ctrl->h_app_explorer = 0;
                            }
#ifdef CONFIG_SOC_SUN20IW1P1
                            /* whenever return from ID_movie, audio channel should be set back to stereo in case of it was changed */
                            int volume;
                            reg_movie_para_t *para;
                            para = (reg_movie_para_t *)dsk_reg_get_para_by_app(REG_APP_MOVIE);

                            if (para)
                            {
                                para->channel = 0;         //save audio channel's value as 0(stereo)
                                volume = dsk_volume_get();
                                dsk_volume_set(volume);
                                __log("@@@ para->channel = %d @@@", para->channel);
                            }

#endif
                            //__here__;
                            __app_home_restore(msg);
                        }
                        break;

                        case EXPLR_SW_TO_MOVIE:
                        {
                            //删除后台音乐
                            __app_root_delete_bg_music(msg);
                            __log("**********explorer to movie**********");
                            //root_para->root_type = msg->dwReserved;
                            root_para->data = 0;
                            root_para->root_type = root_para->explr_root  ;
                            root_ctrl->h_app_movie = app_movie_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_movie);
                        }
                        break;

                        case EXPLR_SW_TO_PHOTO:
                        {
                            //删除后台音乐
                            //__app_root_delete_bg_music(msg);
                            __log("**********explorer to photo**********");
                            //root_para->root_type = msg->dwReserved;
                            root_para->data = 0;
                            root_para->root_type = root_para->explr_root  ;
                            root_ctrl->h_app_photo = app_photo_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_photo);
                        }
                        break;

                        case EXPLR_SW_TO_MUSIC:
                        {
                            __s32 ret;
                            __s32 exist;
                            H_WIN hmusic;
                            gscene_hbar_set_state(HBAR_ST_HIDE);
                            __app_root_change_to_default_bg();
                            //eLIBs_strncpy( root_para->music_file_name , (const char *) msg->dwReserved, sizeof(root_para->music_file_name) ) ;
                            //__log( "root_para->music_file_name =%s",root_para->music_file_name );
                            root_para->explorer_play_music = 1;
                            root_para->root_type = root_para->explr_root  ;
                            //root_para->root_type = root_para->explr_root;
                            __log("app_root_scene, root_para->root_type = %d", root_para->root_type);
                            root_para->data = MUSICPLAYER_NORMAL;
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            exist = __app_root_is_bg_music_exist(msg, &hmusic);

                            if (1 == exist) //重建后台音乐
                            {
                                //ret = app_music_restore(hmusic, root_para->root_type);
                                ret = app_music_restore(hmusic, root_para);

                                if (EPDK_OK == ret)
                                {
                                    if (root_ctrl->h_app_music != hmusic)
                                    {
                                        __log("*******root_ctrl->h_app_music != hmusic********");
                                    }

                                    root_ctrl->h_app_music = hmusic;
                                }
                                else
                                {
                                    __log("*****app_music_restore fail*******");
                                    __app_root_delete_bg_music(msg);
                                    //__here__;
                                    root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
                                    //__here__;
                                }
                            }
                            else
                            {
                                root_ctrl->h_app_music = (H_WIN)app_music_create(root_para);
                            }

                            GUI_WinSetFocusChild(root_ctrl->h_app_music);
                            // gscene_bgd_set_state(BGD_STATUS_HIDE);
                        }
                        break ;

                        case EXPLR_SW_TO_EBOOK:
                        {
                            //删除后台音乐
                            //__app_root_delete_bg_music(msg);
                            __log("**********explorer to ebook**********");
                            //root_para->root_type = msg->dwReserved;
                            root_para->data = 0;
                            root_para->root_type = root_para->explr_root  ;
                            root_ctrl->h_app_ebook = (H_WIN)app_ebook_create(root_para);
                            //__here__;
                            GUI_WinSetFocusChild(root_ctrl->h_app_ebook);
                            //__here__;
                        }
                        break;

                        default:
                        {
                            __err("media type err...");
                        }
                        break;
                    }

                    root_ctrl->h_app_explorer = NULL;//浏览器已自己删除
                }
                break;

                default:
                    break;
            }
        }
        break;

        case APP_MOVIE_ID:
        {
            __log("**********movie to explorer**********");
            __log("msg->dwAddData1=%d", msg->dwAddData1);
            __log("msg->dwAddData2=%d", msg->dwAddData2);
            __log("msg->dwReserved=%d", msg->dwReserved);

            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_EXPLORER:
                {
                    GUI_ManWinDelete(root_ctrl->h_app_movie);
                    root_ctrl->h_app_movie = 0;
                    //__here__;
                    __wrn("shiql root_para->root_type:%d", msg->dwAddData2);
                    root_para->root_type = msg->dwAddData2;
                    root_para->data = ID_EXPLORER_MOVIE;
                    root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                    root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                    break;
                }

                case SWITCH_TO_MMENU:
                {
                    GUI_ManWinDelete(root_ctrl->h_app_movie);
                    root_ctrl->h_app_movie = 0;
                    //__here__;
                    __app_home_restore(msg);
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case APP_MUSIC_ID:
        {
            __log("**********receive msg from music,msg->dwAddData2=%d**********", msg->dwAddData2);

            switch (msg->dwAddData2)
            {
                case SWITCH_TO_MMENU:
                {
#if 0

                    if (MUSICPLAYER_BACKGROUND == msg->dwReserved) //音乐后台，不删除音乐
                    {
                        __log("music background...");
                    }
                    else
#endif
                    {
                        __log("delete music...");
                        GUI_ManWinDelete(root_ctrl->h_app_music);
                        root_ctrl->h_app_music = 0;
                    }

                    __app_home_restore(msg);
                    break;
                }

                case CLOSE_BG_MUSIC:
                {
                    __log("delete background music...");

                    if (root_ctrl->h_app_music)
                    {
                        GUI_ManWinDelete(root_ctrl->h_app_music);
                        root_ctrl->h_app_music = 0;
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }
        }
        break;

        case APP_PHOTO_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_EXPLORER:
                {
                    //删除后台音乐
                    // __app_root_delete_bg_music(msg);
                    __log("**********photo to explorer**********");
                    GUI_ManWinDelete(root_ctrl->h_app_photo);
                    root_ctrl->h_app_photo = 0;
                    //gscene_bgd_set_flag(EPDK_TRUE);
                    //gscene_bgd_refresh();
                    //__here__;
                    root_para->root_type = root_para->explr_root;
                    root_para->data = ID_EXPLORER_PHOTO;
                    root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                    root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                    break;
                }

                case OPEN_BG_MUSIC:
                {
                    __app_root_create_bg_music(msg);
                    break;
                }

                case CLOSE_BG_MUSIC:
                {
                    __app_root_delete_bg_music(msg);
                    break;
                }

                case SWITCH_TO_MMENU:
                {
                    GUI_ManWinDelete(root_ctrl->h_app_photo);
                    root_ctrl->h_app_photo = 0;
                    //__here__;
                    __app_home_restore(msg);
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case APP_EBOOK_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_EXPLORER:
                {
                    //删除后台音乐
                    //__app_root_delete_bg_music(msg);
                    __log("**********ebook to explorer**********");
                    GUI_ManWinDelete(root_ctrl->h_app_ebook);
                    root_ctrl->h_app_ebook = 0;
                    //gscene_bgd_set_flag(EPDK_TRUE);
                    //gscene_bgd_refresh();
                    root_para->root_type = root_para->explr_root;
                    root_para->data = ID_EXPLORER_EBOOK;
                    root_ctrl->h_app_explorer = (H_WIN)app_explorer_create(root_para);
                    GUI_WinSetFocusChild(root_ctrl->h_app_explorer);
                    break;
                }

                case OPEN_BG_MUSIC:
                {
                    __app_root_create_bg_music(msg);
                    break;
                }

                case CLOSE_BG_MUSIC:
                {
                    __app_root_delete_bg_music(msg);
                    break;
                }

                case SWITCH_TO_MMENU:
                {
                    GUI_ManWinDelete(root_ctrl->h_app_ebook);
                    root_ctrl->h_app_ebook = 0;
                    //__here__;
                    __app_home_restore(msg);
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case APP_FM_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_MMENU:
                {
                    __log("**********fm to home**********");
                    GUI_ManWinDelete(root_ctrl->h_app_fm);
                    root_ctrl->h_app_fm = 0;
                    __app_home_restore(msg);
                    {
                        //重新初始化按键音
                        __set_keytone_t state;
                        dsk_keytone_get_state(&state);
                        dsk_keytone_exit();
                        dsk_keytone_init(BEETLES_APP_ROOT"res\\sounds\\chord.wav");
                        dsk_keytone_set_state(state);
                    }
                    break;
                }

                case SWITCH_TO_OTHER_APP:
                {
                    switch (LOWORD(msg->dwAddData2))
                    {
                        case FM_SW_TO_RECORD:
                        {
                            //__here__;
                            GUI_ManWinDelete(root_ctrl->h_app_fm);
                            root_ctrl->h_app_fm = 0;
                            //__app_root_change_to_default_bg();//这里不是通过主界面进入的，不记录进入ap之前背景的状态。
                            __log("**********FM_SW_TO_RECORD**********");
                            root_ctrl->root_para->root_type = HIWORD(msg->dwAddData2);//RAT_USB或者RAT_TF
                            root_ctrl->root_para->record_opt = 1;//fm录音
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            root_ctrl->h_app_record =  app_record_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_record);
                            {
                                //重新初始化按键音
                                __set_keytone_t state;
                                dsk_keytone_get_state(&state);
                                dsk_keytone_exit();
                                dsk_keytone_init(BEETLES_APP_ROOT"res\\sounds\\chord.wav");
                                dsk_keytone_set_state(state);
                            }
                            break;
                        }

                        default:
                        {
                            break;
                        }
                    }
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case APP_RECORD_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_MMENU:
                {
                    __log("**********record to home**********");
                    GUI_ManWinDelete(root_ctrl->h_app_record);
                    root_ctrl->h_app_record = 0;
                    __app_home_restore(msg);
                    break;
                }

                case SWITCH_TO_OTHER_APP:
                {
                    switch (LOWORD(msg->dwAddData2))
                    {
                        case RECORD_SW_TO_FM:
                        {
                            //__here__;
                            //__app_root_change_to_default_bg();//这里不是通过主界面进入的，不记录进入ap之前背景的状态。
                            GUI_ManWinDelete(root_ctrl->h_app_record);
                            root_ctrl->h_app_record = 0;
                            __log("**********FM_SW_TO_RECORD**********");
                            root_ctrl->root_para->h_parent = GUI_WinGetHandFromName(APP_ROOT);
                            root_ctrl->h_app_fm = app_fm_create(root_para);
                            GUI_WinSetFocusChild(root_ctrl->h_app_fm);
                            break;
                        }

                        default:
                        {
                            break;
                        }
                    }
                }

                default:
                {
                    break;
                }
            }

            break;
        }

#if 0

        case APP_CALENDAR_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_MMENU:
                {
                    __log("**********calendar to home**********");
                    GUI_ManWinDelete(root_ctrl->h_app_calendar);
                    root_ctrl->h_app_calendar = 0;
                    __app_home_restore(msg);
                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

#endif

        case APP_SETTING_ID:
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case SWITCH_TO_OTHER_APP:
                {
                    switch (msg->dwAddData2)
                    {
                        case SETTING_SW_TO_MAIN:
                        {
                            __log("**********setting to home**********");
                            GUI_ManWinDelete(root_ctrl->h_app_setting);
                            root_ctrl->h_app_setting = 0;
                            __app_home_restore(msg);
                        }
                        break;

                        case SETTING_SW_TO_MULTIMEDIA:
                        {
                        }
                        break;

                        case SETTING_SW_TO_PROMPT_UPGRADE:
                        {
                        }
                        break;

                        case SETTING_SW_TO_PROMPT_DEFAULT:
                        {
                        }
                        break;

                        case EXPLR_SW_TO_UPGRADE:
                        {
                        }
                        break;

                        default:
                            break;
                    }

                    //root_ctrl->h_app_setting = NULL;
                }
                break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return EPDK_OK;
}

static int32_t __app_root_broadcast_msg(__gui_msg_t *msg)
{
    H_WIN           root, child;
    char            winname[256];
    __gui_msg_t     set_msg;

    eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));

    root    = msg->h_deswin;
    child   = GUI_WinGetFirstChild(root);

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, 256);
        GUI_WinGetName(child, winname);

        __log("app_root_broadcast_msg msg->id=%d,winname = %s ", msg->id, winname);

        set_msg.h_deswin    = child;
        GUI_SendMessage(&set_msg);
        // GUI_SendNotifyMessage(&set_msg);

        child = GUI_WinGetNextBro(child);
    }

    return EPDK_OK;
}

static int32_t  __app_root_send_msg2focus_child(__gui_msg_t *msg)
{
    H_WIN       root, child;
    __gui_msg_t set_msg;

    eLIBs_memcpy(&set_msg, msg, sizeof(__gui_msg_t));

    root    = msg->h_deswin;
    child   = GUI_WinGetFocusChild(root);

    if (child)
    {
        GUI_SendMessage(&set_msg);
    }

    return EPDK_OK;
}

static void __app_root_delete_all_app_except_bg_audio(root_ctrl_t *para)
{
    H_WIN   root, child, next_child;
    char    winname[256];

    if (!para)
    {
        __err("invalid para...");
    }

    root    = GUI_WinGetHandFromName(APP_ROOT);

    if (!root)
    {
        __err("GUI_WinGetHandFromName fail...");
        return ;
    }

    child   = GUI_WinGetFirstChild(root);
    __wrn("begin delete apps except music...");

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, sizeof(winname));
        GUI_WinGetName(child, winname);

        next_child  = GUI_WinGetNextBro(child);

        if (0 != eLIBs_strcmp(winname, APP_MUSIC))
        {
            __wrn("winname=%s", winname);
            GUI_ManWinDelete(child);
        }

        child = next_child;
    }

    para->h_app_home        = NULL;
    para->h_app_explorer    = NULL;
    para->h_app_movie       = NULL;
    //para->h_app_music    = NULL;
    para->h_app_photo       = NULL;
    para->h_app_fm          = NULL;
    para->h_app_setting     = NULL;
    para->h_app_ebook       = NULL;
    para->h_app_calendar    = NULL;
    para->h_app_record      = NULL;
    para->h_app_dtv         = NULL;
    __wrn("*******__app_root_delete_all_app_except_bg_audio end*******");
}

static void __app_root_delete_all_audio_app(root_ctrl_t *para)
{
    H_WIN   root, child, next_child;
    char    winname[256];

    if (!para)
    {
        __err("invalid para...");
    }

    root    = GUI_WinGetHandFromName(APP_ROOT);

    if (!root)
    {
        __err("GUI_WinGetHandFromName fail...");
        return ;
    }

    child   = GUI_WinGetFirstChild(root);
    __wrn("begin delete audio apps...");

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, sizeof(winname));
        GUI_WinGetName(child, winname);
        next_child = GUI_WinGetNextBro(child);

        if (0 == eLIBs_strcmp(winname, APP_MUSIC) || 0 == eLIBs_strcmp(winname, APP_MOVIE) || 0 == eLIBs_strcmp(winname, APP_FM)
            || 0 == eLIBs_strcmp(winname, APP_RECORD) || 0 == eLIBs_strcmp(winname, APP_DTV)
           )
        {
            __wrn("winname=%s", winname);
            GUI_ManWinDelete(child);
        }

        child = next_child;
    }

    para->h_app_movie    = NULL;
    para->h_app_music    = NULL;
    para->h_app_fm       = NULL;
    para->h_app_record   = NULL;
    para->h_app_dtv      = NULL;
    __wrn("*******__app_root_delete_all_audio_app end*******");
}

static void __app_root_delete_all_app(root_ctrl_t *para)
{
    H_WIN   root, child, next_child;
    char    winname[256];

    if (!para)
    {
        __err("invalid para...");
    }

    root    = GUI_WinGetHandFromName(APP_ROOT);

    if (!root)
    {
        __err("GUI_WinGetHandFromName fail...");
        return ;
    }

    child   = GUI_WinGetFirstChild(root);
    __wrn("begin delete app apps...");

    while (child != NULL)
    {
        eLIBs_memset(winname, 0, sizeof(winname));
        GUI_WinGetName(child, winname);

        next_child = GUI_WinGetNextBro(child);
        __wrn("winname=%s", winname);

        GUI_ManWinDelete(child);
        child   = next_child;
    }

    para->h_app_home     = NULL;
    para->h_app_explorer = NULL;
    para->h_app_movie    = NULL;
    para->h_app_music    = NULL;
    para->h_app_photo    = NULL;
    para->h_app_fm       = NULL;
    para->h_app_setting  = NULL;
    para->h_app_ebook    = NULL;
    para->h_app_calendar = NULL;
    para->h_app_record   = NULL;
    para->h_app_dtv      = NULL;
    __wrn("*******__app_root_delete_all_app end*******");
}

static void  app_root_cmd2self(root_ctrl_t *para, __s32 lword, __s32 hword, __s32 data2, __s32 reserved)
{
    __gui_msg_t msg;

    eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

    msg.h_deswin    = para->root_para->h_parent;
    msg.h_srcwin    = NULL;
    msg.id          = GUI_MSG_COMMAND;
    msg.dwAddData1  = MAKELONG(lword, hword);
    msg.dwAddData2  = data2;
    msg.dwReserved  = reserved;

    GUI_SendMessage(&msg);
}

static int32_t __app_root_to_audio(root_ctrl_t *para, int32_t rat_root_type)
{
    H_WIN       root, child, next_child;
    char        winname[256];
    int32_t     ret;

    __wrn("*******__app_root_to_audio*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (RAT_TF != rat_root_type && RAT_USB != rat_root_type)
    {
        __wrn("rat_root_type type error,do not play video...");
        return EPDK_FAIL;
    }

    //不要删除后台音乐
    // if(NULL != para->h_app_music
    //    && para->root_para->root_type == rat_root_type)
    //{
    //    __wrn("music alreadly running...");
    //    return EPDK_FAIL;
    //}
    //外部已经搜索并设置播放列表
    //ret = __app_root_set_playfile(para->quick_root_type);
    //if(EPDK_FAIL == ret)
    //{
    //    __wrn("no media file...");
    //    return -2;//没有媒体文件
    //}
    //如果音乐存在，并且需要播放的盘为当前播放盘，则不要删除后台音乐
    {
        __bool to_delete_music_first;
        to_delete_music_first = EPDK_TRUE;

        if (para->h_app_music)
        {
            int32_t     ret;
            unsigned long    cur_rat_root_type;

            ret     = app_music_get_root_type(para->h_app_music, &cur_rat_root_type);

            if (EPDK_OK == ret && cur_rat_root_type == rat_root_type)
            {
                to_delete_music_first = EPDK_FALSE;
            }
        }

        if (EPDK_TRUE == to_delete_music_first)
        {
            __app_root_delete_all_app(para);
        }
        else
        {
            __app_root_delete_all_app(para);
        }
    }
    para->root_para->explr_root = rat_root_type;
    __wrn("begin send msg to load music...");

    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_MUSIC_SHORTCUT, rat_root_type);
    __wrn("*******__app_root_to_audio success*******");

    dsktop_scene_set_cur_app(ID_HOME_MUSIC);
    return EPDK_OK;
}

static __bool is_on_tv(void)
{
    uint32_t    output;

    output  = dsk_display_get_output_type();

    if (DISP_OUTPUT_TYPE_LCD == output || DISP_OUTPUT_TYPE_NONE == output)
    {
        __wrn("not on tv now...");
        return EPDK_FALSE;
    }
    else
    {
        __wrn("on tv now...");
        return EPDK_TRUE;
    }
}

static int32_t __app_root_to_setting(root_ctrl_t *para)
{
    H_WIN       root, child, next_child;
    char        winname[256];
    int32_t     ret;
    int32_t     storage_type;

    __wrn("*******__app_root_to_setting*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (NULL != para->h_app_setting)
    {
        __wrn("h_app_setting alreadly running...");
        return EPDK_FAIL;
    }

    if (para->h_app_music)
    {
        para->root_para->multimedia_to_setting = EPDK_TRUE;
        para->root_para->media_type = 0;//音频
    }
    else if (para->h_app_movie)
    {
        para->root_para->multimedia_to_setting = EPDK_TRUE;
        para->root_para->media_type = 1;//视频
    }
    else if (para->h_app_ebook)
    {
        para->root_para->multimedia_to_setting = EPDK_TRUE;
        para->root_para->media_type = 2;//电子书
    }
    else if (para->h_app_photo)
    {
        para->root_para->multimedia_to_setting = EPDK_TRUE;
        para->root_para->media_type = 3;//图片
    }
    else
    {
        para->root_para->multimedia_to_setting = EPDK_FALSE;
        para->root_para->media_type = 0xFF;
    }

    __app_root_delete_all_app(para);

    if (para->h_app_music)
    {
        app_music_set_bg_mode(para->h_app_music);
    }

    __wrn("begin send msg to load setting...");
    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_HOME_SETTING, NULL);

    __wrn("*******__app_root_to_setting success*******");
    dsktop_scene_set_cur_app(ID_HOME_SETTING);

    return EPDK_OK;
}


static int32_t __app_root_to_video(root_ctrl_t *para, int32_t rat_root_type)
{
    H_WIN       root, child, next_child;
    char        winname[256];
    int32_t     ret;
    __movie_create_mode_e   tv_mode;

    __wrn("*******__app_root_to_video*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (RAT_TF != rat_root_type && RAT_USB != rat_root_type)
    {
        __wrn("rat_root_type type error,do not play video...");
        return EPDK_FAIL;
    }

    if (NULL != para->h_app_movie && para->root_para->root_type == rat_root_type)
    {
        __wrn("movie alreadly running...");
        return EPDK_FAIL;
    }

    if (EPDK_TRUE == is_on_tv())
    {
        tv_mode = MOVIE_MODE_TV;
    }
    else
    {
        tv_mode = MOVIE_MODE_NORMAL;
    }

    __app_root_delete_all_app(para);

    para->root_para->explr_root     = rat_root_type;
    para->root_para->tv_mode        = tv_mode;

    __wrn("begin send msg to load movie...");

    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_MOVIE_SHORTCUT, rat_root_type);
    __wrn("*******__app_root_to_video success*******");

    dsktop_scene_set_cur_app(ID_HOME_MOVIE);
    return EPDK_OK;
}

static int32_t __app_root_to_ebook(root_ctrl_t *para, int32_t rat_root_type)
{
    H_WIN       root, child, next_child;
    char        winname[256];
    int32_t     ret;

    __wrn("*******__app_root_to_ebook*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (RAT_TF != rat_root_type && RAT_USB != rat_root_type)
    {
        __wrn("rat_root_type type error,do not play video...");
        return EPDK_FAIL;
    }

    if (NULL != para->h_app_ebook && para->root_para->root_type == rat_root_type)
    {
        __wrn("ebook alreadly running...");
        return EPDK_FAIL;
    }

    __app_root_delete_all_app(para);
    para->root_para->explr_root = rat_root_type;
    __wrn("begin send msg to load ebook...");

    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_EBOOK_SHORTCUT, rat_root_type);
    __wrn("*******__app_root_to_ebook success*******");

    dsktop_scene_set_cur_app(ID_HOME_EBOOK);
    return EPDK_OK;
}

static int32_t __app_root_to_photo(root_ctrl_t *para, int32_t rat_root_type)
{
    H_WIN       root, child, next_child;
    char        winname[256];
    int32_t     ret;

    __wrn("*******__app_root_to_photo*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (RAT_TF != rat_root_type && RAT_USB != rat_root_type)
    {
        __wrn("rat_root_type type error,do not play video...");
        return EPDK_FAIL;
    }

    if (NULL != para->h_app_photo && para->root_para->root_type == rat_root_type)
    {
        __wrn("photo alreadly running...");
        return EPDK_FAIL;
    }

    __app_root_delete_all_app(para);

    para->root_para->explr_root = rat_root_type;

    __wrn("begin send msg to load photo...");

    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_PHOTO_SHORTCUT, rat_root_type);

    __wrn("*******__app_root_to_photo success*******");

    dsktop_scene_set_cur_app(ID_HOME_PICTURE);

    return EPDK_OK;
}
static int32_t __app_root_to_fm(root_ctrl_t *para)
{
    H_WIN       root, child, next_child;
    char        winname[256];

    __wrn("*******__app_root_to_fm*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (para->h_app_fm)
    {
        __wrn("fm alreadly running...");
        return EPDK_OK;
    }

    __app_root_delete_all_app(para);

    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_FM_SHORTCUT, NULL);

    __wrn("*******__app_root_to_fm success*******");

    dsktop_scene_set_cur_app(ID_HOME_FM);

    return EPDK_OK;
}

static int32_t __app_root_to_dtv(root_ctrl_t *para)
{
    H_WIN       root, child, next_child;
    char        winname[256];

    __wrn("*******__app_root_to_dtv*******");

    if (!para)
    {
        __err("invalid para...");
    }

    if (para->h_app_dtv)
    {
        __wrn("fm alreadly running...");
        return EPDK_OK;
    }

    __app_root_delete_all_app(para);

    app_root_cmd2self(para, APP_HOME_ID, SWITCH_TO_OTHER_APP, ID_DTV_SHORTCUT, NULL);

    __wrn("*******__app_root_to_fm success*******");

    dsktop_scene_set_cur_app(ID_HOME_DTV);

    return EPDK_OK;
}


#define APP_ROOT_TYPE_SD         0x1
#define APP_ROOT_TYPE_UD         0x2

static __bool __app_root_has_external_disk(__u32 type)
{
    int32_t     ret;
    char        diskname[RAT_MAX_PARTITION][4];

    #if 0
    {
        __target_para_t target;
    __err("err:esKSRV_GetTargetPara the API fail..the bug is waiting to fix");        
        //esKSRV_GetTargetPara(&target);

        if (target.debugenable == EPDK_TRUE)
        {
            return EPDK_TRUE;  //112350 如果是调试，则默认为有外设
        }
    }
    #endif

    if (type & APP_ROOT_TYPE_UD)
    {
        ret = rat_get_partition_name(RAT_USB_DISK, diskname, 0);

        if (EPDK_OK == ret)
        {
            if (diskname[0][0])
            {
                return EPDK_TRUE;
            }
        }
    }

    if (type & APP_ROOT_TYPE_SD)
    {
        ret = rat_get_partition_name(RAT_SD_CARD, diskname, 0);

        if (EPDK_OK == ret)
        {
            if (diskname[0][0])
            {
                return EPDK_TRUE;
            }
        }
    }

    return EPDK_FALSE;
}


static int32_t  __app_root_get_search_path(rat_type_t rat_root_type, char *search_path)
{
    int32_t     ret;
    char        str_type[50];
    char        disk_name[RAT_MAX_PARTITION][4];
 
    eLIBs_memset(str_type,      0, sizeof(str_type));
    eLIBs_memset(disk_name,     0, sizeof(disk_name));
    eLIBs_memset(search_path,   0, sizeof(search_path));

    switch (rat_root_type)
    {
        case RAT_LOCAL:
            eLIBs_strcpy(str_type, RAT_LOCAL_DISK);
            break;

        case RAT_TF:
            eLIBs_strcpy(str_type, RAT_SD_CARD);
            break;

        case RAT_USB:
            eLIBs_strcpy(str_type, RAT_USB_DISK);
            break;

        default:
            return EPDK_FAIL;
    }

    __wrn("===================str_type=%s", str_type);

    ret = rat_get_partition_name(str_type, disk_name, 0);

    eLIBs_strcpy(search_path, disk_name[0]);

    __wrn("===================rat root type=%d", rat_root_type);
    __wrn("===================search_path=%s", search_path);

    if (0 == search_path[0])
    {
        return EPDK_FAIL;
    }

    return ret;
}

//return EPDK_OK, 表示成功,
//return EPDK_FAIL,表示盘符中没有视频\音频文件,则返回到浏览器,告知客户没有文件
static int32_t __app_root_set_playfile(rat_type_t rat_root_type, rat_media_type_t rat_media_type)
{
    char        file_path[RAT_MAX_FULL_PATH_LEN];
    HRAT        hrat = NULL;
    uint32_t    total = 0;
    uint32_t    last_file_index = 0;
    char        *last_file_path = NULL;
    reg_root_para_t     *para;
    int32_t     ret;
    rat_entry_t ItemInfo;

    if (RAT_UNKNOWN == rat_root_type || RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
    {
        return EPDK_FAIL;
    }

    eLIBs_memset(file_path, 0, sizeof(file_path));

    ret     = __app_root_get_search_path(rat_root_type, file_path);

    if (EPDK_FAIL == ret || eLIBs_strlen(file_path) == 0)
    {
        return EPDK_FAIL;
    }

    //hrat = rat_open(file_path, rat_media_type, 0);
    hrat    = rat_open_ex(rat_root_type, rat_media_type, 0);

    if (hrat == NULL)
    {
        return EPDK_FAIL;
    }

    total   = rat_get_cur_scan_cnt(hrat);

    if (total == 0)
    {
        return EPDK_FAIL;
    }

    para    = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

    if (para == NULL)
    {
        return EPDK_FAIL;
    }

    if (RAT_MEDIA_TYPE_VIDEO == rat_media_type)
    {
        if (rat_root_type == RAT_TF)
        {
            last_file_index = para->last_movie_index_sd;
            last_file_path = para->last_movie_path_sd;
        }
        else
        {
            last_file_index = para->last_movie_index_ud;
            last_file_path = para->last_movie_path_ud;
        }
    }
    else if (RAT_MEDIA_TYPE_AUDIO == rat_media_type)
    {
        if (rat_root_type == RAT_TF)
        {
            last_file_index = para->last_music_index_sd;
            last_file_path = para->last_music_path_sd;
        }
        else
        {
            last_file_index = para->last_music_index_ud;
            last_file_path = para->last_music_path_ud;
        }
    }
    else if (RAT_MEDIA_TYPE_EBOOK == rat_media_type)
    {
        if (rat_root_type == RAT_TF)
        {
            last_file_index = para->last_ebook_index_sd;
            last_file_path = para->last_ebook_path_sd;
        }
        else
        {
            last_file_index = para->last_ebook_index_ud;
            last_file_path = para->last_ebook_path_ud;
        }
    }
    else if (RAT_MEDIA_TYPE_PIC == rat_media_type)
    {
        if (rat_root_type == RAT_TF)
        {
            last_file_index = para->last_photo_index_sd;
            last_file_path = para->last_photo_path_sd;
        }
        else
        {
            last_file_index = para->last_photo_index_ud;
            last_file_path = para->last_photo_path_ud;
        }
    }

    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));

    ret     = rat_get_item_info_by_index(hrat, last_file_index, &ItemInfo);

    if (ret == EPDK_OK)
    {
        eLIBs_memset(file_path, 0, sizeof(file_path));

        eLIBs_strcpy(file_path, ItemInfo.Path);

        ret     = eLIBs_strcmp(file_path, last_file_path);

        if (ret == 0)       //相等,则播放上一次的文件
        {
            rat_set_file_for_play(hrat, last_file_path);
            return EPDK_OK;
        }
    }

    ret = rat_get_item_info_by_index(hrat, 0, &ItemInfo);   //直接播放第一首

    if (ret == EPDK_OK)
    {
        eLIBs_memset(file_path, 0, sizeof(file_path));
        eLIBs_strcpy(file_path, ItemInfo.Path);
        rat_set_file_for_play(hrat, file_path);
        return EPDK_OK;
    }

    return EPDK_FAIL;
}

static rat_type_t __app_root_get_rat_root_type(__s32 reg_storage_type)
{
    rat_type_t  rat_root_type   = RAT_UNKNOWN;

    if (0 == reg_storage_type)
    {
        rat_root_type = RAT_TF;
    }
    else if (1 == reg_storage_type)
    {
        rat_root_type = RAT_USB;
    }
    else
    {
        __wrn("****************reg_storage_type type error, change to 0...");
        rat_root_type = RAT_TF;
    }

    return rat_root_type;
}

static int32_t __app_root_get_root_storage_type(int32_t reg_storage_type)
{
    int32_t     app_root_storage_type = -1;

    if (0 == reg_storage_type)
    {
        app_root_storage_type = APP_ROOT_TYPE_SD;
    }
    else if (1 == reg_storage_type)
    {
        app_root_storage_type = APP_ROOT_TYPE_UD;
    }

    return app_root_storage_type;
}

static rat_media_type_t __app_root_get_rat_media_type(int32_t reg_media_type)
{
    rat_media_type_t    rat_media_type = RAT_MEDIA_TYPE_UNKNOWN;

    if (0 == reg_media_type)
    {
        rat_media_type = RAT_MEDIA_TYPE_AUDIO;
    }
    else if (1 == reg_media_type)
    {
        rat_media_type = RAT_MEDIA_TYPE_VIDEO;
    }
    else if (2 == reg_media_type)
    {
        rat_media_type = RAT_MEDIA_TYPE_EBOOK;
    }
    else if (3 == reg_media_type)
    {
        rat_media_type = RAT_MEDIA_TYPE_PIC;
    }
    else
    {
        __wrn("reg_media_type type error, change to 0...");
        rat_media_type = RAT_MEDIA_TYPE_AUDIO;
    }

    return rat_media_type;
}


//根据盘符获取分区名
int32_t __app_root_get_reg_storage_type(unsigned char root)
{
    int32_t     i;
    int32_t     ret;
    char        disk_name[RAT_MAX_PARTITION][4];
    char        cur_disk_name[2];

    eLIBs_memset(cur_disk_name, 0, sizeof(cur_disk_name));

    cur_disk_name[0] = root;
    __wrn("cur_disk_name=%s", cur_disk_name);

    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;

    ret     = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);

    if (EPDK_OK == ret)
    {
        i = 0;

        while (i < RAT_MAX_PARTITION && disk_name[i][0])
        {
            __wrn("disk_name[i]=%s", disk_name[i]);

            if (0 == eLIBs_strnicmp(disk_name[i], cur_disk_name, 1))
            {
                return 0; //SD 卡
            }

            i++;
        }
    }
    else
    {
        __wrn("***************get partition name failed ");
    }

    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;

    ret     = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);

    if (EPDK_OK == ret)
    {
        i = 0;

        while (i < RAT_MAX_PARTITION && disk_name[i][0])
        {
            __wrn("disk_name[i]=%s", disk_name[i]);

            if (0 == eLIBs_strnicmp(disk_name[i], cur_disk_name, 1))
            {
                return 1; //U 盘
            }

            i++;
        }
    }
    else
    {
        __wrn("***************get partition name failed ");
    }

    return -1;
}


//媒体探测
//init_media_type:用于存放探测得到的媒体类型0:音乐 1:视频
//init_storage_type:用于存放探测得到的媒体介质类型0:卡 1:u盘
//intent:探测方式
//返回:EPDK_OK，探测成功，EPDK_FAIL，探测失败，*err_code存放错误代号
static int32_t app_root_prob_media_file_for_play(root_ctrl_t *root_ctrl
        , int32_t *init_media_type
        , int32_t *init_storage_type
        , app_root_prob_intent_t intent
        , app_root_prob_result_t *err_code)
{
    __bool              bhas_disk;
    int32_t             reg_storage_type;
    int32_t             reg_media_type;
    reg_root_para_t     *para;
    rat_media_type_t    rat_media_type;

    __wrn("***********app_root_prob_media_file_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d***********",
          intent, *init_media_type, *init_storage_type, *err_code);

    para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

    if (!para)
    {
        __wrn("para is null...");
        goto fail;
    }

    if (!init_media_type || (*init_media_type < 0 && *init_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage)) )
    {
        __wrn("media type error...");
        goto fail;
    }

    if (!init_storage_type)
    {
        __wrn("storage type is null...");
        goto fail;
    }

    if (/*intent < 0 ||*/ intent >= app_root_prob_intent_max)
    {
        __wrn("intent type error...");
        goto fail;
    }

    reg_storage_type    = *init_storage_type;
    reg_media_type      = *init_media_type;
    bhas_disk           = EPDK_FALSE;

    do
    {
        int32_t     ret;
        rat_type_t  rat_root_type;
        HRAT        hrat;
        uint32_t    media_cnt;
        int32_t     cur_reg_root_type;
        char        path[RAT_MAX_FULL_PATH_LEN];

        __wrn("begin search media file, reg_storage_type=%d, reg_media_type=%d", reg_storage_type, reg_media_type);

        rat_root_type   = __app_root_get_rat_root_type(reg_storage_type);

        if (RAT_UNKNOWN == rat_root_type)
        {
            __wrn("rat_root_type error...");
            goto fail;
        }

        rat_media_type  = __app_root_get_rat_media_type(reg_media_type);

        if (RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
        {
            __wrn("rat_media_type error...");
            goto fail;
        }

        eLIBs_memset(path, 0, sizeof(path));

        ret = __app_root_get_search_path(rat_root_type, path);

        if (EPDK_FAIL == ret) //无磁盘
        {
            __wrn("no such disk, reg_storage_type=%d", reg_storage_type);

            if (app_root_prob_intent_first_play == intent)
            {
                __wrn("intent is first_play, not need switch storage, intent=%d...", intent);
                goto no_storage;
            }
            else if (app_root_prob_intent_plugin_for_play == intent)
            {
                __wrn("may not run here...");
                goto no_media_file;
            }
            else if (app_root_prob_intent_switch_media == intent)
            {
                __wrn("intent is switch_media, goto next_storage");
                goto next_storage;
            }
            else if (app_root_prob_intent_switch_storage == intent)
            {
                __wrn("intent is switch_storage, goto next_storage");
                goto next_storage;
            }
            else
            {
                __wrn("intent type error...");
                goto fail;
            }
        }

        bhas_disk   = EPDK_TRUE;
        media_cnt   = 0;
        //hrat        = rat_open(path, rat_media_type, 0);
        hrat        = rat_open_ex(rat_root_type, rat_media_type, 0);

        if (hrat)
        {
            media_cnt   = rat_get_cur_scan_cnt(hrat);
        }

        if (NULL == hrat || 0 == media_cnt)
        {
            __wrn("no such media file, reg_media_type=%d", reg_media_type);

            if (app_root_prob_intent_first_play == intent)
            {
                __wrn("intent is first_play, not need switch storage, intent=%d...", intent);
                goto next_media_type;//no_media_file;
            }
            else if (app_root_prob_intent_plugin_for_play == intent)
            {
                __wrn("intent is plugin_for_play, goto no_media_file...");
                goto next_media_type;
            }
            else if (app_root_prob_intent_switch_media == intent)
            {
                __wrn("intent is switch_media, goto next_storage");
                goto next_storage;
            }
            else if (app_root_prob_intent_switch_storage == intent)
            {
                __wrn("intent is switch_storage, goto next_storage");
                goto next_storage;
            }
            else
            {
                __wrn("intent type error...");
                goto fail;
            }
        }

        //探测成功
        __wrn("search media file success, media_cnt=%d", media_cnt);
        break;
next_storage:
        ;
        __wrn("app_root_prob_media_file_for_play: next_storage");
        reg_storage_type++;

        if (reg_storage_type >= BEETLES_TBL_SIZE(para->last_storage_play_app))
        {
            reg_storage_type = 0;
        }

        //计算当前播放盘符，盘符切换不会切换到当前盘符
        cur_reg_root_type = para->cur_play_storage;
        __wrn("reg_storage_type=%d", reg_storage_type);
        __wrn("cur_reg_root_type=%d", cur_reg_root_type);

        if (reg_storage_type == *init_storage_type || (cur_reg_root_type == *init_storage_type && app_root_prob_intent_switch_storage == intent))
        {
            if (EPDK_FALSE == bhas_disk)
            {
                __wrn("search finish, not any reg root type");
                goto no_storage;
            }
            else
            {
                __wrn("search finish, no more reg root type");
                goto no_media_file;
            }
        }

        __wrn("try to switch to reg root type:%d", reg_storage_type);
        continue;
next_media_type:
        ;
        __wrn("app_root_prob_media_file_for_play: next_media_type");
        reg_media_type++;

        if (reg_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage))
        {
            reg_media_type = 0;
        }

#if EBOOK_FUNCTION_EX
#else

        if (2 == reg_media_type)
        {
            reg_media_type = 3;
        }

#endif
#if PHOTO_FUNCTION_EX
#else

        if (3 == reg_media_type)
        {
            reg_media_type = 0;
        }

#endif
        __wrn("reg_media_type=%d, *init_media_type=%d", reg_media_type, *init_media_type);

        if (reg_media_type == *init_media_type)
        {
            if (EPDK_FALSE == bhas_disk)
            {
                __wrn("EPDK_FALSE == bhas_disk, may not run here...");
                goto no_storage;//一般不会跑到这里
            }
            else
            {
                __wrn("no ");
                goto no_media_file;
            }
        }

        __wrn("try to search new media type, reg_media_type=%d", reg_media_type);
        continue;
    } while (1);

    __wrn("***********prob media file success********,*init_media_type=%d, *init_storage_type=%d", *init_media_type, *init_storage_type);

    //ok:
    if (err_code)
    {
        if (init_media_type)
        {
            *init_media_type = reg_media_type;
        }

        if (init_storage_type)
        {
            *init_storage_type = reg_storage_type;
        }

        if (app_root_prob_intent_switch_media == intent)
        {
            *err_code = app_root_prob_media_switch;
        }
        else if (app_root_prob_intent_switch_storage == intent)
        {
            *err_code = app_root_prob_storage_switch;
        }
        else if (app_root_prob_intent_plugin_for_play == intent)
        {
            *err_code = app_root_prob_storage_plugined;
        }
        else if (app_root_prob_intent_first_play == intent)
        {
            *err_code = app_root_prob_storage_plugined;
        }
        else
        {
            __wrn("intent type error...");
            *err_code = app_root_prob_ok;
        }
    }

    return EPDK_OK;
no_storage:
    ;

    if (err_code)
    {
        *err_code = app_root_prob_no_correspond_storage;
    }

    return EPDK_FAIL;
no_media_file:
    ;

    if (err_code)
    {
        *err_code = app_root_prob_no_meida_file;
    }

    return EPDK_FAIL;
fail:
    ;

    if (err_code)
    {
        *err_code = app_root_prob_fail;
    }

    return EPDK_FAIL;
}



static __s32 app_root_show_dlg(H_WIN hwin, __s32 text_id, __s32 title_id, __s32 time)
{
    H_WIN dlg;
    __s32 lang_id[2] = {STRING_ROOT_CUE, STRING_ROOT_NULL_DISK};

    if (title_id)
    {
        lang_id[0] = title_id;
    }

    if (text_id)
    {
        lang_id[1] = text_id;
    }

	//GUI_LyrWinCacheOn();

    default_dialog(dlg, hwin, APP_COM_DLG_ID, ADLG_OK, lang_id);

    //GUI_LyrWinCacheOff();

    if (0 != time)
    {
        esKRNL_TimeDly(time);
    }
    else
    {
        esKRNL_TimeDly(120);
    }


    app_dialog_destroy(dlg);

    return 0;
}

//显示错误信息
//bypass_flag :
//          1 => 显示"USB 音频" "SD 视频"等弹出对话框
//          0 => 不显示弹出对话框
static int32_t app_root_show_err_info(root_ctrl_t *root_ctrl
                                    , int32_t init_media_type
                                    , int32_t init_storage_type
                                    , app_root_prob_intent_t intent
                                    , app_root_prob_result_t err_code, __bool bHideHBar,
                                    int32_t bypass_flag)
{
    int32_t             text_id;
    reg_root_para_t     *para;

    __wrn("*********** app_root_show_err_info, init_media_type=%d, init_storage_type=%d, intent=%d, err_code=%d***********", init_media_type, init_storage_type, intent, err_code);

    para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

    if (!para)
    {
        __wrn("para is null...");
        goto fail;
    }

    if ((init_media_type < 0&& init_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage)))
    {
        __wrn("media type error...");
        goto fail;
    }

    if ((init_storage_type < 0&& init_storage_type >= BEETLES_TBL_SIZE(para->last_storage_play_app)))
    {
        __wrn("storage type is null...");
        goto fail;
    }

    if (/*intent < 0 ||*/ intent >= app_root_prob_intent_max)
    {
        __wrn("intent type error...");
        goto fail;
    }

    text_id     = 0;
    __wrn("intent    ==%d", intent);

    switch (intent)
    {
        case app_root_prob_intent_first_play:
        {
            if (app_root_prob_storage_plugined == err_code)
            {
                if (0 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_SD_AUDIO;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_SD_VIDEO;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_SD_EBOOK;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_SD_PHOTO;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else if (1 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_USB_AUDIO;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_USB_VIDEO;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_USB_EBOOK;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_USB_PHOTO;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }

            break;
        }

        case app_root_prob_intent_plugin_for_play:
        {
            if (0) //app_root_prob_storage_plugined == err_code)
            {
                if (0 == init_storage_type)
                {
                    //text_id = STRING_ROOT_SD_PLUGIN; // 30234384 delete
                }
                else if (1 == init_storage_type)
                {
                    //text_id = STRING_ROOT_UD_PLUGIN; // 30234384 delete
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }
            else if (app_root_prob_no_meida_file == err_code)
            {
                if (0 == init_storage_type)
                {
                    text_id = STRING_ROOT_NO_FILE_IN_SD;
                }
                else if (1 == init_storage_type)
                {
                    text_id = STRING_ROOT_NO_FILE_IN_USB;
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }
            else if (app_root_prob_storage_plugined == err_code)
            {
                if (0 == init_storage_type)
                {
                    //text_id = STRING_ROOT_SD_PLUGIN; // 30234384 delete
                }
                else if (1 == init_storage_type)
                {
                    //text_id = STRING_ROOT_UD_PLUGIN; // 30234384 delete
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }

            break;
        }

        case app_root_prob_intent_switch_media:
        {
            __wrn("@@@@@@@ app_root_prob_intent_switch_media@@@@ ");
            __wrn("*********** app_root_show_err_info, init_media_type=%d, init_storage_type=%d, intent=%d, err_code=%d***********", init_media_type, init_storage_type, intent, err_code);

            if (app_root_prob_no_correspond_storage == err_code)
            {
                text_id = STRING_ROOT_NO_ENY_STORAGE;
            }
            else if (app_root_prob_no_meida_file == err_code)
            {
                if (0 == init_media_type)
                {
                    text_id = STRING_ROOT_NO_ENY_AUDIO_FILE;
                }
                else if (1 == init_media_type)
                {
                    text_id = STRING_ROOT_NO_ENY_VIDEO_FILE;
                }
                else if (2 == init_media_type)
                {
                    text_id = STRING_ROOT_NO_ENY_EBOOK_FILE;
                }
                else if (3 == init_media_type)
                {
                    text_id = STRING_ROOT_NO_ENY_PHOTO_FILE;
                }
                else
                {
                    __wrn("init_media_type type error...");
                }
            }
            else if (app_root_prob_media_switch == err_code)
            {
                if (0 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_AUDIO;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_VIDEO;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_EBOOK;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_PHOTO;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else if (1 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_AUDIO;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_VIDEO;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_EBOOK;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_PHOTO;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }

            break;
        }

        case app_root_prob_intent_switch_storage:
        {
            if (app_root_prob_no_correspond_storage == err_code)
            {
                if (0 == init_storage_type)
                {
                    text_id = STRING_ROOT_NO_SD;
                }
                else if (1 == init_storage_type)
                {
                    text_id = STRING_ROOT_NO_USB;
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }
            else if (app_root_prob_no_meida_file == err_code)
            {
                if (0 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_AUDIO_FILE_IN_SD;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_VIDEO_FILE_IN_SD;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_EBOOK_FILE_IN_SD;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_PHOTO_FILE_IN_SD;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else if (1 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_AUDIO_FILE_IN_USB;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_VIDEO_FILE_IN_USB;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_EBOOK_FILE_IN_USB;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_NO_PHOTO_FILE_IN_USB;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }
            else if (app_root_prob_storage_switch == err_code)
            {
                if (0 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_AUDIO;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_VIDEO;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_EBOOK;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_SD_PHOTO;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else if (1 == init_storage_type)
                {
                    if (0 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_AUDIO;
                    }
                    else if (1 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_VIDEO;
                    }
                    else if (2 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_EBOOK;
                    }
                    else if (3 == init_media_type)
                    {
                        text_id = STRING_ROOT_SWITCH_TO_USB_PHOTO;
                    }
                    else
                    {
                        __wrn("init_media_type type error...");
                    }
                }
                else
                {
                    __wrn("init_storage_type type error...");
                }
            }

            break;
        }

        default:
        {
            __wrn("intent type error...");
            goto fail;
            break;
        }
    }

    __wrn("text_id=%d", text_id);

    if (text_id && bypass_flag)
    {
        HBarState sta;

        if (bHideHBar)
        {
            gscene_hbar_get_state(&sta);
            gscene_hbar_set_state(HBAR_ST_HIDE);
        }

        app_root_show_dlg(root_ctrl->root_para->h_parent
                          , text_id, 0, 0);

        if (bHideHBar)
        {
            gscene_hbar_set_state(sta);
        }
    }

    __wrn("app_root_show_err_info ok...");
    return EPDK_OK;
fail:
    ;
    return EPDK_FAIL;
}


static int32_t __app_root_get_reg_storage_type2(rat_type_t rat_root_type)
{
    int32_t     reg_root_type = 0;

    if (RAT_TF == rat_root_type)
    {
        reg_root_type = 0;
    }
    else if (RAT_USB == rat_root_type)
    {
        reg_root_type = 1;
    }

    return reg_root_type;
}

//盘符探测
//init_media_type:用于存放探测得到的媒体类型
//init_storage_type:用于存放探测得到的媒体介质类型
//intent:探测方式
//返回:EPDK_OK，探测成功，EPDK_FAIL，探测失败，*err_code存放错误代号
static int32_t app_root_prob_storage_type_for_play(root_ctrl_t *root_ctrl
        , int32_t *init_media_type
        , int32_t *init_storage_type
        , app_root_prob_intent_t intent
        , app_root_prob_result_t *err_code)
{
    reg_root_para_t *para;

    __wrn("***********app_root_prob_storage_type_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d***********",intent, *init_media_type, *init_storage_type);

    para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

    if (!para)
    {
        __wrn("para is null...");
        goto fail;
    }

    if (!init_media_type || (*init_media_type < 0 && *init_media_type >= BEETLES_TBL_SIZE(para->last_app_play_storage)))
    {
        if (app_root_prob_intent_first_play != intent && app_root_prob_intent_plugin_for_play != intent)
        {
            __wrn("media type error...");
            goto fail;
        }
    }

    if (!init_storage_type)
    {
        if (app_root_prob_intent_first_play != intent)
        {
            __wrn("storage type is null...");
            goto fail;
        }
    }

    if (/*intent < 0 ||*/ intent >= app_root_prob_intent_max)
    {
        __wrn("intent type error...");
        goto fail;
    }

    if (app_root_prob_intent_first_play == intent)
    {
        if (para->last_play_app < 0 || para->last_play_app >= BEETLES_TBL_SIZE(para->last_app_play_storage))
        {
            para->last_play_app     = 0;
        }

        if (init_media_type)
        {
            *init_media_type        = para->last_play_app;
        }

        if (init_storage_type)
        {
            *init_storage_type      = para->last_app_play_storage[para->last_play_app];

            if (1)
            {
                int32_t     ret;
                char        disk_name[RAT_MAX_PARTITION][4];

                if (0 == (*init_storage_type))  //被记忆的媒体介质是TF
                {
                    __wrn("*init_storage_type = sd ");

                    if (EPDK_OK == rat_get_partition_name(RAT_SD_CARD, disk_name, 0))
                    {
                        if (disk_name[0][0])
                        {
                        }
                        else  //TF  不存在
                        {
                            __wrn("sd no plugin ");

                            if (EPDK_OK == rat_get_partition_name(RAT_USB_DISK, disk_name, 0))
                            {
                                if (disk_name[0][0]) //USB  存在
                                {
                                    __wrn("usb plugin ");
                                    *init_storage_type = 1;
                                }
                            }
                        }
                    }
                    else  //TF  不存在
                    {
                        __wrn("sd no plugin ");

                        if (EPDK_OK == rat_get_partition_name(RAT_USB_DISK, disk_name, 0))
                        {
                            if (disk_name[0][0]) //USB  存在
                            {
                                __wrn("usb plugin ");
                                *init_storage_type = 1;
                            }
                        }
                    }
                }
                else if (1 == (*init_storage_type))   //被记忆的媒体介质是USB
                {
                    __wrn("*init_storage_type = usb ");

                    if (EPDK_OK == rat_get_partition_name(RAT_USB_DISK, disk_name, 0))
                    {
                        if (disk_name[0][0])
                        {
                        }
                        else  //USB  不存在
                        {
                            __wrn("usb not plugin ");

                            if (EPDK_OK == rat_get_partition_name(RAT_SD_CARD, disk_name, 0))
                            {
                                if (disk_name[0][0]) //TF  存在
                                {
                                    __wrn("sd plugin ");
                                    *init_storage_type = 0;
                                }
                            }
                        }
                    }
                    else
                    {
                        __wrn("usb not plugin ");

                        if (EPDK_OK == rat_get_partition_name(RAT_SD_CARD, disk_name, 0))
                        {
                            if (disk_name[0][0]) //TF  存在
                            {
                                __wrn("sd plugin ");
                                *init_storage_type = 0;
                            }
                        }
                    }
                }

                //para->cur_storage = *init_storage_type;
            }
        }

        __wrn("app_root_prob_intent_first_play, *init_media_type=%d, *init_storage_type=%d" , *init_media_type, *init_storage_type);
    }
    else if (app_root_prob_intent_switch_media == intent)
    {
        __wrn("app_root_prob_intent_switch_media");

        switch (*init_media_type)
        {
            case 0://音频
            {
                //music已经运行
                if (root_ctrl->h_app_music)
                {
                    __wrn("music alreadly running");
                    goto alreadly_running;
                }

                __wrn("music not running");
                goto from_nearest_storage;
                break;
            }

            case 1://视频
            {
                if (root_ctrl->h_app_movie)
                {
                    __wrn("movie alreadly running");
                    goto alreadly_running;
                }

                __wrn("movie not running");
                goto from_nearest_storage;
                break;
            }

            case 2://电子书
            {
                if (root_ctrl->h_app_ebook)
                {
                    __wrn("ebook alreadly running");
                    goto alreadly_running;
                }

                __wrn("ebook not running");
                goto from_nearest_storage;
                break;
            }

            case 3://图片
            {
                if (root_ctrl->h_app_photo)
                {
                    __wrn("photo alreadly running");
                    goto alreadly_running;
                }

                __wrn("photo not running");
                goto from_nearest_storage;
                break;
            }

            default:
            {
                __wrn("media type error...");
                goto fail;
                break;
            }
        }
    }
    else if (app_root_prob_intent_switch_storage == intent)
    {
        __wrn("app_root_prob_intent_switch_storage");

        switch (*init_media_type)
        {
            case 0://音频
            {
                //music尚未运行
                if (!root_ctrl->h_app_music)
                {
                    __wrn("music not running");
                    goto not_running;
                }

                __wrn("music is running");
                goto from_next_storage;
                break;
            }

            case 1://视频
            {
                //视频尚未运行
                if (!root_ctrl->h_app_movie)
                {
                    __wrn("movie not running");
                    goto not_running;
                }

                __wrn("movie is running");
                goto from_next_storage;
                break;
            }

            case 2://电子书
            {
                //电子书尚未运行
                if (!root_ctrl->h_app_ebook)
                {
                    __wrn("ebook not running");
                    goto not_running;
                }

                __wrn("ebook is running");
                goto from_next_storage;
                break;
            }

            case 3://图片
            {
                //图片尚未运行
                if (!root_ctrl->h_app_photo)
                {
                    __wrn("photo not running");
                    goto not_running;
                }

                __wrn("photo is running");
                goto from_next_storage;
                break;
            }

            default:
            {
                __wrn("media type error...");
                goto fail;
                break;
            }
        }
    }
    else if (app_root_prob_intent_plugin_for_play == intent)
    {
        __wrn("app_root_prob_intent_plugin_for_play");

        if (!init_storage_type)
        {
            __wrn("init_storage_type is null...");
            goto fail;
        }

        if (*init_storage_type < 0 || *init_storage_type >= BEETLES_TBL_SIZE(para->last_storage_play_app))
        {
            __wrn("storage type invalid...");
            goto fail;
        }

        if (init_media_type)
        {
            *init_media_type = para->last_storage_play_app[*init_storage_type];
        }

        __wrn("get last storage play media, reg media type=%d", *init_media_type);
        goto storage_plugin;
    }
    else
    {
        __wrn("intent type error...");
        goto fail;
    }

    __wrn("prob storage type success,*init_media_type=%d, *init_storage_type=%d", *init_media_type, *init_storage_type);
    goto ok;
from_next_storage:
    ;

    __wrn("app_root_prob_storage_type_for_play: from_next_storage");
    {
        //切换盘符
        int32_t             ret;
        HRAT                hrat;
        char                path[RAT_MAX_FULL_PATH_LEN];
        rat_type_t          rat_root_type;
        int32_t             to_root_type;
        rat_media_type_t    rat_media_type;
        uint32_t            media_cnt;

        if (-1 == para->cur_play_storage || para->cur_play_storage >= BEETLES_TBL_SIZE(para->last_app_play_storage))
        {
            __wrn("para->cur_play_storage error, change to 0...");
            para->cur_play_storage = 0;
        }

        to_root_type    = para->cur_play_storage;
        __wrn("current reg disk type is:%d", para->cur_play_storage);

        to_root_type++;

        if (to_root_type >= BEETLES_TBL_SIZE(para->last_storage_play_app))
        {
            to_root_type = 0;
        }

        if (to_root_type == para->cur_play_storage)
        {
            __wrn("there is only one disk currently, reg root type=%d", to_root_type);
            goto no_storage;
        }

        if (init_storage_type)
        {
            *init_storage_type  = to_root_type;
            //para->cur_storage  = *init_storage_type; // 30234384
        }

        rat_root_type   = __app_root_get_rat_root_type(to_root_type);

        if (RAT_UNKNOWN == rat_root_type)
        {
            __wrn("rat_root_type type error...");
            goto no_storage;
        }

        rat_media_type  = __app_root_get_rat_media_type(*init_media_type);

        if (RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
        {
            __wrn("rat_media_type type error...");
            goto no_media_file;
        }

        __wrn("try switch to reg root type:%d", to_root_type);
        eLIBs_memset(path, 0, sizeof(path));

        ret = __app_root_get_search_path(rat_root_type, path);

        if (EPDK_FAIL == ret) //无磁盘
        {
            __wrn("no such reg root type:%d", to_root_type);
            goto no_storage;
        }

        hrat    = rat_open_ex(rat_root_type, rat_media_type, 0);

        if (NULL == hrat)
        {
            __wrn("no corresponden media file, reg root type=%d", to_root_type);
            goto no_media_file;
        }

        media_cnt   = rat_get_cur_scan_cnt(hrat);

        if (0 == media_cnt)
        {
            __wrn("no corresponden media file, reg root type=%d", to_root_type);
            goto no_media_file;
        }

        if (init_storage_type)
        {
            *init_storage_type = to_root_type;
            //para->cur_storage  = *init_storage_type; // 30234384
        }

        __wrn("success to switch reg root to %d, reg media type=%d, media cnt=%d", to_root_type, *init_media_type, media_cnt);
        goto ok;
    }
from_nearest_storage:
    ;
    __wrn("app_root_prob_storage_type_for_play: from_nearest_storage");
    {
        //获取当前播放盘
        __wrn("try to get current playing root type");

        if (-1 != para->cur_play_storage) //如果当前有盘符正在播放
        {
            if (init_storage_type)
            {
                *init_storage_type = para->cur_play_storage;
                //para->cur_storage  = *init_storage_type; // 30234384
            }

            __wrn("current playing reg root type is:%d", para->cur_play_storage);
            goto ok;
        }
        else//获取最近播放该ap的盘
        {
            __wrn("current playing root type is empty...");

            if (init_storage_type)
            {
                *init_storage_type = para->last_app_play_storage[*init_media_type];
                //para->cur_storage  = *init_storage_type; // 30234384
            }

            __wrn("get last disk which play this media,reg media type=%d, reg root type=%d",*init_media_type, para->last_app_play_storage[*init_media_type]);
            goto ok;
        }
    }
ok:
    ;
    __wrn("app_root_prob_storage_type_for_play: ok");

    if (err_code)
    {
        *err_code = app_root_prob_ok;
    }

    return EPDK_OK;
storage_plugin:
    ;
    __wrn("app_root_prob_storage_type_for_play: storage_plugin");

    if (err_code)
    {
        *err_code = app_root_prob_ok;
    }

    return EPDK_OK;
alreadly_running:
    ;
    __wrn("app_root_prob_storage_type_for_play: alreadly_running");

    if (init_storage_type)
    {
        *init_storage_type  = para->cur_play_storage;
        //para->cur_storage  = *init_storage_type; // 30234384
    }

    if (err_code)
    {
        *err_code   = app_root_prob_already_running;
    }

    return EPDK_FAIL;
not_running:
    ;
    __wrn("app_root_prob_storage_type_for_play: not_running");

    if (err_code)
    {
        *err_code   = app_root_prob_not_running;
    }

    return EPDK_FAIL;
no_storage:
    ;
    __wrn("app_root_prob_storage_type_for_play: no_storage");

    if (err_code)
    {
        *err_code   = app_root_prob_no_correspond_storage;
    }

    return EPDK_FAIL;
no_media_file:
    ;
    __wrn("app_root_prob_storage_type_for_play: no_media_file");

    if (err_code)
    {
        *err_code   = app_root_prob_no_meida_file;
    }

    return EPDK_FAIL;
fail:
    ;
    __wrn("app_root_prob_storage_type_for_play: fail");

    if (err_code)
    {
        *err_code   = app_root_prob_fail;
    }

    return EPDK_FAIL;
}


//************************************************************************
//bHideHBar:  1 => 隐藏顶部状态栏,  0=>显示顶部状态栏
//showdlg_bypass_flag :
//                      1 => 显示"USB 音频" "SD 视频"等弹出对话框
//                      0 => 不显示弹出对话框
//************************************************************************
static int32_t app_root_shortcut_process(root_ctrl_t *root_ctrl, app_root_prob_intent_t intent, int32_t reg_media_type, int32_t reg_storage_type, __bool bHideHBar, __s32 showdlg_bypass_flag)
{
    int32_t     ret;
    app_root_prob_result_t err_code;
    __bool  switch_storage_no_meida_file_or_not_running_to_plugin_for_play = EPDK_NO;

    //reg_root_para_t *reg_root_para;
    //reg_root_para = (reg_root_para_t*)dsk_reg_get_para_by_app(REG_APP_ROOT);
    //if(!reg_root_para)
    //{
    //    __wrn("para is null...");
    //    goto err;
    //}
    //eLIBs_printf("(%s)L%d:shortcut begin root_type = %d",__FILE__,__LINE__,reg_storage_type);
    __wrn("***********app_root_shortcut_process, intent=%d, reg_media_type=%d, reg_storage_type=%d***********", intent, reg_media_type, reg_storage_type);
    __log("===app_root_shortcut_process===");

    //检查处理类型
    if (intent >= app_root_prob_intent_max)
    {
        __wrn("intent type error...");
        goto err;
    }

    {
        //如果类型正确，则进行盘符探测
        ret = app_root_prob_storage_type_for_play(root_ctrl, &reg_media_type, &reg_storage_type, intent, &err_code);

        //reg_root_para->cur_storage  = reg_storage_type; // 30234384
        __wrn("***********after app_root_prob_storage_type_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d, ret=%d***********",
              intent, reg_media_type, reg_storage_type, err_code, ret);

        //eLIBs_printf("(%s)L%d: root_type = %d",__FILE__,__LINE__,reg_storage_type);

        if (1) //盘符探测后的预处理
        {
            if (app_root_prob_already_running == err_code)
            {
                if (0 == reg_media_type) //如果是后台音乐，则直接恢复播放
                {
                    if (root_ctrl->h_app_music)
                    {
                        musicplayer_mode_e mode;

                        mode    = app_music_get_mode(root_ctrl->h_app_music);

                        if (-1 == reg_storage_type) //如果第一次探测盘符失败，则获取当前播放盘符
                        {
                            int32_t     ret;
                            unsigned long    rat_root_type;

                            ret = app_music_get_root_type(root_ctrl->h_app_music, &rat_root_type);

                            if (EPDK_OK == ret)
                            {
                                reg_storage_type = __app_root_get_reg_storage_type2(rat_root_type);
                                //eLIBs_printf("(%s)L%d:fail root_type = %d",__FILE__,__LINE__,reg_storage_type);
                            }
                        }

                        if (MUSICPLAYER_BACKGROUND == mode || MUSICPLAYER_BACKGROUND_HOME == mode)
                        {
                            __wrn("background music is running, reseum it directly...");
                            goto set_play_file;
                        }
                    }
                }
            }
        }

        __wrn("***********after app_root_prob_storage_type_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d***********", 
                intent, reg_media_type, reg_storage_type, err_code);

        if (EPDK_OK != ret) //无相同类型媒体, 切换其他类型
        {
            if (app_root_prob_no_correspond_storage != err_code)    //无盘无需执行
            {
                if (app_root_prob_intent_switch_storage == intent && (app_root_prob_no_meida_file == err_code || app_root_prob_not_running == err_code))
                {
                    intent = app_root_prob_intent_plugin_for_play;

                    if (app_root_prob_not_running == err_code)  //not run状态下,没有切换存储类型
                    {
                        if (0 == reg_storage_type)
                        {
                            reg_storage_type = 1;
                        }
                        else
                        {
                            reg_storage_type = 0;
                        }
                    }

                    ret = app_root_prob_storage_type_for_play(root_ctrl, &reg_media_type, &reg_storage_type, intent, &err_code);

                    if (EPDK_OK == ret)
                    {
                        switch_storage_no_meida_file_or_not_running_to_plugin_for_play = EPDK_YES;
                    }

                    __wrn("***********after app_root_prob_storage_type_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d, ret=%d***********",
                          intent, reg_media_type, reg_storage_type, err_code, ret);
                }
            }
        }

        //eLIBs_printf("(%s)L%d: root_type = %d",__FILE__,__LINE__,reg_storage_type);
        __wrn("reg_media_type... = %d", reg_media_type);
        app_root_show_err_info(root_ctrl, reg_media_type, reg_storage_type, intent, err_code, bHideHBar, showdlg_bypass_flag);

        //eLIBs_printf("(%s)L%d:111 root_type = %d",__FILE__,__LINE__,reg_storage_type);

        if (EPDK_OK != ret)
        {
            goto err;
        }

        //探测成功，继续进行媒体探测
        ret = app_root_prob_media_file_for_play(root_ctrl, &reg_media_type, &reg_storage_type, intent, &err_code);

        __wrn("***********after app_root_prob_media_file_for_play, intent=%d, reg_media_type=%d, reg_storage_type=%d, err_code=%d, ret=%d***********",
              intent, reg_media_type, reg_storage_type, err_code, ret);

        //eLIBs_printf("(%s)L%d:111 root_type = %d",__FILE__,__LINE__,reg_storage_type);

        if (EPDK_YES == switch_storage_no_meida_file_or_not_running_to_plugin_for_play
            &&  app_root_prob_intent_plugin_for_play == intent &&  app_root_prob_storage_plugined == err_code)
        {
            intent = app_root_prob_intent_switch_media;
            err_code = app_root_prob_media_switch;
        }

        app_root_show_err_info(root_ctrl, reg_media_type, reg_storage_type, intent, err_code, bHideHBar, showdlg_bypass_flag);

        if (EPDK_OK != ret)
        {
            goto err;
        }

set_play_file:
        ;
        {
            //探测成功，设置播放列表
            int32_t             ret;
            rat_type_t          rat_root_type;
            rat_media_type_t    rat_media_type;

            rat_media_type  = __app_root_get_rat_media_type(reg_media_type);
            rat_root_type   = __app_root_get_rat_root_type(reg_storage_type);

            //eLIBs_printf("(%s)L%d:222 root_type = %d",__FILE__,__LINE__,rat_root_type);

            if (RAT_MEDIA_TYPE_UNKNOWN == rat_media_type)
            {
                __wrn("media type error...");
                goto err;
            }

            if (RAT_UNKNOWN == rat_root_type)
            {
                __wrn("rat root type error...");
                goto err;
            }

            ret = __app_root_set_playfile(rat_root_type, rat_media_type);

            if (EPDK_FAIL == ret)
            {
                __wrn("set file for play fail...");
                goto err;
            }

            __wrn("set file for play success...");
        }
        //begin_play:;
        //探测成功，则开始播放
        {
            rat_type_t      rat_root_type;

            rat_root_type   = __app_root_get_rat_root_type(reg_storage_type);

            if (RAT_UNKNOWN == rat_root_type)
            {
                __wrn("storage type unknown...");
                goto err;
            }

            //eLIBs_printf("(%s)L%d:set root_type = %d",__FILE__,__LINE__,rat_root_type);
            //SetCurrentDevTypeEx(rat_root_type);  //chengf

            if (0 == reg_media_type)
            {
                __log("===__app_root_to_audio===");
                __app_root_to_audio(root_ctrl, rat_root_type);
            }
            else if (1 == reg_media_type)
            {
                __log("===__app_root_to_video===");
                __app_root_to_video(root_ctrl, rat_root_type);
            }
            else if (2 == reg_media_type)
            {
                __log("===__app_root_to_ebook===");
                __app_root_to_ebook(root_ctrl, rat_root_type);
            }
            else if (3 == reg_media_type)
            {
                __log("===__app_root_to_photo===");
                __app_root_to_photo(root_ctrl, rat_root_type);
            }
            else
            {
                __wrn("reg_media_type type unknown...");
                goto err;
            }
        }
    }

    __wrn("***********app_root_shortcut_process success***********");
    return EPDK_OK;
err:
    __wrn("***********app_root_shortcut_process fail***********");
    return EPDK_FAIL;
}


static int32_t app_root_check_volume_key(__gui_msg_t *msg)
{
    int32_t         bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
    static int32_t  longdown_cnt = 0;
    static int32_t  last_key = -1;
    root_ctrl_t     *root_ctrl = NULL;

    root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if (!root_ctrl)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }
    if (KEY_DOWN_ACTION == msg->dwAddData2 || KEY_REPEAT_ACTION == msg->dwAddData2)
    {
        if (GUI_MSG_KEY_VADD == msg->dwAddData1 || GUI_MSG_KEY_LONGVADD == msg->dwAddData1)
        {
            dsk_volume_inc();

            if (root_ctrl->h_app_movie)
            {
                app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_movie);
            }

            if (!is_on_tv()
#ifdef APP_ADD_RECORD_VEDIO
                && root_ctrl->h_app_record_video == NULL
#endif
               )
            {
                gscene_hbar_update_text(HBAR_VOLUME_PAINT);
                gscene_hbar_set_state(HBAR_ST_SHOW); //(HBAR_ST_SHOW_WIDTH_VOLUME); //在karaoke中不让显示音量条
            }
        }
        else if (GUI_MSG_KEY_VDEC == msg->dwAddData1 || GUI_MSG_KEY_LONGVDEC == msg->dwAddData1)
        {
            dsk_volume_dec();

            if (root_ctrl->h_app_movie)
            {
                app_movie_notify_delete_sub_dlg_scene(root_ctrl->h_app_movie);
            }

            if (!is_on_tv()
#ifdef APP_ADD_RECORD_VEDIO
                && root_ctrl->h_app_record_video == NULL
#endif
               )
            {
                gscene_hbar_update_text(HBAR_VOLUME_PAINT);
                gscene_hbar_set_state(HBAR_ST_SHOW); //(HBAR_ST_SHOW_WIDTH_VOLUME);
            }
        }
    }

    //   end:;//快捷操作处理完毕
    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key = -1;
        longdown_cnt = 0;
    }
    else
    {
        last_key = msg->dwAddData1;
    }

    //判断是否需要过滤快捷键
    bok = EPDK_FAIL;

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_VADD:
        case GUI_MSG_KEY_LONGVADD:
        case GUI_MSG_KEY_VDEC:
        case GUI_MSG_KEY_LONGVDEC:
        {
            bok = EPDK_OK;
            break;
        }

        default:
        {
            break;
        }
    }
    //    __wrn("app_root_check_short_key end");
    return bok;
}
#define DESKTOP_REFRESH_MSG                     (14)

#if defined(CONFIG_SUNXI_QA_TEST)
static __bool _app_root_has_tow_disk(void)
{
    int32_t     ret;
	int32_t     len;
    char        diskname[RAT_MAX_PARTITION][4] = {{0},};

    ret     = rat_get_partition_name(RAT_USB_DISK, diskname, 0);
	len = eLIBs_strlen(diskname[0]);

    if ((EPDK_OK != ret)||(len == 0))
    {
        return EPDK_FALSE;
    }
	if (diskname[0][0])
	{
	   __log("======diskname[0][0] = %c =====", diskname[0][0]);
	}

    ret = rat_get_partition_name(RAT_SD_CARD, diskname, 0);
	len = eLIBs_strlen(diskname[0]);

    if ((EPDK_OK != ret)||(len == 0))
    {
        return EPDK_FALSE;
    }
	if (diskname[0][0])
	{
	   __log("======diskname[0][0] = %c =====", diskname[0][0]);
	}

    return EPDK_TRUE;
}
#endif

static int32_t app_root_check_short_key(__gui_msg_t *msg)
{
    int32_t         bok;// 按键消息是否已处理完毕，-1代表往下传，否则直接拦截
    int32_t         ret;
    static int32_t  last_key = -1;
    root_ctrl_t     *root_ctrl;
    reg_root_para_t *para;
    app_root_prob_intent_t  intent;
    int32_t         media_type;
    int32_t         storage_type;
    static int32_t  longdown_cnt = 0;
    rat_type_t      rat_root_type;

    ret     = app_root_check_volume_key(msg);

    if (ret == EPDK_OK)
    {
    	__inf("return EPDK_FAIL");
        return EPDK_FAIL;//往下传，电影、电视需要消隐headbar
    }

    para    = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);
    if (!para)
    {
        __wrn("para is null...");
        //return EPDK_OK;
    }
    root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
    if (!root_ctrl)
    {
        __wrn("invalid para...");
        return EPDK_FAIL;
    }

    //处理power按键回到主界面进入standby模式
    if (KEY_UP_ACTION == msg->dwAddData2)
    {

        if (GUI_MSG_KEY_POWEROFF == msg->dwAddData1)
        {
            __wrn("GUI_MSG_KEY_POWEROFF Short process");

            //dsk_display_lcd_off();
            if (root_ctrl->h_app_home == NULL)
            {
                dsktop_scene_set_cur_app(ID_HOME_DTV);

                __app_root_delete_all_app(root_ctrl);

                app_root_cmd2self(root_ctrl, APP_DTV_ID, SWITCH_TO_MMENU, 0, 0);
            }
            else
            {
                __gui_msg_t msg;

                eLIBs_memset(&msg, 0, sizeof(__gui_msg_t));

                msg.h_deswin    = root_ctrl->h_app_home;
                msg.h_srcwin    = NULL;
                msg.id          = GUI_MSG_COMMAND;
                msg.dwAddData1  = DESKTOP_REFRESH_MSG;
                msg.dwAddData2  = ID_HOME_DTV;
                msg.dwReserved  = 0;
                GUI_SendMessage(&msg);
            }

            dsk_power_off();

            return EPDK_OK;
        }
        else if (GUI_MSG_KEY_MUTE == msg->dwAddData1)
        {
            if (dsk_volume_is_on())
            {
                dsk_volume_off();//mute
            }
            else
            {
                dsk_volume_on();//repoen
            }

            return EPDK_OK;
        }
    }
#if defined(CONFIG_SUNXI_QA_TEST)
	if (KEY_DOWN_ACTION == msg->dwAddData2)
	{

		if (GUI_MSG_KEY_TF_USB == msg->dwAddData1)//强行切换盘符
		{
			__u32 ret1;
			__u32 dev_type;
			reg_root_para_t *root_reg_para;

			ret1 = _app_root_has_tow_disk();
			__log("_app_root_has_tow_disk=%x",ret1);
			if(ret1 == EPDK_TRUE)
			{
				dev_type = GetCurrentDevTypeEx();
				__log("_app_root_has_tow_disk=%x",dev_type);
				if(dev_type == RAT_USB)
				{
					SetCurrentDevTypeEx(RAT_TF);
					root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);
			        if (root_reg_para)
			        {
			            __s32 reg_app_type;
			            reg_app_type = 0;
						for(;reg_app_type < 4;reg_app_type++)
						{
							root_reg_para->cur_play_storage = 0;
							root_reg_para->last_app_play_storage[reg_app_type] = 0;
						}
			        }
			        else
			        {
			            __err("para is null...");
			        }
				}
				else
				{
					SetCurrentDevTypeEx(RAT_USB);
					root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);
			        if (root_reg_para)
			        {
			            __s32 reg_app_type;
			            reg_app_type = 0;
						for(;reg_app_type < 4;reg_app_type++)
						{
							root_reg_para->cur_play_storage = 1;
							root_reg_para->last_app_play_storage[reg_app_type] = 1;
						}
			        }
			        else
			        {
			            __err("para is null...");
			        }
				}
			}
			__log("\n\nGUI_MSG_KEY_NUM0\n\n");
			goto end;
		}
	}
#endif
    //处理FM快捷
    if (KEY_DOWN_ACTION == msg->dwAddData2)
    {

        if (GUI_MSG_KEY_TV == msg->dwAddData1)
        {

            __log("===__app_root_to_fm===");

            if (root_ctrl->h_app_dtv == NULL)
            {
                app_root_show_dlg(root_ctrl->root_para->h_parent, STRING_ROOT_SWITCH_TO_TV, 0, 0);
                __app_root_to_dtv(root_ctrl);
            }

            goto end;
        }
        else if (GUI_MSG_KEY_FM == msg->dwAddData1)
        {
            __log("===__app_root_to_fm===");

            if (root_ctrl->h_app_fm == NULL)
            {
                app_root_show_dlg(root_ctrl->root_para->h_parent, STRING_ROOT_SWITCH_TO_FM, 0, 0);
                __app_root_to_fm(root_ctrl);
            }

            goto end;
        }
    }

    //处理音频和视频快捷
    {
        //得到媒体类型和处理方式
        intent          = app_root_prob_intent_unknown;
        media_type      = -1;
        storage_type    = -1;

        if (KEY_DOWN_ACTION == msg->dwAddData2) //||KEY_REPEAT_ACTION == msg->dwAddData2)
        {
            if (GUI_MSG_KEY_AUDIO == msg->dwAddData1)
            {
                __log("===GUI_MSG_KEY_AUDIO===");
                intent      = app_root_prob_intent_switch_media;
                media_type  = 0;//音频
            }
            else if (GUI_MSG_KEY_EBOOK == msg->dwAddData1)
            {
                intent      = app_root_prob_intent_switch_media;
                media_type  = 2;//电子书
            }
            else if (GUI_MSG_KEY_VIDEO == msg->dwAddData1)
            {
                intent      = app_root_prob_intent_switch_media;
                media_type  = 1;//视频
            }
            else if (GUI_MSG_KEY_PHOTO == msg->dwAddData1)
            {
                intent      = app_root_prob_intent_switch_media;
                media_type  = 3;//图片
            }
        }
    }

    if (intent < app_root_prob_intent_max)
    {
        if (root_ctrl->h_app_home)
        {
            __log("1===intent=%d,media_type=%d===", intent, media_type);
            app_root_shortcut_process(root_ctrl, intent, media_type, storage_type, EPDK_TRUE, 1);
        }
        else
        {
            __log("2===intent=%d,media_type=%d===", intent, media_type);
            app_root_shortcut_process(root_ctrl, intent, media_type, storage_type, EPDK_FALSE, 1);
        }
    }

end:;//快捷操作处理完毕

    if (KEY_UP_ACTION == msg->dwAddData2)
    {
        last_key        = -1;
        longdown_cnt    = 0;
    }
    else
    {
        last_key        = msg->dwAddData1;
    }

    //判断是否需要过滤快捷键
    bok     = EPDK_FAIL;

    switch (msg->dwAddData1)
    {
        case GUI_MSG_KEY_FM:
        case GUI_MSG_KEY_LONGFM:
        case GUI_MSG_KEY_VIDEO:
        case GUI_MSG_KEY_LONGVIDEO:
        case GUI_MSG_KEY_AUDIO:
        case GUI_MSG_KEY_LONGAUDIO:

        //case GUI_MSG_KEY_EBOOK:
        //case GUI_MSG_KEY_LONGEBOOK:
        case GUI_MSG_KEY_PHOTO:
        case GUI_MSG_KEY_LONGPHOTO:
        case GUI_MSG_KEY_TV:
        case GUI_MSG_KEY_LONGTV:
        {
            __log("clear shortcut key");
            bok     = EPDK_OK;
            break;
        }

        default:
        {
            break;
        }
    }

    return bok;
}

static int32_t auto_check_save_reg(void)
{
    //eLIBs_printf("auto_check_save_reg");
    dsk_reg_flush();
    return 0;
}

int32_t app_root_win_proc(__gui_msg_t *msg)
{
    //__log("app_root_win_proc msg->id=0x%x, msg->h_deswin=0x%x, msg->dwAddData1=0x%x, msg->dwAddData2=0x%x"
    //    ,msg->id, msg->h_deswin, msg->dwAddData1, msg->dwAddData2);
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            root_para_t     *root_para = NULL;
            root_ctrl_t     *root_ctrl = NULL;

            root_para   = esMEMS_Malloc(0, sizeof(root_para_t));

            if (!root_para)
            {
                return EPDK_FAIL;
            }

            eLIBs_memset(root_para, 0, sizeof(root_para_t));

            root_ctrl   = esMEMS_Malloc(0, sizeof(root_ctrl_t));

            if (!root_ctrl)
            {
                esMEMS_Mfree(0, root_para);
                return EPDK_FAIL;
            }

            eLIBs_memset(root_ctrl, 0, sizeof(root_ctrl_t));

            root_para->h_parent     = msg->h_deswin;
            root_para->font         = SWFFont;
            root_para->root_type    = 0;
            root_ctrl->root_para    = root_para;
            root_ctrl->h_app_home   = app_home_create(root_para);

            GUI_WinSetFocusChild(root_ctrl->h_app_home);
            GUI_WinSetAddData(msg->h_deswin, (unsigned long)root_ctrl);
            GUI_SetTimer(msg->h_deswin, APP_COM_FLUSH_ID, 3000, NULL);
        
			#if 0
			static bool tp_flag = 0;
			if(!tp_flag)
			{
				tp_flag = 1;
				H_WIN   scene_adjust = NULL;
				__err("goto adjust tp");
				scene_adjust = scene_adjust_create(msg->h_deswin);
				scene_adjust_destroy(scene_adjust);
			}
			#endif

		}

        return EPDK_OK;

        case GUI_MSG_TIMER:
        {
            root_ctrl_t   *root_ctrl = NULL;

            root_ctrl = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (root_ctrl && APP_COM_FLUSH_ID == msg->dwAddData1)
            {
                GUI_ResetTimer(msg->h_deswin, APP_COM_FLUSH_ID, 3000, NULL);
                auto_check_save_reg();
                return EPDK_OK;
            }
        }
        return EPDK_OK;
        case GUI_MSG_CLOSE:
        {
            GUI_ManWinDelete(msg->h_deswin);
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            root_para_t     *root_para = NULL;
            root_ctrl_t     *root_ctrl = NULL;

            root_ctrl   = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);

            if (!root_ctrl)
            {
                __log("******err****");
                return EPDK_OK;
            }

            root_para   = (root_para_t *)root_ctrl->root_para;

            if (!root_para)
            {
                __log("******err****");
                return EPDK_OK;
            }

            if (GUI_IsTimerInstalled(msg->h_deswin, APP_COM_FLUSH_ID))
            {
                GUI_KillTimer(msg->h_deswin, APP_COM_FLUSH_ID);
            }

            esMEMS_Mfree(0, root_para);
            esMEMS_Mfree(0, root_ctrl);
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            __log("svabed......GUI_MSG_COMMAND");
            app_root_command_proc(msg);
            return EPDK_OK;
        }

        case GUI_MSG_KEY:
        {
            int32_t     ret;

            __inf("***************before app_root_win_proc GUI_MSG_KEY**************************");
            ret     = app_root_check_short_key(msg);

            if (EPDK_OK == ret)
            {
                return EPDK_OK;
            }
            break;
        }

        case GUI_MSG_TOUCH:
        {
            __log("app_root_win_proc GUI_MSG_TOUCH");
            break;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            root_ctrl_t *root_ctrl = NULL;
            H_WIN       root, child, next_child;
            char        winname[256];

            __log("**************app_root_win_proc, DSK_MSG_FS_PART_PLUGOUT**************");

#ifdef CONFIG_DRIVERS_USB_GADGET_MTP
			usb_uevent_handle(MTP_STATE_DEL, (__u8)(msg->dwAddData2));
#endif
            if (('D' == msg->dwAddData2) && ('E' == msg->dwAddData2) && ('Z' == msg->dwAddData2))
            {
                __log("internal disk, not need kill apps, msg->dwAddData1=%d", msg->dwAddData2);
                return EPDK_OK;
            }

            __log("app_root_broadcast_msg, msg->dwAddData2=%d", msg->dwAddData2);
            __app_root_broadcast_msg(msg);
            __log("********before __app_do_async***********");
            __log("rat_clear_all_list_by_root=%x,msg->dwAddData2=%x", (unsigned long)rat_clear_all_list_by_root, msg->dwAddData2);
            __app_do_async(msg->h_deswin, APP_ROOT_MSG_DO_ASYNC, rat_clear_all_list_by_root, (void *)(msg->dwAddData2));
            __log("********after __app_do_async***********");
            return EPDK_OK;
        }
#ifdef CONFIG_DRIVERS_USB_GADGET_MTP
		case DSK_MSG_USBD_PLUG_OUT:
		{
			usb_uevent_handle(MTP_STATE_EXIT, 0);
		} break;
		
		case DSK_MSG_USBD_PLUG_IN:
		{
			usb_uevent_handle(MTP_STATE_INIT, 0);
		} break;
#endif		
        case DSK_MSG_FS_PART_PLUGIN:
        {
            int32_t     ret;
            uint64_t    fs_size = 0;
            char        diskname[4];
            uint32_t    last_lun;
            reg_root_para_t     *reg_root_para = NULL;
            root_ctrl_t         *root_ctrl = NULL;

            root_ctrl       = (root_ctrl_t *)GUI_WinGetAddData(msg->h_deswin);
            reg_root_para   = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

#if defined(CONFIG_SUNXI_QA_TEST)
			return EPDK_OK;//qa测试不要开机自动播放
			
#endif

            if (!reg_root_para)
            {
                __wrn("************ ret root para is null...");
            }

            diskname[0]     = (__u8)(msg->dwAddData2);
            diskname[1]     = '\0';
            last_lun        = (__u8)(msg->dwAddData2 >> 8);
            msg->dwAddData2 = diskname[0];

            if (1)
            {
                int32_t     reg_storage_type;
                int32_t     reg_media_type;

                root_ctrl->root_para->multimedia_to_setting = EPDK_FALSE;
                root_ctrl->root_para->media_type            = 0xFF;
                __app_root_broadcast_msg(msg);

                {
                    char path[128];
                    ES_FILE *pfile = NULL;

                    eLIBs_strcpy(path, "f:\\update\\ePDKv100.img");
                    path[0]     = diskname[0];
                    pfile       = eLIBs_fopen(path, "rb");

                    if (pfile)
                    {
                        eLIBs_fclose(pfile);
                        dsk_start_fw_update(path);
                        return EPDK_OK;
                    }
                }				
                reg_storage_type    = __app_root_get_reg_storage_type(diskname[0]);
                reg_media_type      = -1;

                if (-1 != reg_storage_type)
                {
                    //if(root_ctrl->h_app_home)
                    {
                        __wrn("(%s)L%d:shortcut process ", __FILE__, __LINE__);
                        ret = app_root_shortcut_process(root_ctrl, app_root_prob_intent_plugin_for_play, reg_media_type, reg_storage_type, EPDK_TRUE, 1);

                        if (EPDK_OK == ret)
                        {
                        }
                    }
                }
            }
            else
            {
                __log("there are no last_lun");
            }
#ifdef CONFIG_DRIVERS_USB_GADGET_MTP
			usb_uevent_handle(MTP_STATE_ADD, (__u8)(msg->dwAddData2));
#endif
            return EPDK_OK;
        }
        case DSK_MSG_VOLTAGE_CHANGE:
        {
            __log("app_root_broadcast_msg, msg->dwAddData1=%d", msg->dwAddData1);
            __app_root_broadcast_msg(msg);
            return EPDK_OK;
        }

        case DSK_MSG_TVDAC_PLUGOUT:
        case DSK_MSG_HDMI_PLUGOUT:
        {
            __log("__app_root_broadcast_msg, msg->dwAddData1=%d", msg->dwAddData1);
            __app_root_broadcast_msg(msg);
            return EPDK_OK;
        }

        case APP_ROOT_MSG_DO_ASYNC:
        {
            p_app_root_fun pfun;

            __log("msg->dwAddData1=%x, msg->dwAddData2=%x", msg->dwAddData1, msg->dwAddData2);

            if (!msg->dwAddData1)
            {
                return EPDK_OK;
            }

            pfun    = (p_app_root_fun)(msg->dwAddData1);
            __log("pfun=%x, msg->dwAddData2=%x", pfun, msg->dwAddData2);

            pfun((void *)(msg->dwAddData2));
            return EPDK_OK;
        }
    }
    return GUI_ManWinDefaultProc(msg);
}



/* 创建根窗口 */
H_WIN app_root_wincreate(Activity *activity)
{
    H_WIN                       hManWin;
    __gui_manwincreate_para_t   create_info;

    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));

    create_info.attr            = activity;
    create_info.hParent         = activity_get_root_win(activity) ;
    create_info.hHosting        = NULL;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)app_root_win_proc);
    create_info.name            = APP_ROOT;
    hManWin                     = GUI_ManWinCreate(&create_info);
    __log("root win = %x", hManWin);

    if (hManWin == NULL)
    {
        __wrn("home: create movie windows failed!");
        return NULL;
    }

    return hManWin;
}
