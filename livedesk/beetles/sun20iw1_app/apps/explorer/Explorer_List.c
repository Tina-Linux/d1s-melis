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
#include "app_root/app_root_scene.h"
#include "Explorer.h"
#include "Explorer_UI.h"
#include "Explorer_List.h"

#include "FileList.h"

//用于检测对话框是否存在，存在的话，定时时间到，删除对话框
static  __u32 ExplorerTimerId = 0x16;
// 在浏览音乐媒体时，背景音乐存在的话，定时去更新索引号
static  __u32 ExplorerDrawBgMusicIndexTimer = 0xff ;
#define     EXP_DRAW_BG_MUSIC_INDEX_INTERVAL  200   // 2 s 
#define     C_EXP_TIMER_INTERVAL    300             // 2second

//按键操作对应左右的listbar
#define LISTBAR_LEFT 1
#define LISTBAR_RIGHT 0


H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para);
static __s32 _explorer_list_win_cb(__gui_msg_t *msg);           //下划线开始表示为回调函数

static __s32 explorer_listview_create(__gui_msg_t *msg);
static __s32 explorer_file_manager_ignor_sd_usb(__gui_msg_t *msg);
static __s32 explorer_listview_onpaint(__gui_msg_t *msg);
static __s32  explorer_list_win_on_close(__gui_msg_t *msg);
static __s32  explorer_list_win_on_destroy(__gui_msg_t *msg);
static __s32  explorer_list_win_key_proc(__gui_msg_t *msg);

static __s32 explorer_rat_init(explr_list_para_t *list_para);
static __s32 explorer_rat_uninit(explr_list_para_t *list_para);

static void explorer_mediatype_listbar_init(H_WIN  list_win);
static void explorer_listbar_init(H_WIN  list_win);
static void explorer_mediatype_listbar_uninit(H_WIN  list_win);
static void explorer_listbar_uninit(H_WIN  list_win);

static __s32 explorer_file_list_uninit(explr_list_para_t *list_para);
static __s32 explorer_file_list_init(explr_list_para_t *list_para);
static __s32  explorer_file_manager_rebuild(__gui_msg_t *msg);
static __s32 explorer_show_dlg(H_WIN hwin, __s32 text_id, __s32 title_id, __s32 time);
static __s32 explorer_list_long_string_start_roll(explr_list_para_t *this);
static __s32  explorer_list_win_on_enter_key(__gui_msg_t *msg);
static __s32  file_manager_list_on_enter_key(__gui_msg_t *msg);
static __s32  explorer_list_return_backspace(__gui_msg_t *msg);
static __s32 explorer_list_win_update(explr_list_para_t *list_para);





static __s32 _explorer_listbar_item_draw(__lbar_draw_para_t *draw_param);
#if (EXPLORER_SUPPORT_MINIATURE == 1)
static __s32 draw_miniature_view_item(__lbar_draw_para_t *draw_param);
#endif
static __s32 draw_listview_item(__lbar_draw_para_t *draw_param);

__s32 ExplorerListWinOnEnterKey(explr_list_para_t *list_para);
__s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para);
__s32 ExplorerListWinOnPreviousItem(explr_list_para_t *list_para);

__s32 ExplorerMediatypeListWinOnNextItem(__gui_msg_t *msg);
__s32 ExplorerMediatypeListWinOnPreviousItem(__gui_msg_t *msg);

__s32 ExplorerListWinOnNextRow(explr_list_para_t *list_para);
__s32 ExplorerListWinOnPreviousRow(explr_list_para_t *list_para);
__s32 ExplorerListWinOnNextPage(explr_list_para_t *list_para);
__s32 ExplorerListWinOnPreviousPage(explr_list_para_t *list_para);


void  ExplorerListWinGetSearchPath(explr_list_para_t *para);


static __s32 ExtractFileName(char *FileName, char *FilePath);
__s32 GetListItemFileFullPath(explr_list_para_t *list_para, __s32 ItemIndex, char *FilePath);
static __s32 GetListItemFileName(explr_list_para_t *list_para, __s32 ItemIndex, char *FileName);
rat_media_type_t GetListItemFileMediaType(HRAT rat_handle, __s32 ItemIndex);
void ExplorerSendMessage(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);

//改变显示方式
void ExplorerSetViewMode(explr_list_para_t *list_para, explorer_view_mode_e view_mode);


//长文件名滚动显示
__s32 explorer_list_long_string_init(explr_list_para_t *this);
__s32 explorer_list_long_string_uninit(explr_list_para_t *this);
__s32 explorer_list_long_string_start_roll_fast(__lbar_draw_para_t *draw_param, char *string)   ;
__s32 explorer_list_long_string_start_roll(explr_list_para_t *this);
static __s32 explorer_list_long_string_stop_roll(explr_list_para_t *this);
static void explorer_paint_media_list(H_WIN win);


//获得保存上一次浏览的参数
static __s32 explorer_get_last_para(explr_list_para_t *list_para);
static __s32 explorer_save_last_para(explr_list_para_t *list_para)  ;

//__s32 explorer_draw_file_info(explr_list_para_t *list_para);
void explorer_list_on_timer(__gui_msg_t *msg);
#if (EXPLORER_SUPPORT_MINIATURE == 1)
static __s32 draw_miniature_for_pic_item(__lbar_draw_para_t *draw_param);
#endif
static __bool position_in_rect(RECT *rect, __s32 x, __s32 y) ;
#if (BEETLES_GAME_FLAG == 1)
extern __s32 USBJoystickPlugIn(void *p_arg);
extern __s32 USBJoystickPlugOut(void *p_arg);
extern  __u32 game_key_read(void);
#endif

#undef  VK_LEFT
#undef  VK_RIGHT

#define VK_UP              0x00000001      // 向上
#define VK_DOWN            0x00000002      // 向下
#define VK_LEFT            0x00000004      // 向左
#define VK_RIGHT           0x00000008      // 向右
#define VK_PLAY            0x00000020      // PLAY

/*#define KARAOKE_LYRIC_USE_MALLOC 1


#if KARAOKE_LYRIC_USE_MALLOC
#define esMEMS_Balloc(x) esMEMS_Malloc(0, x)
#define esMEMS_Bfree(x, s) esMEMS_Mfree(0, x)
#endif
*/

static __s32 folder_emtpy[] =
{
    STRING_EXPLR_FOLDER_EMPTY,
    STRING_EXPLR_MUSIC_EMPTY,
    STRING_EXPLR_VEDIO_EMPTY,
    STRING_EXPLR_EBOOK_EMPTY,
    STRING_EXPLR_PHOTO_EMPTY,
    STRING_EXPLR_GAME_EMPTY,
};

static __s32 mediatype_tab[] =
{
#ifndef FILE_MANAGER
    RAT_MEDIA_TYPE_ALL,
#endif
    RAT_MEDIA_TYPE_AUDIO,
    RAT_MEDIA_TYPE_VIDEO,
    RAT_MEDIA_TYPE_EBOOK,
    RAT_MEDIA_TYPE_PIC,
    RAT_MEDIA_TYPE_GAME,
};


#if (BEETLES_GAME_FLAG == 1)
static void explorer_handle_thread_cb(void *arg)
{
    __u32 loopFlag = EPDK_TRUE;
    __u32 cur_key;
    __u32 usb_key = 0;
    //explr_list_para_t *list_para;

    //list_para = ()

    while (loopFlag)
    {
        USBJoystickPlugIn(NULL);
        cur_key = game_key_read();

        if ((cur_key & VK_PLAY))
        {
            usb_key = ENTER_SELECT_ITEM_ID;
        }
        else if ((cur_key & VK_UP) || (cur_key & VK_LEFT))
        {
            usb_key = SELECT_UP_ITEM_ID;
            __wrn("up........................");
        }
        else if ((cur_key & VK_DOWN) || (cur_key & VK_RIGHT))
        {
            usb_key = SELECT_DOWN_ITEM_ID;
            __wrn("down........................");
        }

        if (0 != usb_key)
        {
            __gui_msg_t msg;
            msg.dwAddData1 = usb_key;
            msg.id        = GUI_MSG_COMMAND;
            msg.h_deswin   = GUI_WinGetHandFromName("Explorer List window");
            //GUI_SendNotifyMessage(&msg);
            //GUI_WinSetFocusChild(msg.h_deswin);
            GUI_SendNotifyMessage(&msg);//此处不能用GUI_SendMessage(&msg);否则在跳转到另外一个axf时会挂死
            //explorer_list_win_on_command(&msg);//暂时这么处理
            usb_key = 0;
            init_reset_close_scn();
            init_reset_auto_off();
        }

        if (esKRNL_TDelReq(OS_PRIO_SELF) == OS_TASK_DEL_REQ)
        {
            ////__here__;
            loopFlag = EPDK_FALSE;
        }

        esKRNL_TimeDly(10);
    }

    esKRNL_TDel(OS_PRIO_SELF);
}
#endif
static __s32 explorer_list_draw_sd(explr_list_para_t    *explr_list, __s32 foucus)
{
    void *p_buf ;
    explorer_viewer_ui_t *ui_param;

    if (!explr_list)
    {
        __msg("invalid para...");
        return EPDK_FAIL;
    }

    ui_param = explorer_get_viewer_ui_param();
    {
        if (FOCUSED == foucus)
        {
            p_buf = explorer_get_listview_icon_res(ID_EXP_SD_F);
        }
        else
        {
            p_buf = explorer_get_listview_icon_res(ID_EXP_SD_UF);
        }

        if (p_buf)
        {
            GUI_BMP_Draw(p_buf, ui_param->sd_coor.x, ui_param->sd_coor.y);
        }
    }
    return EPDK_OK;
}


static __s32 explorer_list_draw_usb(explr_list_para_t   *explr_list, __s32 foucus)
{
    void *p_buf ;
    explorer_viewer_ui_t *ui_param;

    if (!explr_list)
    {
        __msg("invalid para...");
        return EPDK_FAIL;
    }

    ui_param = explorer_get_viewer_ui_param();
    {
        if (FOCUSED == foucus)
        {
            p_buf = explorer_get_listview_icon_res(ID_EXP_USB_F);
        }
        else
        {
            p_buf = explorer_get_listview_icon_res(ID_EXP_USB_UF);
        }

        if (p_buf)
        {
            GUI_BMP_Draw(p_buf, ui_param->usb_coor.x, ui_param->usb_coor.y);
        }
    }
    return EPDK_OK;
}


static __s32 explorer_switch_usb_2_sd(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __s32 ret;
    __s32 text_id, title_id;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    //if(RAT_TF != list_para->root_type)
    {
        ret = explorer_check_disk_exist(RAT_TF);    // 判断给定的盘符是否存在

        if (EPDK_TRUE != ret)
        {
            H_WIN h_win;
            __msg("explorer_check_disk_exist fail ");
            //no disk
            //no disk
            text_id = STRING_ROOT_NO_SD;
            title_id = STRING_ROOT_CUE;
            h_win = GUI_WinGetParent(msg->h_deswin);
            explorer_show_dlg(h_win, text_id, title_id, 0);
            return EPDK_FAIL;
        }

        explorer_list_draw_sd(list_para, FOCUSED);
        explorer_list_draw_usb(list_para, UNFOCUSED);
        list_para->root_type = RAT_TF;
        explorer_file_manager_rebuild(msg);
    }
    return EPDK_OK;
}

static __s32 explorer_switch_sd_2_usb(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __s32 ret;
    __s32 text_id, title_id;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    //if(RAT_USB != list_para->root_type)
    {
        H_WIN h_win;
        ret = explorer_check_disk_exist(RAT_USB);   // 判断给定的盘符是否存在

        if (EPDK_TRUE != ret)
        {
            __msg("explorer_check_disk_exist fail ");
            //no disk
            text_id = STRING_ROOT_NO_USB;
            title_id = STRING_ROOT_CUE;
            h_win = GUI_WinGetParent(msg->h_deswin);
            explorer_show_dlg(h_win, text_id, title_id, 0);
            return EPDK_OK;
        }

        explorer_list_draw_sd(list_para, UNFOCUSED);
        explorer_list_draw_usb(list_para, FOCUSED);
        list_para->root_type = RAT_USB;
        explorer_file_manager_rebuild(msg);
    }
    return EPDK_OK;
}

static __s32 explorer_switch_sd_usb(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    //__s32 ret;
    //__s32 text_id,title_id;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (RAT_USB == list_para->root_type)
    {
        explorer_switch_usb_2_sd(msg);
    }
    else
    {
        explorer_switch_sd_2_usb(msg);
    }

    return EPDK_OK;
}

static __s32 explorer_switch_left_right_listbar(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __s32 lang;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (LISTBAR_LEFT == list_para->listbar_left_right)
    {
        //变更指示图标到右侧
        /*if(0 == list_para->rat.total)
         {
            __s32 lang_id[]={STRING_EXPLR_CUE , STRING_EXPLR_FOLDER_EMPTY };
            lang_id[1] = folder_emtpy[list_para->mediatype_index];

            default_dialog( list_para->h_dialog , GUI_WinGetParent(msg->h_deswin), 0x2,\
                ADLG_OK, lang_id);
            esKRNL_TimeDly(100);
            app_dialog_destroy(list_para->h_dialog);
            list_para->h_dialog = NULL ;

             return EPDK_FAIL;
        }*/
        list_para->listbar_left_right = LISTBAR_RIGHT;
        lang = STRING_EXPLR_SWITCH_TO_RIGHT;
        LISTBAR_LostFocus(list_para->mediatype_listbar_handle);
        //LISTBAR_ShowPage(list_para->listbar_handle);
        //if(list_para->listbar_left_right == LISTBAR_LEFT)
        {
            //  LISTBAR_LostFocus(list_para->mediatype_listbar_handle );
        }
    }
    else
    {
        list_para->listbar_left_right = LISTBAR_LEFT;
        lang = STRING_EXPLR_SWITCH_TO_LEFT;
        LISTBAR_LostFocus(list_para->listbar_handle);
        LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
        //变更指示图标到左侧
    }

    /*{
        __s32 lang_id[]={STRING_EXPLR_CUE , STRING_EXPLR_FOLDER_EMPTY };
        lang_id[1] = lang;

        default_dialog( list_para->h_dialog , GUI_WinGetParent(msg->h_deswin), 0x2,\
            ADLG_OK, lang_id);
        esKRNL_TimeDly(100);
        app_dialog_destroy(list_para->h_dialog);
        list_para->h_dialog = NULL ;

    }*/
    return EPDK_OK;
}
#if (EXPLORER_SUPPORT_MINIATURE == 1)
__s32 rat_get_pic_info_ex(rat_miniature_para_t *para, rat_pic_info_t *pic_info, explr_list_para_t *list_para)
{
    OS_SEM_DATA         pdata;
    __s32 ret = EPDK_FAIL;
    esKRNL_SemQuery(list_para->get_miniature_sem, &pdata);

    if (1 == pdata.OSCnt)
    {
        __u8 err ;
        esKRNL_SemPend(list_para->get_miniature_sem, 0, &err);
        __msg("request get_miniature_sem signal ok");
        //..PT_MEMINF;
        ret = rat_get_pic_info(para, pic_info);
        //..PT_MEMINF;
        esKRNL_SemPost(list_para->get_miniature_sem);
    }
    else
    {
        __msg("request get_miniature_sem signal fail");
    }

    return ret ;
}
#endif
static __u32 abs(__s32 v)
{
    if (v >= 0)
    {
        return v;
    }

    return -v;
}

__s32   explorer_list_draw_bg_music_play_song_index(H_WIN list_win)
{
    explr_list_para_t *list_para ;
    __u32 index = 0 ;
    __s32 root_type = 0 ;
    __s32  ret = 0 ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    ret = is_app_exist(APP_MUSIC) ; // 查询背景音乐是否存在

    if (EPDK_TRUE == ret)
    {
        __msg(" music exist ");
        root_type = __app_root_get_bg_music_index(&index) ;  // 获取当前背景音乐播放的盘符和索引号
        __msg("root_type = %d  index = %d   list_para->rat.index=%d ", \
              root_type, index, list_para->rat.index);
        list_para->rat.index = LISTBAR_GetFocusItem(list_para->listbar_handle);

        // 当前浏览的盘符跟播放背景音乐的盘符相同
        if ((index != list_para->rat.index) && (root_type == list_para->root_type) && \
            (RAT_MEDIA_TYPE_AUDIO == list_para->media_type))
        {
            __u32 page_item_cnt = 0;                //每页的条目总数
            __listbar_scene_t scene;
            eLIBs_memset(&scene, 0x00, sizeof(scene));
            page_item_cnt = explorer_get_listbar_pagenum(list_para);// 获取listbar 每一页有几个条目
            list_para->last_focused_id = index ;

            if (list_para->last_focused_id < list_para->rat.total)
            {
                if (list_para->last_focused_id < page_item_cnt)
                {
                    list_para->last_start_id = 0 ;
                    list_para->rat.index = list_para->last_focused_id;
                }
                else
                {
                    list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_item_cnt) ;
                    list_para->rat.index = list_para->last_focused_id;
                }

                scene.start_id = list_para->last_start_id ;
                scene.focus_id = list_para->last_focused_id ;
                LISTBAR_SetScene(list_para->listbar_handle, &scene);
                LISTBAR_UpdatePage(list_para->listbar_handle) ;
            }
            else
            {
                __inf("%s %d get bg music index fail" __FILE__, __LINE__);
                return EPDK_FAIL ;
            }
        }
    }

    return  EPDK_OK ;
}
__s32  explorer_check_disk_exist(__s32 root_type)
{
    __s32   ret = EPDK_FALSE, ret1 ;
    __s32 len = 0 ;
    char disk_name[RAT_MAX_PARTITION][4];

    if (RAT_TF == root_type)
    {
        eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
        ret1  = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
        len = eLIBs_strlen(disk_name[0]);

        if ((EPDK_OK == ret1) && (len != 0))
        {
            return EPDK_TRUE ;
        }
    }

    if (RAT_USB == root_type)
    {
        eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
        ret1 = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
        len = eLIBs_strlen(disk_name[0]);

        if ((EPDK_OK == ret1) && (len != 0))
        {
            return EPDK_TRUE  ;
        }
    }

    return ret ;
}

// 检测磁盘个数
__s32  explorer_get_disk_total_num(root_para_t  *para)
{
    __s32 ret = 0 ;
    __s32 count = 0;
    __s32 len = 0 ;
    char disk_name[RAT_MAX_PARTITION][4];
    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
    ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
    len = eLIBs_strlen(disk_name[0]);

    if ((EPDK_OK == ret) && (len != 0)) // 磁盘存在
    {
        count++;
        para->root_type = RAT_USB;
    }

    eLIBs_memset(disk_name, 0, sizeof(disk_name)) ;
    ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
    len = eLIBs_strlen(disk_name[0]);

    if ((EPDK_OK == ret) && (len != 0))
    {
        count++;
        para->root_type = RAT_TF;
    }

    return count ;
}

