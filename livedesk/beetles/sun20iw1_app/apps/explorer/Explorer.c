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

#include "Explorer.h"
#include "Explorer_List.h"
#include "Explorer_UI.h"
#include "explorer_dialog_frame.h"

typedef enum
{
    EXP_MEDIA_TYPE_ALL,             //所有媒体类型
    EXP_MEDIA_TYPE_PIC,             //所有图片文件
    EXP_MEDIA_TYPE_AUDIO,           //所有音频文件
    EXP_MEDIA_TYPE_VIDEO,           //所有视频文件
    EXP_MEDIA_TYPE_EBOOK,           //所有电子书文件
    EXP_MEDIA_TYPE_GAME,            //所有游戏文件
    EXP_MEDIA_TYPE_FIRMWARE,        //所有固件文件
    EXP_MEDIA_TYPE_UNKNOWN,         //所有系统不支持的文件
} explorer_media_type_t;


typedef struct tag_explorer_ctrl
{
    H_WIN h_explr;
    H_LYR BG_layer;         //background layer 8bpp(注意背景图需要设置为8bpp)
    H_LYR list_layer;       //layer for listbar, 32bpp(32bpp,因为listbar缩略图为32bpp)
    H_WIN h_list_framewin;
    H_WIN h_frm_alarm_coming ;  //闹钟定时时间到的时候弹出的对话框的窗口句柄

    H_LYR dialog_layer;
    H_WIN h_dialog_framewin;

    __s32 root_type;                    //USB Device, or  SDCard
    __u32 last_start_id;                //上次浏览起始ID
    __u32 last_focused_id;              //上次选中文件id
    char  *last_filename;               //上次浏览文件 名

    char root[4];                          //???????÷μ??ì·?
    GUI_FONT *explr_font;
    root_para_t  *root_para ;

    __s32 mediatype_index;
    rat_media_type_t media_type;
    explorer_view_mode_e view_mode;     //??ê??￡ê?
} explorer_ctrl_t;

//----------------------------------------------------------------------------------------------------
//define the global variables
//static explorer_param_t *explorer_param = NULL;


//private function
static __s32 _explorer_manager_win_cb(__gui_msg_t *msg);//process the  message of the manager window from system

static __s32 explorer_scene_create(__gui_msg_t *msg);

static H_LYR explorer_8bpp_layer_create(void);
static H_LYR explorer_32bpp_layer_create(__s32 pipe);
//static H_LYR explorer_layer_create(void);
static __s32 explorer_layer_delete(H_LYR layer);

static __s32 app_explorer_on_close(__gui_msg_t *msg);
static __s32 app_explorer_on_destroy(H_WIN h_exp_win);
static H_WIN h_explorer_dialog = NULL;

__s32 __explorer_set_title(__s32 id)
{
    char title[32] = {0};
    dsk_langres_get_menu_text(id, title, sizeof(title));
    gscene_hbar_set_title(title, sizeof(title));
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :           static __s32 explorer_alarm_scene_create()
*Arguments      :
*Return         :
************************************************************************************************************************
*/

static __s32 explorer_alarm_scene_create(__gui_msg_t *msg)
{
    explorer_ctrl_t *explr_ctrl;
    explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!explr_ctrl)
    {
        __msg("explorer main win get win attr fail");
        return EPDK_FAIL;
    }

    /* 发送消息到explorer list 窗口来销毁当前弹出的对话框，以防图层不够*/
    {
        __gui_msg_t sendmsg ;
        sendmsg.h_deswin = explr_ctrl->h_list_framewin ;
        sendmsg.id = msg->id ;
        GUI_SendMessage(&sendmsg);
    }
    explr_ctrl->h_frm_alarm_coming = app_alarm_frm_create(msg->h_deswin);
    return EPDK_OK;
}

