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



#ifndef __EXPLORER_UI_H
#define __EXPLORER_UI_H

#include "beetles_app.h"
#include "Explorer.h"
#include "Explorer_List.h"


typedef enum explr_list_icon_id_s           //
{
    ID_EXP_PALETTE = 0,
    ID_EXP_SD_F,
    ID_EXP_SD_UF,
    ID_EXP_USB_F,
    ID_EXP_USB_UF,

    ID_EXP_CP_MOV,
    ID_EXP_CP_MOV_F,

    ID_EXP_BACKGROUND,

    ID_EXP_USB_ICON_FOCUSED,

    ID_EXP_SD_ICON_FOCUSED,

    ID_EXP_UNKNOWN_FILE,
    ID_EXP_UNKNOWN_FILE_ICON_UNFOCUSED,
    ID_EXP_UNKNOWN_FILE_ICON_FOCUSED,

    ID_EXP_ALL_FILE,
    ID_EXP_ALL_ICON_UNFOCUSED,
    ID_EXP_ALL_ICON_FOCUSED,


    ID_EXP_VIDEO_FILE,
    ID_EXP_VIDEO_ICON_UNFOCUSED,
    ID_EXP_VIDEO_ICON_FOCUSED,

    ID_EXP_MUSIC_FILE,
    ID_EXP_MUSIC_ICON_UNFOCUSED,
    ID_EXP_MUSIC_ICON_FOCUSED,

    ID_EXP_PHOTO_FILE,
    ID_EXP_PHOTO_ICON_UNFOCUSED,
    ID_EXP_PHOTO_ICON_FOCUSED,

    ID_EXP_EBOOK_FILE,
    ID_EXP_EBOOK_ICON_UNFOCUSED,
    ID_EXP_EBOOK_ICON_FOCUSED,

    ID_EXP_GAME_FILE,
    ID_EXP_GAME_ICON_UNFOCUSED,
    ID_EXP_GAME_ICON_FOCUSED,

    ID_EXP_LIST_ITEM_FOCUS_BG,
    ID_EXP_LIST_ITEM_UNFOCUS_BG,
    ID_EXP_LIST_ITEM_ROLL_BG,

    ID_EXP_MEDIATYPE_LIST_ITEM_FOCUS_BG,
    ID_EXP_MEDIATYPE_LIST_ITEM_UNFOCUS_BG,

    ID_EXP_SQUARE_FOCUS,

    ID_EXP_LIST_SCROLL_BG,
    ID_EXP_LIST_SCROLL_HEAD,
    ID_EXP_LIST_SCROLL_BODY,
    ID_EXP_LIST_SCROLL_TAIL,

    ID_EXP_FILE_TYPE_FOCUS_ICON_MOVIE,      //窗口左侧，显示文件类型的图标命名为:LEFT_LIST_MUSIC_F.png
    ID_EXP_FILE_TYPE_UNFOCUS_ICON_MOVIE,        //窗口左侧，显示文件类型的图标命名为:LEFT_LIST_MUSIC_F.png
    ID_EXP_FILE_TYPE_FOCUS_ICON_MUSIC,
    ID_EXP_FILE_TYPE_UNFOCUS_ICON_MUSIC,
    ID_EXP_FILE_TYPE_FOCUS_ICON_PHOTO,
    ID_EXP_FILE_TYPE_UNFOCUS_ICON_PHOTO,
    ID_EXP_FILE_TYPE_FOCUS_ICON_EBOOK,
    ID_EXP_FILE_TYPE_UNFOCUS_ICON_EBOOK,
    ID_EXP_FILE_TYPE_FOCUS_ICON_GAME,
    ID_EXP_FILE_TYPE_UNFOCUS_ICON_GAME,
    ID_EXP_FILE_TYPE_BG,

    ID_EXP_LISTBAR_MEDIA_TYPE_MOVIE,
    ID_EXP_LISTBAR_MEDIA_TYPE_MUSIC,
    ID_EXP_LISTBAR_MEDIA_TYPE_PHOTO,
    ID_EXP_LISTBAR_MEDIA_TYPE_EBOOK,
    ID_EXP_LISTBAR_MEDIA_TYPE_GAME,
    ID_EXP_LISTBAR_FOLDER_ICON,

    ID_FILE_MAGAGER_BIG_ICON,
    ID_FILE_MAGAGER_LITTLE_ICON,
    ID_EXP_RETURN_FOCUS,
    ID_EXP_RETURN_UNFOCUS,
    ID_EXP_DELETE_FILE_UF_ICON,
    ID_EXP_DELETE_FILE_F_ICON,
    ID_EXP_INFO_BG,
    ID_EXP_ERROR_BMP,
    BROWSER_ICON_MAX,
} explr_list_icon_id_e;                         //e:代表enum,  s:代表struct