//拔卡动作响应
static __s32 explorer_on_fs_part_out(__gui_msg_t *msg)
{
    __u32 total = 0;
    __s32 ret  = EPDK_FAIL ;
    char disk_name[RAT_MAX_PARTITION][4];
    __s32 len = 0 ;
    explr_list_para_t *list_para ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        __msg("list_para is null ");
        return EPDK_FAIL;
    }

    __msg("explorer_on_fs_part_out");
    __msg("list_para->enter_sd_usb_flag = %d ", list_para->enter_sd_usb_flag);

    if (list_para->enter_sd_usb_flag == 0)      // 在顶级文件菜单
    {
        delete_file_list_nod(list_para->top_file_list) ;
        list_para->top_file_list = NULL ;
        list_para->top_file_list = new_file_root_list_nod(NULL);

        if (!list_para->top_file_list)
        {
            __msg(" list_para->top_file_list is null ");
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            return EPDK_OK ;
        }

        list_para->cur_file_list = list_para->top_file_list ;
        list_para->rat.total = list_para->top_file_list->total ;
        __msg("list_para->rat.total = %d", list_para->rat.total);
        explorer_listbar_uninit(list_para->list_win) ;
        list_para->last_focused_id = 0 ;
        list_para->last_start_id = 0 ;
        explorer_listbar_init(list_para->list_win) ;
        LISTBAR_ShowPage(list_para->listbar_handle) ;
    }
    else if (list_para->enter_sd_usb_flag == 1)
    {
        __msg("list_para->root_type = %d ", list_para->root_type);

        if (list_para->root_type == RAT_USB)
        {
            ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);   //所拔卡为当前播放的
            len = eLIBs_strlen(disk_name[0]);

            if ((EPDK_OK == ret) && (len == 0)) //卡已经拔出
            {
                file_item_t *temp_item  = NULL ;
                file_item_t *head_item = NULL ;
                head_item = list_para->top_file_list->item_list ;
                temp_item = head_item ;

                while (temp_item->next != head_item)    // 搜索结点
                {
                    if (RAT_MEDIA_TYPE_USB_DEVICE == temp_item->fatdirattr) // 搜索到结点
                    {
                        break ;
                    }
                    else
                    {
                        temp_item = temp_item->next ;   // 没搜索到，指向下一个结点，继续往下搜索
                    }
                }

                if (temp_item == list_para->top_file_list->item_list)   // 搜索出来的是头结点
                {
                    list_para->top_file_list->item_list = list_para->top_file_list->item_list->next;
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }
                else        //搜索出来的结点不是头结点
                {
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }

                if (!list_para->top_file_list->item_list)
                {
                    __msg(" list_para->top_file_list->item_list is null ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }

                list_para->top_file_list->total-- ;

                if (list_para->top_file_list->focus_id > 0)
                {
                    list_para->top_file_list->focus_id -- ;
                }

                if (list_para->top_file_list->total == 0)
                {
                    __msg(" all disk has been plug out ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }

                explorer_list_return_backspace(msg) ;
                return EPDK_OK ;
            }

            ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);    // 所拔卡不是当前播放的
            len = eLIBs_strlen(disk_name[0]);

            if ((EPDK_OK == ret) && (len == 0)) // 卡已拔出
            {
                file_item_t *head_item = NULL, *temp_item = NULL;
                head_item = list_para->top_file_list->item_list ;
                temp_item = head_item ;

                while (temp_item->next != head_item)    // 搜索结点
                {
                    if (RAT_MEDIA_TYPE_SD_DEVICE == temp_item->fatdirattr)
                    {
                        break ;
                    }
                    else
                    {
                        temp_item = temp_item->next ;   // 没搜索到，指向下一个结点，继续往下搜索
                    }
                }

                if (temp_item == list_para->top_file_list->item_list) // 判断搜索出来的是否为头结点
                {
                    list_para->top_file_list->item_list = list_para->top_file_list->item_list->next;
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }
                else        //搜索出来的结点不是头结点
                {
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }

                if (!list_para->top_file_list->item_list)
                {
                    __msg(" list_para->top_file_list->item_list  is null ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }

                list_para->top_file_list->total-- ;

                if (list_para->top_file_list->focus_id > 0)
                {
                    list_para->top_file_list->focus_id -- ;
                }

                if (list_para->top_file_list->total == 0)
                {
                    __msg(" all disk has been plug out ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }
            }

            return EPDK_OK ;
        }

        __msg("list_para->root_type = %d ", list_para->root_type);

        if (list_para->root_type == RAT_TF)
        {
            ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);    //所拔卡为当前播放的
            len = eLIBs_strlen(disk_name[0]);

            if ((EPDK_OK == ret) && (len == 0))
            {
                file_item_t *temp_item  = NULL, *head_item = NULL ;
                head_item = list_para->top_file_list->item_list ;
                temp_item = head_item ;

                while (temp_item->next != head_item)    // 搜索结点
                {
                    if (RAT_MEDIA_TYPE_SD_DEVICE == temp_item->fatdirattr)
                    {
                        break ;
                    }
                    else
                    {
                        temp_item = temp_item->next ;
                    }
                }

                if (temp_item == list_para->top_file_list->item_list) // 判断搜索出来的是否为头结点
                {
                    list_para->top_file_list->item_list = list_para->top_file_list->item_list->next;
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }
                else        //搜索出来的结点不是头结点
                {
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }

                if (!list_para->top_file_list->item_list)
                {
                    __msg(" list_para->top_file_list->item_list  is null ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }

                list_para->top_file_list->total-- ;

                if (list_para->top_file_list->focus_id > 0)
                {
                    list_para->top_file_list->focus_id -- ;
                }

                if (list_para->top_file_list->total == 0)
                {
                    __msg(" all disk has been plug out ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }

                explorer_list_return_backspace(msg) ;
                return EPDK_OK ;
            }

            ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);   // 所拔卡不是当前播放的
            len = eLIBs_strlen(disk_name[0]);

            if ((EPDK_OK == ret) && (len == 0))
            {
                file_item_t *temp_item  = NULL, *head_item = NULL ;
                head_item = list_para->top_file_list->item_list ;
                temp_item = head_item ;

                while (temp_item->next != head_item)    // 搜索结点
                {
                    if (RAT_MEDIA_TYPE_USB_DEVICE == temp_item->fatdirattr)
                    {
                        break ;
                    }
                    else
                    {
                        temp_item = temp_item->next ;
                    }
                }

                if (temp_item == list_para->top_file_list->item_list) // 判断搜索出来的是否为头结点
                {
                    list_para->top_file_list->item_list = list_para->top_file_list->item_list->next;
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }
                else        //搜索出来的结点不是头结点
                {
                    temp_item->next->previous = temp_item->previous ;
                    temp_item->previous ->next = temp_item->next ;
                    delete_file_item(temp_item);    // 删除item ;
                }

                if (!list_para->top_file_list->item_list)
                {
                    __msg(" list_para->top_file_list->item_list  is null ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }

                list_para->top_file_list->total-- ;

                if (list_para->top_file_list->focus_id > 0)
                {
                    list_para->top_file_list->focus_id -- ;
                }

                if (list_para->top_file_list->total == 0)
                {
                    __msg(" all disk has been plug out ");
                    explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
                    return EPDK_OK ;
                }
            }

            return EPDK_OK ;
        }
    }

    return EPDK_OK;
}

//插卡动作响应
static __s32 explorer_on_fs_part_in(__gui_msg_t *msg)
{
    explr_list_para_t *list_para ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        __msg(" explorer_on_fs_part_in get win attr fail  ");
        return EPDK_FAIL;
    }

    __msg("explorer_on_fs_part_in begin");
    __msg("list_para->enter_sd_usb_flag=%d", list_para->enter_sd_usb_flag);

    /*
    if( list_para->enter_sd_usb_flag == 0)
    {
        delete_file_list_nod( list_para->top_file_list ) ;
        list_para->top_file_list = new_file_root_list_nod(NULL) ;
        list_para->cur_file_list = list_para->top_file_list;
        list_para->rat.total = list_para->top_file_list->total;
        __msg( "list_para->rat.total = %d" , list_para->rat.total );
        explorer_listbar_uninit( list_para->list_win ) ;
        explorer_listbar_init( list_para->list_win ) ;
        LISTBAR_ShowPage( list_para->listbar_handle ) ;
    }
    */
    if (list_para->enter_sd_usb_flag == 0)
    {
        __u8    fatdirattr ;
        file_item_t *temp_item = NULL ;
        file_item_t  *new_item = NULL ;
        file_item_t  *head_item = NULL ;
        __s32 ret = 0, len = 0;
        char disk_name[RAT_MAX_PARTITION][4];
        ES_DIRENT dirent;
        fatdirattr = list_para->top_file_list->cur_item->fatdirattr ;

        if (RAT_MEDIA_TYPE_USB_DEVICE == fatdirattr)
        {
            eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
            eLIBs_memset(disk_name, 0, sizeof(disk_name));
            ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
            len = eLIBs_strlen(disk_name[0]);

            if ((ret == EPDK_OK) && (len != 0))     //磁盘存在
            {
                dirent.d_size = 0 ;
                dirent.fatdirattr = RAT_MEDIA_TYPE_SD_DEVICE ;
                eLIBs_strcpy(dirent.d_name, disk_name[0]);
                new_item = (file_item_t *)new_file_item(&dirent);

                if (new_item != NULL)   //把新增加的item 插入到链表结尾
                {
                    head_item = list_para->top_file_list->item_list ;
                    temp_item = head_item->previous ;
                    temp_item->next = new_item ;
                    new_item->previous = temp_item ;
                    new_item->next = head_item ;
                    head_item->previous = new_item ;
                    list_para->top_file_list->total++ ;
                }
                else
                {
                    __msg(" new_item is null ");
                }
            }
        }
        else if (RAT_MEDIA_TYPE_SD_DEVICE == fatdirattr)
        {
            eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
            eLIBs_memset(disk_name, 0, sizeof(disk_name));
            ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
            len = eLIBs_strlen(disk_name[0]);

            if ((ret == EPDK_OK) && (len != 0))
            {
                dirent.d_size = 0;
                dirent.fatdirattr = RAT_MEDIA_TYPE_USB_DEVICE ;
                eLIBs_strcpy(dirent.d_name, disk_name[0]);
                new_item = (file_item_t *) new_file_item(&dirent);

                if (new_item != NULL)   //把新增加的item 插入到链表结尾
                {
                    head_item = list_para->top_file_list->item_list ;
                    temp_item = head_item->previous ;
                    temp_item->next = new_item ;
                    new_item->previous = temp_item ;
                    new_item->next = head_item ;
                    head_item->previous = new_item ;
                    list_para->top_file_list->total++ ;
                }
                else
                {
                    __msg(" new_item is null ");
                }
            }
        }

        list_para->rat.total = list_para->top_file_list->total;
        __msg("list_para->rat.total = %d", list_para->rat.total);
        explorer_listbar_uninit(list_para->list_win) ;
        explorer_listbar_init(list_para->list_win) ;
        LISTBAR_ShowPage(list_para->listbar_handle) ;
    }
    else if (list_para->enter_sd_usb_flag == 1)
    {
        file_item_t *temp_item = NULL ;
        file_item_t  *new_item = NULL ;
        file_item_t  *head_item = NULL ;
        __s32 ret = 0, len = 0;
        char disk_name[RAT_MAX_PARTITION][4];
        ES_DIRENT dirent;
        __msg(" list_para->root_type = %d ", list_para->root_type);

        if (RAT_USB == list_para->root_type)
        {
            eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
            eLIBs_memset(disk_name, 0, sizeof(disk_name));
            ret = rat_get_partition_name(RAT_SD_CARD, disk_name, 0);
            len = eLIBs_strlen(disk_name[0]);

            if ((ret == EPDK_OK) && (len != 0))     //磁盘存在
            {
                dirent.d_size = 0 ;
                dirent.fatdirattr = RAT_MEDIA_TYPE_SD_DEVICE ;
                eLIBs_strcpy(dirent.d_name, disk_name[0]);
                new_item = (file_item_t *)new_file_item(&dirent);

                if (new_item != NULL)   //把新增加的item 插入到链表结尾
                {
                    head_item = list_para->top_file_list->item_list ;
                    temp_item = head_item ;
                    __msg("head_item = %x", head_item);
                    __msg("temp_item->next = %x", temp_item->next);

                    while (temp_item->next != head_item)
                    {
                        __msg(" searching nod ")  ;
                        temp_item = temp_item->next ;
                        __msg("temp_item = %x", temp_item);
                    }

                    temp_item->next = new_item ;
                    new_item->previous = temp_item ;
                    new_item->next = head_item ;
                    head_item->previous = new_item ;
                    list_para->top_file_list->total++ ;
                }
                else
                {
                    __msg(" new_item is null ");
                }
            }
        }

        if (RAT_TF == list_para->root_type)
        {
            eLIBs_memset(&dirent, 0, sizeof(ES_DIRENT));
            eLIBs_memset(disk_name, 0, sizeof(disk_name));
            ret = rat_get_partition_name(RAT_USB_DISK, disk_name, 0);
            len = eLIBs_strlen(disk_name[0]);

            if ((ret == EPDK_OK) && (len != 0))
            {
                dirent.d_size = 0;
                dirent.fatdirattr = RAT_MEDIA_TYPE_USB_DEVICE ;
                eLIBs_strcpy(dirent.d_name, disk_name[0]);
                new_item = (file_item_t *) new_file_item(&dirent);

                if (new_item != NULL)   //把新增加的item 插入到链表结尾
                {
                    head_item = list_para->top_file_list->item_list ;
                    temp_item = head_item ;

                    while (temp_item->next != head_item)
                    {
                        __msg(" *************hello  program running here ***************");
                        temp_item = temp_item->next ;
                    }

                    temp_item->next = new_item ;
                    new_item->previous = temp_item ;
                    new_item->next = head_item ;
                    head_item->previous = new_item ;
                    list_para->top_file_list->total++ ;
                }
                else
                {
                    __msg(" new_item is null ");
                }
            }
        }
    }

    __msg("explorer_on_fs_part_in end");
    return EPDK_OK;
}
// 浏览器时，点击条目时的响应动作
static __s32  explorer_list_enter_next_dir(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __listbar_scene_t ListBarScene;
    file_list_t *temp_file_list = NULL;
    char  path[RAT_MAX_FULL_PATH_LEN];
    __u8  fatdirattr;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    __msg("explorer_list_enter_next_dir") ;

    if (list_para == NULL)
    {
        __msg("list_para is null ");
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        __msg("list_para->cur_file_list is null ");
        return EPDK_FAIL;
    }

    __log("list_para->enter_sd_usb_flag = %d", list_para->enter_sd_usb_flag);

    //当前处于sd / usb 选择窗口中，点击sd / usb 时 搜索出对应盘符的媒体
    if (list_para->enter_sd_usb_flag == 0)
    {
        explorer_list_long_string_stop_roll(list_para);
        //进入下一级子文件夹
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        list_para->top_file_list->start_id = ListBarScene.start_id;
        list_para->top_file_list->focus_id = ListBarScene.focus_id;
        explorer_listbar_uninit(list_para->list_win);
        fatdirattr = list_para->top_file_list->cur_item->fatdirattr ;
        __log("list_para->top_file_list->cur_item->fatdirattr = %d", \
              list_para->top_file_list->cur_item->fatdirattr);

        if (RAT_MEDIA_TYPE_SD_DEVICE == fatdirattr)
        {
            list_para->root_type = RAT_TF ;
        }
        else if (RAT_MEDIA_TYPE_USB_DEVICE == fatdirattr)
        {
            list_para->root_type = RAT_USB ;
        }

        ExplorerListWinGetSearchPath(list_para) ;
        __msg("list_para->search_path = %s", list_para->search_path);
        __msg("list_para->media_type = %d", list_para->media_type);
        //list_para->rat.handle = rat_open( list_para->search_path , list_para->media_type , 0 );
        list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
        list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;

        if (list_para->rat.total == 0)
        {
            __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
            lang_id[1] = folder_emtpy[list_para->mediatype_index];
            default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                           ADLG_OK, lang_id);
            esKRNL_TimeDly(100);
            app_dialog_destroy(list_para->h_dialog);
            list_para->h_dialog = NULL ;
        }

        explorer_get_last_para(list_para) ; // 获取上一次的参数
        list_para->enter_sd_usb_flag = 1 ;

        if (RAT_MEDIA_TYPE_AUDIO == list_para->media_type)
        {
            explorer_list_draw_bg_music_play_song_index(list_para->list_win) ;
        }

        explorer_listbar_init(list_para->list_win);
        LISTBAR_ShowPage(list_para->listbar_handle);

        if (list_para->listbar_left_right == LISTBAR_LEFT)
        {
            LISTBAR_LostFocus(list_para->listbar_handle);
        }
    }
    else        // 当前处于媒体文件列表中，点击时，播放对应媒体文件
    {
        explorer_list_win_on_enter_key(msg) ;
    }

    return EPDK_OK;
}


//所有文件更新filelist 链表
static __s32 explorer_update_file_list_nod(explr_list_para_t *list_para)
{
    file_list_t *parent_file_list = NULL;
    char  file_path[RAT_MAX_FULL_PATH_LEN];
    file_list_t *temp_file_list = NULL;
    //explr_list_para_t *list_para;
    __s32 startid = 0, foucsid = 0, index = 0;
    char  *psufdir = NULL;
    file_item_t *item_list = NULL;
    __u32 page_item_cnt;

    //list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    eLIBs_strcpy(file_path, list_para->cur_file_list->file_path); //备份当前节点路径

    //backup cur foucs dir  before rebuild list , just need to consider dir only
    //usr to redraw dir foucs
    if (FSYS_ATTR_DIRECTORY == list_para->cur_file_list->cur_item->fatdirattr)
    {
        psufdir = list_para->cur_file_list->cur_item->name;
        item_list = list_para->cur_file_list->item_list;

        do
        {
            if ((0 != eLIBs_strcmp(psufdir, item_list->name)))
            {
                item_list = item_list->next;
                index++;
            }
            else
            {
                break;
            }
        } while (1);
    }

    parent_file_list = list_para->cur_file_list->parent;
    delete_file_list_nod(list_para->cur_file_list);
    list_para->cur_file_list = parent_file_list;
    //rnod = new_file_list_nod(file_path, media_type);
    temp_file_list = new_file_list_nod(file_path, NULL);

    if (temp_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    if (temp_file_list->total == 0)
    {
        __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
        lang_id[1] = folder_emtpy[list_para->mediatype_index];
        default_dialog(list_para->h_dialog, GUI_WinGetParent(list_para->list_win), 0x2, \
                       ADLG_OK, lang_id);
        esKRNL_TimeDly(100);
        app_dialog_destroy(list_para->h_dialog);
        list_para->h_dialog = NULL ;
    }

    list_para->rat.total = temp_file_list->total;
    list_para->cur_file_list->child = temp_file_list;
    temp_file_list->parent = list_para->cur_file_list;
    list_para->cur_file_list = temp_file_list;
    page_item_cnt = explorer_get_listbar_pagenum(list_para);    // 获取listbar 每一页有几个条目
    //list_para->last_start_id = index;
    list_para->last_focused_id = index;
    list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_item_cnt);

    //page_item_cnt = LISTBAR_GetPageItemCount(list_para->listbar_handle);

    if (list_para->last_focused_id < page_item_cnt)
    {
        list_para->last_start_id = 0 ;
        list_para->rat.index = list_para->last_focused_id;
        list_para->cur_file_list->focus_id = list_para->last_focused_id;
    }
    else if (list_para->last_focused_id  >= page_item_cnt)
    {
        list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_item_cnt); ;
        list_para->rat.index = list_para->last_focused_id;
        list_para->cur_file_list->focus_id = list_para->last_focused_id;
    }

    list_para->cur_file_list->start_id = list_para->last_start_id;
    //list_para->rat.index = 0;
    list_para->rat.handle = rat_reopen(list_para->top_file_list->file_path, list_para->media_type, 0);
    explorer_listbar_uninit(list_para->list_win);
    explorer_listbar_init(list_para->list_win);
    LISTBAR_ShowPage(list_para->listbar_handle);

    if (list_para->listbar_left_right == LISTBAR_LEFT)
    {
        LISTBAR_LostFocus(list_para->listbar_handle);
    }

    return EPDK_TRUE;
}




//媒体类型文件列表更新
static __s32 explorer_filelist_update(explr_list_para_t *list_para)
{
    //tmp = list_para->rat.index ;
    //if(RAT_MEDIA_TYPE_ALL != list_para->media_type)
    {
        //rat_reopen(list_para->search_path,RAT_MEDIA_TYPE_ALL,0);
    }
    list_para->rat.handle = rat_reopen(list_para->search_path, list_para->media_type, 0);
    list_para->rat.index = 0 ;

    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        list_para->rat.total = list_para->cur_file_list->total;
    }
    else
    {
        list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;
    }

    //explorer_listbar_uninit(list_para->list_win); //重新创建listbar,因为Square为全屏模式
    //explorer_listbar_init(list_para->list_win);
    //LISTBAR_ShowPage(list_para->listbar_handle);
    explorer_list_win_update(list_para);
    return EPDK_OK;
}

//从当前所建树节点中删除一个节点
static void delete_file_item_from_tree_list(explr_list_para_t *list_para, char *delete_file)
{
    file_item_t *item;
    file_list_t *list;
    __s32 ret;
    __s32 flag = EPDK_FALSE;
    char *p1;
    file_list_t *next = NULL;

    if (list_para->top_file_list == list_para->cur_file_list)
    {
        return ;
    }

    p1 = eLIBs_strchrlast(delete_file, '\\') + 1; //未考虑根目录(在此不需要)
    //根据文件名从当前节点向上搜索
    item = list_para->cur_file_list->item_list;
    list = list_para->cur_file_list;

    while (1)
    {
        do
        {
            if (eLIBs_strcmp(p1, item->name) == 0)
            {
                delete_file_item_from_list(item, list);
                flag = EPDK_TRUE;
            }
            else
            {
                item = item->next;
            }
        } while ((list_para->cur_file_list->item_list != item) && (EPDK_TRUE != flag));

        if (EPDK_TRUE != flag)
        {
            if (list->parent != list_para->top_file_list)
            {
                list = list->parent;
                item = list->item_list;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
}

//用于除RAT_MEDIA_TYPE_ALL 以外的媒体类型
static __s32 explorer_list_delete_file(__gui_msg_t *msg)
{
    __s32 ret;
    rat_media_type_t media_type ;
    file_item_t *file_item ;
    char FilePath[RAT_MAX_FULL_PATH_LEN] ;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    eLIBs_memset(FilePath, 0, sizeof(FilePath));
    ret = GetListItemFileFullPath(list_para, list_para->rat.index, FilePath);
    ret = eLIBs_remove(FilePath);

    if (EPDK_FAIL == ret)
    {
        __msg(" delete file fail ");
        //explorer_create_delete_failed_dialog(msg);
        return EPDK_FAIL;
    }

    delete_file_item_from_tree_list(list_para, FilePath);
    explorer_filelist_update(list_para);
    //list_para->filelist_update = EPDK_TRUE;
    return EPDK_OK;
}

__s32 explorer_list_play_file(explr_list_para_t *list_para)
{
    __s32 ret;
    char FileName[RAT_MAX_FULL_PATH_LEN] = { 0 };   //全文件名即包括盘符
    rat_media_type_t media_type ;
    file_item_t file_item;
    GetListItemFileName(list_para, list_para->rat.index, file_item.name);
    __msg("File_manager_play_file filename = %s ", file_item.name);
    media_type = rat_get_file_type(file_item.name) ;
    __msg("media_type = %d", media_type);
    //list_para->rat.handle = rat_open(list_para->search_path , media_type , 0 );   //全部搜索
    list_para->rat.handle = rat_open_ex(list_para->root_type, media_type, 0);   //全部搜索
    __msg("list_para->rat.handle=%d", list_para->rat.handle);
    eLIBs_strcat(FileName, list_para->search_path) ;
    eLIBs_strcat(FileName, "\\") ;
    eLIBs_strcat(FileName, file_item.name) ;
    __msg(" FileName = %s ", FileName);
    ret = rat_set_file_for_play(list_para->rat.handle, FileName);
    __msg("~~~~Set play file name is  = %s", FileName);

    if (EPDK_FAIL == ret)
    {
        __msg(" rat set file for play fail ");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

__s32 File_manager_play_file(explr_list_para_t *list_para)
{
    __s32 ret = 0;
    char FileName[RAT_MAX_FULL_PATH_LEN] = {0}; //全路径
    rat_media_type_t media_type ;
    __u8 root_type = 0;
    __u8  fatdirattr = 0;
    file_item_t *file_item = NULL;
    char  path[RAT_MAX_FULL_PATH_LEN] = {0};
    file_item = get_file_list_item(list_para->cur_file_list, list_para->rat.index);
    eLIBs_strcpy(path, list_para->cur_file_list->file_path);
    eLIBs_strcat(path, "\\");
    eLIBs_strcat(path, file_item->name);
    __msg("File_manager_play_file  path = %s  ", path);
    eLIBs_strcpy(FileName, path) ;
    __msg("full_file_name=%s", FileName);
    fatdirattr = list_para->top_file_list->cur_item->fatdirattr;

    if (fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        root_type = RAT_USB;
    }
    else if (fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        root_type = RAT_TF;
    }
    else
    {
        root_type = 0;
    }

    media_type = rat_get_file_type(file_item->name);
    __msg("media_type = %d", media_type);

    if (RAT_MEDIA_TYPE_AUDIO == media_type)
    {
        __msg("audio media type break ");
        return EPDK_OK ;
    }

    list_para->root_type = root_type ;
    ExplorerListWinGetSearchPath(list_para);
    //list_para->rat.handle = rat_open(list_para->search_path , media_type , 0 );   //全部搜索
    list_para->rat.handle = rat_open_ex(list_para->root_type, media_type, 0);   //全部搜索
    list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;
    __msg("list_para->rat.handle=%d", list_para->rat.handle);
    ret = rat_set_file_for_play(list_para->rat.handle, FileName);

    if (EPDK_FAIL == ret)
    {
        __msg(" rat set file for play fail ");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

static __s32  file_manager_set_current_file_for_play(__gui_msg_t *msg)
{
    __s32 ret;
    H_WIN hExpWin = NULL;
    explr_list_para_t *list_para;
    rat_media_type_t media_type;
    file_item_t *file_item ;
    __u8 root_type = 0;
    __u8  fatdirattr;                   //add by libaiao,2011.8.24
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    //在文件进行播放之前，先停掉listbar,以防listbar滑动线程运行
    explorer_listbar_uninit(list_para->list_win);
    explorer_mediatype_listbar_uninit(list_para->list_win);
    ret = File_manager_play_file(list_para);

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL ;
    }

    //add by libaiao,2011.8.24
    fatdirattr = list_para->top_file_list->cur_item->fatdirattr;

    if (fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
    {
        root_type = RAT_USB;
    }
    else if (fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
    {
        root_type = RAT_TF;
    }
    else
    {
        root_type = 0;
    }

    list_para->root_para->explr_root = root_type ;
    __msg("file_manager_set_current_file_for_play root_type = %d", root_type);
    file_item = get_file_list_item(list_para->cur_file_list, list_para->rat.index);

    if (! file_item)
    {
        __msg(" file item is null ");
        return EPDK_FAIL ;
    }

    __msg("list_para->rat.index = %d file_item->name = %s  ", \
          list_para->rat.index, file_item->name);
    media_type = rat_get_file_type(file_item->name);
    __msg("media_type = %d", media_type);

    switch (media_type)
    {
        case RAT_MEDIA_TYPE_ALL:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            break;

        case RAT_MEDIA_TYPE_PIC:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_PHOTO, 0);
            break;

        case RAT_MEDIA_TYPE_AUDIO:
        {
            file_item_t *file_item = NULL;
            char  path[RAT_MAX_FULL_PATH_LEN] = { 0 };
            file_item = list_para->cur_file_list->cur_item;
            {
                // 获取全路径
                eLIBs_strcpy(path, list_para->cur_file_list->file_path);
                eLIBs_strcat(path, "\\");
                eLIBs_strcat(path, file_item->name);
            }
            __msg("file_manager_set_current_file_for_play  path = %s  ", path);
            eLIBs_strcpy(list_para->root_para->music_file_name, path) ;
            __msg("full_file_name=%s", list_para->root_para->music_file_name);
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MUSIC, 0);
        }
        break;

        case RAT_MEDIA_TYPE_VIDEO:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MOVIE, 0);
            break;

        case RAT_MEDIA_TYPE_EBOOK:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_EBOOK, 0);
            break;

        case RAT_MEDIA_TYPE_GAME:
            //          explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_GAME, 0);
            break;

        default:
            break;
    }

    hExpWin = GUI_WinGetParent(msg->h_deswin);
    delete_file_list_nod(list_para->cur_file_list);
    ExplorerSendMessage(hExpWin, GUI_MSG_CLOSE, 0, 0);
    return EPDK_OK;
}

//
/*
********************************************************************************************
*Function   :      H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para)
*
*Description    :       根据ManagerWin传递过来的参数创建FrameWIn场景
*
*Arguments      :
*
*Return         :
*
********************************************************************************************
*/
H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para)
{
    __gui_framewincreate_para_t framewin_para;
    explr_list_para_t *list_para = NULL;
    FB fb;
    GUI_LyrWinGetFB(para->list_layer, &fb);
    list_para = (explr_list_para_t *)esMEMS_Malloc(0, sizeof(explr_list_para_t));

    if (NULL == list_para)
    {
        __inf("memory alloc fail , return ");
        return (H_WIN)EPDK_FAIL ;
    }

    eLIBs_memset((void *)list_para, 0, sizeof(explr_list_para_t));
    list_para->explr_list_font = para->explr_list_font;
    list_para->BG_layer = para->BG_layer;
    list_para->list_layer = para->list_layer;
    list_para->core = para->core;
    list_para->media_type = para->media_type;       //photo, music, movie, ebook
    list_para->root_type = para->root_type;         //SD or USB device
    list_para->mediatype_index = para->mediatype_index ;
    list_para->listbar_left_right = LISTBAR_RIGHT;
    list_para->view_mode = para->view_mode;         //list or square
    list_para->root_para = para->root_para ;
    ExplorerListWinGetSearchPath(list_para);    // 获取搜索路径，( list_para->search_path)
    //eLIBs_memcpy(list_para->search_path,"e:", sizeof("e:"));
    eLIBs_memset(&framewin_para, 0, sizeof(__gui_framewincreate_para_t));
    framewin_para.name =    "Explorer List window",
    framewin_para.dwStyle = WS_NONE | WS_VISIBLE;
    framewin_para.spCaption =  NULL;
    framewin_para.hOwner    = NULL;
    framewin_para.id        = EXPLR_LIST_WIN_ID;
    framewin_para.hHosting = h_parent;
    framewin_para.FrameWinProc = (__pGUI_WIN_CB)esKRNL_GetCallBack((__pCBK_t)_explorer_list_win_cb);
    framewin_para.rect.x = 0;
    framewin_para.rect.y = 0;
    framewin_para.rect.width = fb.size.width;
    framewin_para.rect.height = fb.size.height;
    framewin_para.BkColor.alpha =  0;
    framewin_para.BkColor.red = 0;
    framewin_para.BkColor.green = 255;
    framewin_para.BkColor.blue = 0;
    framewin_para.attr = (void *)list_para;
    framewin_para.hLayer = list_para->list_layer;           //会被listbar调用
    return (GUI_FrmWinCreate(&framewin_para));
}


//文件管理模式，跳过sd/usb选择界面 ，进入下一级菜?
static __s32 explorer_file_manager_ignor_sd_usb(__gui_msg_t *msg)
{
    __s32 ret = 0;
    explr_list_para_t *list_para = NULL;
    file_list_t *pfilelist = NULL;
    file_item_t *pfileitem = NULL;
    __inf("Enter %s\r", __FUNCTION__);
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    //文件管理模式，跳过sd/usb选择界面 ，进入下一级菜单
    //if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        //list_para->enter_sd_usb_flag = 0;
        //先更新一页，否则file_manager_list_on_enter_key 失败
        //  LISTBAR_ShowPage(list_para->mediatype_listbar_handle); // shiql change for D100 20160715
        LISTBAR_ShowPage(list_para->listbar_handle);

        if (list_para->listbar_left_right == LISTBAR_LEFT)
        {
            LISTBAR_LostFocus(list_para->listbar_handle);
        }

        pfilelist = list_para->top_file_list;
        pfileitem = list_para->top_file_list->cur_item;
        __wrn("pfilelist==%08x", pfilelist);
        __wrn("list_para->cur_file_list->total=%d", list_para->cur_file_list->total);
        __wrn("list_para->root_type= %d", list_para->root_type);
        __wrn("pfilelist->item_list->name=%s", pfilelist->item_list->name);
        __wrn("pfilelist->item_list->fatdirattr=%d", pfilelist->item_list->fatdirattr);
        __wrn("pfilelist->item_list->next->name=%s", pfilelist->item_list->next->name);
        __wrn("pfilelist->item_list->next->fatdirattr=%d", pfilelist->item_list->next->fatdirattr);
        //..__wrn("pfilelist->child->item_list->fatdirattr=%d",pfilelist->child->item_list->fatdirattr);
        __wrn("pfilelist->child=%08x", pfilelist->child);

        if (RAT_TF == list_para->root_type)
        {
            /*while(pfilelist && RAT_MEDIA_TYPE_SD_DEVICE != pfilelist->item_list->fatdirattr)
            {
            __wrn("1111");
                pfilelist = pfilelist->child;


            }*/
            while (pfileitem && RAT_MEDIA_TYPE_SD_DEVICE != pfileitem->fatdirattr)
            {
                __wrn("1111");
                pfileitem = pfileitem->next;
            }

            __wrn("pfilelist1=%08x", pfilelist);
            //list_para->cur_file_list = pfilelist;
            //list_para->cur_file_list->cur_item = list_para->cur_file_list->item_list;
            list_para->cur_file_list->cur_item = pfileitem;
            __wrn("list_para->cur_file_list->cur_item1=%08x", list_para->cur_file_list->cur_item);
        }
        else if (RAT_USB == list_para->root_type)
        {
#if 0

            while (pfilelist && RAT_MEDIA_TYPE_USB_DEVICE != pfilelist->item_list->fatdirattr)
            {
                pfilelist = pfilelist->child;
            }

            __wrn("pfilelist2=%08x", pfilelist);
            list_para->cur_file_list = pfilelist;
            list_para->cur_file_list->cur_item = list_para->cur_file_list->item_list;
            __wrn("list_para->cur_file_list->cur_item2=%08x", list_para->cur_file_list->cur_item);
#else

            while (pfileitem && RAT_MEDIA_TYPE_USB_DEVICE != pfileitem->fatdirattr)
            {
                __wrn("1111");
                pfileitem = pfileitem->next;
            }

            __wrn("pfilelist1=%08x", pfilelist);
            //list_para->cur_file_list = pfilelist;
            //list_para->cur_file_list->cur_item = list_para->cur_file_list->item_list;
            list_para->cur_file_list->cur_item = pfileitem;
#endif
        }

        file_manager_list_on_enter_key(msg);
    }
    return 0;
}


/*
*****************************************************************************************
*Function   :           static __s32 explorer_listview_onpaint(__gui_msg_t *msg)
*
*Description    :       OnPaint处理函数,画背景图，前景ListBar showpage, 其他icon元素
*
*Arguments      :
*
*Return         :
******************************************************************************************
*/
static __s32 explorer_listview_onpaint(__gui_msg_t *msg)
{
    __s32 ret = 0;
    explr_list_para_t *list_para = NULL;
    explorer_viewer_ui_t *ui_param = NULL;
    H_WIN  list_win;
    file_list_t *pfilelist = NULL;
    //GUI_MEMDEV_Handle draw_mem;
    __inf("Enter %s\r", __FUNCTION__);
    list_win = msg->h_deswin;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    ui_param = explorer_get_viewer_ui_param();

    if (GUI_LyrWinGetSta(list_para->list_layer) == GUI_LYRWIN_STA_SUSPEND)
    {
        return EPDK_FAIL;
    }

    if (GUI_LyrWinGetSta(list_para->list_layer) == GUI_LYRWIN_STA_ON)
    {
        __msg("to make sure layer status on");
    }

    //draw_mem = GUI_MEMDEV_Create( 0 , 0 , 480 , 272 );
    //GUI_MEMDEV_Select(draw_mem);

    if (list_para->BG_layer)
    {
        if (GUI_LyrWinGetSta(list_para->BG_layer) == GUI_LYRWIN_STA_SUSPEND)
        {
            __inf("%s %d list_para->BG_layer GUI_LYRWIN_STA_SUSPEND\r", __FILE__, __LINE__);
            return EPDK_FAIL;
        }

        GUI_LyrWinSetBottom(list_para->BG_layer);
    }

    GUI_LyrWinSetTop(list_para->list_layer);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_WinSetFocusChild(list_win);     //frame win get focus
#if 0

    if (RAT_TF == list_para->root_type)
    {
        explorer_list_draw_sd(list_para, FOCUSED);
        explorer_list_draw_usb(list_para, UNFOCUSED);
    }
    else if (RAT_USB == list_para->root_type)
    {
        explorer_list_draw_sd(list_para, UNFOCUSED);
        explorer_list_draw_usb(list_para, FOCUSED);
    }

#endif
    __wrn("list_para->media_type==%d", list_para->media_type);

    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        __wrn("explorer_file_manager_ignor_sd_usb");
        explorer_file_manager_ignor_sd_usb(msg);
    }
    else
    {
        //       LISTBAR_ShowPage(list_para->mediatype_listbar_handle); // shiql change this for d100 20160715
        LISTBAR_ShowPage(list_para->listbar_handle);                //  绘制右侧的listbar
    }

    //    LISTBAR_LostFocus(list_para->mediatype_listbar_handle);// shiql change this for d100 20160715
    //explorer_draw_file_type( list_para, list_para->media_type );  // 绘制左侧的文件类型
    //GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    //GUI_MEMDEV_CopyToLCD(draw_mem);
    //GUI_MEMDEV_Select( NULL );
    //GUI_MEMDEV_Delete(draw_mem);
    //draw_mem = NULL;
    return EPDK_OK;
}

/*
*************************************************************************************************
*Function   :           static __s32  explorer_list_win_on_close(__gui_msg_t *msg)
*
*Description    :       OnClose处理函数，释放FrameWin
*
*Arguments      :
*
*Return         :
*************************************************************************************************
*/
static __s32  explorer_list_win_on_close(__gui_msg_t *msg)
{
    //theme_close();
    GUI_FrmWinDelete(msg->h_deswin);            //send destroy message
    return EPDK_OK;
}

/*
*******************************************************************************************
*Function   :           static __s32  explorer_list_win_on_destroy(__gui_msg_t *msg)
*
*Description    :       OnDestroy处理函数，释放framewin 所有申请的资源
*
*Arguments      :
*
*Return         :
*
********************************************************************************************
*/
static __s32  explorer_list_win_on_destroy(__gui_msg_t *msg)
{
    explr_list_para_t *list_para = NULL ;
    __u8 err ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (!list_para)
    {
        __msg("explorer list win get win attribute fail ");
        return EPDK_FAIL ;
    }

    if (GUI_IsTimerInstalled(list_para->list_win, ExplorerTimerId))
    {
        GUI_KillTimer(list_para->list_win, ExplorerTimerId);
    }

    if (GUI_IsTimerInstalled(list_para->list_win, ExplorerDrawBgMusicIndexTimer))
    {
        GUI_KillTimer(list_para->list_win, ExplorerDrawBgMusicIndexTimer);
    }

    if (list_para->h_dialog != NULL)
    {
        app_dialog_destroy(list_para->h_dialog);        //to delete dialog
        list_para->h_dialog = NULL ;
    }

    if (list_para->h_dialog_msg != NULL)
    {
        app_dialog_destroy(list_para->h_dialog_msg);        //to delete dialog
        list_para->h_dialog_msg = NULL ;
    }

    if (list_para->h_fw_update_dlg != NULL)
    {
        app_dialog_destroy(list_para->h_fw_update_dlg);
        list_para->h_fw_update_dlg = NULL;
    }

#if (EXPLORER_SUPPORT_MINIATURE == 1)

    if (list_para->listbar_miniature_buf)
    {
        esMEMS_Bfree(list_para->listbar_miniature_buf, list_para->listbar_miniature_size);
        list_para->listbar_miniature_buf = NULL ;
    }

    if (list_para->big_miniature_buf)
    {
        esMEMS_Bfree(list_para->big_miniature_buf, list_para->big_miniature_size);
        list_para->big_miniature_buf = NULL ;
    }

#endif
    explorer_list_long_string_uninit(list_para);        //释放long string scroll
    explorer_listbar_uninit(msg->h_deswin);         // listbar uninit 应该放在删除文件列表之前
    explorer_mediatype_listbar_uninit(list_para->list_win);
#if (EXPLORER_SUPPORT_MINIATURE == 1)

    if (list_para->get_miniature_sem)
    {
        esKRNL_SemDel(list_para->get_miniature_sem, OS_DEL_ALWAYS, &err);
        __msg("error=%d", err);
        list_para->get_miniature_sem = NULL ;
    }

#endif

    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        explorer_file_list_uninit(list_para);   //删除文件列表
    }
    else
    {
        explorer_file_list_uninit(list_para);   //删除文件链表( 指向sd 卡, usb 的链表)
        //explorer_save_last_para(list_para);
    }

    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
    }
    else
    {
        explorer_rat_uninit(list_para);
    }

    explorer_free_listview_icon_res();

    if (list_para->handle_thread_id != 0)
    {
        //__here__;
        while (esKRNL_TDelReq(list_para->handle_thread_id) != OS_TASK_NOT_EXIST)
        {
            esKRNL_TimeDly(5);
        }

        list_para->handle_thread_id = 0;
        __msg(" thread has been deleted.");
    }

    if (list_para)
    {
        esMEMS_Mfree(0, list_para);
        list_para = NULL ;
    }

    //if(rat_is_encryption_partition_Insert(NULL,1))//防止插
#if (BEETLES_GAME_FLAG == 1)
    //if(rat_is_encryption_partition_Insert(NULL,1))//防止插
    USBJoystickPlugOut(NULL);
#endif
    return EPDK_OK;
}

/*
*********************************************************************************************
*Function   :          __s32 ExplorerListWinOnEnterKey(explr_list_para_t *list_para)
*
*Description    :       EnterKey处理函数
*
*Arguments      :
*
*Return         :
*
*********************************************************************************************
*/
__s32 ExplorerListWinOnEnterKey(explr_list_para_t *list_para)
{
    char FileName[RAT_MAX_FULL_PATH_LEN] = {0};
    __s32 ret = 0;
    ret = GetListItemFileFullPath(list_para, list_para->rat.index, FileName);

    if (EPDK_FAIL == ret)
    {
        __msg(" GetListItemFileFullPath fail ");
        return EPDK_FAIL;
    }

    __msg("~~~~Set play file name is  = %s", FileName);
    ret = rat_set_file_for_play(list_para->rat.handle, FileName);

    if (EPDK_FAIL == ret)
    {
        __msg("rat_set_file_for_play fail ");
        return EPDK_FAIL ;
    }

    /*
        if(list_para->view_mode == EXPLR_LIST_MODE)
        {
            ExplorerSetViewMode(list_para, EXPLR_SQUARE_MODE);
        }
        else if(list_para->view_mode == EXPLR_SQUARE_MODE)
        {
            ExplorerSetViewMode(list_para, EXPLR_LIST_MODE);
        }
    */
    return EPDK_OK;
}


static __s32 explorer_mediatype_update(__gui_msg_t *msg)
{
    __s32 media_type_bak;
    __s32 index;
    explr_list_para_t *list_para = NULL ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    index = LISTBAR_GetFocusItem(list_para->mediatype_listbar_handle);
    media_type_bak = list_para->media_type;
    list_para->media_type = mediatype_tab[index];
    list_para->mediatype_index = index;
    explorer_listbar_uninit(list_para->list_win);

    if (media_type_bak == RAT_MEDIA_TYPE_ALL
        && list_para->media_type != RAT_MEDIA_TYPE_ALL)
    {
        __wrn("explorer_file_list_uninit....");
        explorer_file_list_uninit(list_para);
        list_para->enter_sd_usb_flag = 0;
    }
    else if (list_para->media_type == RAT_MEDIA_TYPE_ALL
             && media_type_bak != RAT_MEDIA_TYPE_ALL)
    {
        __wrn("explorer_file_list_init....");
        explorer_file_list_init(list_para) ;
    }

    GUI_LyrWinSel(list_para->list_layer);
    __wrn("list_para->enter_sd_usb_flag = %d.....", list_para->enter_sd_usb_flag);
    /*if(0 == list_para->enter_sd_usb_flag
        && RAT_MEDIA_TYPE_ALL == list_para->media_type)
    {
        __wrn("explorer_file_manager_ignor_sd_usb11....");
        explorer_file_manager_ignor_sd_usb(msg);
    }*/
    //文件管理模式，跳过sd/usb选择界面 ，进入下一级菜单
    //if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        //    rat_close(list_para->rat.handle);
        //  explorer_rat_uninit(list_para);
        //  explorer_file_manager_rebuild(msg);
    }
    //else
    {
        //if(RAT_MEDIA_TYPE_ALL == media_type_bak)
        {
            //delete_file_list_chain( list_para->top_file_list ) ;
            //list_para->top_file_list = NULL ;
            //  explorer_file_list_uninit(list_para);
            //  explorer_file_list_init(list_para);
            //  explorer_rat_init(list_para);
        }

        if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
        {
            //if(EPDK_TRUE == list_para->filelist_update)
            {
                //list_para->filelist_update = EPDK_FAIL;
                //explorer_update_file_list_nod(list_para);
                //explorer_filelist_update(list_para);
            }
            //else
            {
                list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                list_para->rat.index = 0 ;
                list_para->rat.total = list_para->cur_file_list->total;
            }
        }
        else
        {
            list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
            list_para->rat.index = 0 ;
            list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;
        }
    }
    __wrn(".............");
    /**if( list_para->rat.total == 0)
    {

        __s32 lang_id[]={STRING_EXPLR_CUE , STRING_EXPLR_FOLDER_EMPTY };
        lang_id[1] = folder_emtpy[list_para->mediatype_index];

        default_dialog( list_para->h_dialog , GUI_WinGetParent(msg->h_deswin), 0x2,\
            ADLG_OK, lang_id);
        esKRNL_TimeDly(100);
        app_dialog_destroy(list_para->h_dialog);
        list_para->h_dialog = NULL ;
        explorer_list_long_string_stop_roll(list_para);
    }**/
    //list_para->last_start_id = 0;
    //list_para->last_focused_id = 0;
    //list_para->rat.index = 0;
    //list_para->last_focus_index = 0;
    explorer_get_last_para(list_para);
    //explorer_mediatype_listbar_init(list_para->list_win);
    //explorer_listbar_uninit(list_para->list_win);
    //为RAT_MEDIA_TYPE_ALL时explorer_file_manager_ignor_sd_usb有调用explorer_listbar_init()
    //在这里再次调用会导致死机
    //..if(list_para->media_type != RAT_MEDIA_TYPE_ALL)
    //..{
    explorer_listbar_init(list_para->list_win);
    //..}

    //..LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
    //..LISTBAR_ShowPage(list_para->listbar_handle);
    if (0 == list_para->enter_sd_usb_flag
        && RAT_MEDIA_TYPE_ALL == list_para->media_type)
    {
        __inf("explorer_file_manager_ignor_sd_usb11....");
        explorer_file_manager_ignor_sd_usb(msg);
    }
    else
    {
        //__here__
#if 0 // shiql solve  mediatype_listbar_handle item blink
        LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
#endif
        LISTBAR_ShowPage(list_para->listbar_handle);                //  绘制右侧的listbar
    }

    if (list_para->rat.total == 0)
    {
        __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
        lang_id[1] = folder_emtpy[list_para->mediatype_index];
        default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                       ADLG_OK, lang_id);
        esKRNL_TimeDly(100);
        app_dialog_destroy(list_para->h_dialog);
        list_para->h_dialog = NULL ;
        explorer_list_long_string_stop_roll(list_para);
    }

    //__here__
#if 0 // shiql solve  mediatype_listbar_handle item blink

    if (list_para->listbar_left_right == LISTBAR_LEFT)
    {
        //__here__
        LISTBAR_LostFocus(list_para->listbar_handle);
    }

#endif
    return 0;
}

/*
***********************************************************************************************
*Function   :          __s32 ExplorerMediatypeListWinOnNextItem(__gui_msg_t *msg)
*
*Description    :       向下一个条目处理函数,在NextKey中被调用
*
*Arguments      :
*
*Return         :
*
***********************************************************************************************
*/

__s32 ExplorerMediatypeListWinOnNextItem(__gui_msg_t *msg)
{
    explr_list_para_t *list_para = NULL ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    LISTBAR_NextItem(list_para->mediatype_listbar_handle);
    explorer_mediatype_update(msg);
    return EPDK_OK;
}

static __s32 explorer_list_win_update(explr_list_para_t *list_para)
{
    __listbar_scene_t ListBarScene;
    LISTBAR_UpdatePage(list_para->listbar_handle);
    eLIBs_memset(&ListBarScene, 0x00, sizeof(ListBarScene));
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    __inf("listbar start id = %d", ListBarScene.start_id);
    __inf("listbar focus id = %d", ListBarScene.focus_id);
    list_para->rat.index = ListBarScene.focus_id;
    return EPDK_OK;
}

/*
***********************************************************************************************
*Function   :          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description    :       向下一个条目处理函数,在NextKey中被调用
*
*Arguments      :
*
*Return         :
*
***********************************************************************************************
*/

__s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
{
    __listbar_scene_t ListBarScene;
    LISTBAR_NextItem(list_para->listbar_handle);
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    __msg("listbar start id = %d", ListBarScene.start_id);
    __msg("listbar focus id = %d", ListBarScene.focus_id);
    list_para->rat.index = ListBarScene.focus_id;
    return EPDK_OK;
}

/*
********************************************************************************************
*Function   :          __s32 ExplorerMediatypeListWinOnPreviousItem(__gui_msg_t *msg)
*
*Description    :       向上一个条目处理函数,在PreviousKey中被调用
*
*Arguments      :
*
*Return         :
********************************************************************************************
*/
__s32 ExplorerMediatypeListWinOnPreviousItem(__gui_msg_t *msg)
{
    explr_list_para_t *list_para = NULL ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    LISTBAR_PrevItem(list_para->mediatype_listbar_handle);
    explorer_mediatype_update(msg);
    return EPDK_OK;
}



/*
********************************************************************************************
*Function   :          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description    :       向上一个条目处理函数,在PreviousKey中被调用
*
*Arguments      :
*
*Return         :
********************************************************************************************
*/
__s32 ExplorerListWinOnPreviousItem(explr_list_para_t *list_para)
{
    __listbar_scene_t ListBarScene;
    LISTBAR_PrevItem(list_para->listbar_handle);
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    __msg("listbar start id = %d", ListBarScene.start_id);
    __msg("listbar focus id = %d", ListBarScene.focus_id);
    list_para->rat.index = ListBarScene.focus_id;
    //  list_para->cur_file_list->cur_item = list_para->cur_file_list->cur_item->previous;
    return EPDK_OK;
}

/*
**********************************************************************************************
*Function   :          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description    :       响应上一行条目处理函数,在PreviousKey中被调用
*
*Arguments      :
*
*Return         :
*
***********************************************************************************************
*/
__s32 ExplorerListWinOnNextRow(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 RowCount = LISTBAR_GetRowItemCount(list_para->listbar_handle);    //获得一行的条目数
    ret = rat_move_cursor_forward(list_para->rat.handle, RowCount);

    if (ret == 0)
    {
        if (rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!");
            return EPDK_FAIL;
        }
    }

    list_para->rat.index += ret;            //ret为实际向下移动的条目数

    if (list_para->rat.index >= list_para->rat.total)
    {
        list_para->rat.total = list_para->rat.index;
        LISTBAR_SetTotalItem(list_para->listbar_handle, list_para->rat.total);
    }

    LISTBAR_NextRow(list_para->listbar_handle);
    return EPDK_OK;
}

/*
**********************************************************************************************
*Function   :          __s32 ExplorerListWinOnNextItem(explr_list_para_t *list_para)
*
*Description    :       向上一行条目处理函数,在PreviousKey中被调用
*
*Arguments      :
*
*Return         :
*
**********************************************************************************************
*/
__s32 ExplorerListWinOnPreviousRow(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 RowCount = LISTBAR_GetRowItemCount(list_para->listbar_handle);    //获得一行的条目数

    if (list_para->rat.index <= 0)
    {
        return EPDK_FAIL;
    }

    ret = rat_move_cursor_backward(list_para->rat.handle, RowCount);

    if (ret == 0)
    {
        if (rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!");
            return EPDK_FAIL;
        }
    }

    if (list_para->rat.index >= ret)
    {
        list_para->rat.index -= ret;            //ret为实际向下移动的条目数
    }
    else
    {
        list_para->rat.index = 0;               //已经在第一页，定位到第一个条目
        LISTBAR_SetFocusItem(list_para->listbar_handle, list_para->rat.index);
        //__wrn("Explorer_list.c, line296, there is a exception with listview!!!");
        return EPDK_OK;
    }

    LISTBAR_PrevRow(list_para->listbar_handle);
    return EPDK_OK;
}


//向下一页处理函数
__s32 ExplorerListWinOnNextPage(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 PageCount = LISTBAR_GetPageItemCount(list_para->listbar_handle);  //获得一行的条目数

    if (list_para->rat.index + PageCount >= list_para->rat.total)
    {
        return EPDK_FAIL;           //only for test
    }

    ret = rat_move_cursor_forward(list_para->rat.handle, PageCount);

    if (ret == 0)
    {
        if (rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!");
            return EPDK_FAIL;
        }
    }

    /*
    if(list_para->rat.index + ret >= list_para->rat.total)
    {
        list_para->rat.total = list_para->rat.index;
        LISTBAR_SetTotalItem(list_para->listbar_handle,list_para->rat.total);
    }
    */
    {
        __listbar_scene_t ListBarScene;
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d", ListBarScene.start_id);
        __msg("listbar focus id = %d", ListBarScene.focus_id);
    }
    //list_para->rat.index += ret;          //ret为实际向下移动的条目数
    {
        __listbar_scene_t ListBarScene;
        LISTBAR_NextPage(list_para->listbar_handle);
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d", ListBarScene.start_id);
        __msg("listbar focus id = %d", ListBarScene.focus_id);
        list_para->rat.index = ListBarScene.focus_id;
    }
    //LISTBAR_SetFocusItem(list_para->listbar_handle, list_para->rat.index);
    return EPDK_OK;
}


//向上一页处理函数
__s32 ExplorerListWinOnPreviousPage(explr_list_para_t *list_para)
{
    __s32 ret;
    __s32 PageCount = LISTBAR_GetPageItemCount(list_para->listbar_handle);  //获得一行的条目数

    if (list_para->rat.index <= 0)
    {
        return EPDK_FAIL;
    }

    if (list_para->rat.index < PageCount)       //only for test
    {
        return EPDK_FAIL;
    }

    ret = rat_move_cursor_backward(list_para->rat.handle, PageCount);

    if (ret == 0)
    {
        if (rat_is_cursor_end(list_para->rat.handle))
        {
            return EPDK_FAIL;
        }
        else
        {
            __wrn("there is a exception in rat!!!");
            return EPDK_FAIL;
        }
    }

    {
        __listbar_scene_t ListBarScene;
        LISTBAR_PrevPage(list_para->listbar_handle);
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        __msg("listbar start id = %d", ListBarScene.start_id);
        __msg("listbar focus id = %d", ListBarScene.focus_id);
        list_para->rat.index = ListBarScene.focus_id;
    }

    //LISTBAR_SetFocusItem(list_para->listbar_handle, list_para->rat.index);
    return EPDK_OK;
}

__s32 explorer_create_delete_failed_dialog(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_DELETE_FAILED };
    H_WIN list_win;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    list_win = list_para->list_win;
    //list_para->del_dlg_open = EPDK_TRUE;
    //explorer_draw_delete_file_failed();
    __msg("--------------draw del file dialog!");
    lang_id[1] = folder_emtpy[list_para->mediatype_index];
    default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                   ADLG_OK, lang_id);
    esKRNL_TimeDly(100);
    app_dialog_destroy(list_para->h_dialog);
    list_para->h_dialog = NULL ;

    if (GUI_IsTimerInstalled(list_win, ExplorerTimerId))
    {
        GUI_ResetTimer(list_win, ExplorerTimerId, C_EXP_TIMER_INTERVAL, NULL);
        __msg("GUI_reset timer");
    }

    return 0;
}
static __s32  file_manager_list_on_delete(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    file_item_t *file_item = NULL;
    int ret = 0;
    __listbar_scene_t ListBarScene;
    char  path[RAT_MAX_FULL_PATH_LEN];
    __s32 page_item_cnt;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list->total < 1)
    {
        return EPDK_FAIL;
    }

    file_item = list_para->cur_file_list->cur_item;

    if (file_item == NULL)
    {
        return EPDK_FAIL;
    }

    eLIBs_memset(path, 0, sizeof(path));
    eLIBs_strcpy(path, list_para->cur_file_list->file_path);
    eLIBs_strcat(path, "\\");
    eLIBs_strcat(path, file_item->name);

    if (file_item->fatdirattr & FSYS_ATTR_DIRECTORY)
    {
        if (file_item->name_len != 0)
        {
            //ret = eLIBs_rmdir(path);
            {
                file_op_info_t op_info;
                eLIBs_memset(&op_info, 0, sizeof(file_op_info_t));
                ret = get_item_num_in_dir(path, &(op_info.total_num));

                if (EPDK_FAIL == ret)
                {
                    return EPDK_FAIL;
                }

                //op_info.OnDraw = explorer_delete_file_draw_process;
                __msg("start to delect folder  ");
                //explorer_delete_file_draw_process_ext( list_para );
                ret = delete_dir(path, &op_info);

                if (ret != 0)
                {
                    LISTBAR_ShowPage(list_para->listbar_handle);

                    if (list_para->listbar_left_right == LISTBAR_LEFT)
                    {
                        LISTBAR_LostFocus(list_para->listbar_handle);
                    }

                    //explorer_create_delete_failed_dialog(msg);
                    //LISTBAR_ShowPage(list_para->listbar_handle);
                    return EPDK_FAIL;
                }
            }
        }
    }
    else
    {
        if (file_item->name_len != 0)
        {
            __msg("start to  delete file ");
            //explorer_delete_file_draw_process_ext( list_para );
            ret = eLIBs_remove(path);

            if (ret != 0)   //删除失败
            {
                //LISTBAR_ShowPage(list_para->listbar_handle);
                //explorer_create_delete_failed_dialog(msg);
                //LISTBAR_ShowPage(list_para->listbar_handle);
                return EPDK_FAIL;
            }
        }
    }

    //explorer_clean_delete_file_hint_area();
    //保护现场
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    list_para->cur_file_list->start_id = ListBarScene.start_id;
    list_para->cur_file_list->focus_id = ListBarScene.focus_id;
    list_para->cur_file_list->total -= 1;
    list_para->rat.total = list_para->cur_file_list->total;

    if (list_para->cur_file_list->focus_id >= list_para->rat.total)
    {
        list_para->cur_file_list->focus_id = list_para->rat.total;
        ListBarScene.focus_id = list_para->rat.total;
    }

    page_item_cnt = explorer_get_listbar_pagenum(list_para);
    //list_para->last_start_id = ListBarScene.start_id;
    list_para->last_focused_id = ListBarScene.focus_id;
    list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_item_cnt);

    if (list_para->last_focused_id >= list_para->rat.total)
    {
        if (list_para->rat.total > 1)
        {
            if (list_para->last_start_id > 0)
            {
                list_para->last_start_id--;
            }

            list_para->last_focused_id = list_para->rat.total - 1;
        }
        else
        {
            list_para->last_focused_id = 0;
        }
    }

    ret = delete_file_item_from_list(file_item, list_para->cur_file_list);  // 删除条目
    explorer_listbar_uninit(list_para->list_win);   //重新创建listbar,因为Square为全屏模式
    explorer_listbar_init(list_para->list_win);
    LISTBAR_ShowPage(list_para->listbar_handle);

    if (list_para->listbar_left_right == LISTBAR_LEFT)
    {
        LISTBAR_LostFocus(list_para->listbar_handle);
    }

    return EPDK_OK;
}