static __s32 explorer_alarm_scene_delete(__gui_msg_t *msg)
{
    explorer_ctrl_t *explr_ctrl;
    explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!explr_ctrl)
    {
        __msg("explorer main win get win attr fail");
        return EPDK_FAIL;
    }

    if (explr_ctrl->h_frm_alarm_coming)
    {
        explr_ctrl->h_frm_alarm_coming = NULL ;
    }

    return EPDK_OK;
}
signed long app_explorer_create(root_para_t  *para)
{
    __s32 disk_num = 0 ;    // ′??ì×üêy
    __s32 ret =  EPDK_FALSE ;
    explorer_ctrl_t *explorer_ctrl = NULL ;
    __gui_manwincreate_para_t create_info;
    __log("****************************************************************************************");
    __log("********  enter explorer app  **************");
    __log("****************************************************************************************");
    gscene_bgd_set_state(BGD_STATUS_SHOW);
    __log("disk_num:%d", disk_num);
    ret = explorer_check_disk_exist(para->root_type);   // 判断给定的盘符是否存在

    if (EPDK_TRUE != ret)
    {
        __wrn("explorer_check_disk_exist fail ");
        para->return_to_explorer_file_list = 0 ;        //给定盘符不存在，不用进入的搜索链表中，进入盘符选择窗口
    }

    __log("para->root_type = %d", para->root_type);
    explorer_ctrl = (explorer_ctrl_t *)esMEMS_Malloc(0, sizeof(explorer_ctrl_t));       //为私有属性申请内存

    if (explorer_ctrl == NULL)
    {
        __err("explorer_ctrl malloc fail");
        return NULL;
    }

    eLIBs_memset(explorer_ctrl, 0x00, sizeof(explorer_ctrl_t));
    //root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);
    explorer_ctrl->root_para = para ;
    explorer_ctrl->explr_font = para->font;
    __inf("para->data = %d,para->root_type = %d", para->data, para->root_type);

    switch (para->data)
    {
        case ID_EXPLORER_ALL:       //文件管理
        {
            explorer_ctrl->view_mode = EXPLR_LIST_MODE;
            explorer_ctrl->media_type = RAT_MEDIA_TYPE_ALL;
            explorer_ctrl->root_type = para->root_type;
            explorer_ctrl->mediatype_index = ID_MEDIATYPE_ALL_ITEM;
            //explr_ctrl->last_start_id = 0;
            //explr_ctrl->last_focused_id = 0;
            //explr_ctrl->last_filename = NULL;
        }
        break;

        case ID_EXPLORER_MOVIE:
        {
            __wrn("set movie explorer para ");
            explorer_ctrl->view_mode = EXPLR_LIST_MODE;
            explorer_ctrl->media_type = RAT_MEDIA_TYPE_VIDEO;
            explorer_ctrl->root_type = para->root_type;
            explorer_ctrl->mediatype_index = ID_MEDIATYPE_MOVIE_ITEM;
            /*
            if(para->root_type == RAT_TF)
            {
                explr_ctrl->last_start_id = root_para->last_movie_start_index_sd;
                explr_ctrl->last_focused_id = root_para->last_movie_index_sd;
                explr_ctrl->last_filename = root_para->last_movie_path_sd;
            }
            else if(para->root_type == RAT_USB)
            {
                explr_ctrl->last_start_id = root_para->last_movie_start_index_ud;
                explr_ctrl->last_focused_id = root_para->last_movie_index_ud;
                explr_ctrl->last_filename = root_para->last_movie_path_ud;
            }
            */
        }
        break;

        case ID_EXPLORER_PHOTO:
        {
                __log("grgasdb....ID_EXPLORER_PHOTO");
            explorer_ctrl->view_mode = EXPLR_LIST_MODE;     //EXPLR_SQUARE_MODE;
            explorer_ctrl->media_type = RAT_MEDIA_TYPE_PIC;
            explorer_ctrl->root_type = para->root_type;
            explorer_ctrl->mediatype_index = ID_MEDIATYPE_PHOTO_ITEM;
            /*
            if(para->root_type == RAT_TF)
            {
                explr_ctrl->last_start_id = root_para->last_photo_start_index_sd;
                explr_ctrl->last_focused_id = root_para->last_photo_index_sd;
                explr_ctrl->last_filename = root_para->last_photo_path_sd;
            }
            else if(para->root_type == RAT_USB)
            {
                explr_ctrl->last_start_id = root_para->last_photo_start_index_ud;
                explr_ctrl->last_focused_id = root_para->last_photo_index_ud;
                explr_ctrl->last_filename = root_para->last_photo_path_ud;
            }
            */
        }
        break;

        case ID_EXPLORER_EBOOK:
        {
            explorer_ctrl->view_mode = EXPLR_LIST_MODE;
            explorer_ctrl->media_type = RAT_MEDIA_TYPE_EBOOK;
            explorer_ctrl->root_type = para->root_type;
            explorer_ctrl->mediatype_index = ID_MEDIATYPE_EBOOK_ITEM;
            /*
            if(para->root_type == RAT_TF)
            {
                explr_ctrl->last_start_id = root_para->last_ebook_start_index_sd;
                explr_ctrl->last_focused_id = root_para->last_ebook_index_sd;
                explr_ctrl->last_filename = root_para->last_ebook_path_sd;
            }
            else if(para->root_type == RAT_USB)
            {
                explr_ctrl->last_start_id = root_para->last_ebook_start_index_ud;
                explr_ctrl->last_focused_id = root_para->last_ebook_index_ud;
                explr_ctrl->last_filename = root_para->last_ebook_path_ud;
            }
            */
        }
        break;

        case ID_EXPLORER_MUSIC:
        {
            explorer_ctrl->view_mode = EXPLR_LIST_MODE;
            explorer_ctrl->media_type = RAT_MEDIA_TYPE_AUDIO;
            explorer_ctrl->root_type = para->root_type;
            explorer_ctrl->mediatype_index = ID_MEDIATYPE_MUSIC_ITEM;
            /*
            if(para->root_type == RAT_TF)
            {
                explr_ctrl->last_start_id = root_para->last_music_start_index_sd;
                explr_ctrl->last_focused_id = root_para->last_music_index_sd;
                explr_ctrl->last_filename = root_para->last_music_path_sd;
            }
            else if(para->root_type == RAT_USB)
            {
                explr_ctrl->last_start_id = root_para->last_music_start_index_ud;
                explr_ctrl->last_focused_id = root_para->last_music_index_ud;
                explr_ctrl->last_filename = root_para->last_music_path_ud;
            }
            */
        }
        break;

        case ID_EXPLORER_RESTORE:
        {
            explorer_ctrl->media_type = RAT_MEDIA_TYPE_UNKNOWN;
        }
        break;

        default:
            break;
    }

    __log("explorer_ctrl->mediatype_index = %d ......", explorer_ctrl->mediatype_index);
    eLIBs_memset(&create_info, 0, sizeof(__gui_manwincreate_para_t));
    create_info.name            = APP_EXPLORER;
    create_info.hParent         = para->h_parent;
    create_info.ManWindowProc   = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_explorer_manager_win_cb);
    create_info.attr            = (void *)explorer_ctrl;            //设置Managerwin 私有属性
    create_info.id              = APP_EXPLORER_ID;
    create_info.hHosting        = NULL;
    __wrn("explr_ctrl->list_layer =%08x,explr_ctrl->h_list_framewin=%08x", explorer_ctrl->list_layer, explorer_ctrl->h_list_framewin);
    return ((signed long)GUI_ManWinCreate(&create_info));
}

