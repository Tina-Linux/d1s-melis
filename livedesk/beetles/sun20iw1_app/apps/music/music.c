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
#include "music.h"
#include "music_player.h"
#include "music_ui.h"
#include "spctrm.h"
#include "MusicMenu/music_menu.h"

extern music_menu_ctrl_t        *g_music_menu_ctrl;
static H_WIN h_music_dialog = NULL;
H_WIN app_music_create(root_para_t  *para);
__s32 app_music_delete(void  *para);
static __s32 music_scene_create(__gui_msg_t *msg);                          //创建music场景
static __s32 _music_manager_win_cb(__gui_msg_t *msg);

__s32 music_menu_create_dialog(H_WIN Win, __s32 *lang);
__s32 music_menu_destory_dialog(void);

__s32 app_music_on_close(__gui_msg_t *msg);
__s32 app_music_on_destroy(__gui_msg_t *msg);


static H_LYR music_8bpp_layer_create(__u8 pipe);
static H_LYR music_32bpp_layer_create(__u8 pipe);

static __s32 music_layer_create(music_ctrl_t *music_ctrl);
static __s32 music_layer_delete(music_ctrl_t *music_ctrl);
static __s32 music_scene_delete(__gui_msg_t *msg);
static __s32 music_check_disk(__s32 *root_type);