//文件管理回到上一级菜单,不再退回到SD/USB盘符选择界面，直接退出
static __s32  file_manager_list_on_backspace(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    file_list_t *parent_file_list = NULL;
    __s32 page_item_cnt;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list->parent == list_para->top_file_list)
    {
        parent_file_list = list_para->cur_file_list->parent;
        delete_file_list_nod(list_para->cur_file_list);
        list_para->cur_file_list = parent_file_list;

        if (list_para->cur_file_list)
        {
            list_para->cur_file_list->child = NULL;
        }

        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        return EPDK_OK;
    }

    if (list_para->cur_file_list->parent == NULL)
    {
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        return EPDK_OK;
    }

    parent_file_list = list_para->cur_file_list->parent;
    delete_file_list_nod(list_para->cur_file_list);
    list_para->cur_file_list = parent_file_list;

    if (list_para->cur_file_list)
    {
        list_para->cur_file_list->child = NULL;
    }

    if (list_para->cur_file_list == list_para->top_file_list)
    {
        list_para->enter_sd_usb_flag = 0 ;
    }

    page_item_cnt = explorer_get_listbar_pagenum(list_para);
    //list_para->last_start_id = list_para->cur_file_list->start_id;
    list_para->last_focused_id = list_para->cur_file_list->focus_id;
    list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_item_cnt);
    list_para->rat.index = list_para->last_focused_id;
    list_para->rat.total = list_para->cur_file_list->total;
    explorer_listbar_uninit(list_para->list_win);
    explorer_listbar_init(list_para->list_win);
    LISTBAR_ShowPage(list_para->listbar_handle);
    //explorer_draw_file_type(  list_para , RAT_MEDIA_TYPE_ALL ) ;
    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);

    if (list_para->listbar_left_right == LISTBAR_LEFT)
    {
        LISTBAR_LostFocus(list_para->listbar_handle);
    }

    return EPDK_OK;
}
// 浏览器从文件列表回退到sd/usb盘符选择界面，或者从sd/usb盘符界面退出到主界面
//现在改成不退回到SD/USB盘符选择界面，直接退出
static __s32  explorer_list_return_backspace(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    file_list_t *parent_file_list = NULL;
    __u8  fatdirattr;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    //if( list_para->enter_sd_usb_flag == 0)  // 当前处于最顶级菜单, 回退到主界面
    {
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        return EPDK_OK;
    }
#if 0

    if (list_para->top_file_list->total <= 0)
    {
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        return EPDK_OK;
    }

    explorer_listbar_uninit(list_para->list_win);
    explorer_save_last_para(list_para); //保存当前参数
    list_para->cur_file_list = list_para->top_file_list ;       //指向最顶级文件
    list_para->last_start_id = list_para->top_file_list->start_id;
    list_para->last_focused_id = list_para->top_file_list->focus_id;
    list_para->rat.total = list_para->top_file_list->total;
    __msg("list_para->rat.total=%d", list_para->rat.total);
    // 此时已经回退到第一级菜单，需将enter_sd_usb_flag置0
    list_para->enter_sd_usb_flag = 0 ;
    // 从文件浏览列表中退出来，退出到sd / usb 选择窗口中，需将return_to_explorer_file_list 置0
    list_para->root_para->return_to_explorer_file_list = 0 ;
    list_para->big_miniature_index = -1 ;
    explorer_listbar_init(list_para->list_win);
    LISTBAR_ShowPage(list_para->listbar_handle);
    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
    //explorer_draw_file_type( list_para , list_para->media_type ) ;
#endif
    return EPDK_OK;
}

static __s32  sd_usb_enter_next_dir(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    char  path[RAT_MAX_FULL_PATH_LEN] = { 0 };
    __u8  fatdirattr;
    __listbar_scene_t ListBarScene;
    __u8 root_type = 0 ;
    file_list_t  *temp_file_list ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        return EPDK_FAIL;
    }

    if (list_para->enter_sd_usb_flag == 0)      // 处于顶级文件菜单,( sd / usb)
    {
        fatdirattr = list_para->top_file_list->cur_item->fatdirattr;

        if (fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
        {
            list_para->root_type = RAT_USB ;
        }
        else if (fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE)
        {
            list_para->root_type = RAT_USB ;
        }
        else
        {
            list_para->root_type = RAT_UNKNOWN ;
        }
    }

    explorer_list_long_string_stop_roll(list_para);
    //进入下一级子文件夹
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    list_para->cur_file_list->start_id = ListBarScene.start_id;
    list_para->cur_file_list->focus_id = ListBarScene.focus_id;
    explorer_listbar_uninit(list_para->list_win);
    //list_para->rat.handle = rat_open( list_para->search_path , list_para->media_type , 0 ) ;
    list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0) ;
    list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;
    explorer_listbar_init(list_para->list_win);
    explorer_get_last_para(list_para);
    LISTBAR_ShowPage(list_para->listbar_handle);

    if (list_para->listbar_left_right == LISTBAR_LEFT)
    {
        LISTBAR_LostFocus(list_para->listbar_handle);
    }

    return EPDK_OK;
}
//文件管理进入下一级菜单
static __s32  file_manager_list_on_enter_key(__gui_msg_t *msg)
{
    explr_list_para_t *list_para = NULL;
    __listbar_scene_t ListBarScene;
    file_list_t *temp_file_list = NULL;
    char  path[RAT_MAX_FULL_PATH_LEN] = {0};
    __u8  fatdirattr;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    __wrn("file_manager_list_on_enter_key1");

    if (list_para == NULL)
    {
        __wrn("list_para == NULL");
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list == NULL)
    {
        __wrn("list_para->cur_file_list == NULL");
        return EPDK_FAIL;
    }

    if (list_para->cur_file_list->cur_item == NULL)
    {
        __wrn("list_para->cur_file_list->cur_item == NULL");
        return EPDK_FAIL;
    }

    fatdirattr = list_para->cur_file_list->cur_item->fatdirattr;

    if (RAT_MEDIA_TYPE_USB_DEVICE == fatdirattr)
    {
        __wrn("RAT_MEDIA_TYPE_USB_DEVICE");
        list_para->root_type = RAT_USB ;
    }

    if (RAT_MEDIA_TYPE_SD_DEVICE == fatdirattr)
    {
        list_para->root_type = RAT_TF ;
    }

    __wrn("file_manager_list_on_enter_key2");
    ExplorerListWinGetSearchPath(list_para) ;
    __wrn("fatdirattr = %d", fatdirattr);

    if ((fatdirattr & FSYS_ATTR_DIRECTORY)
        || (fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
        || (fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE))
    {
        explorer_list_long_string_stop_roll(list_para);
        //进入下一级子文件夹
        __wrn("file_manager_list_on_enter_key4");
        eLIBs_memset(&ListBarScene, 0x00, sizeof(ListBarScene));
        LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
        list_para->cur_file_list->start_id = ListBarScene.start_id;
        list_para->cur_file_list->focus_id = ListBarScene.focus_id;

        if (list_para->cur_file_list == list_para->top_file_list)
        {
            eLIBs_strcpy(path, list_para->cur_file_list->cur_item->name);
            //eLIBs_strcat(path, "\\");
            list_para->enter_sd_usb_flag =  1 ;
        }
        else
        {
            eLIBs_strcpy(path, list_para->cur_file_list->file_path);
            eLIBs_strcat(path, "\\");
            eLIBs_strcat(path, list_para->cur_file_list->cur_item->name);
        }

        __msg("explorer list  path is %s", path);
        temp_file_list = new_file_list_nod(path, NULL);

        if (temp_file_list == NULL)
        {
            __wrn("file_manager_list_on_enter_key5");
            return EPDK_FAIL;
        }

        if (temp_file_list->total == 0)
        {
            __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
            lang_id[1] = folder_emtpy[list_para->mediatype_index];
            default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                           ADLG_OK, lang_id);
            esKRNL_TimeDly(100);
            app_dialog_destroy(list_para->h_dialog);
            list_para->h_dialog = NULL ;
        }

        list_para->rat.total = temp_file_list->total;
        list_para->cur_file_list->child = temp_file_list;
        temp_file_list->parent = list_para->cur_file_list;
        list_para->cur_file_list = temp_file_list;
        list_para->last_start_id = 0;
        list_para->last_focused_id = 0;
        list_para->rat.index = 0;
        __wrn("file_manager_list_on_enter_key6");
        explorer_listbar_uninit(list_para->list_win);
        __wrn("file_manager_list_on_enter_key7");
        explorer_listbar_init(list_para->list_win);
        __wrn("file_manager_list_on_enter_key8");
        //explorer_mediatype_listbar_init(list_para->list_win);
        LISTBAR_ShowPage(list_para->listbar_handle);
        __wrn("file_manager_list_on_enter_key9");
        //explorer_draw_file_type( list_para , RAT_MEDIA_TYPE_ALL ) ;
        LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
        __wrn("file_manager_list_on_enter_key10");

        if (list_para->listbar_left_right == LISTBAR_LEFT)
        {
            LISTBAR_LostFocus(list_para->listbar_handle);
        }

        __wrn("file_manager_list_on_enter_key6");
    }
    else
    {
        file_item_t *file_item ;
        rat_media_type_t media_type ;
        file_item = get_file_list_item(list_para->cur_file_list, list_para->rat.index);
        media_type = rat_get_file_type(file_item->name);
        __wrn("file_manager_list_on_enter_key7");

        if ((RAT_MEDIA_TYPE_AUDIO == media_type) || (RAT_MEDIA_TYPE_VIDEO == media_type)
            || (RAT_MEDIA_TYPE_EBOOK == media_type) || (RAT_MEDIA_TYPE_PIC == media_type)
            || (RAT_MEDIA_TYPE_GAME == media_type))
        {
            file_manager_set_current_file_for_play(msg);
        }
        else if (media_type == RAT_MEDIA_TYPE_FIRMWARE) // 固件升级
        {
            if (0) //卡升级操作危险，实际意义不大
            {
                file_item_t *file_item = NULL;
                char  path[RAT_MAX_FULL_PATH_LEN] = {0};
                //__here__ ;
                file_item = list_para->cur_file_list->cur_item;
                //eLIBs_memset(path, 0, sizeof(path));
                eLIBs_strcpy(path, list_para->cur_file_list->file_path);
                eLIBs_strcat(path, "\\");
                eLIBs_strcat(path, file_item->name);
                eLIBs_strcpy(list_para->fw_update_file, path);
                {
                    __s32 str[] = {0, STRING_EXPLR_FW_UPDATE_CONFIRM};
                    H_WIN parent = GUI_WinGetParent(list_para->list_win);
                    default_dialog(list_para->h_fw_update_dlg, parent, FW_UPDATE_DIALOG_ID, ADLG_YESNO, str);
                }
                return EPDK_FAIL;
            }
        }
    }

    return EPDK_OK;
}