/*
************************************************************************************************************************
*Function   :           __s32 app_explorer_on_close(H_WIN h_exp_win)
*
*Description    :       设置EXPLORER之后的AP, 保存设置,释放窗口
*                   (释放窗口函数之后将响应destroy消息)
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 app_explorer_on_close(__gui_msg_t *msg)
{
    explorer_ctrl_t *explr_ctrl;
    explr_ctrl   = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    //save_setting(explr_ctrl);
    GUI_ManWinDelete(msg->h_deswin);                //it will send detroy message after call this function.
    //cat_item = (__cat_item_t *)msg->dwAddData1;
    //if (cat_item == 0)
    {
        //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
    }
    //else
    {
        switch (LOWORD(msg->dwAddData2))
        {
            case EXP_MEDIA_TYPE_VIDEO:
                //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MOVIE, HIWORD(msg->dwAddData2));
                break;

            case EXP_MEDIA_TYPE_PIC:
                //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_PHOTO, HIWORD(msg->dwAddData2));
                break;

            case EXP_MEDIA_TYPE_EBOOK:
                //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_EBOOK, HIWORD(msg->dwAddData2));
                break;

            case EXP_MEDIA_TYPE_AUDIO:
                //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MUSIC, HIWORD(msg->dwAddData2));
                break;

            case EXP_MEDIA_TYPE_GAME:
                //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_GAME, HIWORD(msg->dwAddData2));
                break;

            default:
                break;
        }
    }
    return EPDK_OK;
}
/*
************************************************************************************************************************
*Function   :           __s32 app_explorer_on_destroy(H_WIN h_exp_win)
*
*Description    :       释放图层，释放内存
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 app_explorer_on_destroy(H_WIN h_exp_win)
{
    explorer_ctrl_t *explr_ctrl;
    explr_ctrl   = (explorer_ctrl_t *)GUI_WinGetAttr(h_exp_win);

    if (explr_ctrl->h_frm_alarm_coming)
    {
        app_alarm_frm_destory(explr_ctrl->h_frm_alarm_coming);
        explr_ctrl->h_frm_alarm_coming = NULL ;
    }

    explorer_layer_delete(explr_ctrl->list_layer);

    if (explr_ctrl->BG_layer)
    {
        GUI_LyrWinSetTop(explr_ctrl->BG_layer);
    }

    //gscene_bgd_set_state(BGD_STATUS_SHOW); //chengf  delete
    if (explr_ctrl->BG_layer)
    {
        explorer_layer_delete(explr_ctrl->BG_layer);
        explr_ctrl->BG_layer = NULL;
    }

    if (explr_ctrl->dialog_layer)
    {
        GUI_LyrWinDelete(explr_ctrl->dialog_layer);
        explr_ctrl->dialog_layer = NULL;
    }

    esMEMS_Mfree(0, explr_ctrl);

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :           static __s32 app_explorer_on_command(__gui_msg_t *msg)
*
*Description    :       用户自定义命令处理函数
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 app_explorer_on_command(__gui_msg_t *msg)
{
    __gui_msg_t my_msg;
    explorer_ctrl_t *explr_ctrl;
    explr_ctrl   = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (NULL != explr_ctrl->dialog_layer)
    {
        GUI_LyrWinDelete(explr_ctrl->dialog_layer);
        explr_ctrl->dialog_layer = NULL;
        explr_ctrl->h_dialog_framewin = NULL;
    }

    if (LOWORD(msg->dwAddData1) == DELETE_FILE_DIALOG_ID
        || LOWORD(msg->dwAddData1) == FW_UPDATE_DIALOG_ID
        || ((EXPLORER_DIALOG_COPY <= msg->dwAddData1) && (EXPLORER_DIALOG_DEL >= msg->dwAddData1)))
    {
        if (explr_ctrl->h_list_framewin == NULL)
        {
            return EPDK_OK;
        }

        my_msg.h_deswin = explr_ctrl->h_list_framewin;
        my_msg.h_srcwin = msg->h_deswin;
        my_msg.id = GUI_MSG_COMMAND;
        my_msg.dwAddData1 = msg->dwAddData1;//
        my_msg.dwAddData2 = msg->dwAddData2;
        my_msg.dwReserved = msg->dwReserved;
        //GUI_SendNotifyMessage(msg);
        GUI_SendMessage(&my_msg);
    }
    else if (msg->dwAddData1 == EXPLORER_DIALOG_CREATE)
    {
        return explorer_dialog_create(msg);
    }

    //else if(msg->dwAddData1 == EXPLORER_DIALOG_CANCEL)
    {
    }
    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :          void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
*
*Description    :       向父窗口传送命令消息
*
*Arguments      :       hwin, 主窗口句柄
*                   id,     命令参数
*Return         :
*
************************************************************************************************************************
*/

