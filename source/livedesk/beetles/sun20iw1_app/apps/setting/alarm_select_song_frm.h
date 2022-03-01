
#ifndef __ALARM_SELECT_SONG_FRM_H__
#define __ALARM_SELECT_SONG_FRM_H__

#include "app_setting_i.h"

typedef enum
{
    BUTTON_FCS_OK = 0,
    BUTTON_FCS_LISTEN,
    BUTTON_FCS_CANCLE,

    BUTTON_FCS_MAX

} button_fcs_e;

typedef struct tag_rat_info
{
    HRAT  handle;               //文件搜索句柄
    __s32 index;                //当前文件索引号
    __s32 total;                //文件总数
} rat_info_t;

typedef struct alarm_sel_song_s
{
    H_LYR layer ;
    GUI_FONT *font ;
    H_LBAR              listbar_handle;
    __hdle              long_str_handle;        //长字符串滚动句柄
    rat_info_t          rat;                    //文件搜索数据结构
    char                search_path[RAT_MAX_FULL_PATH_LEN];             //搜索路径
    rat_media_type_t    media_type;             //搜索文件的类型
    rat_type_t          root_type ;             // rat_tf , rat_usb
    HTHEME              h_button_fc_bmp, h_button_uf_bmp, h_list_item_bg ;
    button_fcs_e        button_fcs_id ;         //当前焦点为确定，还是取消
    char                FullFileName[RAT_MAX_FULL_PATH_LEN];        //返回给父窗口的此次选择的音乐名
    __bool              is_in_sel_disk_scene;       // 1 表示在磁盘选择场景
    __u32               current_sel_disk_index ;    //当前选择的磁盘索引
    __bool              is_listening_music;         // 是否是在试听铃声
    __bool              bg_music_is_exist;          // 1 表示背景音乐存在
    rat_play_mode_e     bg_music_play_mode;         // 有背景音乐时，背景音乐的播放模式
    __u32               current_npl_index ;         // 背景音存在时，当前播放的音乐索引
} alarm_sel_song_t ;


extern H_WIN app_alarm_sel_song_create(H_WIN hparent, __u16 win_id);
extern __s32  app_alarm_sel_song_delete(H_WIN win);
extern rat_type_t  app_alarm_sel_song_get_root_type(__u32 *index);

#endif