static __s32  explorer_list_win_on_enter_key(__gui_msg_t *msg)
{
    __s32 ret;
    __u8  fatdirattr ;
    __u8   root_type = 0 ;
    H_WIN hExpWin = NULL;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    //在文件进行播放之前，先停掉listbar,以防listbar解码线程运行
    //explorer_listbar_uninit( list_para->list_win );
#if (EXPLORER_SUPPORT_MINIATURE == 1)
    list_para->big_miniature_index = list_para->rat.index ; //这句主要用来当左侧的大缩略图还没有画而直接播放文件时不用再重新画大缩略图
#endif
    ret = ExplorerListWinOnEnterKey(list_para);

    if (EPDK_FAIL == ret)
    {
        return EPDK_OK;
    }

    __log("...");

    if (list_para->root_para->return_to_explorer_file_list == 0)
    {
        list_para->root_para->explr_root = list_para->root_type ;
        list_para->root_para->return_to_explorer_file_list = 1; // 播放完文件之后回退到播放列表中
    }

    switch (list_para->media_type)
    {
        case RAT_MEDIA_TYPE_ALL:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            break;

        case RAT_MEDIA_TYPE_PIC:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_PHOTO, 0);
            break;

        case RAT_MEDIA_TYPE_AUDIO:
        {
            reg_root_para_t *root_para = NULL;
            char FileName[RAT_MAX_FULL_PATH_LEN] = "";

            if (list_para->root_type == RAT_TF)
            {
                root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);
                root_para->last_music_start_index_sd = list_para->rat.index;
                ret = MusicGetListItemFileFullPath(list_para->rat.handle, \
                                                   root_para->last_music_start_index_sd, FileName);

                if (EPDK_FAIL == ret)
                {
                    return EPDK_FAIL;
                }

                dsk_reg_save_cur_play_info(REG_APP_MUSIC, \
                                           root_para->last_music_start_index_sd, FileName, list_para->root_type);
            }
            else
            {
                root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);
                root_para->last_music_index_ud = list_para->rat.index;
                ret = MusicGetListItemFileFullPath(list_para->rat.handle, \
                                                   root_para->last_music_index_ud, FileName);

                if (EPDK_FAIL == ret)
                {
                    return EPDK_FAIL;
                }

                dsk_reg_save_cur_play_info(REG_APP_MUSIC, \
                                           root_para->last_music_index_ud, FileName, list_para->root_type);
            }

            list_para->root_para->music_file_name[0] = '\0';
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MUSIC, 0);
        }
        break;

        case RAT_MEDIA_TYPE_VIDEO:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MOVIE, 0);
            break;

        case RAT_MEDIA_TYPE_EBOOK:
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_EBOOK, 0);
            break;

        case RAT_MEDIA_TYPE_GAME:
            //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_GAME, 0);
            break;

        default:
            break;
    }

    hExpWin = GUI_WinGetParent(msg->h_deswin);
    ExplorerSendMessage(hExpWin, GUI_MSG_CLOSE, 0, 0);
    return EPDK_OK;
}


//filename: full path of file
static __s32 explorer_search_no_use_name(char *filename)
{
    char tmp1[789], tmp2[789], tmp3[10];
    ES_FILE *fp1;
    char *p1;
    __s32 i = 0;
    char tmpbuf[1024];

    if (NULL == filename)
    {
        return EPDK_FAIL;
    }

    eLIBs_strcpy(tmp1, filename); //不直接对传入的文件名进行操作
    p1 = eLIBs_strchrlast(tmp1, '\\');
    *p1++ = '\0';
    eLIBs_memset(tmp2, 0, sizeof(tmp2));
    eLIBs_strcpy(tmp2, tmp1);

    do
    {
        //eLIBs_sprintf(tmpbuf,"_copy%d.",i++);
        //eLIBs_strcat(tmp2,tmpbuf);
        //eLIBs_strcat(tmp2,tmp3);
        eLIBs_sprintf(tmp2, "%s\ew%d_%s", tmp1, i++, p1);
        fp1 = eLIBs_fopen(tmp2, "rb");

        if (NULL == fp1)
        {
            break;//无此文件，退出
        }
        else
        {
            eLIBs_memset(tmpbuf, 0, sizeof(tmpbuf));
            eLIBs_fclose(fp1);//文件存在，继续查找
            eLIBs_strcpy(tmp2, tmp1); //取前缀
        }
    } while (1);

    eLIBs_strcpy(filename, tmp2);
    return EPDK_OK;
}




//复制单个文件
static __s32 explorer_parst_file(explr_list_para_t *list_para)
{
    ES_FILE *fp1, *fp2, *fp3;
    char newfilename[1024] = {0};
    char *p1;
    char *pbuf;
    __s32 len;
    __s32 ret;
    __s64 total_size, file_size;
    char tmp[3];
    fp1 = eLIBs_fopen(list_para->copy_name_back, "rb+");

    if (NULL == fp1)
    {
        __wrn("source file not exict ");
        return EPDK_FAIL;
    }

    if (RAT_MEDIA_TYPE_ALL == list_para->media_type) //复制当前所在目录
    {
        eLIBs_strcpy(newfilename, list_para->cur_file_list->file_path);
        p1 = eLIBs_strchrlast(list_para->copy_name_back, '\\');
        eLIBs_strcat(newfilename, p1);

        if (NULL != (fp3 = eLIBs_fopen(newfilename, "rb+")))
        {
            eLIBs_fclose(fp3);
            explorer_search_no_use_name(newfilename); //若存在相同文件,重查
        }
    }
    else //默认与原文件同一目录
    {
        eLIBs_strcpy(newfilename, list_para->copy_name_back);
        explorer_search_no_use_name(newfilename);
    }

    if (NULL != fp1)
    {
        fp2 = eLIBs_fopen(newfilename, "wb+");

        if (NULL == fp2)
        {
            __msg("parse file err!");
            eLIBs_fclose(fp1);
            return EPDK_FAIL;
        }

        pbuf = esMEMS_Palloc(CAT_READ_BUFFER_SIZE >> 10, 0);

        if (NULL == pbuf)
        {
            __wrn("palloc err");
            eLIBs_fclose(fp1);
            eLIBs_fclose(fp2);
            return EPDK_FAIL;
        }

        while (1)
        {
            len = eLIBs_fread(pbuf, 1, CAT_READ_BUFFER_SIZE, fp1);

            if (0 < len)
            {
                eLIBs_fwrite(pbuf, 1, len, fp2);
                //esKRNL_TimeDly(2);
            }
            else
            {
                break;
            }

            esKRNL_TimeDly(5);
        };

        eLIBs_fclose(fp1);

        eLIBs_fclose(fp2);

        esMEMS_Pfree(pbuf, 1);
    }
    else
    {
        __msg("parse file error!");
        return EPDK_FAIL;
    }

    return EPDK_OK;
}

//复制一个目录
static __s32 explorer_parst_dir(char *dst, char *src)
{
    file_op_info_t op_info;
    __s32 ret;
    ES_FILE *fp1;
    fp1 = eLIBs_fopen(dst, "rb");

    if (NULL == fp1)
    {
        if (0 != eLIBs_mkdir(dst))
        {
            __wrn("create dir [ %s ] fail", dst);
            return EPDK_FAIL;
        }
    }
    else
    {
        eLIBs_fclose(fp1);
        fp1 = NULL;
    }

    eLIBs_memset(&op_info, 0, sizeof(file_op_info_t));
    ret = get_item_num_in_dir(src, &(op_info.total_num));

    if (EPDK_FAIL == ret)
    {
        return EPDK_FAIL;
    }

    __msg("start to delect folder  ");
    ret = copy_dir(dst, src, &op_info);
    return ret;
}

static __s32 explorer_parst(explr_list_para_t *list_para)
{
#if 1
    char *p1;
    __s32 len;
    __s32 ret;
    __s64 total_size = 0, file_size = 0;
    total_size = eLIBs_GetVolFSpace(list_para->search_path);
    ret = eLIBs_GetFileAttributes(list_para->copy_name_back);

    if (FSYS_ATTR_DIRECTORY & ret) //是目录
    {
        //return EPDK_OK;
        //文件杂的复制容量是否足够在explorer_parst_dir中判断
    }
    else
    {
        file_size = eLIBs_GetFileSize(list_para->copy_name_back);
    }

    __wrn("total_size =%d file_size = %d", total_size, file_size);

    if (file_size > total_size)
    {
        __wrn("no space........");
        return EPDK_FALSE;
    }

    if (FSYS_ATTR_DIRECTORY & ret) //是目录
    {
        //复制目录
        char path[RAT_MAX_FULL_PATH_LEN];
        eLIBs_strcpy(path, list_para->cur_file_list->file_path);
        //eLIBs_strcat(path,"\\");
        //eLIBs_strcat(path,list_para->cur_file_list->cur_item->name);
        len = eLIBs_strlen(list_para->copy_name_back);

        if (0 == eLIBs_strncmp(path, list_para->copy_name_back, len))
        {
            //想将当前目录复制到其子目录中，不复制
            __wrn("In the sub dir");
            return EPDK_FAIL;
        }

        p1 = eLIBs_strchrlast(list_para->copy_name_back, '\\');
        eLIBs_strcat(path, p1);

        if (0 == eLIBs_strcmp(path, list_para->copy_name_back))
        {
            //复制目录与粘贴目录路径相同
            __wrn("Dir has already exict");
            return EPDK_FAIL;
        }

        explorer_parst_dir(path, list_para->copy_name_back);
    }
    else
    {
        //复制文件
        explorer_parst_file(list_para);
    }

    if (RAT_MEDIA_TYPE_ALL == list_para->media_type)
    {
        //list_para->cur_file_list->total++;
        explorer_update_file_list_nod(list_para);
        //添加到链表中
    }

    explorer_filelist_update(list_para);
    list_para->filelist_update = EPDK_TRUE;
#endif
    __msg("parse file done..........%s", list_para->copy_name_back);
    return EPDK_OK;
}


static __s32  explorer_list_win_on_command(__gui_msg_t *msg)
{
    __s32 ret;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
#if 1

    if (msg->dwAddData1 == EXPLR_SW_TO_OTHER_DISK)
    {
        extern __s32 explorer_destory_dialog(void);
        list_para->root_type = msg->dwAddData2;
        __wrn("%s EXPLR_SW_TO_OTHER_DISK\r", __FUNCTION__);
        __wrn("%s root_type:%d\r", __FUNCTION__, msg->dwAddData2);
        rat_close(list_para->rat.handle);
        list_para->rat.handle = NULL;
        ExplorerListWinGetSearchPath(list_para);
        list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);

        if (list_para->rat.handle == NULL)
        {
            //__s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_EXPLR_OPEN_DISK_FAIL};
            //H_WIN h_dialog = NULL;
            __wrn("!!!!!!!!!!!!!open rat fail!!!!!!!!!!!!!!!!!");
            explorer_destory_dialog();
            //  default_dialog(h_dialog, msg->h_deswin, 200, ADLG_OK, lang_id);
            //  esKRNL_TimeDly(200);
            //  app_dialog_destroy(h_dialog);
            //  h_dialog = NULL;
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            return EPDK_FAIL;
        }

        //__here__;
        list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle);

        if (list_para->rat.total <= 0)
        {
            //  __s32 lang_id[] = {STRING_MOVIE_TIPS, STRING_EXPLR_UNFOUND_TYPE_FILE};
            //      H_WIN h_dialog = NULL;
            __wrn("list_para->rat.total:%d\r", list_para->rat.total);
            explorer_destory_dialog();
            //  default_dialog(h_dialog, msg->h_deswin, 200, ADLG_OK, lang_id);
            //  esKRNL_TimeDly(200);
            //  app_dialog_destroy(h_dialog);
            //  h_dialog = NULL;
            explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            return EPDK_FAIL;
        }

        LISTBAR_SetFocusItem(list_para->listbar_handle, 0);
        LISTBAR_SetTotalItem(list_para->listbar_handle, list_para->rat.total);
        explorer_destory_dialog();
    }
    else
#endif
        if (LOWORD(msg->dwAddData1) == DELETE_FILE_DIALOG_ID)
        {
            switch (HIWORD(msg->dwAddData1))
            {
                case ADLG_CMD_SET_UI:
                {
                    //set dialog ui para
                    //app_dialog_set_ui(msg->h_srcwin, ADLG_UI_OK, 0, 0);
                }
                break;

                default:
                    break;
            }
        }
        else if (LOWORD(msg->dwAddData1) == FW_UPDATE_DIALOG_ID)
        {
            //__here__;
            switch (HIWORD(msg->dwAddData1))
            {
                case ADLG_CMD_EXIT:
                    if (list_para->h_fw_update_dlg != NULL)
                    {
                        app_dialog_destroy(list_para->h_fw_update_dlg);         //to delete dialog

                        if (msg->dwAddData2 == ADLG_IDYES)
                        {
                            __msg("fw update, file=%s", list_para->fw_update_file);
                            dsk_start_fw_update(list_para->fw_update_file);
                        }
                        else//ADLG_IDNO
                        {
                            __msg("fw not update...");
                        }

                        list_para->h_fw_update_dlg = NULL;
                    }

                    break;
            }
        }
        else if (msg->dwAddData1 == ENTER_SELECT_ITEM_ID)
        {
            //文件管理模式，按下enter键 ，进入下一级菜单
            if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
            {
                __u8    fatdirattr ;
                fatdirattr = list_para->cur_file_list->cur_item->fatdirattr;

                if ((RAT_MEDIA_TYPE_SD_DEVICE == fatdirattr) \
                    || (RAT_MEDIA_TYPE_USB_DEVICE == fatdirattr))
                {
                    list_para->enter_sd_usb_flag = 1 ;
                }

                list_para->del_dlg_open = EPDK_FALSE;
                file_manager_list_on_enter_key(msg);
            }
            else
            {
                // return_to_explorer_file_list = 1表示当前存于文件列表当中
                if (list_para->root_para->return_to_explorer_file_list == 1)
                {
                    explorer_list_long_string_stop_roll(list_para);
                    explorer_list_win_on_enter_key(msg);
                }
                else
                {
                    explorer_list_enter_next_dir(msg);
                }
            }
        }
        else if (msg->dwAddData1 == SELECT_UP_ITEM_ID)
        {
            ret = GUI_IsTimerInstalled(msg->h_deswin, ExplorerDrawBgMusicIndexTimer) ;

            if (EPDK_TRUE == ret)
            {
                GUI_ResetTimer(msg->h_deswin, ExplorerDrawBgMusicIndexTimer, \
                               EXP_DRAW_BG_MUSIC_INDEX_INTERVAL, NULL);
            }

            explorer_list_long_string_stop_roll(list_para);

            if (LISTBAR_LEFT == list_para->listbar_left_right)
            {
                ExplorerMediatypeListWinOnPreviousItem(msg);
            }
            else
            {
                ExplorerListWinOnPreviousItem(list_para);
            }
        }
        else if (msg->dwAddData1 == SELECT_DOWN_ITEM_ID)
        {
            ret = GUI_IsTimerInstalled(msg->h_deswin, ExplorerDrawBgMusicIndexTimer) ;

            if (EPDK_TRUE == ret)
            {
                GUI_ResetTimer(msg->h_deswin, ExplorerDrawBgMusicIndexTimer, \
                               EXP_DRAW_BG_MUSIC_INDEX_INTERVAL, NULL);
            }

            explorer_list_long_string_stop_roll(list_para);

            if (LISTBAR_LEFT == list_para->listbar_left_right)
            {
                ExplorerMediatypeListWinOnNextItem(msg);
            }
            else if (LISTBAR_RIGHT == list_para->listbar_left_right)
            {
                ExplorerListWinOnNextItem(list_para);
            }
        }
        else if (msg->dwAddData1 == EXPLORER_DIALOG_COPY)
        {
            if (RAT_MEDIA_TYPE_ALL == list_para->media_type)
            {
                eLIBs_memset(list_para->copy_name_back, 0, sizeof(list_para->copy_name_back));
                eLIBs_strcpy(list_para->copy_name_back, list_para->cur_file_list->file_path);
                eLIBs_strcat(list_para->copy_name_back, "\\");
                eLIBs_strcat(list_para->copy_name_back, list_para->cur_file_list->cur_item->name);
            }
            else
            {
                GetListItemFileFullPath(list_para, list_para->rat.index, list_para->copy_name_back);
            }
        }
        else if (msg->dwAddData1 == EXPLORER_DIALOG_PARSE)
        {
            explorer_parst(list_para);
        }
        else if (msg->dwAddData1 == EXPLORER_DIALOG_CUT)
        {
        }
        else if (msg->dwAddData1 == EXPLORER_DIALOG_DEL)
        {
            if (RAT_MEDIA_TYPE_ALL == list_para->media_type)
            {
                file_manager_list_on_delete(msg);
            }
            else
            {
                explorer_list_delete_file(msg);
            }
        }

    return EPDK_OK;
}

static __s32 explorer_mediatype_listbar_win_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 total_cnt = 0;
    __s32 ret = EPDK_FAIL ;
    __s32 media_type_bak;
    static __s32 is_touch_down = EPDK_FALSE ;
    static __s32 is_touch_moving = EPDK_FALSE ;
    explr_list_para_t *list_para = NULL ;
    explorer_viewer_ui_t *ui_param = NULL ;
    RECT    rect, rect1;
    GUI_MEMDEV_Handle   draw_mem;
    static __pos_t  touch_down_pos, touch_up_pos ;  // 用于避免误判的
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ui_param = explorer_get_viewer_ui_param();
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);
    __msg("pos.x = %d pos.y = %d", x, y);
    rect.x = ui_param->mediatype_list_item_ui_param.ListBarArea.x ;
    rect.y = ui_param->mediatype_list_item_ui_param.ListBarArea.y ;
    rect.width = ui_param->mediatype_list_item_ui_param.ListBarArea.width ;
    rect.height = ui_param->mediatype_list_item_ui_param.ListBarArea.height ;

    //__msg("rect.x = %d %d %d %d" , rect.x , rect.y , rect.width ,rect.height);
    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
            //case GUI_MSG_TOUCH_LONGDOWN:
        {
            ret = position_in_rect(&rect, x, y);

            if (EPDK_TRUE == ret)
            {
                __msg("listbar get touch down message ");

                if (LBAR_STATE_MOVING == LISTBAR_GetState(list_para->mediatype_listbar_handle))
                {
                    __msg(" receive GUI_MSG_TOUCH_DOWN message , listbar is moving , break ");
                    break ;
                }

                //__here__ ;
                //explorer_list_long_string_stop_roll( list_para );
                ret = LISTBAR_TouchDown(list_para->mediatype_listbar_handle, x, y, direct, speed);

                if (EPDK_OK == ret)
                {
                    __msg(" LISTBAR_TouchDown succeed  ");
                    GUI_WinSetCaptureWin(msg->h_deswin);
                    is_touch_down = EPDK_TRUE ;
                    is_touch_moving = EPDK_FALSE ;
                    touch_down_pos.x = x ;
                    touch_down_pos.y = y ;
                }
            }

            break;
        }

        case GUI_MSG_TOUCH_MOVE:
        {
            __msg("listbar get touch move message ");

            if (is_touch_down == EPDK_TRUE)
            {
                //__here__ ;
                //explorer_list_long_string_stop_roll( list_para );
                ret = LISTBAR_TouchMove(list_para->mediatype_listbar_handle, x, y, direct, speed);

                if (EPDK_OK == ret)
                {
                    is_touch_moving = EPDK_TRUE ;
                }
            }

            break;
        }

        case GUI_MSG_TOUCH_UP:
        {
            RECT rect;
            __msg("-LISTBAR_TouchUp--x: %d, y: %d, direct: %d, speed: %d", x, y, direct, speed);

            if (is_touch_down == EPDK_FALSE)
            {
                __msg("is_touch_down == EPDK_FALSE ");
                break;
            }
            else
            {
                is_touch_down = EPDK_FALSE ;
            }

            //__here__;
            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }

            //ret = GUI_IsTimerInstalled( msg->h_deswin , ExplorerDrawBgMusicIndexTimer ) ;
            //if( EPDK_TRUE == ret )
            //{
            //  GUI_ResetTimer( msg->h_deswin , ExplorerDrawBgMusicIndexTimer ,
            //      EXP_DRAW_BG_MUSIC_INDEX_INTERVAL , NULL );
            //}
            ret  = LISTBAR_TouchUp(list_para->mediatype_listbar_handle, x, y, direct, speed);

            if (EPDK_OK == ret)
            {
                touch_up_pos.x = x ;
                touch_up_pos.y = y ;
            }

            index = LISTBAR_GetFocusItem(list_para->mediatype_listbar_handle);
            total_cnt = LISTBAR_GetTotalItem(list_para->mediatype_listbar_handle);
            LISTBAR_GetItemRect(list_para->mediatype_listbar_handle, index, &rect);
            __msg("----rect -- x: %d, y: %d, width: %d, height: %d", rect.x, rect.y, rect.width, rect.height);
            //  if( LBAR_STATE_MOVING == LISTBAR_GetState( list_para->listbar_handle ) )
            //  {
            //      __msg( " listbar is moving , break ");
            //      break ;
            //  }
            /* 主要是避免应用收不到gui_msg_touch_move消息增加的判断*/
            __msg("touch_down_pos.x=%d touch_down_pos.y=%d touch_up_pos.x=%d touch_up_pos.y=%d ", touch_down_pos.x, touch_down_pos.y, touch_up_pos.x, touch_up_pos.y);

            if (abs(touch_up_pos.x - touch_down_pos.x) > 30 || abs(touch_up_pos.y - touch_down_pos.y) > 30)
            {
                //__here__;
                break ;
            }

            if (is_touch_moving == EPDK_FALSE)
            {
                if (index != list_para->mediatype_index)
                {
                    list_para->mediatype_index = index;
                    //list_para->media_type = mediatype_tab[index];
                }

                if (index >= 0 && index < total_cnt)
                {
                    //mediatype process
                    if (list_para->media_type == mediatype_tab[index])  //相同媒体类型，不用重新搜索
                    {
                        __msg(" the same media type , need not to search again, return ");
                        return EPDK_OK ;
                    }

                    rect1.x = ui_param->ListBarArea.x;
                    rect1.y = ui_param->ListBarArea.y ;
                    rect1.width = ui_param->ListBarArea.width ;
                    rect1.height = ui_param->ListBarArea.height ;
                    //draw_mem = GUI_MEMDEV_Create( rect1.x , rect1.y , rect1.width , rect1.height );
                    //GUI_MEMDEV_Select(draw_mem);
                    media_type_bak = list_para->media_type;
                    list_para->media_type = mediatype_tab[index];
                    explorer_listbar_uninit(list_para->list_win);
                    GUI_LyrWinSel(list_para->list_layer);
                    //文件管理模式，跳过sd/usb选择界面 ，进入下一级菜单
                    //if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
                    {
                        //  explorer_file_manager_rebuild(msg);
                    }
                    //else
                    {
                        //   if(RAT_MEDIA_TYPE_ALL == media_type_bak)
                        {
                            //delete_file_list_chain( list_para->top_file_list ) ;
                            //list_para->top_file_list = NULL ;
                            //      explorer_file_list_uninit(list_para);
                        }

                        if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
                        {
                            if (list_para->enter_sd_usb_flag == 0)
                            {
                                explorer_file_manager_ignor_sd_usb(msg);
                            }

                            //if(EPDK_TRUE == list_para->filelist_update)
                            {
                                //list_para->filelist_update = EPDK_FAIL;
                                //explorer_update_file_list_nod(list_para);
                                //explorer_filelist_update(list_para);
                            }
                            list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                            list_para->rat.index = 0 ;
                            list_para->rat.total = list_para->cur_file_list->total;
                        }
                        else
                        {
                            list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                            list_para->rat.index = 0 ;
                            list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;
                        }
                    }

                    if (list_para->rat.total == 0)
                    {
                        __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
                        lang_id[1] = folder_emtpy[list_para->mediatype_index];
                        default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                                       ADLG_OK, lang_id);
                        esKRNL_TimeDly(100);
                        app_dialog_destroy(list_para->h_dialog);
                        list_para->h_dialog = NULL ;
                        explorer_list_long_string_stop_roll(list_para);
                    }

                    //list_para->last_start_id = 0;
                    //list_para->last_focused_id = 0;
                    //list_para->rat.index = 0;
                    //list_para->last_focus_index = 0;
                    explorer_get_last_para(list_para);
                    //explorer_mediatype_listbar_init(list_para->list_win);
                    explorer_listbar_init(list_para->list_win);
                    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                    LISTBAR_ShowPage(list_para->listbar_handle);

                    if (list_para->listbar_left_right == LISTBAR_LEFT)
                    {
                        LISTBAR_LostFocus(list_para->listbar_handle);
                    }

                    //GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
                    //GUI_MEMDEV_CopyToLCD(draw_mem);
                    //GUI_MEMDEV_Select( NULL );
                    //GUI_MEMDEV_Delete(draw_mem);
                    //draw_mem = NULL;
                }
                else
                {
                    __wrn("vacant item click...");
                }
            }
            else
            {
                LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}