void  app_explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128] = {0};
    //H_WIN hExpWin;
    //hExpWin = GUI_WinGetParent(hwin);
    msg.h_deswin = GUI_WinGetParent(hwin);
    //msg.h_srcwin = NULL;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hwin), id);
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_WinGetName(msg.h_deswin, buf);
    __msg("%s", buf);
    GUI_SendNotifyMessage(&msg);
}

static __s32  ExplorerGetSearchPath(__s32 root_type, char *path)
{
    __s32 ret = 0;
    char str_root[50] = {0};
    char disk_name[RAT_MAX_PARTITION][4] = {{0},};
    ASSERT(path != NULL);

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

    __inf("root_type=%d", root_type);
    ret = rat_get_partition_name(str_root, disk_name, 0);
    eLIBs_strcpy(path, disk_name[0]);
    __msg("disk_name[0]=%d", disk_name[0]);

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
*Function   :           static __s32 _explorer_manager_win_cb(__gui_msg_t *msg)
*
*Description    :       manager window contrl block
*                   gets message from system, & dispatch command to child
*Arguments      :       get a message from system
*
*Return         :
*
************************************************************************************************************************
*/
static __s32 _explorer_manager_win_cb(__gui_msg_t *msg)
{
    __inf("Enter %s", __FUNCTION__);

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
        {
            __log("----explorer manager window GUI_MSG_CREATE begin----");
            explorer_scene_create(msg);
            //com_set_backcolor(0xffa0a0a0);
            __inf("----explorer manager window GUI_MSG_CREATE end----");
            //while(1)esKRNL_TimeDly(111);
        }

        return EPDK_OK;

        case DSK_MSG_ALARM: // 闹钟到来消息
        {
            explorer_alarm_scene_create(msg);
        }

        return EPDK_OK ;

        case ALARM_MSG_CLOSE:   // 闹钟退出消息
        {
            explorer_alarm_scene_delete(msg);
        }

        return EPDK_OK ;

        case DSK_MSG_HOME:
        {
            __log("----explorer manager window DSK_MSG_HOME begin----");            
            app_explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        }

        return EPDK_OK;

        case GUI_MSG_CLOSE:
        {
            __log("----explorer manager window GUI_MSG_CLOSE begin----");
            app_explorer_on_close(msg);
            __msg("----explorer manager window GUI_MSG_CLOSE end----");
        }

        return EPDK_OK;

        case GUI_MSG_DESTROY:
        {
            __log("----explorer manager window GUI_MSG_DESTROY begin----");
            app_explorer_on_destroy(msg->h_deswin);         //release  layer,memory
            //com_set_backcolor(0xff000000);
            __msg("----explorer manager window GUI_MSG_DESTROY end----");
        }

        return EPDK_OK;

        case GUI_MSG_COMMAND:
        {
            __log("----explorer manager window GUI_MSG_CLOSE begin----");
            app_explorer_on_command(msg);
            {
                /*
                if (LOWORD(msg->dwAddData1) == EXPLR_LIST_ID)
                {
                    //explorer_list_command_proc(msg);
                }
                else if (LOWORD(msg->dwAddData1) == EXPLR_SQUARE_ID)
                {

                }
                */
            }
        } return EPDK_OK;

        case DSK_MSG_FS_PART_PLUGIN:
        {
#if 0
            explorer_ctrl_t *explr_ctrl = NULL;
            explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
            __msg("----explorer manager window DSK_MSG_FS_PART_PLUGIN begin----");

            if (explr_ctrl->h_list_framewin)    //将消息发往explorer_list win
            {
                __gui_msg_t  sendmsg ;
                eLIBs_memcpy(&sendmsg, msg, sizeof(__gui_msg_t)) ;
                sendmsg.h_deswin = explr_ctrl->h_list_framewin ;
                GUI_SendMessage(&sendmsg) ;
            }

            __msg("----explorer manager window DSK_MSG_FS_PART_PLUGIN end----");
#endif
            return EPDK_OK;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __u32 total = 0;
            __s32 len = 0 ;
            __s32 ret  = EPDK_FAIL ;
            explorer_ctrl_t *explr_ctrl = NULL;
            __s32 current_disk = 0, other_disk = 0;
            __u8  current_disk_is_exist = EPDK_FALSE, other_disk_is_exist = EPDK_FALSE;
            explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

            if (explr_ctrl->h_list_framewin == NULL)
            {
                __gui_msg_t my_msg;
                __wrn("explr_ctrl->h_list_framewin == NULL");
                eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                my_msg.h_deswin = msg->h_deswin;
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.id = DSK_MSG_HOME;
                my_msg.dwAddData1 = 0;
                my_msg.dwAddData2 = 0;
                my_msg.dwReserved = NULL;
                GUI_SendNotifyMessage(&my_msg);
                return EPDK_OK;
            }

            current_disk = explr_ctrl->root_type;

            if (current_disk == RAT_TF)
            {
                other_disk = RAT_USB;
            }
            else if (current_disk == RAT_USB)
            {
                other_disk = RAT_TF;
            }
            else
            {
                other_disk = RAT_USB;
            }

            other_disk_is_exist =  explorer_check_disk_exist(other_disk);
            current_disk_is_exist = explorer_check_disk_exist(current_disk);
            __wrn("current_disk:%d other_disk:%d", current_disk, other_disk);
            __wrn("other_disk_is_exist:%d current_disk_is_exist:%d", other_disk_is_exist, current_disk_is_exist);

            if (current_disk_is_exist == EPDK_FALSE && EPDK_FALSE == other_disk_is_exist)
            {
                __gui_msg_t my_msg;
                __wrn("will back to explorer menu");
                eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                my_msg.h_deswin = msg->h_deswin;
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.id = DSK_MSG_HOME;
                my_msg.dwAddData1 = 0;
                my_msg.dwAddData2 = 0;
                my_msg.dwReserved = NULL;
                GUI_SendNotifyMessage(&my_msg);
                return EPDK_OK;
            }
            else if (current_disk_is_exist == EPDK_TRUE && EPDK_FALSE == other_disk_is_exist)
            {
                __wrn("other disk out....do nothing!");
                return EPDK_OK;
            }
            else if (current_disk_is_exist == EPDK_FALSE && EPDK_TRUE == other_disk_is_exist)
            {
                __gui_msg_t my_msg;
                __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_MUSIC_SWITCH_USB_TF_INFO};
                lang_id[1] = STRING_MUSIC_SWITCH_USB_TF_INFO;
                default_dialog(h_explorer_dialog, msg->h_deswin, 200, ADLG_OK, lang_id);
                __wrn("will switch other disk!");
                explr_ctrl->root_type = other_disk;
                eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                my_msg.h_deswin = explr_ctrl->h_list_framewin;
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.id = GUI_MSG_COMMAND;
                my_msg.dwAddData1 = EXPLR_SW_TO_OTHER_DISK;
                my_msg.dwAddData2 = other_disk;
                my_msg.dwReserved = NULL;
                GUI_SendNotifyMessage(&my_msg);
            }

            return EPDK_OK;
        }

        default:
            break;
    }

    return GUI_ManWinDefaultProc(msg);
}