/*
************************************************************************************************************************
*Function   :           __s32 app_music_create(root_para_t  *para)
*
*Description    :       创建MusicPlayer应用程序
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
H_WIN app_music_create(root_para_t  *para)
{
    H_WIN hwin;
    __gui_manwincreate_para_t create_info;
    music_ctrl_t *music_ctrl = NULL;
    __log("****************************************************************************************");
    __log("********  enter music player application  **************");
    __log("****************************************************************************************");
    g_disable_standby();
    //__wait__;
    //SetCurrentDevTypeEx(para->root_type);
    GUI_Memdev_FrameBuffer_Release(1);
    music_ctrl = (music_ctrl_t *)esMEMS_Malloc(0, sizeof(music_ctrl_t));                    //为私有属性申请内存

    if (music_ctrl == NULL)
    {
        __err("%s %d esMEMS_Malloc fail ", __FILE__, __LINE__);
        return NULL;
    }

    eLIBs_memset(music_ctrl, 0, sizeof(music_ctrl_t));
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    //    music_modify_icon_res(para->root_type);
    music_ctrl->font = para->font;
    music_ctrl->root_type = para->root_type;
    music_ctrl->mode = para->data;

    if (para->explorer_play_music != 0)
    {
        music_ctrl->explorer_play_music = para->explorer_play_music;
        music_ctrl->music_file_name = para->music_file_name;
    }

#if HT_1628_LED_EX
    SetFoucsWindowEx(APP_MUSIC_ID);
#endif
    create_info.name            = APP_MUSIC;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_music_manager_win_cb);
    create_info.attr            = (void *)music_ctrl;                                                       //设置Managerwin 私有属性
    create_info.id                          = APP_MUSIC_ID;
    create_info.hHosting        = NULL;
    hwin = GUI_ManWinCreate(&create_info);
                           // while(1)esKRNL_TimeDly(111);

    if (hwin)
    {
        reg_root_para_t *root_reg_para;
        __s32 reg_storage_type;
        reg_storage_type = 0;

        if (para)
        {
            if (RAT_TF == para->root_type)
            {
                SetCurrentDevTypeEx(RAT_TF);
                reg_storage_type = 0;
            }
            else if (RAT_USB == para->root_type)
            {
                SetCurrentDevTypeEx(RAT_USB);
                reg_storage_type = 1;
            }
            else
            {
                __wrn("para->root_para->root_type invalid...");
            }
        }

        root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

        if (root_reg_para)
        {
            __s32 reg_app_type;
            reg_app_type = 0;
            root_reg_para->cur_play_storage = reg_storage_type;
            root_reg_para->last_play_app = reg_app_type;
            root_reg_para->last_app_play_storage[reg_app_type] = reg_storage_type;
            root_reg_para->last_storage_play_app[reg_storage_type] = reg_app_type;
        }
        else
        {
            __wrn("para is null...");
        }
    }

    return hwin;
}

__s32 app_music_restore(H_WIN music_man_win, root_para_t *root_para)
{
    __s32 ret = 0;
    __u32 cur_time = 0;
    music_ctrl_t *music_ctrl = NULL;
    __s32 root_type = root_para->root_type;
    char cur_file_name[RAT_MAX_FULL_PATH_LEN];

    if (music_man_win == NULL)
    {
        return EPDK_FAIL;
    }

    __msg("++++++++++++app_music_restore++++++++++++++");
    music_modify_icon_res(root_para->root_type);

    if (root_para->explorer_play_music != 0)            //从文件管理进来
    {
        eLIBs_memset(cur_file_name, 0, sizeof(cur_file_name));  //判断是否与当前播放文件相同，相同则恢复后台播放
        robin_get_fsm_cur_file(cur_file_name, sizeof(cur_file_name));
        __msg("cur_file_name = %s", cur_file_name);
        __msg("root_para->music_file_name = %s", root_para->music_file_name);
        ret = eLIBs_strcmp(cur_file_name, root_para->music_file_name);

        if (ret != 0)
        {
            //__here__;
            return EPDK_FAIL;
        }
    }

    if (robin_get_fsm_status() == CEDAR_STAT_STOP)
    {
        //return EPDK_FAIL;
    }

    //__wait__;
    {
        //cur_time = robin_get_cur_time();
        //cur_time = robin_get_cur_time();
        //__msg("cur_time= d%", cur_time);
    }
    {
        //cur_time = robin_get_cur_time();
        //cur_time = robin_get_cur_time();
        //__msg("cur_time= d%", cur_time);
    }
    //GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
    {
        //cur_time = robin_get_cur_time();
        //cur_time = robin_get_cur_time();
    }
    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);
    __msg("music_ctrl=%d", music_ctrl);
    __msg("music_ctrl->root_type=%d", music_ctrl->root_type);

    if (music_ctrl->root_type != root_type) //如果换了盘符，则返回失败，主框架会重建一个music
    {
        return EPDK_FAIL;
    }

    music_ctrl->root_type = root_type;
    music_ctrl->explorer_play_music = root_para->explorer_play_music;
    music_ctrl->music_file_name = root_para->music_file_name;
    __msg("root_para->explorer_play_music=%d", root_para->explorer_play_music);
    {
        music_player_ctrl_t *music_player_ctrl;
        music_player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_ctrl->h_framewin);

        if (music_player_ctrl == NULL)
        {
            return EPDK_FAIL;
        }

        music_player_ctrl->explorer_play_music = music_ctrl->explorer_play_music;
        music_player_ctrl->music_file_name = music_ctrl->music_file_name;
    }
    GUI_WinSetFocusChild(music_man_win);
    __wrn("music_ctrl->mode=%d", music_ctrl->mode);

    if (music_ctrl->mode == MUSICPLAYER_BACKGROUND)
    {
        music_ctrl->mode =  MUSICPLAYER_NORMAL;
    }

    if (MUSICPLAYER_NORMAL == music_ctrl->mode)
    {
        gscene_bgd_set_state(BGD_STATUS_HIDE);
    }

    __wrn("music_ctrl->mode=%d", music_ctrl->mode);
    {
        //cur_time = robin_get_cur_time();
        //cur_time = robin_get_cur_time();
    }
    //先设为sleep，后面再打开
    ret = GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_SLEEP);
    //clear_music_list_area();
    __msg("--------------music player set layer status on, ret = %d", ret);
    {
        //cur_time = robin_get_cur_time();
        //cur_time = robin_get_cur_time();
    }
    //__wait__;
    music_player_restore(music_ctrl->h_framewin, music_ctrl->root_type);
    GUI_WinSetFocusChild(music_ctrl->h_framewin);       //frame win get focus.
    //ret = GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
    return ret;
}
static __s32 music_check_disk(__s32 *root_type)
{
    __s32 len = 0 ;
    char disk_name[RAT_MAX_PARTITION][4] = {{0}, {NULL}};

    if (root_type == NULL)
    {
        __wrn("root_type == NULL");
        return EPDK_FAIL;
    }

    eLIBs_memset(root_type, 0x00, sizeof(__s32) * 2);

    if ((rat_get_partition_name(RAT_SD_CARD, disk_name, 0) == EPDK_OK) && (eLIBs_strlen(disk_name[0]) != 0))
    {
        *root_type = EPDK_TRUE;// root_type[0]// sd card
    }
    else
    {
        *root_type = EPDK_FALSE;
    }

    eLIBs_memset(disk_name, 0x00, sizeof(disk_name));

    if ((rat_get_partition_name(RAT_USB_DISK, disk_name, 0) == EPDK_OK) && (eLIBs_strlen(disk_name[0]) != 0))
    {
        *(root_type + 1) = EPDK_TRUE;// root_type[1]// RAT_USB_DISK
    }
    else
    {
        *(root_type + 1) = EPDK_FALSE;
    }

    return EPDK_OK;
}

__s32 app_music_get_index(H_WIN music_man_win, __u32 *index)
{
    music_ctrl_t            *music_ctrl = NULL;
    music_player_ctrl_t *music_player_ctrl = NULL;

    if (music_man_win == NULL)
    {
        *index = 0;
        return EPDK_FAIL;
    }

    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

    if (music_ctrl == NULL)
    {
        *index = 0;
        return EPDK_FAIL;
    }

    music_player_ctrl = (music_player_ctrl_t *)GUI_WinGetAttr(music_ctrl->h_framewin);

    if (music_player_ctrl == NULL)
    {
        *index = 0;
        return EPDK_FAIL;
    }

    if (music_player_ctrl->list_ctrl != NULL)
    {
        *index = music_player_ctrl->list_ctrl->play_index;
    }
    else
    {
        *index = music_player_ctrl->play_index;
    }

    return music_ctrl->root_type;
}
/*
************************************************************************************************************************
*Function   :           __s32 app_music_delete(root_para_t  *para)
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 app_music_delete(void *para)
{
    return EPDK_OK;
}



__s32 app_music_on_key_proc(__gui_msg_t *msg)
{
    if ((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
                //  __msg("+++++++++GUI_MSG_KEY_RIGHT+++++++++");
                break;

            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
                __msg("+++++++++GUI_MSG_KEY_UP+++++++++");
                break;

            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
                __msg("+++++++++GUI_MSG_KEY_LEFT+++++++++");
                break;

            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
                __msg("+++++++++GUI_MSG_KEY_DOWN+++++++++");
                break;

            case GUI_MSG_KEY_VDEC:
                __msg("+++++++++GUI_MSG_KEY_VDEC+++++++++");
                break;

            case GUI_MSG_KEY_ENTER:
                __msg("+++++++++GUI_MSG_KEY_ENTER+++++++++");
                break;

            case GUI_MSG_KEY_MENU:
                __msg("+++++++++GUI_MSG_KEY_MENU+++++++++");
                break;

            case GUI_MSG_KEY_VADD:
                __msg("+++++++++GUI_MSG_KEY_VADD+++++++++");
                break;

            case GUI_MSG_KEY_ESCAPE:
                __msg("+++++++++GUI_MSG_KEY_ESCAPE+++++++++");
                break;

            case GUI_MSG_KEY_RISE:
                break;

            default:
                break;
        }
    }

    return EPDK_OK;
}


__s32 app_music_on_command(__gui_msg_t *msg)
{
    switch (msg->dwAddData1)
    {
        case MUSICPLAYER_BACKGROUND_HOME:
        case MUSICPLAYER_BACKGROUND:
            __msg("+++++++++MUSICPLAYER_BACKGROUND_HOME++++++++++++++");
            music_scene_delete(msg);
            break;
    }

    return EPDK_OK;
}

//发送一个命令参数给主界面
__s32 app_music_send_command(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128] = {0};
    H_WIN hDstWin;
    //H_WIN hManagerWin;
    //robin_set_cmd_stop();
    //hManagerWin = GUI_WinGetParent(hwin);
    hDstWin = GUI_WinGetParent(hwin);
    msg.h_deswin = hDstWin;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);//
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_WinGetName(msg.h_deswin, buf);
    __msg("%s", buf);
    GUI_SendNotifyMessage(&msg);
    //GUI_SendMessage(&msg);
    return EPDK_OK;
}

void music_send_to_player(H_WIN dstwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    msg.h_deswin = dstwin;
    msg.h_srcwin = NULL;
    msg.id = id;
    msg.dwAddData1 = data1;//
    msg.dwAddData2 = 0;
    msg.dwReserved = 0;
    GUI_SendNotifyMessage(&msg);
}



/*
************************************************************************************************************************
*Function   :           static __s32 _music_manager_win_cb(__gui_msg_t *msg)
*
*Description    :       manager window contrl block
*                   gets message from system, & dispatch command to child
*Arguments      :       get a message from system
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 _music_manager_win_cb(__gui_msg_t *msg)
{
    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            __log("*****before GUI_MSG_CREATE***");
            //dsk_amplifier_onoff(1);//打开功放,播放暂停处理较复杂，robin处理
            music_scene_create(msg);
            __wrn("*****after GUI_MSG_CREATE***");
            return EPDK_OK;

        case GUI_MSG_CLOSE:
            __log("----music manager window GUI_MSG_CLOSE begin----");
            app_music_on_close(msg);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
            __log("----music manager window GUI_MSG_DESTROY begin----");
            dsk_amplifier_onoff(0);//关闭功放,播放暂停处理较复杂，robin处理
            app_music_on_destroy(msg);
            {
                reg_root_para_t *root_reg_para;
                root_reg_para = (reg_root_para_t *)dsk_reg_get_para_by_app(REG_APP_ROOT);

                if (root_reg_para)
                {
                    root_reg_para->cur_play_storage = -1;
                }
                else
                {
                    __wrn("para is null...");
                }
            }
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
            __wrn("----music manager window GUI_MSG_DESTROY end----");
            return EPDK_OK;

        case GUI_MSG_KEY:
            //DE_INFO;
            app_music_on_key_proc(msg);
            break;

        case GUI_MSG_TOUCH:
            break;

        case GUI_MSG_COMMAND:
        {
            music_ctrl_t *music_ctrl = NULL;
            music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            switch (msg->dwAddData1)
            {
                case MUSICPLAYER_MUSIC_MENU_CREATE:
                {
                    __msg("+++++++++MUSICPLAYER_MUSIC_MENU_CREATE++++++++++++++");

                    if (music_ctrl->h_frm_menu == NULL)
                    {
                        music_ctrl->h_frm_menu = (H_WIN)music_menu_create(msg->h_deswin, music_ctrl->root_type, msg->dwAddData2, msg->dwReserved);
                        __msg("music_ctrl->h_frm_menu = %x", music_ctrl->h_frm_menu);

                        if (NULL != music_ctrl->h_frm_menu)
                        {
                            GUI_WinSetFocusChild(music_ctrl->h_frm_menu);
                        }
                        else
                        {
                            GUI_WinSetFocusChild(music_ctrl->h_framewin);
                        }
                    }
                    else
                    {
                        __wrn("the h_frm_menu has some err");
                        GUI_WinSetFocusChild(music_ctrl->h_framewin);
                    }

                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_MENU_DESTROY:
                {
                    music_menu_destory(music_ctrl->h_frm_menu);
                    music_ctrl->h_frm_menu = NULL;
                    GUI_WinSetFocusChild(music_ctrl->h_framewin);
                    music_send_to_player(music_ctrl->h_framewin, GUI_MSG_COMMAND, MUSIC_REBUILD_SPECTRUM, 1);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_PRE:
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    eLIBs_memcpy(&my_msg, msg, sizeof(__gui_msg_t));
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_PRE;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_NEXT:
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    eLIBs_memcpy(&my_msg, msg, sizeof(__gui_msg_t));
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_NEXT;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_MENU_EQ:
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    eLIBs_memcpy(&my_msg, msg, sizeof(__gui_msg_t));
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_EQ;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_MENU_PLAYMODE:
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    eLIBs_memcpy(&my_msg, msg, sizeof(__gui_msg_t));
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_PLAYMODE;
                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_SPCTRM_CREATE:
                {
                    __msg("music_ctrl = %x", music_ctrl);
                    __msg("music_ctrl.music_layer = %x", music_ctrl->music_layer);
                    //if (music_ctrl->h_spectrum_win != NULL)
                    {
                        if (!RegisterSpectrumControl())
                        {
                            __msg("!!!!!!!!register spectrum ocx error!!!");
                        }

                        __msg("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
                        music_ctrl->h_spectrum_win = SPCTRM_SceneCreate((H_WIN)(msg->h_deswin), (H_WIN)(music_ctrl->music_layer));

                        if (music_ctrl->h_spectrum_win == NULL)
                        {
                            __msg("creat spectrum frame win error!");
                        }
                    }
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_ARTIST_INFO:
                {
                    char cur_playing_file[RAT_MAX_FULL_PATH_LEN] = {0};
                    robin_get_fsm_cur_file(cur_playing_file, sizeof(cur_playing_file));
                    __wrn(" cur_playing_file=%s",   cur_playing_file);
                    return EPDK_OK;
                }

                case MUSICPLAYER_MUSIC_MENU_SWITCH_DISK:
                {
                    __gui_msg_t my_msg;
                    music_ctrl_t    *music_ctrl = NULL;
                    HBarState state = HBAR_ST_HIDE;
                    __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MUSIC_SWITCH_USB_TF_INFO};
                    __wrn("##############MUSICPLAYER_MUSIC_MENU_SWITCH_DISK###############");
                    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    eLIBs_memcpy(&my_msg, msg, sizeof(__gui_msg_t));
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_DESTROY;
                    my_msg.dwAddData2 = msg->dwAddData2;
                    GUI_SendNotifyMessage(&my_msg);
                    gscene_hbar_get_state(&state);

                    if (state == HBAR_ST_SHOW)
                    {
                        gscene_hbar_set_state(HBAR_ST_HIDE);
                    }

                    if (msg->dwAddData2 == RAT_NOT_FOUND_TF)
                    {
                        lang_id[1] = STRING_MUSIC_SWITCH_TF_CARD_FAIL;
                    }
                    else if (msg->dwAddData2 == RAT_NOT_FOUND_USB)
                    {
                        lang_id[1] = STRING_MUSIC_SWITCH_USB_DEVICE_FAIL;
                    }
                    else
                    {
                        lang_id[1] = STRING_MUSIC_SWITCH_USB_TF_INFO;
                    }

                    music_menu_create_dialog(msg->h_deswin, lang_id);

                    if (msg->dwAddData2 == RAT_NOT_FOUND_USB || msg->dwAddData2 == RAT_NOT_FOUND_TF)
                    {
                        esKRNL_TimeDly(200);
                        music_menu_destory_dialog();
                        return EPDK_FAIL;
                    }

                    __wrn("music_ctrl->root_type:%d", music_ctrl->root_type);
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    eLIBs_memcpy(&my_msg, msg, sizeof(__gui_msg_t));
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_SWITCH_DISK;
                    my_msg.dwReserved = msg->dwReserved;

                    if (msg->dwAddData2 == RAT_TF_SWITCH_TO_USB)
                    {
                        music_ctrl->root_type = RAT_USB;
                        my_msg.dwAddData2 = RAT_USB;
                    }
                    else if (msg->dwAddData2 == RAT_USB_SWITCH_TO_TF)
                    {
                        music_ctrl->root_type = RAT_TF;
                        my_msg.dwAddData2 = RAT_TF;
                    }
                    else
                    {
                        return EPDK_FAIL;
                    }

                    GUI_SendNotifyMessage(&my_msg);
                    return EPDK_OK;
                }
                break;

                default:
                {
                    app_music_on_command(msg);
                }
                break;
            }
        }

        case DSK_MSG_FS_PART_PLUGIN:
            break;

        case DSK_MSG_FS_PART_PLUGOUT:
        {
#if 0// shiql change 20160812
            char diskname[2] = {0};
#endif
            music_ctrl_t    *music_ctrl = NULL;
#if 0// shiql change 20160812
            music_ctrl   = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __msg("SD card plugout!");
            diskname[0] = (char)(msg->dwAddData2);
            diskname[1] = 0;
            __wrn("music_ctrl->root=%s", music_ctrl->root);
            __wrn("diskname=%s", diskname);

            if (0 == music_ctrl->root[0] //临界状态此时还没有获得盘符
                || 0 == eLIBs_strnicmp(music_ctrl->root, diskname, 1))
            {
                if (MUSICPLAYER_BACKGROUND == music_ctrl->mode)
                {
                    app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, CLOSE_BG_MUSIC, music_ctrl->mode);
                }
                else
                {
                    app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
                }
            }

#else
            __s32 root_type[2] = {0};
            music_ctrl   = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            music_check_disk(root_type);

            if (music_ctrl->root_type == RAT_USB)
            {
                if ((root_type[1] == EPDK_FALSE) && (root_type[0] == EPDK_TRUE))
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    my_msg.h_deswin = msg->h_deswin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_SWITCH_DISK;
                    my_msg.dwAddData2 = RAT_USB_SWITCH_TO_TF;
                    my_msg.dwReserved = DSK_MSG_FS_PART_PLUGOUT;
                    GUI_SendNotifyMessage(&my_msg);
                }
                else if ((root_type[1] == EPDK_FALSE) && (root_type[0] == EPDK_FALSE))
                {
                    if (MUSICPLAYER_BACKGROUND == music_ctrl->mode)
                    {
                        app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, CLOSE_BG_MUSIC, music_ctrl->mode);
                    }
                    else
                    {
                        app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
                    }
                }
            }
            else if (music_ctrl->root_type == RAT_TF)
            {
                if ((root_type[1] == EPDK_TRUE) && (root_type[0] == EPDK_FALSE))
                {
                    __gui_msg_t my_msg;
                    eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                    my_msg.h_deswin = msg->h_deswin;
                    my_msg.h_srcwin = NULL;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_MUSIC_MENU_SWITCH_DISK;
                    my_msg.dwAddData2 = RAT_TF_SWITCH_TO_USB;
                    my_msg.dwReserved = DSK_MSG_FS_PART_PLUGOUT;
                    GUI_SendNotifyMessage(&my_msg);
                }
                else if ((root_type[1] == EPDK_FALSE) && (root_type[0] == EPDK_FALSE))
                {
                    if (MUSICPLAYER_BACKGROUND == music_ctrl->mode)
                    {
                        app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, CLOSE_BG_MUSIC, music_ctrl->mode);
                    }
                    else
                    {
                        app_music_send_command(msg->h_deswin, GUI_MSG_COMMAND, SWITCH_TO_MMENU, 0);
                    }
                }
            }

#endif
        }

        return EPDK_OK;

        case DSK_MSG_HOME:
        {
            music_ctrl_t    *music_ctrl = NULL;
            music_ctrl   = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (MUSICPLAYER_BACKGROUND != music_ctrl->mode)
            {
                __gui_msg_t my_msg;

                if (robin_get_fsm_status() != CEDAR_STAT_PLAY)
                {
                    __wrn("music is not playing, kill music...");
                    //音乐停止,清理窗口
                    GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_SLEEP);
                    my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    my_msg.h_srcwin = msg->h_deswin;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(msg->h_deswin), GUI_MSG_COMMAND);//
                    my_msg.dwAddData2 = SWITCH_TO_MMENU;
                    my_msg.dwReserved = 0;
                }
                else
                {
                    __wrn("music is playing, let background...");
                    my_msg.h_deswin = music_ctrl->h_framewin;
                    my_msg.h_srcwin = msg->h_deswin;
                    my_msg.id = GUI_MSG_COMMAND;
                    my_msg.dwAddData1 = MUSICPLAYER_BACKGROUND_HOME;//发送消息给音乐AP要进入背景播放状态，释放场景
                }

                GUI_SendNotifyMessage(&my_msg);
            }
            else
            {
                if (robin_get_fsm_status() != CEDAR_STAT_PLAY)
                {
                    __gui_msg_t my_msg;
                    my_msg.h_deswin = msg->h_deswin;
                    my_msg.h_srcwin = msg->h_deswin;
                    my_msg.id = GUI_MSG_CLOSE;
                    GUI_SendNotifyMessage(&my_msg);
                }
            }

            return EPDK_OK;
        }

        case DSK_MSG_ALARM:
        {
            __gui_msg_t my_msg;
            music_ctrl_t    *music_ctrl = NULL;
            music_ctrl   = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            //robin_set_cmd_stop( ) ;   ///
            //robin_close( );           ///
            robin_cancle_ab_v2();
            music_ctrl->current_npl_index = robin_npl_get_cur();
            __music_save_breakpoint_info(music_ctrl->current_npl_index);

            if (MUSICPLAYER_BACKGROUND != music_ctrl->mode)
            {
                music_ctrl->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
            }

            my_msg.h_deswin = music_ctrl->h_framewin;
            my_msg.h_srcwin = msg->h_deswin;
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.dwAddData1 = MUSICPLAYER_MUSIC_PAUSE;//发送消息给音乐AP要进入背景播放状态，释放场景
            GUI_SendMessage(&my_msg);
            return EPDK_OK ;
        }

        case ALARM_MSG_CLOSE:
        {
            __gui_msg_t my_msg;
            music_ctrl_t    *music_ctrl = NULL;
            char    FileName[RAT_MAX_FULL_PATH_LEN] = {0};
            music_ctrl   = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (music_ctrl->h_frm_alarm_coming)
            {
                music_ctrl->h_frm_alarm_coming = NULL ;
            }

            robin_npl_index2file(music_ctrl->current_npl_index, FileName);
            {
                __s32 ret;
                __cedar_tag_inf_t tag_info;
                ret = __music_load_breakpoint_info(music_ctrl->current_npl_index, &tag_info);

                if (EPDK_OK == ret)
                {
                    ret = robin_play_file(FileName, &tag_info);
                }
                else
                {
                    ret = robin_play_file(FileName, NULL);
                }

                if (ret == EPDK_OK)
                {
                    __msg("index: %d", music_ctrl->current_npl_index);
                }
            }
            my_msg.h_deswin = music_ctrl->h_framewin;
            my_msg.h_srcwin = msg->h_deswin;
            my_msg.id = GUI_MSG_COMMAND;
            my_msg.dwAddData1 = MUSICPLAYER_MUSIC_PLAY;//发送消息给音乐AP要进入背景播放状态，释放场景
            GUI_SendMessage(&my_msg);
            robin_enable_ab();

            if (MUSICPLAYER_NORMAL == music_ctrl->mode)
            {
                GUI_WinSetFocusChild(music_ctrl->h_framewin);
            }

            return EPDK_OK ;
        }

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}

/*
************************************************************************************************************************
*Function   :           __s32 app_music_on_close(__gui_msg_t *msg)
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 app_music_on_close(__gui_msg_t *msg)
{
    //music_ctrl_t  *music_ctrl = NULL;
    //music_ctrl     = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    GUI_ManWinDelete(msg->h_deswin);    //it will send detroy message after call this function.
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :           __s32 app_music_on_destroy(__gui_msg_t *msg)
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 app_music_on_destroy(__gui_msg_t *msg)
{
    music_ctrl_t    *music_ctrl = NULL;
    music_ctrl   = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    g_enable_standby();

    if (g_music_menu_ctrl != NULL)
    {
        //__here__;
        esMEMS_Mfree(0, g_music_menu_ctrl);
        g_music_menu_ctrl = NULL;
    }

    if (music_ctrl->h_frm_alarm_coming)
    {
        app_alarm_frm_destory(music_ctrl->h_frm_alarm_coming);
        music_ctrl->h_frm_alarm_coming = NULL ;
    }

    if (music_ctrl->h_frm_menu)
    {
        music_menu_destory(music_ctrl->h_frm_menu);
        music_ctrl->h_frm_menu = NULL;
    }

    __msg("before music_layer_delete");
    music_layer_delete(music_ctrl);
    __msg("after music_layer_delete");
    //__here__;
    //if(music_ctrl->mode != MUSICPLAYER_BACKGROUND)
    {
        UnRegisterSpectrumControl();                    //delete spectrum class
    }
    //__here__;
    esMEMS_Mfree(0, music_ctrl);
    //__here__;
    gscene_hbar_set_music_state(EPDK_FALSE);
    return EPDK_OK;
}

static __s32  MusicGetSearchPath(__s32 root_type, char *path)
{
    __s32 ret = 0;
    char str_root[50];
    char disk_name[RAT_MAX_PARTITION][4];
    eLIBs_memset(str_root, 0, sizeof(str_root));
    eLIBs_memset(disk_name, 0, sizeof(disk_name));

    switch (root_type)
    {
        case RAT_LOCAL:
            eLIBs_strcpy(str_root, RAT_LOCAL_DISK);
            break;

        case RAT_TF:
            eLIBs_strcpy(str_root, RAT_SD_CARD);
            break;

        case RAT_USB:
            eLIBs_strcpy(str_root, RAT_USB_DISK);
            break;

        default:
            break;
    }

    __msg("root_type=%d", root_type);
    ret = rat_get_partition_name(str_root, disk_name, 0);
    eLIBs_strcpy(path, disk_name[0]);

    if (path[0])
    {
        return EPDK_OK;
    }
    else
    {
        return EPDK_FAIL;
    }
}


/*
************************************************************************************************************************
*Function   :           static __s32 music_scene_create(__gui_msg_t *msg)
*
*Description    :       创建音乐播放器场景，ListBar
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/static __s32 __music_set_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}

static __s32 music_scene_create(__gui_msg_t *msg)
{
    music_ctrl_t    *music_ctrl = NULL;
    __s32 ret;
    music_player_ctrl_t player_para;
    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    eLIBs_memset(&player_para, 0, sizeof(music_player_ctrl_t));
    player_para.root_type = music_ctrl->root_type;
    MusicGetSearchPath(music_ctrl->root_type, music_ctrl->root);
    //player_para.media_type =music_ctrl->media_type;
    player_para.media_type = RAT_MEDIA_TYPE_AUDIO;
    //player_para.view_mode = music_ctrl->view_mode;        //view mode,
    player_para.font = SWFFont;
    player_para.AB_mode = MUSIC_AB;
    __wrn("music_ctrl->mode=%d", music_ctrl->mode);

    switch (music_ctrl->mode)
    {
        case MUSICPLAYER_NORMAL:
            gscene_bgd_set_state(BGD_STATUS_HIDE);
            player_para.visiable = EPDK_TRUE;
            break;

        case MUSICPLAYER_BACKGROUND:
            player_para.visiable = EPDK_FALSE;
            break;

        default:
            player_para.visiable = EPDK_TRUE;
            break;
    }

    //app_print_memory_usage("music scene create", NULL);
    //player_para.view_mode = music_ctrl->view_mode;        //view mode,
    ret = music_layer_create(music_ctrl);                               //创建图层

    if (ret == EPDK_FAIL)
    {
        __wrn("create layer for music player failed!");
        return EPDK_FAIL;
    }

    //app_print_memory_usage("music layer create finished", NULL);
    //app_print_memory_usage("spectrum scene create finished!", NULL);
    //player_para.background_layer =    music_ctrl->background_layer;
    player_para.music_layer =   music_ctrl->music_layer;
    player_para.explorer_play_music = music_ctrl->explorer_play_music;
    player_para.music_file_name = music_ctrl->music_file_name;
    music_ctrl->h_framewin = music_player_win_create(msg->h_deswin, &player_para);
    __wrn("music_ctrl->h_framewin=%x", music_ctrl->h_framewin);

    if (MUSICPLAYER_NORMAL == music_ctrl->mode)
    {
        GUI_WinSetFocusChild(music_ctrl->h_framewin);       //frame win get focus.                  //set picture layer as top level
    }

    if (player_para.visiable)
    {
        GUI_LyrWinCacheOn();
        //esKRNL_TimeDly(20);
        GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_ON);
        GUI_LyrWinSetTop(music_ctrl->music_layer);
        //gscene_hbar_set_state(HBAR_ST_SHOW);
        GUI_LyrWinFlushFB(music_ctrl->music_layer);
        GUI_LyrWinCacheOff();
    }
    else
    {
        GUI_LyrWinSetSta(music_ctrl->music_layer, GUI_LYRWIN_STA_SLEEP);
        __wrn("music layer is off for background play mode");
    }

#if 0
    //if(player_para.visiable)
    {
        if (!RegisterSpectrumControl())
        {
            __msg("!!!!!!!!register spectrum ocx error!!!");
        }

        __msg("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
        music_ctrl->h_spectrum_win = SPCTRM_SceneCreate((H_WIN)(msg->h_deswin), (H_WIN)(music_ctrl->music_layer));

        if (music_ctrl->h_spectrum_win == NULL)
        {
            __msg("creat spectrum frame win error!");
        }
    }
#endif
    gscene_hbar_set_music_state(EPDK_TRUE);
    __music_set_title(STRING_MUSIC_TITLE);
    gscene_hbar_set_state(HBAR_ST_SHOW);
    return EPDK_OK;
}


static __s32 music_scene_delete(__gui_msg_t *msg)
{
    music_ctrl_t    *This = NULL;
    This = (music_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    __wrn("*******music_scene_delete********");

    if (This == NULL)
    {
        return EPDK_FAIL;
    }

    if (This->music_layer == NULL)
    {
        return EPDK_FAIL;
    }

    if (This->h_frm_menu)    //删除菜单
    {
        music_menu_destory(This->h_frm_menu);
        This->h_frm_menu = NULL;
    }

    This->mode = MUSICPLAYER_BACKGROUND;

    if (This->h_spectrum_win != NULL)
    {
        //robin_disable_spectrum();
        //GUI_FrmWinDelete(This->h_spectrum_win);               //删除频谱图窗口
        //UnRegisterSpectrumControl();
        //GUI_FrmWinDelete(This->h_spectrum_win);
        //This->h_spectrum_win = NULL;
    }

    GUI_Clear();
    GUI_LyrWinSetSta(This->music_layer, GUI_LYRWIN_STA_SUSPEND);
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
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :          static H_LYR music_8bpp_layer_create(void)
*
*Description    :       背景图层，主要用来显示8bpp 背景图片
*                   注意与前景图应处于不同的pipe
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/

static H_LYR music_8bpp_layer_create(__u8 pipe)
{
    H_LYR layer = NULL;
    RECT LayerRect;
    //int ret;
    //__layerwincreate_para_t lyrcreate_info;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        //{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        //{FB_TYPE_RGB, {PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */
    };
    __disp_layer_para_t lstlyr =
    {
        MOD_DISP_LAYER_WORK_MODE_NORMAL,                    /* mode      */
        //DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        0,                                              /* alpha_val */
        0,                                              /* pipe      */
        8,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_MUSIC",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    music_get_screen_rect(&LayerRect);
    fb.size.width       = LayerRect.width;
    fb.size.height      = LayerRect.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = LayerRect.x;
    lstlyr.src_win.y        = LayerRect.y;
    lstlyr.src_win.width    = LayerRect.width;
    lstlyr.src_win.height   = LayerRect.height;
    lstlyr.scn_win.x        = LayerRect.x;
    lstlyr.scn_win.y        = LayerRect.y;
    lstlyr.scn_win.width    = LayerRect.width;
    lstlyr.scn_win.height   = LayerRect.height;
    lstlyr.pipe = pipe;     //0;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("music layer create error !");
    }

    return layer;
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
static H_LYR music_32bpp_layer_create(__u8 pipe)
{
    H_LYR layer = NULL;
    RECT LayerRect;
    //  int ret;
    //__layerwincreate_para_t lyrcreate_info;
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
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_MUSIC",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    music_get_screen_rect(&LayerRect);
    fb.size.width       = LayerRect.width;
    fb.size.height      = LayerRect.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;
    lstlyr.src_win.x        = LayerRect.x;
    lstlyr.src_win.y        = LayerRect.y;
    lstlyr.src_win.width    = LayerRect.width;
    lstlyr.src_win.height   = LayerRect.height;
    lstlyr.scn_win.x        = LayerRect.x;
    lstlyr.scn_win.y        = LayerRect.y;
    lstlyr.scn_win.width    = LayerRect.width;
    lstlyr.scn_win.height   = LayerRect.height;
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

/*
************************************************************************************************************************
*Function   :           static __s32 music_layer_create(music_ctrl_t *music_ctrl)
*
*Description    :           创建两个图层
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 music_layer_create(music_ctrl_t *music_ctrl)
{
    /*
    if(music_ctrl->spectrum_layer == NULL)
    {
        music_ctrl->spectrum_layer = spectrum_32bpp_layer_create(1);
        if(music_ctrl->spectrum_layer == NULL)
        {
            __err("spectrum layer create error !");
            return EPDK_FAIL;
        }
    }
    */
    if (music_ctrl->music_layer == NULL)
    {
        music_ctrl->music_layer = music_32bpp_layer_create(1);

        if (music_ctrl->music_layer == NULL)
        {
            __err("music layer create error !");
            return EPDK_FAIL;
        }
    }
    else
    {
        __err("music layer create error !");
    }

    /*
    if(music_ctrl->background_layer == NULL)
    {
            music_ctrl->background_layer = music_8bpp_layer_create(0);
            if(music_ctrl->background_layer == NULL)
            {
                __err("music layer create error !");
                return EPDK_FAIL;
            }
    }
    else
    {
        __err("music layer create error !");
        return EPDK_FAIL;
    }
    */
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :           static __s32 music_layer_delete(H_LYR layer)
*
*Description    :       删除图层
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 music_layer_delete(music_ctrl_t *music_ctrl)
{
    if (music_ctrl->music_layer != NULL)
    {
        GUI_LyrWinDelete(music_ctrl->music_layer);
    }

    return EPDK_OK;
}

__s32 app_music_get_root_type(H_WIN music_man_win, unsigned long *root_type)
{
    music_ctrl_t            *music_ctrl = NULL;
    music_player_ctrl_t *music_player_ctrl = NULL;

    if (music_man_win == NULL)
    {
        return EPDK_FAIL;
    }

    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

    if (music_ctrl == NULL)
    {
        return EPDK_FAIL;
    }

    if (root_type)
    {
        *root_type = music_ctrl->root_type;
    }

    return EPDK_OK;
}

__s32 app_music_get_mode(H_WIN music_man_win)
{
    music_ctrl_t            *music_ctrl = NULL;
    musicplayer_mode_e   mode;
    mode = -1;

    if (music_man_win == NULL)
    {
        goto end;
    }

    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

    if (music_ctrl == NULL)
    {
        goto end;
    }

    mode = music_ctrl->mode;
end:
    ;
    return mode;
}

__s32 app_music_set_bg_mode(H_WIN music_man_win)
{
    __s32 ret;
    music_ctrl_t            *music_ctrl = NULL;
    __gui_msg_t my_msg;

    if (music_man_win == NULL)
    {
        goto fail;
    }

    music_ctrl = (music_ctrl_t *)GUI_WinGetAttr(music_man_win);

    if (music_ctrl == NULL)
    {
        goto fail;
    }

    if (MUSICPLAYER_NORMAL != music_ctrl->mode)
    {
        __wrn("music is not in background mode...");
        goto fail;
    }

    if (NULL == music_ctrl->h_framewin)
    {
        goto fail;
    }

    __wrn("music_ctrl->h_framewin=%x", music_ctrl->h_framewin);
    ret = music_player_scene_delete(music_ctrl->h_framewin);

    if (EPDK_FAIL == ret)
    {
        goto fail;
    }

    eLIBs_memset(&my_msg, 0, sizeof(__gui_msg_t));
    my_msg.h_deswin = music_man_win;
    ret = music_scene_delete(&my_msg);
    __wrn("set music bg msg ok...");
    return EPDK_OK;
fail:
    ;
    __wrn("set music bg msg fail...");
    return EPDK_FAIL;
}
__s32 music_menu_create_dialog(H_WIN Win, __s32 *lang)
{
    __wrn("music_menu_create_dialog strat:");

    if (h_music_dialog == NULL && Win != NULL && lang != NULL)
    {
        default_dialog(h_music_dialog, Win, 200, ADLG_OK, lang);
    }

    __wrn("music_menu_create_dialog end:");
    return 0;
}
__s32 music_menu_destory_dialog(void)
{
    __wrn("music_menu_destory_dialog strat:");
    app_dialog_destroy(h_music_dialog);
    gscene_hbar_set_state(HBAR_ST_SHOW);
    h_music_dialog = NULL;
    __wrn("music_menu_destory_dialog end:");
    return 0;
}