static __s32 explorer_listbar_win_touch_proc(__gui_msg_t *msg)
{
    __s32 x = 0, y = 0;
    __s32 speed = 0, direct = 0;
    __s32 index = 0;
    __s32 total_cnt = 0;
    __s32 ret = EPDK_FAIL ;
    static __s32 is_touch_down = EPDK_FALSE ;
    static __s32 is_touch_longdown = EPDK_FALSE ;
    static __s32 is_touch_moving = EPDK_FALSE ;
    explr_list_para_t *list_para = NULL ;
    explorer_viewer_ui_t *ui_param = NULL ;
    RECT    rect ;
    static __pos_t  touch_down_pos, touch_up_pos ;  // 用于避免误判的
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ui_param = explorer_get_viewer_ui_param();
    x = LOSWORD(msg->dwAddData2);
    y = HISWORD(msg->dwAddData2);
    speed = LOSWORD(msg->dwReserved);
    direct = HISWORD(msg->dwReserved);
    __msg("pos.x = %d pos.y = %d", x, y);
    rect.x = ui_param->list_item_ui_param.ListBarArea.x ;
    rect.y = ui_param->list_item_ui_param.ListBarArea.y ;
    rect.width = ui_param->list_item_ui_param.ListBarArea.width ;
    rect.height = ui_param->list_item_ui_param.ListBarArea.height ;

    //__msg("rect.x = %d %d %d %d" , rect.x , rect.y , rect.width ,rect.height);
    switch (msg->dwAddData1)
    {
        case GUI_MSG_TOUCH_DOWN:
            //case GUI_MSG_TOUCH_LONGDOWN:
        {
            ret = position_in_rect(&rect, x, y);

            if (EPDK_TRUE == ret)
            {
                __msg("listbar get touch down message ");

                if (LBAR_STATE_MOVING == LISTBAR_GetState(list_para->listbar_handle))
                {
                    __msg(" receive GUI_MSG_TOUCH_DOWN message , listbar is moving , break ");
                    break ;
                }

                //__here__ ;
                explorer_list_long_string_stop_roll(list_para);
                ret = LISTBAR_TouchDown(list_para->listbar_handle, x, y, direct, speed);

                if (EPDK_OK == ret)
                {
                    __msg(" LISTBAR_TouchDown succeed  ");
                    GUI_WinSetCaptureWin(msg->h_deswin);
                    is_touch_down = EPDK_TRUE ;
                    is_touch_moving = EPDK_FALSE ;
                    touch_down_pos.x = x ;
                    touch_down_pos.y = y ;
                }
            }

            break;
        }

        case GUI_MSG_TOUCH_LONGDOWN:
        {
            is_touch_longdown = EPDK_TRUE;
            break;
        }

        case GUI_MSG_TOUCH_MOVE:
        {
            __msg("listbar get touch move message ");

            if (is_touch_longdown == EPDK_TRUE)
            {
                is_touch_longdown = EPDK_FALSE;
            }

            if (is_touch_down == EPDK_TRUE)
            {
                //__here__ ;
                explorer_list_long_string_stop_roll(list_para);
                ret = LISTBAR_TouchMove(list_para->listbar_handle, x, y, direct, speed);

                if (EPDK_OK == ret)
                {
                    is_touch_moving = EPDK_TRUE ;
                }
            }

            break;
        }

        case GUI_MSG_TOUCH_UP:
        {
            RECT rect;
            __msg("-LISTBAR_TouchUp--x: %d, y: %d, direct: %d, speed: %d", x, y, direct, speed);

            if (is_touch_down == EPDK_FALSE)
            {
                __msg("is_touch_down == EPDK_FALSE ");
                break;
            }
            else
            {
                is_touch_down = EPDK_FALSE ;
            }

            //__here__;
            if (GUI_WinGetCaptureWin() == msg->h_deswin)
            {
                GUI_WinReleaseCapture();
            }

            if (is_touch_longdown == EPDK_FALSE)
            {
            }
            else
            {
                /**  //send command to explorer ctrl to create dialog
                    __listbar_scene_t scene;
                    is_touch_longdown = EPDK_FALSE ;
                    //explorer_draw_cp_mov_dialog(list_para);
                    list_para->cp_mov_dialog = EPDK_TRUE;

                    LISTBAR_GetScene(list_para->listbar_handle, &scene);
                    list_para->last_focused_id = scene.focus_id;
                    list_para->last_start_id = scene.start_id;

                    explorer_save_last_para(list_para);
                    break;
                    **/
                if (0 == list_para->rat.total)
                {
                    __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
                    lang_id[1] = folder_emtpy[list_para->mediatype_index];
                    default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                                   ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);
                    app_dialog_destroy(list_para->h_dialog);
                    list_para->h_dialog = NULL ;
                }
                else
                {
                    __gui_msg_t dmsg;
                    eLIBs_memset(&dmsg, 0, sizeof(__gui_msg_t));
                    dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    dmsg.id = GUI_MSG_COMMAND;
                    dmsg.dwAddData1 = EXPLORER_DIALOG_CREATE;
                    GUI_SendNotifyMessage(&dmsg);
                }

                break;
            }

            ret = GUI_IsTimerInstalled(msg->h_deswin, ExplorerDrawBgMusicIndexTimer) ;

            if (EPDK_TRUE == ret)
            {
                GUI_ResetTimer(msg->h_deswin, ExplorerDrawBgMusicIndexTimer, \
                               EXP_DRAW_BG_MUSIC_INDEX_INTERVAL, NULL);
            }

            ret  = LISTBAR_TouchUp(list_para->listbar_handle, x, y, direct, speed);

            if (EPDK_OK == ret)
            {
                touch_up_pos.x = x ;
                touch_up_pos.y = y ;
            }

            index = LISTBAR_GetFocusItem(list_para->listbar_handle);
            total_cnt = LISTBAR_GetTotalItem(list_para->listbar_handle);
            LISTBAR_GetItemRect(list_para->listbar_handle, index, &rect);
            __msg("----rect -- x: %d, y: %d, width: %d, height: %d", rect.x, rect.y, rect.width, rect.height);
            //  if( LBAR_STATE_MOVING == LISTBAR_GetState( list_para->listbar_handle ) )
            //  {
            //      __msg( " listbar is moving , break ");
            //      break ;
            //  }
            /* 主要是避免应用收不到gui_msg_touch_move消息增加的判断*/
            __msg("touch_down_pos.x=%d touch_down_pos.y=%d touch_up_pos.x=%d touch_up_pos.y=%d ", touch_down_pos.x, touch_down_pos.y, touch_up_pos.x, touch_up_pos.y);

            if (abs(touch_up_pos.x - touch_down_pos.x) > 30 || abs(touch_up_pos.y - touch_down_pos.y) > 30)
            {
                //__here__;
                break ;
            }

            if (is_touch_moving == EPDK_FALSE)
            {
                if ((x < rect.x) || (x > rect.x + rect.width) || (y < rect.y) || (y > rect.y + rect.height))
                {
                    explorer_list_long_string_start_roll(list_para);
                    break;
                }

                if (index != list_para->last_focus_index)
                {
                    list_para->last_focus_index = index;
                }
                else if (index >= 0 && index < total_cnt)
                {
                    //文件管理模式，按下enter键 ，进入下一级菜单
                    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
                    {
                        __u8    fatdirattr ;
                        fatdirattr = list_para->cur_file_list->cur_item->fatdirattr;

                        if ((RAT_MEDIA_TYPE_SD_DEVICE == fatdirattr) \
                            || (RAT_MEDIA_TYPE_USB_DEVICE == fatdirattr))
                        {
                            list_para->enter_sd_usb_flag = 1 ;
                        }

                        file_manager_list_on_enter_key(msg);
                    }
                    else
                    {
                        if (list_para->root_para->return_to_explorer_file_list == 1)
                        {
                            //__here__;
                            explorer_list_win_on_enter_key(msg);
                        }
                        else
                        {
                            //__here__;
                            explorer_list_enter_next_dir(msg);
                        }
                    }
                }
                else
                {
                    __wrn("vacant item click...");
                }

                break;
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return ret;
}

static __bool position_in_rect(RECT *rect, __s32 x, __s32 y)
{
    if ((x > rect->x) && (y > rect->y) && (x < (rect->x + rect->width)) && (y < (rect->y + rect->height)))
    {
        return EPDK_TRUE;
    }
    else
    {
        return EPDK_FALSE;
    }
}

static __s32 explorer_show_dlg(H_WIN hwin, __s32 text_id, __s32 title_id, __s32 time)
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

    default_dialog(dlg, hwin, (APP_EXPLORER_ID + 0x11), ADLG_OK, lang_id);

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


static __s32  explorer_file_manager_rebuild(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    file_list_t *pfilelist = NULL;
    file_item_t *pfileitem = NULL;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    delete_file_list_chain(list_para->top_file_list) ;
    list_para->top_file_list = NULL ;
    list_para->last_focused_id = 0;
    list_para->last_start_id = 0;
    explorer_file_list_init(list_para);
    __wrn("list_para->top_file_list==%08x", list_para->top_file_list);

    //list_para->top_file_list = new_file_root_list_nod(&list_para->media_type);
    if (!list_para->top_file_list)
    {
        __msg(" list_para->top_file_list is null ");
        explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
        return EPDK_OK ;
    }

    pfilelist = list_para->top_file_list;
    pfileitem = list_para->top_file_list->cur_item;
#if 0

    if (RAT_TF == list_para->root_type)
    {
        while (pfilelist && RAT_MEDIA_TYPE_SD_DEVICE != pfilelist->item_list->fatdirattr)
        {
            pfilelist = pfilelist->child;
        }

        list_para->cur_file_list = pfilelist;
        list_para->cur_file_list->cur_item = list_para->cur_file_list->item_list;
    }
    else if (RAT_USB == list_para->root_type)
    {
        while (pfilelist && RAT_MEDIA_TYPE_USB_DEVICE != pfilelist->item_list->fatdirattr)
        {
            pfilelist = pfilelist->child;
        }

        list_para->cur_file_list = pfilelist;
        list_para->cur_file_list->cur_item = list_para->cur_file_list->item_list;
    }

#endif

    if (RAT_TF == list_para->root_type)
    {
        while (pfileitem && RAT_MEDIA_TYPE_SD_DEVICE != pfileitem->fatdirattr)
        {
            __wrn("1111");
            pfileitem = pfileitem->next;
        }

        __wrn("pfilelist1=%08x", pfilelist);
        list_para->cur_file_list->cur_item = pfileitem;
        __wrn("list_para->cur_file_list->cur_item1=%08x", list_para->cur_file_list->cur_item);
    }
    else if (RAT_USB == list_para->root_type)
    {
        while (pfileitem && RAT_MEDIA_TYPE_USB_DEVICE != pfileitem->fatdirattr)
        {
            __wrn("1111");
            pfileitem = pfileitem->next;
        }

        __wrn("pfilelist1=%08x", pfilelist);
        list_para->cur_file_list->cur_item = pfileitem;
    }

    //文件管理模式，跳过sd/usb选择界面 ，进入下一级菜单
    //if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        //list_para->enter_sd_usb_flag = 0;
        //先更新一页，否则file_manager_list_on_enter_key 失败
        explorer_listbar_uninit(list_para->list_win);
        explorer_listbar_init(list_para->list_win);
        //LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
        //LISTBAR_ShowPage(list_para->listbar_handle);
        file_manager_list_on_enter_key(msg);
    }
    //update_file_list_nod(msg);
    return 0;
}


static __s32  explorer_list_win_touch_proc(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    __s32 pos_x, pos_y;
    __bool  ret ;
    explorer_viewer_ui_t *ui_param ;
    file_type_e  file_type  ;
    RECT  return_icon_rect, audio_media_icon_rect, video_media_icon_rect;
    RECT photo_media_icon_rect, ebook_media_icon_rect ;
    RECT game_media_icon_rect;
    __s32 text_id;
    __s32 title_id;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
    ui_param = explorer_get_viewer_ui_param();
    pos_x = LOSWORD(msg->dwAddData2);
    pos_y = HISWORD(msg->dwAddData2);
    __msg("touch action msg->dwAddData1=%d", msg->dwAddData1);
    __msg("pos.x=%d  pos.y=%d", LOSWORD(msg->dwAddData2), HISWORD(msg->dwAddData2));
#if 0
    return_icon_rect.x = ui_param->file_type_uiparam.return_bmp_rect.x ;
    return_icon_rect.y = ui_param->file_type_uiparam.return_bmp_rect.y ;
    return_icon_rect.width = ui_param->file_type_uiparam.return_bmp_rect.width ;
    return_icon_rect.height =  ui_param->file_type_uiparam.return_bmp_rect.height ;
    __msg(" list_para->media_type = %d  ", list_para->media_type);

    if (ret = position_in_rect(&return_icon_rect,  pos_x, pos_y), EPDK_TRUE == ret)
    {
        if ((GUI_MSG_TOUCH_UP == msg->dwAddData1) || (GUI_MSG_TOUCH_OVERUP == msg->dwAddData1))
        {
            __msg(" touch position in return bmp rect ");
            explorer_list_long_string_stop_roll(list_para);
            // 画return 焦点图标
            {
                void *p_buf ;
                __s32  picX, picY ;
                p_buf = explorer_get_listview_icon_res(ID_EXP_RETURN_FOCUS) ;
                picX = ui_param->file_type_uiparam.return_bmp_rect.x ;
                picY = ui_param->file_type_uiparam.return_bmp_rect.y ;
                GUI_LyrWinSel(list_para->list_layer);
                GUI_BMP_Draw(p_buf, picX, picY);
            }

            if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
            {
                file_manager_list_on_backspace(msg);    //回退到上一级目录
            }
            else
            {
                explorer_list_return_backspace(msg);    // 回退到sd/usb选择窗口或退出到主界面
            }

            return EPDK_OK ;
        }
    }

#endif
    //if( RAT_MEDIA_TYPE_ALL == list_para->media_type )     //文件管理时，点击删除按钮处理
    {
#if 0

        /* 在删除按钮图标位置*/
        if ((pos_x > ui_param->file_type_uiparam.delete_file_icon_rect.x)
            && (pos_x < ui_param->file_type_uiparam.delete_file_icon_rect.x
                + ui_param->file_type_uiparam.delete_file_icon_rect.width)
            && (pos_y > ui_param->file_type_uiparam.delete_file_icon_rect.y)
            && (pos_y < ui_param->file_type_uiparam.delete_file_icon_rect.y +
                ui_param->file_type_uiparam.delete_file_icon_rect.height))
        {
            __msg(" touch position in delete icon position ");

            if (list_para->cur_file_list == NULL)       //文件夹为空
            {
                __msg("list_para->cur_file_list  is null , return ");
                return EPDK_OK;
            }

            if (list_para->cur_file_list == list_para->top_file_list)
            {
                __msg(" in top file list , can't delete it ");
                return EPDK_OK;                 //顶层目录不可以删除
            }

            if (list_para->cur_file_list->total < 1)
            {
                __msg(" list_para->cur_file_list->total is little than 1 , break ");
                return EPDK_OK;
            }

            if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
            {
                void *pic_buf  ;
                __msg("receive GUI_MSG_TOUCH_DOWN message ");
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_DELETE_FILE_F_ICON);
                GUI_LyrWinSel(list_para->list_layer);
                GUI_BMP_Draw(pic_buf, ui_param->file_type_uiparam.delete_file_icon_rect.x, \
                             ui_param->file_type_uiparam.delete_file_icon_rect.y);
            }

            if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
            {
                void *pic_buf  ;
                __s32 str[] = {0, STRING_EXPLR_DELETE_CONFIRM};
                H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                pic_buf =  explorer_get_listview_icon_res(ID_EXP_DELETE_FILE_UF_ICON);
                GUI_LyrWinSel(list_para->list_layer);
                GUI_BMP_Draw(pic_buf, ui_param->file_type_uiparam.delete_file_icon_rect.x, \
                             ui_param->file_type_uiparam.delete_file_icon_rect.y);
                __msg("receive GUI_MSG_TOUCH_UP message ");
                default_dialog(list_para->h_dialog, parent, DELETE_FILE_DIALOG_ID, ADLG_YESNO, str);
            }
        }
        else
#endif
        {
            //explorer_listbar_win_touch_proc(msg);
        }
    }

    //else
    if ((RAT_MEDIA_TYPE_AUDIO == list_para->media_type) \
        || (RAT_MEDIA_TYPE_VIDEO == list_para->media_type) \
        || (RAT_MEDIA_TYPE_PIC == list_para->media_type) \
        || (RAT_MEDIA_TYPE_EBOOK == list_para->media_type) \
        || (RAT_MEDIA_TYPE_GAME == list_para->media_type) \
        || (RAT_MEDIA_TYPE_ALL == list_para->media_type))
    {
#if 0

        switch (list_para->media_type)
        {
            case RAT_MEDIA_TYPE_AUDIO:
                file_type = FILE_TYPE_MUSIC ;
                break;

            case RAT_MEDIA_TYPE_VIDEO:
                file_type = FILE_TYPE_MOVIE ;
                break ;

            case RAT_MEDIA_TYPE_PIC:
                file_type = FILE_TYPE_PHOTO ;
                break;

            case RAT_MEDIA_TYPE_EBOOK:
                file_type = FILE_TYPE_EBOOK;
                break;

            case RAT_MEDIA_TYPE_GAME:
                file_type = FILE_TYPE_GAME;
                break;

            default:
                __msg("media type error ");
                break;
        }

        audio_media_icon_rect.x = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_music_rect.x ;
        audio_media_icon_rect.y = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_music_rect.y ;
        audio_media_icon_rect.width = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                      .file_type_music_rect.width ;
        audio_media_icon_rect.height = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                       .file_type_music_rect.height ;
        video_media_icon_rect.x = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_movie_rect.x;
        video_media_icon_rect.y = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_movie_rect.y ;
        video_media_icon_rect.width = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                      .file_type_movie_rect.width ;
        video_media_icon_rect.height =  ui_param->file_type_uiparam.file_type_pos[file_type]\
                                        .file_type_movie_rect.height ;
        photo_media_icon_rect.x = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_photo_rect.x;
        photo_media_icon_rect.y = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_photo_rect.y ;
        photo_media_icon_rect.width = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                      .file_type_photo_rect.width ;
        photo_media_icon_rect.height =  ui_param->file_type_uiparam.file_type_pos[file_type]\
                                        .file_type_photo_rect.height ;
        ebook_media_icon_rect.x = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_ebook_rect.x ;
        ebook_media_icon_rect.y = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                  .file_type_ebook_rect.y ;
        ebook_media_icon_rect.width = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                      .file_type_ebook_rect.width ;
        ebook_media_icon_rect.height = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                       .file_type_ebook_rect.height ;
        game_media_icon_rect.x = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                 .file_type_game_rect.x ;
        game_media_icon_rect.y = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                 .file_type_game_rect.y ;
        game_media_icon_rect.width = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                     .file_type_game_rect.width ;
        game_media_icon_rect.height = ui_param->file_type_uiparam.file_type_pos[file_type]\
                                      .file_type_game_rect.height ;

        //触摸位置在音乐媒体图标范围内
        if (ret = position_in_rect(&audio_media_icon_rect,  pos_x, pos_y), EPDK_TRUE == ret)
        {
            __msg(" touch position in audio media icon rect ");

            if ((GUI_MSG_TOUCH_UP == msg->dwAddData1) || (GUI_MSG_TOUCH_OVERUP == msg->dwAddData1))
            {
                if (list_para->media_type == RAT_MEDIA_TYPE_AUDIO)  //相同媒体类型，不用重新搜索
                {
                    __msg(" the same media type , need not to search again, return ");
                    return EPDK_OK ;
                }

                if (list_para->enter_sd_usb_flag == 0)  //在 sd/usb选择窗口中，不需重新搜索
                {
                    __msg(" in top file list ,no need to search again ");
                    list_para->media_type = RAT_MEDIA_TYPE_AUDIO ;
                    GUI_LyrWinSel(list_para->list_layer);
                    //explorer_draw_file_type( list_para , list_para->media_type );
                    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                    return EPDK_OK ;
                }

                explorer_list_long_string_stop_roll(list_para);
                explorer_listbar_uninit(list_para->list_win);
                list_para->media_type = RAT_MEDIA_TYPE_AUDIO ;
                GUI_LyrWinSel(list_para->list_layer);
                //explorer_draw_file_type( list_para , list_para->media_type );
                LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                //list_para->rat.handle = rat_open(list_para->search_path,list_para->media_type,0);
                list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                list_para->rat.index = 0 ;
                list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;

                if (list_para->rat.total == 0)
                {
                    __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_NULL_FILE };
                    default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                                   ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);
                    app_dialog_destroy(list_para->h_dialog);
                    list_para->h_dialog = NULL ;
                }

                explorer_get_last_para(list_para);
                explorer_list_draw_bg_music_play_song_index(list_para->list_win) ;
                /* list_para->big_miniature_index = -1 应该放在 LISTBAR_ShowPage之前，这样才能重新画缩略图*/
                list_para->big_miniature_index = -1 ;   //重新把画大缩略图的下标置为-1，以便点击图片时重新画大缩略图
                explorer_listbar_init(list_para->list_win);
                LISTBAR_ShowPage(list_para->listbar_handle);
            }
        }
        //触摸位置在视频媒体图标范围内
        else if (ret = position_in_rect(&video_media_icon_rect, pos_x, pos_y), EPDK_TRUE == ret)
        {
            __msg(" touch position in video media icon rect ");

            if ((GUI_MSG_TOUCH_UP == msg->dwAddData1) || (GUI_MSG_TOUCH_OVERUP == msg->dwAddData1))
            {
                if (list_para->media_type == RAT_MEDIA_TYPE_VIDEO)  //相同媒体类型，不用重新搜索
                {
                    __msg(" the same media type , need not to search again, return ");
                    return EPDK_OK ;
                }

                if (list_para->enter_sd_usb_flag == 0)  //在 sd/usb选择窗口中，不需重新搜索
                {
                    __msg(" in top file list ,no need to search again ");
                    list_para->media_type = RAT_MEDIA_TYPE_VIDEO ;
                    GUI_LyrWinSel(list_para->list_layer);
                    //explorer_draw_file_type( list_para , list_para->media_type );
                    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                    return EPDK_OK ;
                }

                explorer_list_long_string_stop_roll(list_para);
                explorer_listbar_uninit(list_para->list_win);
                list_para->media_type = RAT_MEDIA_TYPE_VIDEO;
                GUI_LyrWinSel(list_para->list_layer);
                //explorer_draw_file_type( list_para , list_para->media_type );
                LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                //list_para->rat.handle = rat_open(list_para->search_path,list_para->media_type,0);
                list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                list_para->rat.index = 0 ;
                list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;

                if (list_para->rat.total == 0)
                {
                    __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_NULL_FILE };
                    default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                                   ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);
                    app_dialog_destroy(list_para->h_dialog);
                    list_para->h_dialog = NULL ;
                }

                explorer_get_last_para(list_para);
                /* list_para->big_miniature_index = -1 应该放在 LISTBAR_ShowPage之前，这样才能重新画缩略图*/
                list_para->big_miniature_index = -1 ;   //重新把画大缩略图的下标置为-1，以便点击图片时重新画大缩略图
                explorer_listbar_init(list_para->list_win);
                LISTBAR_ShowPage(list_para->listbar_handle);
            }
        }
        //触摸位置在photo 媒体图标范围内
        else if (ret = position_in_rect(&photo_media_icon_rect, pos_x, pos_y), EPDK_TRUE == ret)
        {
            __msg(" touch position in photo media icon rect ");

            if ((GUI_MSG_TOUCH_UP == msg->dwAddData1) || (GUI_MSG_TOUCH_OVERUP == msg->dwAddData1))
            {
                if (list_para->media_type == RAT_MEDIA_TYPE_PIC)    //相同媒体类型，不用重新搜索
                {
                    __msg(" the same media type , need not to search again, return ");
                    return EPDK_OK ;
                }

                if (list_para->enter_sd_usb_flag == 0)  //在 sd/usb选择窗口中，不需重新搜索
                {
                    __msg(" in top file list ,no need to search again ");
                    list_para->media_type = RAT_MEDIA_TYPE_PIC ;
                    GUI_LyrWinSel(list_para->list_layer);
                    //explorer_draw_file_type( list_para , list_para->media_type );
                    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                    return EPDK_OK ;
                }

                explorer_list_long_string_stop_roll(list_para);
                explorer_listbar_uninit(list_para->list_win);
                list_para->media_type = RAT_MEDIA_TYPE_PIC;
                GUI_LyrWinSel(list_para->list_layer);
                //explorer_draw_file_type( list_para , list_para->media_type );
                LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                //              list_para->rat.handle = rat_open(list_para->search_path,list_para->media_type,0);
                list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                list_para->rat.index = 0 ;
                list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;

                if (list_para->rat.total == 0)
                {
                    __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_NULL_FILE };
                    default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), \
                                   0x2, ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);
                    app_dialog_destroy(list_para->h_dialog);
                    list_para->h_dialog = NULL ;
                }

                explorer_get_last_para(list_para);
                /* list_para->big_miniature_index = -1 应该放在 LISTBAR_ShowPage之前，这样才能重新画缩略图*/
                list_para->big_miniature_index = -1 ;   //重新把画大缩略图的下标置为-1，以便点击图片时重新画大缩略图
                explorer_listbar_init(list_para->list_win);
                LISTBAR_ShowPage(list_para->listbar_handle);
            }
        }
        //触摸位置在电子书媒体图标范围内
        else if (ret = position_in_rect(&ebook_media_icon_rect, pos_x, pos_y), EPDK_TRUE == ret)
        {
            __msg(" touch position in EBOOK media icon rect ");

            if ((GUI_MSG_TOUCH_UP == msg->dwAddData1) || (GUI_MSG_TOUCH_OVERUP == msg->dwAddData1))
            {
                if (list_para->media_type == RAT_MEDIA_TYPE_EBOOK)  //相同媒体类型，不用重新搜索
                {
                    __msg(" the same media type , need not to search again, return ");
                    return EPDK_OK ;
                }

                if (list_para->enter_sd_usb_flag == 0)      //在 sd/usb选择窗口中，不需重新搜索
                {
                    __msg(" in top file list ,no need to search again ");
                    list_para->media_type = RAT_MEDIA_TYPE_EBOOK ;
                    GUI_LyrWinSel(list_para->list_layer);
                    //explorer_draw_file_type( list_para , list_para->media_type );
                    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                    return EPDK_OK ;
                }

                explorer_list_long_string_stop_roll(list_para);
                explorer_listbar_uninit(list_para->list_win);
                //explorer_rat_uninit( list_para );
                list_para->media_type = RAT_MEDIA_TYPE_EBOOK;
                GUI_LyrWinSel(list_para->list_layer);
                //explorer_draw_file_type( list_para , list_para->media_type );
                LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                //list_para->rat.handle = rat_open( list_para->search_path,list_para->media_type,0);
                list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                list_para->rat.index = 0 ;
                list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;

                if (list_para->rat.total == 0)
                {
                    __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_NULL_FILE };
                    default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                                   ADLG_OK, lang_id);
                    esKRNL_TimeDly(100);
                    app_dialog_destroy(list_para->h_dialog);
                    list_para->h_dialog = NULL ;
                }

                explorer_get_last_para(list_para);
                /* list_para->big_miniature_index = -1 应该放在 LISTBAR_ShowPage之前，这样才能重新画缩略图*/
                list_para->big_miniature_index = -1 ;   //重新把画大缩略图的下标置为-1，以便点击图片时重新画大缩略图
                explorer_listbar_init(list_para->list_win);
                LISTBAR_ShowPage(list_para->listbar_handle);
            }
        }
        //触摸位置在游戏媒体图标范围内
        else if (ret = position_in_rect(&game_media_icon_rect, pos_x, pos_y), EPDK_TRUE == ret)
        {
            __msg(" touch position in GAME media icon rect ");

            if ((GUI_MSG_TOUCH_UP == msg->dwAddData1) || (GUI_MSG_TOUCH_OVERUP == msg->dwAddData1))
            {
                if ((rat_is_encryption_partition_Insert(NULL, 1))
                    && (CFG_get_game_and_dict_flag(ID_HOME_GAME)))
                {
                    if (list_para->media_type == RAT_MEDIA_TYPE_GAME)   //相同媒体类型，不用重新搜索
                    {
                        __msg(" the same media type , need not to search again, return ");
                        return EPDK_OK ;
                    }

                    if (list_para->enter_sd_usb_flag == 0)      //在 sd/usb选择窗口中，不需重新搜索
                    {
                        __msg(" in top file list ,no need to search again ");
                        list_para->media_type = RAT_MEDIA_TYPE_GAME ;
                        GUI_LyrWinSel(list_para->list_layer);
                        //explorer_draw_file_type( list_para , list_para->media_type );
                        LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                        return EPDK_OK ;
                    }

                    explorer_list_long_string_stop_roll(list_para);
                    explorer_listbar_uninit(list_para->list_win);
                    //explorer_rat_uninit( list_para );
                    list_para->media_type = RAT_MEDIA_TYPE_GAME;
                    GUI_LyrWinSel(list_para->list_layer);
                    //explorer_draw_file_type( list_para , list_para->media_type );
                    LISTBAR_ShowPage(list_para->mediatype_listbar_handle);
                    //list_para->rat.handle = rat_open( list_para->search_path,list_para->media_type,0);
                    list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);
                    list_para->rat.index = 0 ;
                    list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;

                    if (list_para->rat.total == 0)
                    {
                        __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_NULL_FILE };
                        default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2, \
                                       ADLG_OK, lang_id);
                        esKRNL_TimeDly(100);
                        app_dialog_destroy(list_para->h_dialog);
                        list_para->h_dialog = NULL ;
                    }

                    explorer_get_last_para(list_para);
                    /* list_para->big_miniature_index = -1 应该放在 LISTBAR_ShowPage之前，这样才能重新画缩略图*/
                    list_para->big_miniature_index = -1 ;   //重新把画大缩略图的下标置为-1，以便点击图片时重新画大缩略图
                    explorer_listbar_init(list_para->list_win);
                    LISTBAR_ShowPage(list_para->listbar_handle);
                }
            }
        }
        else
#endif
            if (pos_x > ui_param->sd_coor.x
                && pos_x < ui_param->sd_coor.x + ui_param->sd_coor.width
                && pos_y > ui_param->sd_coor.y
                && pos_y < ui_param->sd_coor.y + ui_param->sd_coor.height
               )
            {
                static __s32 sd_down_equ_up = EPDK_FALSE;

                if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
                {
                    sd_down_equ_up = EPDK_TRUE;
                }
                else if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
                {
                    if (EPDK_TRUE == sd_down_equ_up)
                    {
                        ret = explorer_switch_sd_2_usb(msg);
                        sd_down_equ_up = EPDK_FALSE;
                    }
                }
            }
            else if (pos_x > ui_param->usb_coor.x
                     && pos_x < ui_param->usb_coor.x + ui_param->usb_coor.width
                     && pos_y > ui_param->usb_coor.y
                     && pos_y < ui_param->usb_coor.y + ui_param->usb_coor.height
                    )
            {
                if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
                {
                    static __s32 usb_down_equ_up = EPDK_FALSE;

                    if (GUI_MSG_TOUCH_DOWN == msg->dwAddData1)
                    {
                        usb_down_equ_up = EPDK_TRUE;
                    }
                    else if (GUI_MSG_TOUCH_UP == msg->dwAddData1)
                    {
                        if (EPDK_TRUE == usb_down_equ_up)
                        {
                            ret = explorer_switch_usb_2_sd(msg);
                            usb_down_equ_up = EPDK_FALSE;
                        }
                    }
                }
            }
            else if (pos_x > ui_param->list_item_ui_param.ListBarArea.x
                     && pos_x < ui_param->list_item_ui_param.ListBarArea.x + ui_param->list_item_ui_param.ListBarArea.width
                     && pos_y > ui_param->list_item_ui_param.ListBarArea.y
                     && pos_y < ui_param->list_item_ui_param.ListBarArea.y + ui_param->list_item_ui_param.ListBarArea.height
                    )
            {
                if (EPDK_TRUE == list_para->cp_mov_dialog)
                {
                    list_para->cp_mov_dialog = EPDK_FALSE;
                    //LISTBAR_SetState(list_para->listbar_handle,LBAR_STATE_STOP);
                    explorer_listbar_uninit(list_para->list_win);
                    explorer_listbar_init(list_para->list_win);
                    explorer_get_last_para(list_para);
                    LISTBAR_UpdatePage(list_para->listbar_handle);
                    LISTBAR_ShowPage(list_para->listbar_handle);

                    if (list_para->listbar_left_right == LISTBAR_LEFT)
                    {
                        LISTBAR_LostFocus(list_para->listbar_handle);
                    }
                }
                else
                {
                    explorer_listbar_win_touch_proc(msg);
                }
            }
            else if (pos_x > ui_param->mediatype_list_item_ui_param.ListBarArea.x
                     && pos_x < ui_param->mediatype_list_item_ui_param.ListBarArea.x + ui_param->mediatype_list_item_ui_param.ListBarArea.width
                     && pos_y > ui_param->mediatype_list_item_ui_param.ListBarArea.y
                     && pos_y < ui_param->mediatype_list_item_ui_param.ListBarArea.y + ui_param->mediatype_list_item_ui_param.ListBarArea.height
                    )
            {
                explorer_mediatype_listbar_win_touch_proc(msg);
            }
    }

    return EPDK_OK;
}