/*
************************************************************************************************************************
*Function   :          static H_LYR explorer_8bpp_layer_create(void)
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
static H_LYR explorer_8bpp_layer_create(void)
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
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_EXPLORER",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    explorer_get_screen_rect(&LayerRect);
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
    lstlyr.pipe = 0;
    lstlyr.fb = &fb;
    lyrcreate_info.lyrpara = &lstlyr;
    layer = GUI_LyrWinCreate(&lyrcreate_info);

    if (!layer)
    {
        __err("app bar layer create error !");
    }

    return layer;
}

/*
************************************************************************************************************************
*Function   :          static H_LYR explorer_32bpp_layer_create(void)
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
static H_LYR explorer_32bpp_layer_create(__s32 pipe)
{
    H_LYR layer = NULL;
    //RECT LayerRect;
    explorer_viewer_ui_t *ui_param = NULL;
    //  int ret;
    //__layerwincreate_para_t lyrcreate_info;
    FB  fb =
    {
        {0, 0},                                         /* size      */
        {0, 0, 0},                                      /* buffer    */
        //{FB_TYPE_RGB, {PIXEL_MONO_8BPP, 0, (__rgb_seq_t)0}},    /* fmt       */
        {FB_TYPE_RGB, {{PIXEL_COLOR_ARGB8888, 0, (__rgb_seq_t)0,{0,0}}},0},    /* fmt       */

    };
    __disp_layer_para_t lstlyr =
    {
        //DISP_LAYER_WORK_MODE_PALETTE,                    /* mode      */
        MOD_DISP_LAYER_WORK_MODE_NORMAL,
        0,                                              /* ck_mode   */
        0,                                              /* alpha_en  */
        4,                                              /* alpha_val */
        1,                                              /* pipe      */
        9,                                           /* prio      */
        {0, 0, 0, 0},                                   /* screen    */
        {0, 0, 0, 0},                                   /* source    */
        DISP_LAYER_OUTPUT_CHN_DE_CH1,                   /* channel   */
        NULL                                            /* fb        */
    };
    __layerwincreate_para_t lyrcreate_info =
    {
        "APP_EXPLORER",
        NULL,
        GUI_LYRWIN_STA_SUSPEND,
        GUI_LYRWIN_NORMAL
    };
    __log("Enter %s", __FUNCTION__);
    //explorer_get_screen_rect(&LayerRect);
    ui_param = explorer_get_viewer_ui_param();
    fb.size.width       = ui_param->layer.fb_size.width;
    fb.size.height      = ui_param->layer.fb_size.height;
    fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888; //PIXEL_MONO_8BPP; //PIXEL_COLOR_ARGB8888;
    lstlyr.scn_win.x        = ui_param->layer.scn_rect.x;
    lstlyr.scn_win.y        = ui_param->layer.scn_rect.y;
    lstlyr.scn_win.width    = ui_param->layer.scn_rect.width;
    lstlyr.scn_win.height   = ui_param->layer.scn_rect.height;
    lstlyr.src_win.x        = ui_param->layer.src_rect.x;
    lstlyr.src_win.y        = ui_param->layer.src_rect.y;
    lstlyr.src_win.width    = ui_param->layer.src_rect.width;
    lstlyr.src_win.height   = ui_param->layer.src_rect.height;
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

