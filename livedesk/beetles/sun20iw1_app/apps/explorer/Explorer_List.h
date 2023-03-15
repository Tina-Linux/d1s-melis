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


#ifndef __EXPLORER_LIST_H__
#define __EXPLORER_LIST_H__

#include "FileList.h"
#define   EXPLORER_SUPPORT_MINIATURE    (0)
typedef enum exp_icon_status_s
{
    UNFOCUSED = 0,
    FOCUSED = 1,
    PRESSED = 2,
} exp_icon_status_e;


//定义rat 文件搜索数据结构
typedef struct tag_rat_ctrl
{
    HRAT  handle;               //文件搜索句柄
    __s32 index;                //当前文件索引号
    __s32 total;                //文件总数
    //rat_media_type_t media_type;//搜索文件的类型
} rat_ctrl_t;

typedef struct file_type_res_s
{
    HTHEME  file_type_bg_bmp;
    HTHEME  file_type_big_movie_bmp, file_type_big_music_bmp, file_type_big_photo_bmp, file_type_big_ebook_bmp, file_type_big_game_bmp;
    HTHEME  file_type_ebook_f_bmp, file_type_ebook_uf_bmp;
    HTHEME  file_type_movie_f_bmp, file_type_movie_uf_bmp;
    HTHEME  file_type_music_f_bmp, file_type_music_uf_bmp;
    HTHEME  file_type_photo_f_bmp, file_type_photo_uf_bmp;
    HTHEME  file_type_game_f_bmp, file_type_game_uf_bmp;
    HTHEME  fiile_manager_icon_bmp;             //文件管理图标
    HTHEME  file_manager_big_bmp;           //文件管理大图标
} file_type_res_t;
typedef struct tag_explr_list_para
{
    H_WIN               list_win;           //窗口句柄
    GUI_FONT            *explr_list_font;
    H_LYR               BG_layer;           //背景图层
    H_LYR               list_layer;             //listbar 图层
    void                *core;
    __s32               root_type;
    __u32               last_start_id;      //上次浏览起始ID
    __u32               last_focused_id;    //上次选中文件id
    char                *last_filename;     //上次浏览文件 名

    GUI_FONT            *font;

    char                search_path[RAT_MAX_FULL_PATH_LEN];             //搜索路径
    H_LBAR              listbar_handle;
    H_LBAR              mediatype_listbar_handle;

    __hdle              long_str_handle;        //长字符串滚动句柄

    rat_ctrl_t          rat;                    //文件搜索数据结构
    rat_media_type_t    media_type;             //搜索文件的类型
    explorer_view_mode_e view_mode;             //显示模式,List or square

    file_list_t        *top_file_list;          //top filelist
    file_list_t        *cur_file_list;          //current filelist
    file_item_t        *file_item;              //画icon的file_item;

    H_WIN               h_dialog;
    H_WIN               h_dialog_msg;
    __bool              del_dlg_open;           //删除对话框是否open
    H_WIN               h_fw_update_dlg;        //固約蒳级对话框

    char                fw_update_file[RAT_MAX_FULL_PATH_LEN];
    root_para_t         *root_para  ;       //记录从root 传进来的参数
    file_type_res_t     h_file_type_res;
    HTHEME              h_item_fcs_bmp, h_item_unfcs_bmp, h_bg_bmp, h_return_bmp;
    __bool              enter_sd_usb_flag ; // 1 表示已经进入usb / sd 路径了
#if (EXPLORER_SUPPORT_MINIATURE == 1)
    void               *listbar_miniature_buf ; //每个item 显示的buf
    __u32               listbar_miniature_size ;    // buffer 大小
    void               *big_miniature_buf ;     // 左侧文件中显示的大图片的buf
    __u32               big_miniature_size ;
    __s32               big_miniature_index ;
    __u8                draw_big_miniature_thread ; //画大缩略图线程
    __krnl_event_t     *willow_miniature_sem ;  // 获取大小缩略图时使用的信号量
    __krnl_event_t     *get_miniature_sem ;     //获取缩略图时的信号量
    __u32               draw_miniature_delay_time ;
#endif
    __s32               last_focus_index;//从0开始算
    __s32               mediatype_index;
    __s32               cp_mov_dialog;
    __s32               listbar_left_right;//按键的上下操作对应的是左边还是右边的listbar
    char                copy_name_back[RAT_MAX_FULL_PATH_LEN];//复制文件名，暂时保存
    __bool              filelist_update;

    __u8                handle_thread_id;

} explr_list_para_t;



#define EXPLR_LIST_ENTER            0
#define EXPLR_LIST_RETURN           1

#define MSG_EXPLR_UPDATE            (GUI_MSG_USER_DEF+0)


H_WIN explorer_list_win_create(H_WIN h_parent, explr_list_para_t *para);
__s32 explorer_list_win_delete(H_WIN list_win);
extern __s32  explorer_get_disk_total_num(root_para_t  *para)  ;
extern __s32  explorer_check_disk_exist(__s32 root_type);
extern __s32 GetListItemFileFullPath(explr_list_para_t *list_para, __s32 ItemIndex, char *FilePath);
void  explorer_cmd2parent(H_WIN hwin, __s32 id, __s32 data1, __s32 data2);
#if (EXPLORER_SUPPORT_MINIATURE == 1)
extern __s32 explorer_draw_pic_miniature_bmp(explr_list_para_t *list_para, __u32 item_index);
#endif
extern __s32 rat_get_pic_info_ex(rat_miniature_para_t *para, rat_pic_info_t *pic_info, explr_list_para_t *list_para);

#endif