/*
**************************************************************************
*Function   :   static __s32  explorer_list_win_key_proc(__gui_msg_t *msg)
*
*Description    :       framewin 响应按键处理
*
*Arguments      :
*
*Return         :
*
***************************************************************************
*/
static __s32  explorer_list_win_key_proc(__gui_msg_t *msg)
{
    explr_list_para_t *list_para = NULL;
    static __u32 last_key = 0;
    __s32 ret = EPDK_FALSE ;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if ((KEY_DOWN_ACTION == msg->dwAddData2) || (KEY_REPEAT_ACTION == msg->dwAddData2))
    {
        switch (msg->dwAddData1)
        {
            case GUI_MSG_KEY_NEXT:
            case GUI_MSG_KEY_LONGNEXT:
            case GUI_MSG_KEY_DOWN:
            case GUI_MSG_KEY_LONGDOWN:
            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
                __wrn("----explorer list window on UP key----");
                //_long_string_stop_roll();
                ret = GUI_IsTimerInstalled(msg->h_deswin, ExplorerDrawBgMusicIndexTimer) ;

                if (EPDK_TRUE == ret)
                {
                    GUI_ResetTimer(msg->h_deswin, ExplorerDrawBgMusicIndexTimer, \
                                   EXP_DRAW_BG_MUSIC_INDEX_INTERVAL, NULL);
                }

                explorer_list_long_string_stop_roll(list_para);

                if (LISTBAR_LEFT == list_para->listbar_left_right)
                {
                    ExplorerMediatypeListWinOnNextItem(msg);
                    //  LISTBAR_LostFocus(list_para->listbar_handle);
                }
                else if (LISTBAR_RIGHT == list_para->listbar_left_right)
                {
                    ExplorerListWinOnNextItem(list_para);
                }

                break;
#if 0

            case GUI_MSG_KEY_RIGHT:
            case GUI_MSG_KEY_LONGRIGHT:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
                explorer_switch_left_right_listbar(msg);
                break;
#endif

            case GUI_MSG_KEY_PREV:
            case GUI_MSG_KEY_LONGPREV:
            case GUI_MSG_KEY_UP:
            case GUI_MSG_KEY_LONGUP:
            case GUI_MSG_KEY_LEFT:
            case GUI_MSG_KEY_LONGLEFT:
                __wrn("----explorer list window on DOWN key----");    /////
                ret = GUI_IsTimerInstalled(msg->h_deswin, ExplorerDrawBgMusicIndexTimer) ;

                if (EPDK_TRUE == ret)
                {
                    GUI_ResetTimer(msg->h_deswin, ExplorerDrawBgMusicIndexTimer, \
                                   EXP_DRAW_BG_MUSIC_INDEX_INTERVAL, NULL);
                }

                explorer_list_long_string_stop_roll(list_para);

                if (LISTBAR_LEFT == list_para->listbar_left_right)
                {
                    ExplorerMediatypeListWinOnPreviousItem(msg);
                    //LISTBAR_LostFocus(list_para->listbar_handle);
                }
                else
                {
                    ExplorerListWinOnPreviousItem(list_para);
                }

                break;

            case GUI_MSG_KEY_LONGVADD:      // 用于九宫格和列表模式切换
                //ExplorerListWinOnNextPage(list_para);
                //__wait__;
                /*
                explorer_list_long_string_stop_roll(list_para);
                if(list_para->view_mode == EXPLR_LIST_MODE)
                {
                    ExplorerSetViewMode(list_para, EXPLR_SQUARE_MODE);
                }
                else if(list_para->view_mode == EXPLR_SQUARE_MODE)
                {
                    ExplorerSetViewMode(list_para, EXPLR_LIST_MODE);
                }*/
                //当前用于弹出编辑菜单测试
                /*if(list_para->rat.total > 0)
                {
                   __gui_msg_t dmsg;
                    last_key = GUI_MSG_KEY_LONGVADD;

                    eLIBs_memset(&dmsg,0,sizeof(__gui_msg_t));
                    dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                    dmsg.id = GUI_MSG_COMMAND;
                    dmsg.dwAddData1 = EXPLORER_DIALOG_CREATE;

                    GUI_SendNotifyMessage(&dmsg);
                }*/
                break;

            case GUI_MSG_KEY_VDEC:
                //ExplorerListWinOnPreviousPage(list_para);
                break;

            case GUI_MSG_KEY_PLAY_PAUSE:
            case GUI_MSG_KEY_ENTER:
                last_key = GUI_MSG_KEY_ENTER;

                /*
                if(list_para->media_type == RAT_MEDIA_TYPE_ALL)
                {
                    list_para->del_dlg_open = EPDK_FALSE;
                    file_manager_list_on_enter_key(msg);
                }
                else
                {
                    explorer_list_long_string_stop_roll(list_para);
                    explorer_list_enter_next_dir( msg );
                }
                */
                //文件管理模式，按下enter键 ，进入下一级菜单
                if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
                {
                    __u8    fatdirattr ;
                    fatdirattr = list_para->cur_file_list->cur_item->fatdirattr;

                    if ((RAT_MEDIA_TYPE_SD_DEVICE == fatdirattr) \
                        || (RAT_MEDIA_TYPE_USB_DEVICE == fatdirattr))
                    {
                        list_para->enter_sd_usb_flag = 1 ;
                    }

                    list_para->del_dlg_open = EPDK_FALSE;
                    __wrn("file_manager_list_on_enter_key");
                    file_manager_list_on_enter_key(msg);
                }
                else
                {
                    // return_to_explorer_file_list = 1表示当前存于文件列表当中
                    if (list_para->root_para->return_to_explorer_file_list == 1)
                    {
                        __wrn("explorer_list_long_string_stop_roll");
                        explorer_list_long_string_stop_roll(list_para);
                        __wrn("explorer_list_win_on_enter_key");
                        __log("...");
                        explorer_list_win_on_enter_key(msg);
                    }
                    else
                    {
                        list_para->enter_sd_usb_flag = 1;//
                        __wrn("explorer_list_enter_next_dir");
                        explorer_list_enter_next_dir(msg);
                        __log("...");
                    }
                }

                break;

            //case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_ESCAPE:
            {
                last_key = GUI_MSG_KEY_ESCAPE;

                if (last_key == GUI_MSG_KEY_ESCAPE)     //回到上一级菜单.
                {
                    explorer_list_long_string_stop_roll(list_para);

                    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
                    {
                        file_manager_list_on_backspace(msg);    //回退到上一级目录
                    }
                    else
                    {
                        explorer_list_return_backspace(msg);// 回退到sd/usb选择窗口或退出到主界面
                    }
                }

                break ;
            }
                //explorer_list_long_string_stop_roll(list_para);
                //explorer_cmd2parent(msg->h_deswin, SWITCH_TO_OTHER_APP, EXPLR_SW_TO_MAIN, 0);
            break;

            case GUI_MSG_KEY_LONGMODE:
                last_key = GUI_MSG_KEY_LONGMODE;
                explorer_switch_sd_usb(msg);
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
            //case GUI_MSG_KEY_MENU:
            case GUI_MSG_KEY_LONGMODE:
            {
                if (last_key == GUI_MSG_KEY_LONGMODE) //USB and SD switch
                {
                    // explorer_switch_sd_usb(msg);
                    /**if(list_para->cur_file_list == NULL)//删除文件夹或文件
                    {
                        return EPDK_OK;
                    }
                    if(list_para->cur_file_list == list_para->top_file_list)
                    {
                        return EPDK_OK;                 //顶层目录不可以删除
                    }
                    if(list_para->cur_file_list->total < 1)
                    {
                        return EPDK_OK;
                    }

                    if(list_para->del_dlg_open)
                    {
                        list_para->del_dlg_open = EPDK_FALSE ;
                        GUI_LyrWinSel( list_para->list_layer ) ;
                        //explorer_draw_file_info(list_para);
                        __msg("explorer list destory del file failed dialog");
                    }
                    {
                        __s32 str[] = {0, STRING_EXPLR_DELETE_CONFIRM};
                        H_WIN parent = GUI_WinGetParent(msg->h_deswin);
                        default_dialog(list_para->h_dialog,parent, DELETE_FILE_DIALOG_ID,\
                            ADLG_YESNO, str);
                    }**/
                }

                break;
            }

            /*case GUI_MSG_KEY_LONGVADD:
            {
                __gui_msg_t dmsg;

                eLIBs_memset(&dmsg,0,sizeof(__gui_msg_t));
                dmsg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                dmsg.id = GUI_MSG_COMMAND;
                dmsg.dwAddData1 = EXPLORER_DIALOG_CREATE;
            }
            break;*/
            default:
                break;
        }
    }

    return EPDK_OK;
}
/*
***********************************************************************************************
*Function   :          static __s32 draw_list_vacant_item(__lbar_draw_para_t *draw_param)
*
*Description    :       画空白条目
*
*Arguments      :
*
*Return         :
*
************************************************************************************************
*/
static __s32 draw_list_vacant_item(__lbar_draw_para_t *draw_param)
{
    GUI_RECT disp_rect;
    //char *filename;
    //int picX, picY, picW, picH;       //picture X, Y, width, height
    //void *pic_buf;
    //explr_list_para_t *list_para;
    //list_para = (explr_list_para_t *)draw_param->attr;
    //GUI_LyrWinSel(list_para->list_layer);
    //GUI_SetBkColor(GUI_LIGHTGREEN);
    //GUI_SetColor(GUI_RED);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    __msg("Vacant item id =: %d", draw_param->index);
    disp_rect.x0 = draw_param->rect.x;
    disp_rect.y0 = draw_param->rect.y;
    disp_rect.x1 = draw_param->rect.x + draw_param->rect.width;
    disp_rect.y1 = draw_param->rect.y + draw_param->rect.height;
    GUI_ClearRect(disp_rect.x0, disp_rect.y0, disp_rect.x1, disp_rect.y1);
    return EPDK_OK;
}
/*
********************************************************************************************
*Function   :          static __s32 ExtractFileName(char *FileName, char *FilePath)
*
*Description    :       从全路径中获取文件名
*
*Arguments      :
*
*Return         :
*
**********************************************************************************************
*/
static __s32 ExtractFileName(char *FileName, char *FilePath)
{
    char *pFileName;
    pFileName = eLIBs_strchrlast(FilePath, '\\');
    pFileName++;
    eLIBs_strcpy(FileName, pFileName);
    return EPDK_OK;
}

//目前使用ItemInfo.format返回mediatype
rat_media_type_t GetListItemFileMediaType(HRAT rat_handle, __s32 ItemIndex)
{
    int ret;
    rat_entry_t ItemInfo;
    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
    ret = rat_get_item_info_by_index(rat_handle, ItemIndex, &ItemInfo);
    return ItemInfo.Format;
}
/*
********************************************************************************************
*Function:__s32 GetListItemFileFullPath(explr_list_para_t*list_para,__s32 ItemIndex,char *FilePath)
*
*Description    :       根据索引号获得条目文件名全路径
*
*Arguments      :
*
*Return         :
*
***************************************************************************************************
*/
__s32 GetListItemFileFullPath(explr_list_para_t *list_para, __s32 ItemIndex, char *FilePath)
{
    int ret;
    rat_entry_t ItemInfo;
    eLIBs_memset(&ItemInfo, 0, sizeof(rat_entry_t));
    ret = rat_get_item_info_by_index(list_para->rat.handle, ItemIndex, &ItemInfo);

    if (ret == EPDK_FAIL)
    {
        __wrn("get file information form rat fail!!");
        return EPDK_FAIL;
    }

    eLIBs_strcpy(FilePath, ItemInfo.Path);          //不去操作rat 内存
    return EPDK_OK;
}

/*
**********************************************************************************************
*Function:static __s32 GetListItemFileName(explr_list_para_t*list_para,__s32ItemIndex,char*FileName)
*
*Description    :       根据索引号获得条目文件名(只有文件名，供显示)
*
*Arguments      :
*
*Return         :
*
***********************************************************************************************
*/
static __s32 GetListItemFileName(explr_list_para_t *list_para, __s32 ItemIndex, char *FileName)
{
    int ret;
    char FilePath[RAT_MAX_FULL_PATH_LEN] ;
    eLIBs_memset(FilePath, 0, sizeof(FilePath));
    ret = GetListItemFileFullPath(list_para, ItemIndex, FilePath);

    if (ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    ExtractFileName(FileName, FilePath);
    return EPDK_OK;
}

/*
***********************************************************************************************
*Function   :         static __s32 draw_miniature_for_pic_item(__lbar_draw_para_t *draw_param)
*
*Description    :       画当前图片对应的缩略图，listbar区域的左侧，即显示在文件名前面的缩略图
*
*Arguments      :
*
*Return         :
*
**********************************************************************************************
*/
#if (EXPLORER_SUPPORT_MINIATURE == 1)
static __s32 draw_miniature_for_pic_item(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;
    OS_SEM_DATA         pdata;
    char FilePath[RAT_MAX_FULL_PATH_LEN] = { 0 };
    int ret = 0;
    rat_miniature_para_t in_para;
    rat_pic_info_t out_para;
    RECT miniature_rect;
    __u8 err = 0 ;
    __msg("draw_param->mode=%d,draw_param->index=%d", draw_param->mode, draw_param->index);
    list_para = (explr_list_para_t *)draw_param->attr;

    if (EXPLR_LIST_MODE != list_para->view_mode)
    {
        __msg("only for list view mode ");
        return EPDK_FAIL;
    }

    if (RAT_MEDIA_TYPE_PIC != list_para->media_type)
    {
        __msg("only for picture media ");
        return EPDK_FAIL ;
    }

    /*if( list_para->enter_sd_usb_flag == 0)    //在sd/usb选择窗口时，直接返回
    {
        __msg("only for file list view ");
        return EPDK_FAIL ;
    }*/
    ret = GetListItemFileFullPath(list_para, draw_param->index, FilePath);

    if (ret == EPDK_FAIL)
    {
        __msg("get list item full path fail");
        return EPDK_FAIL;
    }

    ret = explorer_get_item_miniature_rect(list_para, &miniature_rect);
    eLIBs_strcpy(in_para.file, FilePath);
    in_para.format = PIXEL_COLOR_ARGB8888;
    in_para.width = miniature_rect.width;
    in_para.height = miniature_rect.height;
    in_para.mode = 0;   // 1为拉伸模式

    if (list_para->listbar_miniature_buf == NULL)
    {
        __msg("list_para->listbar_miniature_buf is null ");
        return EPDK_FAIL;
    }

    eLIBs_memset(list_para->listbar_miniature_buf, 0, list_para->listbar_miniature_size);
    eLIBs_memset(&out_para, 0, sizeof(out_para));
    out_para.miniature.buf = list_para->listbar_miniature_buf ;
    //ret = rat_get_pic_info(&in_para, &out_para);
    ret = rat_get_pic_info_ex(&in_para, &out_para, list_para);

    if (EPDK_OK == ret)
    {
        FB fb;
        eLIBs_memset(&fb, 0, sizeof(FB));
        fb.size.width = in_para.width;
        fb.size.height = in_para.height;
        fb.addr[0] = out_para.miniature.buf;;
        fb.fmt.type = FB_TYPE_RGB;
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        fb.fmt.fmt.rgb.br_swap = 0;
        fb.fmt.fmt.rgb.pixseq = (__rgb_seq_t)0;
        __msg("start to draw current pic miniature  ");
        GUI_BitString_Draw(&fb, miniature_rect.x + draw_param->rect.x, \
                           miniature_rect.y + draw_param->rect.y);
    }
    else
    {
        void *p_bmp_buf ;
        __msg("get miniature fail ");
        p_bmp_buf = explorer_get_listview_icon_res(ID_EXP_ERROR_BMP) ;

        if (!p_bmp_buf)
        {
            __msg("get error bmp res fail");
        }

        GUI_BMP_Draw(p_bmp_buf, miniature_rect.x + draw_param->rect.x, \
                     miniature_rect.y + draw_param->rect.y);
    }

    return EPDK_OK;
}
#endif
/*
**************************************************************************************
*Function   :  static __s32 draw_miniature_view_item(__lbar_draw_para_t *draw_param)
*
*Description    :       画缩略图in square mode
*
*Arguments      :
*
*Return         :
*
************************************************************************************
*/
#if (EXPLORER_SUPPORT_MINIATURE == 1)
static __s32 draw_miniature_view_item(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;
    char FilePath[RAT_MAX_FULL_PATH_LEN];
    int ret = 0;
    void *pBuf = NULL;
    //explorer_viewer_ui_t* ui_param;
    rat_miniature_para_t in_para;
    rat_pic_info_t out_para;
    RECT miniature_rect;
    list_para = (explr_list_para_t *)draw_param->attr;

    if (!(list_para->media_type == RAT_MEDIA_TYPE_PIC))     //图片时才有缩略图
    {
        return EPDK_FAIL;
    }

    if (list_para->view_mode == EXPLR_LIST_MODE)
    {
        return EPDK_FAIL;
    }

    eLIBs_memset(FilePath, 0, sizeof(FilePath));
    ret = GetListItemFileFullPath(list_para, draw_param->index, FilePath);

    if (ret == EPDK_FAIL)
    {
        return EPDK_FAIL;
    }

    ret = explorer_get_item_miniature_rect(list_para, &miniature_rect);
    eLIBs_strcpy(in_para.file, FilePath);
    in_para.format = PIXEL_COLOR_ARGB8888;
    in_para.width = miniature_rect.width;
    in_para.height = miniature_rect.height;
    in_para.mode = 0;   // 1为拉伸模式
    pBuf = esMEMS_Balloc(in_para.width * in_para.height * 4);   //ARGB8888,因此4倍大小

    if (pBuf == NULL)                                       //可以放到开始申请，退出时释放
    {
        __wrn("memory is not enough!!");
        return EPDK_FAIL;
    }

    //rat_start_miniature_decode();
    eLIBs_memset(&out_para, 0, sizeof(out_para));
    eLIBs_memset(pBuf, 0, in_para.width * in_para.height * 4);
    out_para.miniature.buf = pBuf;
    //..PT_MEMINF;
    ret = rat_get_pic_info(&in_para, &out_para);

    //..PT_MEMINF;
    //rat_stop_miniature_decode();
    if (EPDK_OK == ret)
    {
        FB fb;
        eLIBs_memset(&fb, 0, sizeof(FB));
        fb.size.width = in_para.width;
        fb.size.height = in_para.height;
        fb.addr[0] = out_para.miniature.buf;;
        fb.fmt.type = FB_TYPE_RGB;
        fb.fmt.fmt.rgb.pixelfmt = PIXEL_COLOR_ARGB8888;
        fb.fmt.fmt.rgb.br_swap = 0;
        fb.fmt.fmt.rgb.pixseq = (__rgb_seq_t)0;
        GUI_BitString_DrawEx(&fb,   miniature_rect.x + draw_param->rect.x, miniature_rect.y + draw_param->rect.y);
    }

    //tractFileName(FileName, FilePath);
    esMEMS_Bfree(pBuf, in_para.width * in_para.height * 4);
    return EPDK_OK;
}
#endif
/*
************************************************************************************
*Function   :   static __s32 draw_mediatype_listview_item_icon(__lbar_draw_para_t *draw_param)
*
*Description    :       画item icon图标(Item左边的Icon)
*
*Arguments      :
*
*Return         :
*
***********************************************************************************
*/
static __s32 draw_mediatype_listview_item_icon(__lbar_draw_para_t *draw_param)
{
    RECT IconRect;
    void *pic_buf = NULL;
    //int ret;
    __s32 picX = 0, picY = 0;
    explr_list_para_t *this = NULL;
    this = (explr_list_para_t *)draw_param->attr;
    explorer_get_mediatype_item_icon_rect(this, &IconRect);

    if (draw_param->mode == LBAR_MODE_NORMAL)
    {
        pic_buf = explorer_get_file_mediatype_item_icon(draw_param, UNFOCUSED);
    }
    else if (draw_param->mode == LBAR_MODE_FOCUS)
    {
        pic_buf = explorer_get_file_mediatype_item_icon(draw_param, FOCUSED);
    }

    if (pic_buf != NULL)
    {
        picX = IconRect.x + draw_param->rect.x  ;
        picY = IconRect.y + draw_param->rect.y ;
        GUI_BMP_Draw(pic_buf, picX, picY);
    }

    return EPDK_OK;
}


/*
************************************************************************************
*Function   :   static __s32 draw_listview_item_icon(__lbar_draw_para_t *draw_param)
*
*Description    :       画item icon图标(Item左边的Icon,或者是缩略图Icon)
*
*Arguments      :
*
*Return         :
*
***********************************************************************************
*/
static __s32 draw_listview_item_icon(__lbar_draw_para_t *draw_param, rat_media_type_t media_type)
{
    RECT IconRect;
    void *pic_buf = NULL;
    //int ret;
    __s32 picX = 0, picY = 0;
    explr_list_para_t *this = NULL;
    this = (explr_list_para_t *)draw_param->attr;
    explorer_get_item_icon_rect(this, &IconRect);

    /*
    if(this->media_type == RAT_MEDIA_TYPE_ALL)      //文件管理
    {
        //media_type = get_file_list_item_file_type(this->cur_file_list->cur_item);
    }
    else
    {
        media_type = GetListItemFileMediaType(this->rat.handle, draw_param->index);
    }
    */
    if (draw_param->mode == LBAR_MODE_NORMAL)
    {
        pic_buf = explorer_get_file_item_icon(this->view_mode, media_type, UNFOCUSED);
    }
    else if (draw_param->mode == LBAR_MODE_FOCUS)
    {
        pic_buf = explorer_get_file_item_icon(this->view_mode, media_type, FOCUSED);
    }

    if (pic_buf != NULL)
    {
        picX = IconRect.x + draw_param->rect.x  ;
        picY = IconRect.y + draw_param->rect.y ;
        GUI_BMP_Draw(pic_buf, picX, picY);
    }

    return EPDK_OK;
}
/*
******************************************************************************************
*Function   :    static __s32 draw_square_item_focus_icon(__lbar_draw_para_t *draw_param)
*
*Description    :       画缩略图获得焦点时显示的图标
*
*Arguments      :
*
*Return         :
*
*******************************************************************************************
*/
static __s32 draw_square_item_focus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    //void *pic_buf;
    int ret;
    //__s32 picX, picY;
    __s32 X0, Y0, X1, Y1;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    ret = explorer_get_item_focus_icon_rect(list_para, &FocusIconRect);
    /*
    pic_buf = explorer_get_listview_icon_res(ID_EXP_SQUARE_FOCUS);          //draw select border

    //picW =  GUI_BMP_GetXSize(pic_buf);                                        //获得图片宽
    //picH =  GUI_BMP_GetYSize(pic_buf);                                        //获得图片高度

    picX = draw_param->rect.x + FocusIconRect.x;
    picY = draw_param->rect.y + FocusIconRect.y;
    GUI_BMP_Draw(pic_buf, picX, picY);
    */
    //画焦点方框
    GUI_SetColor(GUI_YELLOW);
    X0 = draw_param->rect.x + FocusIconRect.x;
    Y0 = draw_param->rect.y + FocusIconRect.y;
    X1 = X0 + FocusIconRect.width;
    Y1 = Y0 + FocusIconRect.height;
    GUI_DrawLine(X0, Y0, X1, Y0);
    GUI_DrawLine(X0, Y0 + 1, X1, Y0 + 1);
    GUI_DrawLine(X0, Y0 + 2, X1, Y0 + 2);
    GUI_DrawLine(X0, Y0, X0, Y1);
    GUI_DrawLine(X0 + 1, Y0, X0 + 1, Y1);
    GUI_DrawLine(X0 + 2, Y0, X0 + 2, Y1);
    GUI_DrawLine(X0, Y1, X1, Y1);
    GUI_DrawLine(X0, Y1 - 1, X1, Y1 - 1);
    GUI_DrawLine(X0, Y1 - 2, X1, Y1 - 2);
    GUI_DrawLine(X1, Y0, X1, Y1);
    GUI_DrawLine(X1 - 1, Y0, X1 - 1, Y1);
    GUI_DrawLine(X1 - 2, Y0, X1 - 2, Y1);
    return ret;
}


//Draw focus icon, 颗将上下两个函数优化为一个函数
static __s32 draw_mediatype_list_item_vacent_icon(__lbar_draw_para_t *draw_param)
{
    void *pic_buf;
    int ret = 0;
    __s32 picX, picY;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    pic_buf = explorer_get_listview_icon_res(ID_EXP_MEDIATYPE_LIST_ITEM_UNFOCUS_BG);
    picX = draw_param->rect.x ;
    picY = draw_param->rect.y ;
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}



//Draw focus icon, 颗将上下两个函数优化为一个函数
static __s32 draw_list_item_vacent_icon(__lbar_draw_para_t *draw_param)
{
    //RECT FocusIconRect;
    void *pic_buf;
    int ret = 0;
    __s32 picX, picY;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    //ret = explorer_get_item_focus_icon_rect(list_para, &FocusIconRect);
    pic_buf = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_UNFOCUS_BG);
    picX = draw_param->rect.x ;
    picY = draw_param->rect.y ;
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}


static __s32 draw_mediatype_list_item_unfocus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    void *pic_buf;
    int ret = 0;
    __s32 picX, picY;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    pic_buf = explorer_get_listview_icon_res(ID_EXP_MEDIATYPE_LIST_ITEM_UNFOCUS_BG);    //draw select border
    picX = draw_param->rect.x ;
    picY = draw_param->rect.y ;
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}


static __s32 draw_list_item_unfocus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    void *pic_buf;
    int ret = 0;
    __s32 picX, picY;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    pic_buf = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_UNFOCUS_BG);  //draw select border
    //picW =  GUI_BMP_GetXSize(pic_buf);                                        //获得图片宽
    //picH =  GUI_BMP_GetYSize(pic_buf);                                        //获得图片高度
    //  if(list_para->h_item_fcs_bmp)
    //  {
    //      pic_buf = theme_hdl2buf(list_para->h_item_fcs_bmp);
    //  }
    //  else
    //  {
    //      list_para->h_item_fcs_bmp = theme_open(ID_EXPLORER_LISTBAR_ITEM_F_BMP);
    //      pic_buf = theme_hdl2buf(list_para->h_item_fcs_bmp);
    //  }
    //picX = draw_param->rect.x + FocusIconRect.x;
    //picY = draw_param->rect.y + FocusIconRect.y;
    picX = draw_param->rect.x ;
    picY = draw_param->rect.y ;
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}


//Draw focus icon, 颗将上下两个函数优化为一个函数
static __s32 draw_mediatype_list_item_focus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    void *pic_buf;
    int ret = EPDK_OK;
    __s32 picX, picY;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    pic_buf = explorer_get_listview_icon_res(ID_EXP_MEDIATYPE_LIST_ITEM_FOCUS_BG);  //draw select border
    picX = draw_param->rect.x ;
    picY = draw_param->rect.y ;
    GUI_BMP_Draw(pic_buf, picX, picY);
    return ret;
}


//Draw focus icon, 颗将上下两个函数优化为一个函数
static __s32 draw_list_item_focus_icon(__lbar_draw_para_t *draw_param)
{
    RECT FocusIconRect;
    void *pic_buf;
    int ret = 0;
    __s32 picX, picY;
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)draw_param->attr;
    pic_buf = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_FOCUS_BG);    //draw select border
    //picW =  GUI_BMP_GetXSize(pic_buf);                                        //获得图片宽
    //picH =  GUI_BMP_GetYSize(pic_buf);                                        //获得图片高度
    //  if(list_para->h_item_fcs_bmp)
    //  {
    //      pic_buf = theme_hdl2buf(list_para->h_item_fcs_bmp);
    //  }
    //  else
    //  {
    //      list_para->h_item_fcs_bmp = theme_open(ID_EXPLORER_LISTBAR_ITEM_F_BMP);
    //      pic_buf = theme_hdl2buf(list_para->h_item_fcs_bmp);
    //  }
    //picX = draw_param->rect.x + FocusIconRect.x;
    //picY = draw_param->rect.y + FocusIconRect.y;
    picX = draw_param->rect.x ;
    picY = draw_param->rect.y ;
    GUI_BMP_Draw(pic_buf, picX, picY);
    //GUI_BMP_Draw(pic_buf, picX, picY+45);
    return ret;
}
/*
*************************************************************************************
*Function   :       static __s32 save_focus_item_info(__lbar_draw_para_t *draw_param)
*
*Description    :   保存焦点条目信息
*
*Arguments      :
*
*Return         :
*
**************************************************************************************
*/
static __s32 save_focus_item_info(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;
    file_item_t *file_item = NULL;
    list_para = (explr_list_para_t *)draw_param->attr;

    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)    ///
    {
        file_item = get_file_list_item(list_para->cur_file_list, draw_param->index);

        if (file_item != NULL)
        {
            list_para->file_item = file_item;

            if (draw_param->mode == LBAR_MODE_FOCUS)
            {
                list_para->cur_file_list->cur_item = file_item;     //保存当前条目信息
            }
        }
        else
        {
            __msg("error in file list");
            return EPDK_FAIL;
        }
    }
    else            //
    {
        if (list_para->enter_sd_usb_flag == 0)  //在sd /usb 选择窗口中
        {
            file_item = get_file_list_item(list_para->cur_file_list, draw_param->index);

            if (file_item != NULL)
            {
                list_para->file_item = file_item;

                if (draw_param->mode == LBAR_MODE_FOCUS)
                {
                    list_para->cur_file_list->cur_item = file_item;     //保存当前条目信息
                }
            }
        }
    }

    return EPDK_OK;
}

/*
********************************************************************************************
*Function   :       static __s32 draw_mediatype_listview_item_text(__lbar_draw_para_t *draw_param)
*
*Description    :    显示条目文本
*
*Arguments      :
*
*Return         :
*
*******************************************************************************************
*/
static __s32 draw_mediatype_listview_item_text(__lbar_draw_para_t *draw_param)
{
    int ret;
    explr_list_para_t *list_para;
    char FileName[RAT_MAX_FULL_PATH_LEN];
    GUI_RECT GUI_TextRect;
    RECT TextRect;
    file_item_t *file_item = NULL;
    list_para = (explr_list_para_t *)draw_param->attr;
    ret = explorer_get_mediatype_item_text_rect(list_para, &TextRect);

    //switch(draw_param->index)
    switch (list_para->mediatype_index)
    {
        case ID_MEDIATYPE_ALL_ITEM:
        {
            dsk_langres_get_menu_text(STRING_EXPLR_ALL_MEDIA, FileName, sizeof(FileName)) ;
        }
        break;

        case ID_MEDIATYPE_MUSIC_ITEM:
        {
            dsk_langres_get_menu_text(STRING_EXPLR_MUSIC_MEDIA, FileName, sizeof(FileName)) ;
        }
        break;

        case ID_MEDIATYPE_MOVIE_ITEM:
        {
            dsk_langres_get_menu_text(STRING_EXPLR_MOVIE_MEDIA, FileName, sizeof(FileName)) ;
        }
        break;

        case ID_MEDIATYPE_EBOOK_ITEM:
        {
            dsk_langres_get_menu_text(STRING_EXPLR_EBOOK_MEDIA, FileName, sizeof(FileName)) ;
        }
        break;

        case ID_MEDIATYPE_PHOTO_ITEM:
        {
            dsk_langres_get_menu_text(STRING_EXPLR_PHOTO_MEDIA, FileName, sizeof(FileName)) ;
        }
        break;
#if GAME_SW

        case ID_MEDIATYPE_GAME_ITEM:
        {
            dsk_langres_get_menu_text(STRING_EXPLR_GAME_MEDIA, FileName, sizeof(FileName)) ;
        }
        break;
#endif

        default:
            break;
    }

    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_TextRect.x0 = draw_param->rect.x  + TextRect.x;
    GUI_TextRect.y0 = draw_param->rect.y  + TextRect.y;
    GUI_TextRect.x1 = draw_param->rect.x  + (TextRect.x + TextRect.width);
    GUI_TextRect.y1 = draw_param->rect.y  + (TextRect.y + TextRect.height);
    GUI_DispStringInRect(FileName, &GUI_TextRect, GUI_TA_LEFT | GUI_TA_VCENTER);
    return EPDK_OK;
}



/*
********************************************************************************************
*Function   :       static __s32 draw_listview_item_text(__lbar_draw_para_t *draw_param)
*
*Description    :    显示条目文本
*
*Arguments      :
*
*Return         :
*
*******************************************************************************************
*/

static __s32 draw_listview_item_text(__lbar_draw_para_t *draw_param)
{
    int ret;
    explr_list_para_t *list_para = NULL;
    char FileName[RAT_MAX_FULL_PATH_LEN] = {0};
    GUI_RECT GUI_TextRect;
    RECT TextRect;
    file_item_t *file_item = NULL;
    __s64 file_size;
    list_para = (explr_list_para_t *)draw_param->attr;

    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        file_item = get_file_list_item(list_para->cur_file_list, draw_param->index);

        if (file_item != NULL)
        {
            list_para->file_item = file_item;

            if ((file_item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE) || (file_item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE))
            {
                get_root_device_disp_name(FileName, file_item->fatdirattr);
            }
            else
            {
                eLIBs_strcpy(FileName, file_item->name);
            }
        }
        else
        {
            __msg("draw_listview_item_text error in file list");
            return EPDK_FAIL;
        }
    }
    else
    {
        /**if( list_para->enter_sd_usb_flag == 0 )
        {

            file_item = get_file_list_item(list_para->cur_file_list, draw_param->index);
            if(file_item != NULL)
            {
                list_para->file_item = file_item;
                if((file_item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
                    ||(file_item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE))
                {
                    get_root_device_disp_name(FileName, file_item->fatdirattr);
                }
            }
        }
        else
            **/
        {
            eLIBs_memset(FileName, 0, sizeof(FileName));
            ret = GetListItemFileName(list_para, draw_param->index, FileName);//只取文件名

            if (ret == EPDK_FAIL)
            {
                __wrn("get file information from rat fail!!");
                return EPDK_FAIL;
            }
        }
    }

    ret = explorer_get_item_text_rect(list_para, &TextRect);
    GUI_SetFont(SWFFont);
    GUI_UC_SetEncodeUTF8();
    GUI_SetColor(GUI_BLACK);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_TextRect.x0 = draw_param->rect.x  + TextRect.x;
    GUI_TextRect.y0 = draw_param->rect.y  + TextRect.y;
    GUI_TextRect.x1 = draw_param->rect.x  + (TextRect.x + TextRect.width);
    GUI_TextRect.y1 = draw_param->rect.y  + (TextRect.y + TextRect.height);
    GUI_DispStringInRect(FileName, &GUI_TextRect, GUI_TA_LEFT | GUI_TA_VCENTER);