/*
************************************************************************************************************************
*Function   :            __s32 explorer_dialog_create()
*
*Description    :
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/
__s32 explorer_dialog_create(__gui_msg_t *msg)
{
    explorer_ctrl_t *explr_ctrl;
    explr_dialog_para_t DlgPara;
    eLIBs_memset(&DlgPara, 0, sizeof(explr_dialog_para_t));
    //__cat_item_t  *cat_cur_item;
    explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);
    DlgPara.font = explr_ctrl->explr_font;
    explr_ctrl->dialog_layer = explorer_32bpp_layer_create(1);

    if (NULL == explr_ctrl->dialog_layer)
    {
        return EPDK_FAIL;
    }

    DlgPara.dialog_layer = explr_ctrl->dialog_layer;
    explr_ctrl->h_dialog_framewin = explorer_dialog_frame_win_create(msg->h_deswin, &DlgPara);

    if (NULL == explr_ctrl->h_dialog_framewin)
    {
        return EPDK_FAIL;
    }

    GUI_WinSetFocusChild(explr_ctrl->h_dialog_framewin);        //frame win get focus.                  //set picture dialog_layer as top level
    GUI_LyrWinCacheOn();
    GUI_LyrWinSetSta(explr_ctrl->dialog_layer, GUI_LYRWIN_STA_SLEEP);
    GUI_LyrWinSetSta(explr_ctrl->dialog_layer, GUI_LYRWIN_STA_ON);
    GUI_LyrWinFlushFB(explr_ctrl->dialog_layer);
    GUI_LyrWinCacheOff();
    GUI_LyrWinSetTop(explr_ctrl->dialog_layer);
    return EPDK_OK;
}

static __s32 explorer_layer_delete(H_LYR layer)
{
    if (layer != NULL)
    {
        GUI_LyrWinDelete(layer);
    }

    return EPDK_OK;
}

static __s32 explorer_draw_bg(explorer_ctrl_t   *explr_ctrl)
{
    void *p_buf = NULL;
    __inf("Enter %s", __FUNCTION__);

    if ((explr_ctrl != NULL) && (explr_ctrl->BG_layer != NULL))
    {
        __inf("explr_ctrl or explr_ctrl->BG_layer NULL");
        return EPDK_FAIL;
    }

    GUI_LyrWinSetSta(explr_ctrl->BG_layer, GUI_LYRWIN_STA_ON);
    p_buf = explorer_get_listview_icon_res(ID_EXP_BACKGROUND);
    GUI_LyrWinSel(explr_ctrl->BG_layer);

    if (p_buf != NULL)
    {
        GUI_BMP_Draw(p_buf, 0, 0);
        gscene_bgd_set_state(BGD_STATUS_HIDE);
    }

    return EPDK_OK;
}

/*
************************************************************************************************************************
*Function   :           static __s32 explorer_scene_create()
*
*Description    :       创建浏览器场景，ListBar
*
*Arguments      :
*
*Return         :
*
************************************************************************************************************************
*/