typedef struct explorer_layer_ui_param_s
{
    __pixel_rgbfmt_t fb_fmt;        //图层格式
    SIZE fb_size;                   //framebuffer大小
    RECT src_rect;                  //srcwin区域
    RECT scn_rect;                  //scnwin区域
    __u32 reserved[32];             //预留
} explorer_layer_ui_param_t;


//ID_MEDIATYPE_MAX 确定左边listbar的条目
enum
{
    ID_MEDIATYPE_ALL_ITEM = 0,
    ID_MEDIATYPE_MUSIC_ITEM,
    ID_MEDIATYPE_MOVIE_ITEM,
    ID_MEDIATYPE_EBOOK_ITEM,
    ID_MEDIATYPE_PHOTO_ITEM,
#if GAME_SW
    ID_MEDIATYPE_GAME_ITEM,
#endif
    ID_MEDIATYPE_MAX,

};



typedef struct explorer_mediatype_list_item_ui_param_s
{
    RECT ListBarArea;
    RECT item_rect;
    RECT icon_rect;
    RECT text_rect;
} explorer_mediatype_list_item_ui_param_t;


typedef struct explorer_list_item_ui_param_s
{
    RECT ListBarArea;                       //list display area in list mode
    RECT item_rect;                                                     //list item  UI 参数

    RECT item_top_line;
    RECT item_bottom_line;
    RECT item_dividing_line;

    RECT icon_rect;
    RECT miniature_rect;

    RECT text_rect;

    RECT focus_icon_rect;
    RECT media_type_rect;           //媒体类型图标

    RECT title_rect;
    RECT file_size_rect;            //文件大小
    RECT time_rect;
    RECT number_rect;
    RECT file_info_area_rect ;      //文件信息区域
} explorer_list_item_ui_param_t;

typedef struct explorer_square_item_ui_param_s
{
    RECT ListBarArea;                       //list display area in square mode
    RECT item_rect;                         //条目框
    RECT focus_rect;                        //焦点框
    RECT icon_rect;                         //图标
    RECT miniature_rect;                    //缩略图宽高
    RECT title_rect;
} explorer_square_item_ui_param_t;


typedef struct explorer_icon_data_s
{
    __u32 res_id;
    HTHEME res_handle;
    void *res_buffer;
} explorer_icon_data_t;         //icon data struct

typedef enum
{
    FILE_TYPE_MUSIC = 0,
    FILE_TYPE_MOVIE,
    FILE_TYPE_PHOTO,
    FILE_TYPE_EBOOK,
    FILE_TYPE_GAME,

    FILE_TYPE_MAX,
} file_type_e ;

typedef struct file_typ_s
{
    RECT  file_type_music_rect ;
    RECT  file_type_movie_rect ;
    RECT  file_type_photo_rect ;
    RECT  file_type_ebook_rect ;
    RECT  file_type_game_rect ;
    RECT  file_type_bg_rect ;
    RECT  file_type_big_bmp_rect;   //相对于file_type_bg_rect 而言
    RECT  return_bmp_rect;      //相对于file_type_bg_rect 而言
} file_type_pos_t ;
typedef struct explorer_left_file_type_s   //左侧显示movie,music,photo,ebook等媒体类型图标
{
    RECT     file_type_rect ;       //左侧显示文件类型的整个方框
    file_type_pos_t    file_type_pos[FILE_TYPE_MAX];

    RECT    file_manage_icon_rect;  //文件管理图标
    RECT    file_type_txt_rect ;            //显示文件类型字符
    RECT    delete_file_icon_rect ;     //删除文件按钮位置
    RECT    return_bmp_rect;    //记录返去图标坐标位置
    RECT    pic_miniature_bmp_rect ;    //图片对应的缩略图坐标
} explorer_left_file_type_t;