#if 0

    if (FSYS_ATTR_DIRECTORY != file_item->fatdirattr)
    {
        if (RAT_MEDIA_TYPE_ALL == list_para->media_type)
        {
            eLIBs_memset(FileName, 0, sizeof(FileName));
            eLIBs_strcat(FileName, list_para->cur_file_list->file_path);
            eLIBs_strcat(FileName, "\\");
            eLIBs_strcat(FileName, list_para->file_item->name);
        }
        else
        {
            ret = GetListItemFileFullPath(list_para, draw_param->index, FileName);//取全路径名
        }

        file_size = eLIBs_GetFileSize(FileName);
        explorer_draw_FileSize(draw_param, file_size, list_para);
        explorer_draw_file_create_time(draw_param, FileName, list_para);
    }

#endif

    if (draw_param->mode == LBAR_MODE_FOCUS)
    {
        explorer_list_long_string_start_roll_fast(draw_param, FileName);
    }

    return EPDK_OK;
}

#if 0
__s32 explorer_draw_file_info(explr_list_para_t *list_para)
{
    rat_media_type_t media_type;
    char filename[RAT_MAX_FULL_PATH_LEN];
    file_item_t *cur_item = NULL;
    GUI_MEMDEV_Handle   draw_mem;
    explorer_viewer_ui_t *ui_param;
    void *p_buf ;
    RECT rect ;
    __s32 file_size;

    if (list_para == NULL)
    {
        return EPDK_FAIL;
    }

    cur_item = list_para->cur_file_list->cur_item;

    if (cur_item == NULL)
    {
        return EPDK_FAIL;
    }

    ui_param = explorer_get_viewer_ui_param();
    p_buf = explorer_get_listview_icon_res(ID_EXP_INFO_BG);
    rect.x = ui_param->list_item_ui_param.file_info_area_rect.x ;
    rect.y = ui_param->list_item_ui_param.file_info_area_rect.y ;
    rect.width = ui_param->list_item_ui_param.file_info_area_rect.width ;
    rect.height = ui_param->list_item_ui_param.file_info_area_rect.height ;
    draw_mem = GUI_MEMDEV_Create(rect.x, rect.y, rect.width, rect.height);
    GUI_MEMDEV_Select(draw_mem);
    media_type = get_file_list_item_file_type(cur_item);

    if ((media_type & FSYS_ATTR_DIRECTORY)
        || (media_type == RAT_MEDIA_TYPE_SD_DEVICE)
        || (media_type == RAT_MEDIA_TYPE_USB_DEVICE))
    {
        //  explorer_clear_filesize_area( list_para );
        //  explorer_clear_file_create_time_area( list_para );
    }
    else        //文件属性时显示文件信息
    {
        file_size = cur_item->file_size;
        explorer_draw_FileSize(file_size, list_para);
        eLIBs_strcpy(filename, list_para->cur_file_list->file_path);
        eLIBs_strcat(filename, "\\");
        eLIBs_strcat(filename, cur_item->name);
        explorer_draw_file_create_time(filename, list_para);
    }

    GUI_SetDrawMode(LCD_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}
#endif
//draw item in list mode, 不要在Listbar的回调函数中选择图层
static __s32 draw_mediatype_listview_item(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;
    rat_media_type_t media_type;
    GUI_MEMDEV_Handle   draw_mem = NULL ;
    list_para = (explr_list_para_t *)draw_param->attr;

    if (draw_param->mode == LBAR_MODE_FOCUS)
    {
    }
    else if (draw_param->mode == LBAR_MODE_NORMAL)
    {
        draw_mediatype_list_item_unfocus_icon(draw_param);      //画unfocus item 背景框图
    }

    __msg("draw_param->rect.x=%d %d %d %d", draw_param->rect.x, draw_param->rect.y, draw_param->rect.width, draw_param->rect.height);
    draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y, \
                                 draw_param->rect.width, draw_param->rect.height);
    GUI_MEMDEV_Select(draw_mem);

    if (draw_param->mode == LBAR_MODE_FOCUS)
    {
        draw_mediatype_list_item_focus_icon(draw_param);        //画focus item 背景框图
    }

    // 对于图片，画其对应的缩略图，在lbar_mode_diff中画
    if (draw_param->mode == LBAR_MODE_NORMAL
        || draw_param->mode == LBAR_MODE_FOCUS)
    {
        //画媒体对应类型的图标(显示在每个item 的左侧)
        draw_mediatype_listview_item_icon(draw_param);
    }

    draw_mediatype_listview_item_text(draw_param);
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL) ;
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL ;
    return 0;
}


//draw item in list mode, 不要在Listbar的回调函数中选择图层
static __s32 draw_listview_item(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;
    rat_media_type_t media_type;
    list_para = (explr_list_para_t *)draw_param->attr;

    if (draw_param->mode == LBAR_MODE_FOCUS)
    {
        if (list_para->view_mode == EXPLR_LIST_MODE)
        {
            //draw_list_item_focus_icon(draw_param);        //画focus item 背景框图
        }
        else if (list_para->view_mode == EXPLR_SQUARE_MODE)
        {
            draw_square_item_focus_icon(draw_param);
        }
        else
        {
            //  draw_list_item_focus_icon(draw_param);      //画focus item 背景框图
        }
    }
    else if (draw_param->mode == LBAR_MODE_NORMAL)      //modify 2011.8.3
    {
        if (list_para->view_mode == EXPLR_LIST_MODE)
        {
            draw_list_item_unfocus_icon(draw_param);        //画unfocus item 背景框图
        }
        else if (list_para->view_mode == EXPLR_SQUARE_MODE)
        {
            //  draw_square_item_focus_icon(draw_param);
        }
    }

    if ((list_para->view_mode == EXPLR_SQUARE_MODE)
        && (list_para->media_type == RAT_MEDIA_TYPE_PIC))
    {
        explorer_list_long_string_stop_roll(list_para);     //停止长文件名滚动
    }
    else
    {
        save_focus_item_info(draw_param);

        if (list_para->media_type != RAT_MEDIA_TYPE_ALL)
        {
            file_item_t *file_item ;
            /*
            if( list_para->enter_sd_usb_flag == 0)
            {
                file_item = get_file_list_item(list_para->cur_file_list, draw_param->index);
                if(  (file_item != NULL) &&  (  (file_item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE)
                    || (file_item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE) ) )
                {

                    if( file_item->fatdirattr == RAT_MEDIA_TYPE_USB_DEVICE )
                    {

                        media_type = RAT_MEDIA_TYPE_USB_DEVICE ;
                    }
                    else if( file_item->fatdirattr == RAT_MEDIA_TYPE_SD_DEVICE )
                    {

                        media_type = RAT_MEDIA_TYPE_SD_DEVICE ;
                    }
                    else
                    {
                        media_type = RAT_MEDIA_TYPE_UNKNOWN ;

                    }
                }
            }
            else
            */
            {
                //获取媒体对应的类型
                media_type = GetListItemFileMediaType(list_para->rat.handle, draw_param->index);
                __wrn("media_type:%d\r", media_type);
            }
        }
        else
        {
            media_type = get_file_list_item_file_type(list_para->file_item);
        }

        {
            GUI_MEMDEV_Handle   draw_mem = NULL ;
            __msg("draw_param->rect.x=%d %d %d %d", draw_param->rect.x, draw_param->rect.y, draw_param->rect.width, draw_param->rect.height);
            draw_mem = GUI_MEMDEV_Create(draw_param->rect.x, draw_param->rect.y, \
                                         draw_param->rect.width, draw_param->rect.height);

            if (!draw_mem)
            {
                __msg("create memory device fail ");

                if (draw_param->mode == LBAR_MODE_FOCUS)
                {
                    if (list_para->view_mode == EXPLR_LIST_MODE)
                    {
                        draw_list_item_focus_icon(draw_param);      //画focus item 背景框图
                    }
                    else if (list_para->view_mode == EXPLR_SQUARE_MODE)
                    {
                        draw_square_item_focus_icon(draw_param);
                    }
                }

                // 对于图片，画其对应的缩略图，在lbar_mode_diff中画
                if (draw_param->mode == LBAR_MODE_NORMAL || draw_param->mode == LBAR_MODE_FOCUS)
                {
                    //画媒体对应类型的图标(显示在每个item 的左侧)
                    draw_listview_item_icon(draw_param, media_type);
                }

                draw_listview_item_text(draw_param);
            }
            else
            {
                GUI_MEMDEV_Select(draw_mem);

                if (draw_param->mode == LBAR_MODE_FOCUS)
                {
                    if (list_para->view_mode == EXPLR_LIST_MODE)
                    {
                        draw_list_item_focus_icon(draw_param);      //画focus item 背景框图
                    }
                    else if (list_para->view_mode == EXPLR_SQUARE_MODE)
                    {
                        draw_square_item_focus_icon(draw_param);
                    }
                }

                // 对于图片，画其对应的缩略图，在lbar_mode_diff中画
                if (draw_param->mode == LBAR_MODE_NORMAL || draw_param->mode == LBAR_MODE_FOCUS)
                {
                    //画媒体对应类型的图标(显示在每个item 的左侧)
                    draw_listview_item_icon(draw_param, media_type);
                }

                draw_listview_item_text(draw_param);
                GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
                GUI_MEMDEV_CopyToLCD(draw_mem);
                GUI_MEMDEV_Select(NULL) ;
                GUI_MEMDEV_Delete(draw_mem);
                draw_mem = NULL ;
            }
        }
    }

    //if(draw_param->mode == LBAR_MODE_FOCUS)
    {
        //if(list_para->media_type == RAT_MEDIA_TYPE_ALL)       //文件管理
        {
            //explorer_draw_file_info(list_para);
        }
    }
    return EPDK_OK;
}
/*
****************************************************************************************
*Function   :  static __s32 _explorer_mediatype_listbar_item_draw(__lbar_draw_para_t *draw_param)
*
*Description    :    ListBar绘制消息处理函数
*
*Arguments      :
*
*Return         :
**************************************************************************************
*/
static __s32 _explorer_mediatype_listbar_item_draw(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para;
    explorer_viewer_ui_t *ui_param;
    ui_param = explorer_get_viewer_ui_param();
    list_para = (explr_list_para_t *)draw_param->attr;

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
            draw_mediatype_listview_item(draw_param);
            break;

        case LBAR_MODE_FOCUS:
            draw_mediatype_listview_item(draw_param);
            list_para->rat.index = draw_param->index ;
            break;

        case LBAR_MODE_INVALID:
            break;

        case LBAR_MODE_VACANT:
            draw_mediatype_list_item_vacent_icon(draw_param);
            break;

        default:
            break;
    }

    return EPDK_OK;
}
#if 1
static __s32 explorer_show_item_detail(__lbar_draw_para_t *draw_param)
{
    __s32 i = 0;
    GUI_RECT gui_rect = {0};
    ES_TIMESTAMP mtime = {0};
    char temp_array[24] = {0};
    explr_list_para_t *list_para = NULL;
    GUI_MEMDEV_Handle   draw_mem = NULL;
    char string_detail_prompt[128] = {0};
    char FileName[RAT_MAX_FULL_PATH_LEN] = {0};
    __wrn("Enter %s\r", __FUNCTION__);
    list_para = (explr_list_para_t *)draw_param->attr;
    draw_mem = GUI_MEMDEV_Create(60, 54 * 4, 300 - 60 * 2, 54 * 2);

    if (draw_mem == NULL)
    {
        __wrn("GUI_MEMDEV_Create ERROR\r");
    }

    GUI_MEMDEV_Select(draw_mem);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_SetBkColor(0xccffffff);
    gui_rect.x0 = 60;
    gui_rect.y0 = 54 * 4;
    gui_rect.x1 = gui_rect.x0 + 300 - 60 * 2;
    gui_rect.y1 = gui_rect.y0 + 54;
    GUI_ClearRectEx(&gui_rect);
    GetListItemFileName(list_para, draw_param->index, FileName);
    GetListItemFileFullPath(list_para, draw_param->index, FileName);
    filesize2str(eLIBs_GetFileSize(FileName), temp_array);
    dsk_langres_get_menu_text(STRING_EXPLR_FILE_SIZE, string_detail_prompt, sizeof(string_detail_prompt));
    eLIBs_strcat(string_detail_prompt, temp_array);
    __wrn("dispaly file size:%s\r", string_detail_prompt);
    GUI_DispStringInRect(string_detail_prompt, &gui_rect, GUI_TA_LEFT);
    eLIBs_memset(temp_array, 0x00, sizeof(temp_array));
    eLIBs_memset(string_detail_prompt, 0x00, sizeof(string_detail_prompt));
    eLIBs_memset(&gui_rect, 0x00, sizeof(gui_rect));
    gui_rect.x0 = 60;
    gui_rect.y0 = 54 * 4 + 54;
    gui_rect.x1 = gui_rect.x0 + 300 - 60 * 2;
    gui_rect.y1 = gui_rect.y0 + 54;
    GUI_ClearRectEx(&gui_rect);
    eLIBs_GetFileMTime(FileName, &mtime);
    mtime_to_date_string(&mtime, temp_array);
    dsk_langres_get_menu_text(STRING_EXPLR_FILE_TIME, string_detail_prompt, sizeof(string_detail_prompt));
    eLIBs_strcat(string_detail_prompt, temp_array);
    __wrn("dispaly file time:%s\r", string_detail_prompt);
    GUI_DispStringInRect(string_detail_prompt, &gui_rect, GUI_TA_LEFT);
    eLIBs_memset(temp_array, 0x00, sizeof(temp_array));
    eLIBs_memset(string_detail_prompt, 0x00, sizeof(string_detail_prompt));
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return EPDK_OK;
}
#endif
/*
****************************************************************************************
*Function   :  static __s32 _explorer_listbar_item_draw(__lbar_draw_para_t *draw_param)
*
*Description    :    ListBar绘制消息处理函数
*
*Arguments      :
*
*Return         :
**************************************************************************************
*/
static __s32 _explorer_listbar_item_draw(__lbar_draw_para_t *draw_param)
{
    explr_list_para_t *list_para = NULL;
    explorer_viewer_ui_t *ui_param = NULL;
    //__s32 page_item_cnt;
    //__listbar_scene_t scene;
    ui_param = explorer_get_viewer_ui_param();
    list_para = (explr_list_para_t *)draw_param->attr;

    if (list_para->rat.total <= 0)
    {
        return EPDK_FAIL;
    }

    switch (draw_param->mode)
    {
        case LBAR_MODE_NORMAL:
            __wrn("_explorer_listbar_item_draw LBAR_MODE_NORMAL\r");;
            draw_listview_item(draw_param);
            break;

        case LBAR_MODE_FOCUS:

            /*shiql solve file item blink*/
            if (LISTBAR_RIGHT == list_para->listbar_left_right)
            {
                __wrn("_explorer_listbar_item_draw LBAR_MODE_FOCUS\r");;
                __wrn("draw_param->index:%d\r", draw_param->index);;
                draw_listview_item(draw_param);
                explorer_show_item_detail(draw_param);
            }

            list_para->rat.index = draw_param->index ;

            if (list_para->media_type == RAT_MEDIA_TYPE_PIC)
            {
                //    draw_miniature_for_pic_item(draw_param);      // need to fixed
            }

            break;

        case LBAR_MODE_INVALID:
            break;

        case LBAR_MODE_VACANT:
            draw_list_item_vacent_icon(draw_param);
            break;

        case LBAR_MODE_DIFFUSE:
            __wrn("_explorer_listbar_item_draw LBAR_MODE_DIFFUSE\r");;
            __msg("draw_param->mode=%d,draw_param->index=%d", draw_param->mode, draw_param->index);
            //draw_miniature_for_pic_item(draw_param);  // need to fixed
            break;
#if (EXPLORER_SUPPORT_MINIATURE == 1)

        case LBAR_MODE_MINIATURE:

            //          __msg("LBAR_MODE_MINIATURE draw_param->index =%d list_para->big_miniature_index =%d",draw_param->index , list_para->big_miniature_index);
            //  __wrn("_explorer_listbar_item_draw LBAR_MODE_MINIATURE\r");
            //  __msg("draw_param->mode=%d,draw_param->index=%d",draw_param->mode,draw_param->index);
            if ((draw_param->index != list_para->big_miniature_index) && (list_para->enter_sd_usb_flag == 1))
            {
                //__here__;
                list_para->big_miniature_index = draw_param->index;
                //explorer_draw_pic_miniature_bmp( list_para , draw_param->index );
            }

            break;
#endif

        default:
            break;
    }

    return EPDK_OK;
}
/*
******************************************************************************************
*Function   :       void explorer_mediatype_listbar_init(H_WIN  list_win)
*
*Description    :   list左边 ListBar 初始化函数
*
*Arguments      :
*
*Return         :
*
*****************************************************************************************
*/
#if 0 //shiql change for D100 explorer left item change
static void explorer_mediatype_listbar_init(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    __listbar_config_t config;
    __s32 item_width = 0, item_height = 0;
    explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();//获得整个AP的UI设计参数
    __msg("~~~~~~~~~~Listbar is initializing~~~~~~~~~~~");
    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    //list_para->last_focus_index = list_para->last_focused_id;
    //if(list_para->view_mode == EXPLR_LIST_MODE)
    {
        config.item_width = ui_param->mediatype_list_item_ui_param.item_rect.width; //根据list or square模式不同
        config.item_height = ui_param->mediatype_list_item_ui_param.item_rect.height;
        config.list_rect.x = ui_param->mediatype_list_item_ui_param.ListBarArea.x;
        config.list_rect.y = ui_param->mediatype_list_item_ui_param.ListBarArea.y;
        config.list_rect.width = ui_param->mediatype_list_item_ui_param.ListBarArea.width;
        config.list_rect.height = ui_param->mediatype_list_item_ui_param.ListBarArea.height;
    }
    __wrn("list_para->mediatype_index = %d ...............", list_para->mediatype_index);

    if (list_para->mediatype_index > 2) //  item_per_page -1
    {
        config.start_id = list_para->mediatype_index - list_para->mediatype_index % 3; // config.focus_id -
    }
    else
    {
        config.start_id = 0;//112350 list_para->last_start_id;
    }

    config.focus_id = list_para->mediatype_index;
    //112350 list_para->last_focused_id;
    config.bk_color = 0;
    config.alpha_status = 1;                        //打开Alpha开关
    config.list_attr = (void *)list_para;
    config.item_cnt = ID_MEDIATYPE_MAX;
    list_para->mediatype_listbar_handle = LISTBAR_Create(list_win);                 //new listbar
    //config parametters for listbar
    LISTBAR_Config(list_para->mediatype_listbar_handle, _explorer_mediatype_listbar_item_draw, &config);
    //scrollbar belongs to listbar
    //LISTBAR_ScrollConfig(list_para->listbar_handle, &scroll_param);
    //LISTBAR_ShowPage(list_para->listbar_handle);
}
static void explorer_mediatype_listbar_init(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    __listbar_config_t config;
    __s32 item_width = 0, item_height = 0;
    explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();//获得整个AP的UI设计参数
    __msg("~~~~~~~~~~Listbar is initializing~~~~~~~~~~~");
    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    //list_para->last_focus_index = list_para->last_focused_id;
    //if(list_para->view_mode == EXPLR_LIST_MODE)
    {
        config.item_width = ui_param->mediatype_list_item_ui_param.item_rect.width; //根据list or square模式不同
        config.item_height = ui_param->mediatype_list_item_ui_param.item_rect.height;
        config.list_rect.x = ui_param->mediatype_list_item_ui_param.ListBarArea.x;
        config.list_rect.y = ui_param->mediatype_list_item_ui_param.ListBarArea.y;
        config.list_rect.width = ui_param->mediatype_list_item_ui_param.ListBarArea.width;
        config.list_rect.height = ui_param->mediatype_list_item_ui_param.ListBarArea.height;
    }
    __wrn("list_para->mediatype_index = %d ...............", list_para->mediatype_index);
    config.start_id = 0;//112350 list_para->last_start_id;
    config.focus_id = 0;
    //112350 list_para->last_focused_id;
    config.bk_color = 0;
    config.alpha_status = 1;                        //打开Alpha开关
    config.list_attr = (void *)list_para;
    config.item_cnt = 1;
    list_para->mediatype_listbar_handle = LISTBAR_Create(list_win);                 //new listbar
    //config parametters for listbar
    LISTBAR_Config(list_para->mediatype_listbar_handle, _explorer_mediatype_listbar_item_draw, &config);
    //scrollbar belongs to listbar
    //LISTBAR_ScrollConfig(list_para->listbar_handle, &scroll_param);
    //LISTBAR_ShowPage(list_para->listbar_handle);
}

#endif


/*
******************************************************************************************
*Function   :       void explorer_listbar_init(H_WIN  list_win)
*
*Description    :    ListBar 初始化函数,注意在这之前Rat必须已经过初始化
*
*Arguments      :
*
*Return         :
*
*****************************************************************************************
*/
static void explorer_paint_media_list(H_WIN win)
{
    HTHEME handle = NULL;
    void *res_buffer = NULL;
    GUI_RECT gui_rect = {0};
    GUI_MEMDEV_Handle   draw_mem = NULL;
    explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();
    explr_list_para_t *list_para = (explr_list_para_t *)GUI_WinGetAttr(win);
    __wrn("Enter :%s\r", __FUNCTION__);
    GUI_LyrWinSel(list_para->list_layer);
    draw_mem = GUI_MEMDEV_Create(ui_param->mediatype_list_item_ui_param.ListBarArea.x, ui_param->mediatype_list_item_ui_param.ListBarArea.y,
                                 ui_param->mediatype_list_item_ui_param.ListBarArea.width, ui_param->mediatype_list_item_ui_param.ListBarArea.height);
    GUI_MEMDEV_Select(draw_mem);
    //    GUI_SetFont(general_attr->font);
    GUI_UC_SetEncodeUTF8();
    GUI_SetDrawMode(GUI_DRAWMODE_TRANS);
    GUI_SetBkColor(0xccffffff);
    //  GUI_SetColor(general_attr->focus_txt_color);
    handle = theme_open(ID_EXPLORER_LEFT_LIST_BG_BMP);
    res_buffer = theme_hdl2buf(handle);
    GUI_BMP_Draw(res_buffer, ui_param->mediatype_list_item_ui_param.ListBarArea.x, ui_param->mediatype_list_item_ui_param.ListBarArea.y);
    dsk_theme_close(handle);
    handle = NULL;
    res_buffer = NULL;

    if (list_para->media_type == RAT_MEDIA_TYPE_PIC)//PICTURE
    {
        handle = theme_open(ID_HOME_NEW_PICTURE_UF_BMP);
    }
    else if (list_para->media_type == RAT_MEDIA_TYPE_VIDEO) //VIDEO
    {
        handle = theme_open(ID_HOME_NEW_MOVIE_UF_BMP);
    }
    else  // ebook
    {
        handle = theme_open(ID_HOME_NEW_EBOOK_UF_BMP);
    }

    res_buffer = theme_hdl2buf(handle);
    GUI_BMP_Draw(res_buffer, ui_param->mediatype_list_item_ui_param.ListBarArea.x + 60, ui_param->mediatype_list_item_ui_param.ListBarArea.y + 20);
    dsk_theme_close(handle);
    handle = NULL;
    GUI_SetDrawMode(GUI_DRAWMODE_NORMAL);
    GUI_MEMDEV_CopyToLCD(draw_mem);
    GUI_MEMDEV_Select(NULL);
    GUI_MEMDEV_Delete(draw_mem);
    draw_mem = NULL;
    return ;
}
static void explorer_listbar_init(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    __listbar_config_t config;
    __scroll_bar_t scroll_param;                        //new scroll bar
    __s32 item_width = 0, item_height = 0;
    explorer_viewer_ui_t *ui_param = explorer_get_viewer_ui_param();//获得整个AP的UI设计参数
    __msg("~~~~~~~~~~Listbar is initializing~~~~~~~~~~~");
    eLIBs_memset(&config, 0, sizeof(__listbar_config_t));
    eLIBs_memset(&scroll_param, 0, sizeof(__scroll_bar_t));
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);
    list_para->last_focus_index = list_para->last_focused_id;

    if (list_para->view_mode == EXPLR_LIST_MODE)
    {
        item_width = ui_param->list_item_ui_param.item_rect.width;  //根据list or square模式不同
        item_height = ui_param->list_item_ui_param.item_rect.height;
        config.list_rect.x = ui_param->list_item_ui_param.ListBarArea.x;
        config.list_rect.y = ui_param->list_item_ui_param.ListBarArea.y;
        config.list_rect.width = ui_param->list_item_ui_param.ListBarArea.width;
        config.list_rect.height = ui_param->list_item_ui_param.ListBarArea.height;
    }
    else if (list_para->view_mode == EXPLR_SQUARE_MODE)
    {
        item_width = ui_param->square_item_ui_param.item_rect.width;
        item_height = ui_param->square_item_ui_param.item_rect.height;
        config.list_rect.x = ui_param->square_item_ui_param.ListBarArea.x;
        config.list_rect.y = ui_param->square_item_ui_param.ListBarArea.y;
        config.list_rect.width = ui_param->square_item_ui_param.ListBarArea.width;
        config.list_rect.height = ui_param->square_item_ui_param.ListBarArea.height;
    }

    config.item_width = item_width;
    config.item_height = item_height;
    config.start_id = list_para->last_start_id;
    config.focus_id = list_para->last_focused_id;
    config.bk_color = 0;
    config.alpha_status = 1;                        //打开Alpha开关
    config.list_attr = (void *)list_para;
    config.item_cnt = list_para->rat.total;   //Rat.total必须已经获得
    __wrn("===========config.item_cnt=%d", config.item_cnt);
#if 0
    scroll_param.show_rect.x = ui_param->scroll_bg_rect.x;
    scroll_param.show_rect.y = ui_param->scroll_bg_rect.y;
    scroll_param.show_rect.width = ui_param->scroll_bg_rect.width;
    scroll_param.show_rect.height = ui_param->scroll_bg_rect.height;//整个Scoll bar显示区域，
    scroll_param.scroll_rect.x = ui_param->scroll_rect.x;
    scroll_param.scroll_rect.y = ui_param->scroll_rect.y;
    scroll_param.scroll_rect.width = ui_param->scroll_rect.width;
    scroll_param.scroll_rect.height = ui_param->scroll_rect.height; //滚动条区域(除去head,body,tail)
    scroll_param.head_height = ui_param->scroll_head_height;
    scroll_param.tail_height = ui_param->scroll_tail_height;                //head, tail
    scroll_param.back_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_BG);
    scroll_param.head_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_HEAD);
    scroll_param.body_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_BODY);
    scroll_param.tail_bmp = explorer_get_listview_icon_res(ID_EXP_LIST_SCROLL_TAIL);
    __wrn("===========apply res complete");
#endif
    /*__msg("----back_bmp: %x, head_bmp: %x, body_bmp: %x, tail_bmp: %x",\
       scroll_param.back_bmp, scroll_param.head_bmp, scroll_param.body_bmp, scroll_param.tail_bmp);
     */
    list_para->listbar_handle = LISTBAR_Create(list_win);                   //new listbar
    //config parametters for listbar
    __wrn("=============list_para->listbar_handle=%0x", list_para->listbar_handle);
    LISTBAR_Config(list_para->listbar_handle, _explorer_listbar_item_draw, &config);
    //scrollbar belongs to listbar
    __wrn("=============_explorer_listbar_item_draw is ok");
    //   LISTBAR_ScrollConfig(list_para->listbar_handle, &scroll_param);
    //LISTBAR_ShowPage(list_para->listbar_handle);
}

/*
*********************************************************************************
*Function   :       void explorer_mediatype_listbar_uninit(H_WIN  list_win)
*
*Description    :    释放Listbar
*
*Arguments      :
*
*Return         :
*
**********************************************************************************
*/
static void explorer_mediatype_listbar_uninit(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);

    if (list_para->mediatype_listbar_handle != NULL)
    {
        __msg("----LISTBAR_Delete begin: %x", list_para->mediatype_listbar_handle);
        LISTBAR_SuspendPage(list_para->mediatype_listbar_handle);
        LISTBAR_Delete(list_para->mediatype_listbar_handle);
        list_para->mediatype_listbar_handle = NULL;
    }

    return;
}


/*
*********************************************************************************
*Function   :       void explorer_listbar_uninit(H_WIN  list_win)
*
*Description    :    释放Listbar
*
*Arguments      :
*
*Return         :
*
**********************************************************************************
*/
static void explorer_listbar_uninit(H_WIN  list_win)
{
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);

    if (list_para->listbar_handle != NULL)
    {
        __msg("----LISTBAR_Delete begin: %x", list_para->listbar_handle);
        LISTBAR_SuspendPage(list_para->listbar_handle);
        LISTBAR_Delete(list_para->listbar_handle);
        list_para->listbar_handle = NULL;
    }

    return;
}