static __s32 explorer_scene_create(__gui_msg_t *msg)
{
    explr_list_para_t ListPara;
    explorer_ctrl_t *explr_ctrl = NULL;
    __inf("Enter %s", __FUNCTION__);
    explr_ctrl = (explorer_ctrl_t *)GUI_WinGetAttr(msg->h_deswin);

    if (explr_ctrl == NULL)
    {
        __err("explr_ctrl null");
        return EPDK_FAIL;
    }

    eLIBs_memset(&ListPara, 0, sizeof(explr_list_para_t));
    ListPara.root_type = explr_ctrl->root_type;
    ListPara.last_start_id = explr_ctrl->last_start_id;
    ListPara.last_focused_id = explr_ctrl->last_focused_id;
    ListPara.last_filename = explr_ctrl->last_filename;     //获得上一次浏览信息
    ListPara.media_type = explr_ctrl->media_type;
    ListPara.view_mode = explr_ctrl->view_mode;     //view mode,
    ListPara.root_para = explr_ctrl->root_para ;
    __inf("explr_ctrl->media_type=%d", explr_ctrl->media_type);

    switch (explr_ctrl->media_type)
    {
        case RAT_MEDIA_TYPE_ALL:
        {
            __explorer_set_title(STRING_EXPLR_TITLE);   //设置hbar title
            break;
        }

        case    RAT_MEDIA_TYPE_VIDEO:
        {
            __explorer_set_title(STRING_MOVIE_TITLE);   //设置hbar title
            break;
        }

        case    RAT_MEDIA_TYPE_AUDIO:
        {
            __explorer_set_title(STRING_MUSIC_TITLE);   //设置hbar title
            break;
        }

        case    RAT_MEDIA_TYPE_PIC:
        {
                __log("vrbsdb....RAT_MEDIA_TYPE_PIC");
            __explorer_set_title(STRING_HOME_PHOTO);    //设置hbar title
            break;
        }

        case    RAT_MEDIA_TYPE_EBOOK:
        {
            __explorer_set_title(STRING_HOME_EBOOK);    //设置hbar title
            break;
        }

        default :
            break;
    }

    //gscene_bgd_set_state(BGD_STATUS_HIDE);
    explr_ctrl->BG_layer = NULL; //explorer_32bpp_layer_create(0);    //要用不同的pipe
    explr_ctrl->list_layer = explorer_32bpp_layer_create(0);
    // explorer_draw_bg(explr_ctrl);             // 画背景
    ListPara.BG_layer = explr_ctrl->BG_layer;       //背景图层
    ListPara.list_layer = explr_ctrl->list_layer;       //listbar layer
    ListPara.mediatype_index = explr_ctrl->mediatype_index;
    __log("explr_ctrl->mediatype_index = %d ......", explr_ctrl->mediatype_index);
    //..PT_MEMINF;
    explr_ctrl->h_list_framewin = explorer_list_win_create(msg->h_deswin, &ListPara);
    //..PT_MEMINF;
    GUI_WinSetFocusChild(explr_ctrl->h_list_framewin);      //frame win get focus.                  //set picture layer as top level
    GUI_LyrWinCacheOn();

    if (explr_ctrl->BG_layer)
    {
        GUI_LyrWinSetSta(explr_ctrl->BG_layer, GUI_LYRWIN_STA_SLEEP);
        GUI_LyrWinSetSta(explr_ctrl->BG_layer, GUI_LYRWIN_STA_ON);
    }

    //..PT_MEMINF;
    GUI_LyrWinSetSta(explr_ctrl->list_layer, GUI_LYRWIN_STA_SLEEP);
    GUI_LyrWinSetSta(explr_ctrl->list_layer, GUI_LYRWIN_STA_ON);

    if (explr_ctrl->BG_layer)
    {
        GUI_LyrWinSetBottom(explr_ctrl->BG_layer);
    }

    //..PT_MEMINF;
    GUI_LyrWinFlushFB(explr_ctrl->list_layer);
    GUI_LyrWinCacheOff();
    //..PT_MEMINF;
    return EPDK_OK;
}
__s32 explorer_destory_dialog(void)
{
    app_dialog_destroy(h_explorer_dialog);
    h_explorer_dialog = NULL;
    return 0;
}