typedef struct explorer_viewer_ui_s
{
    explorer_layer_ui_param_t layer;    //layer window

    RECT listview;                      //menuview(frame window)
    RECT listview_bg_rect;
    RECT listview_bg_top_rect;
    RECT listview_bg_bottom_rect;
    RECT listview_bg_left_rect;

    RECT scroll_bg_rect;
    RECT scroll_rect;
    __u32 scroll_head_height;
    __u32 scroll_body_height;
    __s32 scroll_tail_height;

    RECT sd_coor;
    RECT usb_coor;
    RECT cp_mov_dialog_coor;

    RECT ListBarArea;      //listbar content

    explorer_mediatype_list_item_ui_param_t         mediatype_list_item_ui_param;
    explorer_list_item_ui_param_t       list_item_ui_param;                         //UI in list mode
    explorer_square_item_ui_param_t     square_item_ui_param;               //UI in square mode

    explorer_icon_data_t                    icon_table[BROWSER_ICON_MAX + 1];

    explorer_left_file_type_t            file_type_uiparam;
} explorer_viewer_ui_t;



//获得整个explorer AP设计的UI参数
extern explorer_viewer_ui_t *explorer_get_viewer_ui_param(void);

extern RECT *explorer_get_listview_ui_param(void);

//获得list 中 item 设计的UI参数
extern explorer_list_item_ui_param_t *explorer_get_list_item_ui_param(void);


//获得list 中 square 设计的UI参数
extern explorer_square_item_ui_param_t *explorer_get_square_item_ui_param(void);


extern void *explorer_get_listview_icon_res(__u32 index);
extern __s32 explorer_free_listview_icon_res(void);


//获得Explorer Layer rectangle
__s32 explorer_get_screen_rect(RECT *screen_rect);

//显示title字符串
extern __s32 explorer_draw_file_manager_title(void);
//获得缩略图参数(according to the viewmode of list)
#if (EXPLORER_SUPPORT_MINIATURE == 1)
extern __s32 explorer_get_item_miniature_rect(explr_list_para_t *para, RECT *miniature_rect);
#endif
extern __s32 explorer_get_item_text_rect(explr_list_para_t *para, RECT *text_rect);
extern __s32 explorer_get_item_icon_rect(explr_list_para_t *para, RECT *icon_rect);
extern __s32 explorer_get_item_focus_icon_rect(explr_list_para_t *para, RECT *focus_icon_rect);

extern __s32 explorer_clear_filesize_area(explr_list_para_t *list_para);
extern __s32 explorer_draw_FileSize(__lbar_draw_para_t *draw_param, __s32 file_size, explr_list_para_t *list_para);
extern void explorer_clean_delete_file_hint_area(void);
extern void  explorer_delete_file_draw_process(file_op_info_t *op_info);
extern void  explorer_delete_file_draw_process_ext(explr_list_para_t *list_para);
extern void  explorer_draw_delete_file_failed(void);

extern __s32 explorer_clear_file_create_time_area(explr_list_para_t *list_para);
extern __s32 explorer_draw_file_create_time(__lbar_draw_para_t *draw_param, char *file_name, explr_list_para_t *list_para);
extern __s32 explorer_draw_background(explr_list_para_t *list_para);
extern __s32 explorer_draw_FileTypeIcon(explr_list_para_t *list_para, rat_media_type_t media_type);
extern void *explorer_get_file_item_icon(explorer_view_mode_e view_mode, rat_media_type_t media_type, exp_icon_status_e status);

extern __u32 explorer_get_listbar_pagenum(explr_list_para_t *list_para);
#if (EXPLORER_SUPPORT_MINIATURE == 1)
extern __s32 explorer_draw_pic_miniature_bmp(explr_list_para_t *list_para, __u32 item_index);
#endif
extern __s32 explorer_get_mediatype_item_icon_rect(explr_list_para_t *para, RECT *icon_rect);

extern void *explorer_get_file_mediatype_item_icon(__lbar_draw_para_t *draw_param, exp_icon_status_e status);
__s32 explorer_get_mediatype_item_text_rect(explr_list_para_t *para, RECT *text_rect);

extern __s32 CFG_get_game_and_dict_flag(__s32 id);
__s32 mtime_to_date_string(ES_TIMESTAMP *m_time, char *string);


#endif