/*
*************************************************************************************************
*Function:void ExplorerSetViewMode(explr_list_para_t *list_para, explorer_view_mode_e view_mode)
*
*Description    :    改变Explorer显示方式List mode or square mode
*
*Arguments      :
*
*Return         :
*
*************************************************************************************************
*/
void ExplorerSetViewMode(explr_list_para_t *list_para, explorer_view_mode_e view_mode)
{
    if (view_mode == EXPLR_LIST_MODE)
    {
        list_para->view_mode = EXPLR_LIST_MODE;
        GUI_LyrWinSel(list_para->list_layer);
        GUI_SetBkColor(0);
        GUI_Clear();
    }
    else if (view_mode == EXPLR_SQUARE_MODE)
    {
        list_para->view_mode = EXPLR_SQUARE_MODE;
    }

    explorer_save_last_para(list_para);
    explorer_get_last_para(list_para) ;
    explorer_listbar_uninit(list_para->list_win);       //重新创建listbar,因为Square为全屏模式
    explorer_listbar_init(list_para->list_win);
    //rat_move_cursor_to_first(list_para->rat.handle);
    LISTBAR_ShowPage(list_para->listbar_handle);

    //LISTBAR_ChangeMode(   list_para->listbar_handle,item_width,item_height);
    //LISTBAR_UpdatePage(list_para->listbar_handle);
    if (list_para->view_mode == EXPLR_LIST_MODE)
    {
        explorer_draw_FileTypeIcon(list_para, list_para->media_type);
    }

    if (list_para->listbar_left_right == LISTBAR_LEFT)
    {
        LISTBAR_LostFocus(list_para->listbar_handle);
    }

    return;
}

//创建文件列表顶部断点
static __s32 explorer_file_list_init(explr_list_para_t *list_para)
{
    __u32 total = 0;
    __inf("Enter %s\r", __FUNCTION__);

    if (list_para == NULL)
    {
        __inf("%s %d list_para == NULL \r", __FILE__, __LINE__);
        return EPDK_FAIL;
    }

    if (list_para->top_file_list == NULL)
    {
#if 1
        list_para->top_file_list = new_file_root_list_nod(NULL);

        if (list_para->top_file_list == NULL)
        {
            __inf("list_para->top_file_list null!\r");
            return EPDK_FAIL;
        }

        list_para->cur_file_list = list_para->top_file_list;
        __inf("list_para->cur_file_list->total= %d", list_para->cur_file_list->total);
        list_para->rat.total = list_para->cur_file_list->total;
#else
        list_para->top_file_list = new_file_list_nod(list_para->search_path, NULL);

        if (list_para->top_file_list == NULL)
        {
            return EPDK_FAIL;
        }

        list_para->cur_file_list = list_para->top_file_list;
        list_para->rat.total = list_para->cur_file_list->total;
#endif
    }

    //  __wrn("explorer_file_list_init 3");
    //rat_start_miniature_decode(); //开始缩略图线程
    // __wrn("explorer_file_list_init 4");
    return EPDK_OK;
}


//删除整个文件列表
static __s32 explorer_file_list_uninit(explr_list_para_t *list_para)
{
    if (list_para->top_file_list != NULL)
    {
        delete_file_list_chain(list_para->top_file_list);
        list_para->top_file_list = NULL;
    }

    //rat_stop_miniature_decode();  //停止 缩略图线程
    return EPDK_FAIL;
}

/*
**********************************************************************************
*Function   :       static __s32 explorer_rat_init(explr_list_para_t *list_para)
*
*Description    :    搜索文件Module, Rat 初始化
*
*Arguments      :
*
*Return         :
*
*********************************************************************************
*/
static __s32 explorer_rat_init(explr_list_para_t *list_para)
{
    /*
    {
        file_item_t *item_list = NULL;
        __u32 total = 0;
        //__wait__;
        total = new_file_list(list_para->search_path,&item_list, NULL);
        delete_file_list(item_list,total);
    }
    */
    if (list_para->rat.handle == NULL)
    {
        //list_para->rat.handle = rat_open(list_para->search_path, list_para->media_type, 0);
        list_para->rat.handle = rat_open_ex(list_para->root_type, list_para->media_type, 0);

        if (list_para->rat.handle == NULL)
        {
            __wrn("open rat failed");
        }
        else
        {
            __msg("open rat successed");
        }
    }
    else
    {
        __wrn("!!!!!!!!!!!!!please dont open rat again!!!!!!!!!!!!!!!!!");
    }

    if (list_para->rat.handle == NULL)
    {
        __wrn("!!!!!!!!!!!!!open rat fail!!!!!!!!!!!!!!!!!");
        return EPDK_FAIL;
    }

    list_para->rat.index = 0;

    //rat_move_cursor_to_first(list_para->rat.handle);
    if (list_para->media_type == RAT_MEDIA_TYPE_ALL)
    {
        list_para->rat.total = list_para->cur_file_list->total;
        __wrn("list_para->rat.total = %d", list_para->rat.total);
    }
    else
    {
        list_para->rat.total = rat_get_cur_scan_cnt(list_para->rat.handle) ;
    }

    //list_para->rat.total = rat_get_total_number(list_para->rat.handle);
    //wrn(" begain rat_start_miniature_decode1");
#if (EXPLORER_SUPPORT_MINIATURE == 1)
    //rat_start_miniature_decode(); //开始缩略图线程
#endif
    return EPDK_OK;
}
/*
******************************************************************************************
*Function   :      static __s32 explorer_rat_uninit(explr_list_para_t *list_para)
*
*Description    :    释放rat
*
*Arguments      :
*
*Return         :
*
********************************************************************************************
*/

static __s32 explorer_rat_uninit(explr_list_para_t *list_para)
{
    if (list_para->rat.handle != NULL)
    {
        //rat_close(list_para->rat.handle);
    }

    //rat_stop_miniature_decode();
    return EPDK_OK;
}

static __s32 explorer_get_last_para(explr_list_para_t *list_para)
{
    __u32 page_item_cnt = 0;                //每页的条目总数
    char FileName[RAT_MAX_FULL_PATH_LEN] = {0};
    int ret = 0;
    reg_root_para_t *root_para = NULL;
    __s32 page_count = 0;
    __inf("Enter %s\r", __FUNCTION__);
    //page_item_cnt = LISTBAR_GetPageItemCount(list_para->listbar_handle);
    page_item_cnt = explorer_get_listbar_pagenum(list_para);    // 获取listbar 每一页有几个条目
    root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);
    page_count = explorer_get_listbar_pagenum(list_para);

    if (list_para->rat.handle != NULL)
    {
        switch (list_para->media_type)
        {
            case RAT_MEDIA_TYPE_ALL:
                list_para->last_start_id = 0;
                break;

            case RAT_MEDIA_TYPE_PIC:

                //list_para->last_start_id = photo_start_id;
                if (list_para->root_type == RAT_TF)
                {
                    //list_para->last_start_id = root_para->last_photo_start_index_sd;
                    list_para->last_focused_id = root_para->last_photo_index_sd;
                    list_para->last_filename = root_para->last_photo_path_sd;
                }
                else if (list_para->root_type == RAT_USB)
                {
                    //list_para->last_start_id = root_para->last_photo_start_index_ud;
                    list_para->last_focused_id = root_para->last_photo_index_ud;
                    list_para->last_filename = root_para->last_photo_path_ud;
                }

                break;

            case RAT_MEDIA_TYPE_AUDIO:
                if (list_para->root_type == RAT_TF)
                {
                    //list_para->last_start_id = root_para->last_music_start_index_sd;
                    list_para->last_focused_id = root_para->last_music_index_sd;
                    list_para->last_filename = root_para->last_music_path_sd;
                }
                else if (list_para->root_type == RAT_USB)
                {
                    //list_para->last_start_id = root_para->last_music_start_index_ud;
                    list_para->last_focused_id = root_para->last_music_index_ud;
                    list_para->last_filename = root_para->last_music_path_ud;
                }

                break;

            case RAT_MEDIA_TYPE_VIDEO:
                if (list_para->root_type == RAT_TF)
                {
                    //list_para->last_start_id = root_para->last_movie_start_index_sd;
                    list_para->last_focused_id = root_para->last_movie_index_sd;
                    list_para->last_filename = root_para->last_movie_path_sd;
                }
                else if (list_para->root_type == RAT_USB)
                {
                    //list_para->last_start_id = root_para->last_movie_start_index_ud;
                    list_para->last_focused_id = root_para->last_movie_index_ud;
                    list_para->last_filename = root_para->last_movie_path_ud;
                }

                break;

            case RAT_MEDIA_TYPE_EBOOK:
                if (list_para->root_type == RAT_TF)
                {
                    //list_para->last_start_id = root_para->last_ebook_start_index_sd;
                    list_para->last_focused_id = root_para->last_ebook_index_sd;
                    list_para->last_filename = root_para->last_ebook_path_sd;
                }
                else if (list_para->root_type == RAT_USB)
                {
                    //list_para->last_start_id = root_para->last_ebook_start_index_ud;
                    list_para->last_focused_id = root_para->last_ebook_index_ud;
                    list_para->last_filename = root_para->last_ebook_path_ud;
                }

                break;

            case RAT_MEDIA_TYPE_GAME:
                if (list_para->root_type == RAT_TF)
                {
                    //list_para->last_start_id = root_para->last_game_start_index_sd;
                    list_para->last_focused_id = root_para->last_game_index_sd;
                    list_para->last_filename = root_para->last_game_path_sd;
                }
                else if (list_para->root_type == RAT_USB)
                {
                    //list_para->last_start_id = root_para->last_game_start_index_ud;
                    list_para->last_focused_id = root_para->last_game_index_ud;
                    list_para->last_filename = root_para->last_game_path_ud;
                }

                break;

            default:
                break;
        }

        list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_count);
        __msg("list_para->last_start_id = %d ", list_para->last_start_id) ;
        __msg("list_para->last_focused_id = %d ", list_para->last_focused_id) ;
        __msg("list_para->last_filename = %s ", list_para->last_filename) ;

        if ((list_para->last_focused_id < list_para->rat.total)
            && (list_para->last_start_id < list_para->rat.total))
        {
            eLIBs_memset(FileName, 0, sizeof(FileName));
            GetListItemFileFullPath(list_para, list_para->last_focused_id, FileName);
            __msg("FileName=%s", FileName);
            ret = eLIBs_strcmp(FileName, list_para->last_filename);

            if (ret == 0)
            {
                if (list_para->last_focused_id < page_item_cnt)
                {
                    list_para->last_start_id = 0 ;
                    list_para->rat.index = list_para->last_focused_id;
                }
                else if (list_para->last_focused_id  >= page_item_cnt)
                {
                    list_para->last_start_id = list_para->last_focused_id - (list_para->last_focused_id % page_item_cnt);
                    list_para->rat.index = list_para->last_focused_id;
                }

                __msg(" list_para->last_focused_id %d", list_para->last_focused_id);
                return EPDK_OK;
            }
        }
    }

    list_para->last_start_id = 0;
    list_para->last_focused_id = 0;
    list_para->rat.index = 0;
    return EPDK_OK;
}


static __s32 explorer_save_last_para(explr_list_para_t *list_para)
{
    __s32 ret = 0;
    reg_app_e AppId = 0;
    char FileName[RAT_MAX_FULL_PATH_LEN];
    __listbar_scene_t ListBarScene = {0, 0};
    reg_root_para_t *root_para = NULL;
    root_para = dsk_reg_get_para_by_app(REG_APP_ROOT);
    //__wait__;
    LISTBAR_GetScene(list_para->listbar_handle, &ListBarScene);
    eLIBs_memset(FileName, 0, sizeof(FileName));

    switch (list_para->media_type)
    {
        case RAT_MEDIA_TYPE_VIDEO:
            AppId = REG_APP_MOVIE;

            //movie_start_id = ListBarScene.start_id;
            if (list_para->root_type == RAT_TF)
            {
                root_para->last_movie_start_index_sd = ListBarScene.start_id;
            }
            else if (list_para->root_type == RAT_USB)
            {
                root_para->last_movie_start_index_ud = ListBarScene.start_id;
            }

            break;

        case RAT_MEDIA_TYPE_PIC:
            AppId = REG_APP_PHOTO;

            if (list_para->root_type == RAT_TF)
            {
                root_para->last_photo_start_index_sd = ListBarScene.start_id;
            }
            else if (list_para->root_type == RAT_USB)
            {
                root_para->last_photo_start_index_ud = ListBarScene.start_id;
            }

            break;

        case RAT_MEDIA_TYPE_AUDIO:
            AppId = REG_APP_MUSIC;

            if (list_para->root_type == RAT_TF)
            {
                root_para->last_music_start_index_sd = ListBarScene.start_id;
            }
            else if (list_para->root_type == RAT_USB)
            {
                root_para->last_music_start_index_ud = ListBarScene.start_id;
            }

            break;

        case RAT_MEDIA_TYPE_EBOOK:
            AppId = REG_APP_EBOOK;

            if (list_para->root_type == RAT_TF)
            {
                root_para->last_ebook_start_index_sd = ListBarScene.start_id;
            }
            else if (list_para->root_type == RAT_USB)
            {
                root_para->last_ebook_start_index_ud = ListBarScene.start_id;
            }

            break;

        case RAT_MEDIA_TYPE_GAME:
            AppId = REG_APP_GAME;

            if (list_para->root_type == RAT_TF)
            {
                root_para->last_game_start_index_sd = ListBarScene.start_id;
            }
            else if (list_para->root_type == RAT_USB)
            {
                root_para->last_game_start_index_ud = ListBarScene.start_id;
            }

            break;

        default:
            break;
    }

    //ret = GetListItemFileFullPath(list_para, ListBarScene.focus_id, FileName);
    ret = GetListItemFileFullPath(list_para,  list_para->rat.index, FileName);
    __msg("explorer_save_last_para filename = %s", FileName);

    if (ret == EPDK_OK)
    {
        dsk_reg_save_cur_play_info(AppId, list_para->rat.index, FileName, list_para->root_type);
        //dsk_reg_save_cur_play_info(AppId, ListBarScene.focus_id, FileName, list_para->root_type);
    }

    return EPDK_OK;
}
static __s32 explorer_listview_create(__gui_msg_t *msg)
{
    explr_list_para_t *list_para = NULL ;
    __s32   ret ;
#if (EXPLORER_SUPPORT_MINIATURE == 1)
    RECT miniature_rect;
#endif
    explorer_viewer_ui_t *ui_param = NULL;
    H_WIN  list_win;
    //..PT_MEMINF;
    list_win = msg->h_deswin;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(list_win);

    if (NULL == list_para)
    {
        __inf("explorer list win get attribute fail ");
        return EPDK_FAIL ;
    }

    list_para->list_win = list_win;     //保存窗口句柄
    list_para->font = SWFFont;
    __wrn("explorer is initializing!");
    /*if(list_para->media_type == RAT_MEDIA_TYPE_ALL)   // 文件管理     //..
    {
        ret  =  explorer_file_list_init ( list_para ) ;
        if( EPDK_FAIL == ret )
        {
            __msg("explorer file list init fail ") ;
            return EPDK_FAIL ;
        }
        //建立一个timer，用于删除" 删除对话框"
        GUI_SetTimer( list_win , ExplorerTimerId , C_EXP_TIMER_INTERVAL , NULL );
    }
    else*/
    {
        /*if( list_para->root_para->return_to_explorer_file_list == 0 ) // 进入usb / sd 选择窗口中
        {
            ret  =  explorer_file_list_init ( list_para ) ;
            if( EPDK_FAIL == ret )
            {
                __msg("explorer file list init fail ") ;
                return EPDK_FAIL ;
            }
        }
        else*/      // 进入文件浏览列表中                         //..
        {
            ret  =  explorer_file_list_init(list_para) ;

            if (EPDK_FAIL == ret)
            {
                __msg("explorer file list init fail ") ;
                return EPDK_FAIL ;
            }

            list_para->root_para->explr_root = list_para->root_type;    //保存盘符
            explorer_rat_init(list_para);

            if (list_para->rat.total <= 0)
            {
                __gui_msg_t my_msg;
                __s32 lang_id[] = {STRING_EXPLR_CUE, STRING_EXPLR_FOLDER_EMPTY };
                lang_id[1] = folder_emtpy[list_para->mediatype_index];
                default_dialog(list_para->h_dialog, GUI_WinGetParent(msg->h_deswin), 0x2,  ADLG_OK, lang_id);
                esKRNL_TimeDly(200);
                app_dialog_destroy(list_para->h_dialog);
                list_para->h_dialog = NULL;
                explorer_list_long_string_stop_roll(list_para);
                eLIBs_memset(&my_msg, 0x00, sizeof(my_msg));
                my_msg.h_deswin = GUI_WinGetParent(msg->h_deswin);
                my_msg.h_srcwin = msg->h_deswin;
                my_msg.id = DSK_MSG_HOME;
                GUI_SendNotifyMessage(&my_msg);
                return EPDK_FAIL;
            }

            explorer_get_last_para(list_para) ;

            if (RAT_MEDIA_TYPE_AUDIO == list_para->media_type)
            {
                explorer_list_draw_bg_music_play_song_index(list_win);
            }
        }
    }
    //explorer_mediatype_listbar_init(list_win); // shiql change this for D100 20160715
    explorer_listbar_init(list_win);
    explorer_list_long_string_init(list_para);
    // 如果背景音乐存在，或者是进入音乐列表，设置一个timer
    /*if( ret = is_app_exist( APP_MUSIC ) , ( EPDK_TRUE == ret )\
        ||(RAT_MEDIA_TYPE_AUDIO == list_para->media_type) )
    {
        GUI_SetTimer( list_win , ExplorerDrawBgMusicIndexTimer , \
            EXP_DRAW_BG_MUSIC_INDEX_INTERVAL , 0 );
    }
    */
#if (EXPLORER_SUPPORT_MINIATURE == 1)
    ret = explorer_get_item_miniature_rect(list_para, &miniature_rect);
    list_para->listbar_miniature_buf = esMEMS_Balloc(miniature_rect.width * miniature_rect.height * 4); //ARGB8888,因此4倍大小
    eLIBs_memset(list_para->listbar_miniature_buf, 0, miniature_rect.width * miniature_rect.height * 4);
    list_para->listbar_miniature_size = miniature_rect.width * miniature_rect.height * 4 ;
    ui_param = explorer_get_viewer_ui_param();
    miniature_rect.x = ui_param->file_type_uiparam.pic_miniature_bmp_rect.x ;
    miniature_rect.y = ui_param->file_type_uiparam.pic_miniature_bmp_rect.y ;
    miniature_rect.width = ui_param->file_type_uiparam.pic_miniature_bmp_rect.width ;
    miniature_rect.height = ui_param->file_type_uiparam.pic_miniature_bmp_rect.height ;
    list_para->big_miniature_buf = esMEMS_Balloc(miniature_rect.width * miniature_rect.height * 4);
    eLIBs_memset(list_para->big_miniature_buf, 0, miniature_rect.width * miniature_rect.height * 4);
    list_para->big_miniature_size = miniature_rect.width * miniature_rect.height * 4 ;
    list_para->get_miniature_sem = esKRNL_SemCreate(1);
    list_para->big_miniature_index = -1 ;
#endif
    return EPDK_OK ;
}

void explorer_list_on_timer(__gui_msg_t *msg)
{
    explr_list_para_t *list_para;
    list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

    if (list_para == NULL)
    {
        return;
    }

    if (ExplorerTimerId == msg->dwAddData1) //only for file manager delete file dialog timer
    {
        if (list_para->del_dlg_open)    // 判断删除对话框是否存在，存在把对话框删除
        {
            list_para->del_dlg_open = EPDK_FALSE;
            //explorer_draw_file_info(list_para);
        }
    }

    if (ExplorerDrawBgMusicIndexTimer == msg->dwAddData1)
    {
        if (list_para->enter_sd_usb_flag == 1)  //进入了文件列表浏览窗口才响应timer
        {
            explorer_list_draw_bg_music_play_song_index(msg->h_deswin);
        }
    }

    return;
}

/*
************************************************************************************
*Function   :          static __s32 _explorer_list_win_cb(__gui_msg_t *msg)
*
*Description    :       ListWindow contrl block
*
*Arguments      :
*
*Return         :
*
*************************************************************************************
*/
static __s32 _explorer_list_win_cb(__gui_msg_t *msg)
{
    __s32 ret = EPDK_OK;
    __gui_msg_t send;
    eLIBs_memset(&send, 0, sizeof(__gui_msg_t));

    switch (msg->id)
    {
        case GUI_MSG_CREATE:
            __msg("----explorer frame window GUI_MSG_CREATE begin----");
            explorer_listview_create(msg);
            return EPDK_OK;

        case GUI_MSG_PAINT:
        {
            explr_list_para_t *list_para = NULL;
            list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);
            __msg("----explorer frame window GUI_MSG_ONPAINT----");

            if (list_para->list_layer && GUI_LyrWinGetSta(list_para->list_layer) == GUI_LYRWIN_STA_SUSPEND)
            {
                return EPDK_FAIL;
            }

            if (list_para->rat.total > 0)
            {
                explorer_paint_media_list(msg->h_deswin);
                explorer_listview_onpaint(msg);
            }

            gscene_hbar_set_state(HBAR_ST_SHOW);        // 显示hbar
            return EPDK_OK;
        }

        case GUI_MSG_CLOSE:
            __msg("----explorer frame window GUI_MSG_CLOSE begin----");
            explorer_list_win_on_close(msg);
            return EPDK_OK;

        case GUI_MSG_DESTROY:
            __msg("----explorer frame window GUI_MSG_DESTROY begin----");
            explorer_list_win_on_destroy(msg);
            __msg("----explorer frame window GUI_MSG_DESTROY end----");
            return EPDK_OK;

        case GUI_MSG_COMMAND:
            //__here__;
            explorer_list_win_on_command(msg);
            return EPDK_OK;

        case GUI_MSG_KEY:
            explorer_list_win_key_proc(msg);
            return EPDK_OK;

        case GUI_MSG_TIMER:
            explorer_list_on_timer(msg);
            return EPDK_OK ;

        case DSK_MSG_FS_PART_PLUGIN:
        {
#if 0
            __msg("explorer list on DSK_MSG_FS_PART_PLUGIN msg begin  ");
            explorer_on_fs_part_in(msg);
            __msg("explorer list on DSK_MSG_FS_PART_PLUGIN msg end  ");
#endif
            return EPDK_OK ;
        }

        case DSK_MSG_FS_PART_PLUGOUT:
        {
            __msg("explorer list on DSK_MSG_FS_PART_PLUGOUT msg begin  ");
            //  explorer_on_fs_part_out(msg);
            __msg("explorer list on DSK_MSG_FS_PART_PLUGOUT msg end  ");
            return EPDK_OK ;
        }

        case DSK_MSG_ALARM: //explorer list 收到这个消息时，只需把当前弹出的窗口删除掉即可
        {
            explr_list_para_t *list_para;
            list_para = (explr_list_para_t *)GUI_WinGetAttr(msg->h_deswin);

            if (list_para->h_dialog != NULL)
            {
                app_dialog_destroy(list_para->h_dialog);        //to delete dialog
                list_para->h_dialog = NULL ;
            }

            if (list_para->h_dialog_msg != NULL)
            {
                app_dialog_destroy(list_para->h_dialog_msg);        //to delete dialog
                list_para->h_dialog_msg = NULL ;
            }

            return EPDK_OK ;
        }

        default:
            ret = GUI_FrmWinDefaultProc(msg);
            break;
    }

    return ret;
}


//释放Listwindow
__s32 explorer_list_win_delete(H_WIN list_win)
{
    GUI_FrmWinDelete(list_win);
    return EPDK_OK;
}


/*
******************************************************************************************
*Function   : void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
*
*Description    :       向父窗口传送命令消息
*
*Arguments      :       hwin, 主窗口句柄
*                   id,     命令参数
*Return         :
*
*******************************************************************************************
*/
void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128] = {0};
    H_WIN hExpWin;
    hExpWin = GUI_WinGetParent(hwin);
    msg.h_deswin = GUI_WinGetParent(hExpWin);
    //msg.h_srcwin = NULL;
    msg.h_srcwin = hwin;
    msg.id = GUI_MSG_COMMAND;
    msg.dwAddData1 = MAKELONG(GUI_WinGetItemId(hExpWin), id);//
    msg.dwAddData2 = data1;
    msg.dwReserved = data2;
    GUI_WinGetName(msg.h_deswin, buf);
    __msg("%s", buf);
    GUI_SendNotifyMessage(&msg);
}

/*
****************************************************************************************
*Function   :  void ExplorerSendMessage(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
*
*Description    :   SendMessage,发送同步消息，改消息回调函数将会被立即执行
*
*Arguments      :   异步消息&NotifyMessage 将会被插入到消息队列中，不会被立即执行
*
*Return         :
*
*****************************************************************************************
*/
void ExplorerSendMessage(H_WIN hwin, __s32 id, __s32 data1, __s32 data2)
{
    __gui_msg_t msg;
    char buf[128] = {0};
    GUI_WinGetName(hwin, buf);
    __msg("%s", buf);
    msg.h_srcwin = NULL;
    msg.h_deswin = hwin;
    msg.id = id;
    msg.dwAddData1 = data1;//
    msg.dwAddData2 = data2;
    GUI_SendMessage(&msg);
    return;
}

/*
*****************************************************************************************
*Function   :          void  ExplorerListWinGetSearchPath(explr_list_para_t *para)
*
*Description    :       根据RootType 获得Search path
*
*Arguments      :
*
*Return         :
*
******************************************************************************************
*/
void  ExplorerListWinGetSearchPath(explr_list_para_t *para)
{
    __s32 ret = 0;
    char root_type[50] = {0};
    char disk_name[RAT_MAX_PARTITION][4] = {{0},};
    eLIBs_memset(para->search_path, 0, sizeof(para->search_path));
    //ret = rat_get_partition_name(RAT_SD_CARD, para->search_path);

    switch (para->root_type)
    {
        case RAT_LOCAL:
            eLIBs_strcpy(root_type, RAT_LOCAL_DISK);
            break;

        case RAT_TF:
            eLIBs_strcpy(root_type, RAT_SD_CARD);
            break;

        case RAT_USB:
            __wrn("GetSearchPath RAT_USB ");
            eLIBs_strcpy(root_type, RAT_USB_DISK);
            break;

        default:
            break;
    }

    __wrn("root_type=%s", root_type);
    ret = rat_get_partition_name(root_type, disk_name, 0);
    eLIBs_strcpy(para->search_path, disk_name[0]);
    __wrn("para->search_path=%s", para->search_path);
    //  eLIBs_memcpy(para->search_path,"f:", sizeof("f:"));
    return ;
}


__s32 explorer_list_long_string_init(explr_list_para_t *this)
{
    if (this->long_str_handle == NULL)
    {
        __show_info_move_t show_info;
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        show_info.hlayer = this->list_layer;
        show_info.alpha_en = EPDK_TRUE;
        show_info.pFont = NULL;
        show_info.string = "";
        //show_info.fontColor = GUI_WHITE;
        show_info.fontColor = GUI_BLACK;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;
        //show_info.bmp = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_FOCUS_BG);
        show_info.bmp = NULL ;
        show_info.bmp_pos.x = 0;
        show_info.bmp_pos.y = 0;
        this->long_str_handle = GUI_LongStringCreate(&show_info);

        if (this->long_str_handle == NULL)
        {
            __wrn("create long string failed!");
        }
    }
    else
    {
        __msg("long string scroll has already started!");
    }

    return EPDK_OK;
}

__s32 explorer_list_long_string_uninit(explr_list_para_t *this)
{
    if (this->long_str_handle != NULL)
    {
        GUI_LongStringDelete(this->long_str_handle);
        this->long_str_handle = NULL;
    }

    return EPDK_OK;
}

__s32 explorer_list_long_string_start_roll_fast(__lbar_draw_para_t *draw_param, char *string)
{
    __show_info_move_t show_info;
    __epdk_charset_enm_e encode = EPDK_CHARSET_ENM_UTF8;
    RECT roll_rect, text_rect;
    explorer_viewer_ui_t *ui_para = NULL;
    explr_list_para_t *this = NULL;
    __u32 font_size = 0;
    this = (explr_list_para_t *)draw_param->attr;

    if (this->long_str_handle != NULL)
    {
        eLIBs_memset(&show_info, 0, sizeof(__show_info_move_t));
        ui_para = explorer_get_viewer_ui_param();
        //roll_rect.x = draw_param->rect.x  + ui_para->list_item_ui_param.text_rect.x;
        //roll_rect.y = draw_param->rect.y  + ui_para->list_item_ui_param.text_rect.y;
        //roll_rect.width = ui_para->list_item_ui_param.text_rect.width+1;
        //roll_rect.height = ui_para->list_item_ui_param.text_rect.height;
        explorer_get_item_text_rect(this, &text_rect);  //获得text rectangle
        roll_rect.x = draw_param->rect.x  + text_rect.x ;
        roll_rect.y = draw_param->rect.y  + text_rect.y ;
        roll_rect.width = text_rect.width ;
        roll_rect.height = text_rect.height ;
        font_size = GUI_GetFontDistY() ;
        show_info.hlayer = this->list_layer;
        show_info.region.x = roll_rect.x;
        show_info.region.y = roll_rect.y;
        show_info.region.width = roll_rect.width - 1 ;
        show_info.region.height = roll_rect.height - 1 ;
        show_info.alpha_en = EPDK_TRUE;
        show_info.pFont = this->font;
        show_info.string = string;
        show_info.encode_id = encode;
        //show_info.fontColor = GUI_WHITE;
        show_info.bkColor = GUI_BLACK;
        show_info.align = GUI_TA_LEFT | GUI_TA_VCENTER;

        if (this->view_mode == EXPLR_LIST_MODE)
        {
            show_info.bmp = explorer_get_listview_icon_res(ID_EXP_LIST_ITEM_ROLL_BG);
            show_info.bmp_pos.x = draw_param->rect.x + ui_para->list_item_ui_param.text_rect.x;// roll_rect.x
            show_info.bmp_pos.y = draw_param->rect.y + ui_para->list_item_ui_param.text_rect.y;//roll_rect.y
        }
        else
        {
            show_info.bmp = NULL;
            show_info.bmp_pos.x = 0;
            show_info.bmp_pos.y = 0;
        }

        GUI_LongStringStop(this->long_str_handle);
        GUI_LongStringReset(this->long_str_handle, &show_info);
        GUI_LongStringStart(this->long_str_handle);
    }

    return EPDK_OK;
}
//
static __s32 explorer_list_long_string_start_roll(explr_list_para_t *this)
{
    if (this->long_str_handle != NULL)
    {
        return GUI_LongStringStart(this->long_str_handle);
    }

    return EPDK_FAIL;
}

static __s32 explorer_list_long_string_stop_roll(explr_list_para_t *this)
{
    if (this->long_str_handle != NULL)
    {
        return GUI_LongStringStop(this->long_str_handle);
    }

    return EPDK_FAIL;
}
