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
#ifndef __MUSIC_LIST_H__
#define __MUSIC_LIST_H__

#include "beetles_app.h"

typedef struct music_list_para_s
{
    H_WIN   framewin;
    H_LYR   layer;
    __bool  visiable;
    GUI_FONT *font;

    __s32   root_type;

    rat_media_type_t    media_type;

    __s32  explorer_play_music;
    char  *music_file_name;
} music_list_para_t;

typedef struct music_list_ctrl_s
{
    H_WIN   framewin;
    H_LYR   layer;
    __bool  visiable;

    __bool   lost_focus; //add by lee ;
    __s32   root_type;
    __u32   last_start_id;
    __u32   last_focused_id;
    char    *last_filename;

    char    search_path[RAT_MAX_FULL_PATH_LEN];
    rat_media_type_t    media_type;


    RECT        area;
    GUI_FONT    *font;

    H_LBAR      listbar_handle;
    HRAT        rat_handle;
    __hdle      long_str_handle;
    __u32       total;
    __u32       play_index;
    __listbar_scene_t   listbar_play_scene;

    __s32(*OnDraw)(void *list_ctrl);
    __s32(*OnNextKey)(void *list_ctrl);
    __s32(*OnPreviousKey)(void *list_ctrl);
    __s32(*OnNextPageKey)(void *list_ctrl);
    __s32(*OnPreviousPageKey)(void *list_ctrl);
    __s32(*OnEnterKey)(void *list_ctrl);

    __s32(*OnTouchDown)(void *list_ctrl, __s32 x, __s32 y, __s32 direct, __s32 speed);
    __s32(*OnTouchMove)(void *list_ctrl, __s32 x, __s32 y, __s32 direct, __s32 speed);
    __s32(*OnTouchUp)(void *list_ctrl, __s32 x, __s32 y, __s32 direct, __s32 speed);
} music_list_ctrl_t;



#define     C_SEARCH_FILE_CNT_AT_FIRST          0

extern void clear_music_list_area(void);
extern __s32 init_bg_music_list(music_list_ctrl_t *list_ctrl, music_list_para_t *list_para);
extern music_list_ctrl_t *new_music_list(music_list_para_t *list_para);
extern __s32 delete_music_list(music_list_ctrl_t *list_ctrl);
extern __s32 music_listbar_restore(music_list_ctrl_t *list_ctrl);

extern __s32 show_music_list(music_list_ctrl_t *list_ctrl);
extern __s32 music_list_set_focus_item(music_list_ctrl_t *list_ctrl, __u32 item_num);

extern __s32 MusicGetListItemFileFullPath(HRAT rat_handle, __s32 ItemIndex, char *FilePath);


extern __s32 get_music_list_focus_item_full_filename(music_list_ctrl_t *list_ctrl, char *filename);
extern __s32 music_list_long_string_stop_roll(music_list_ctrl_t *this);

extern __s32 music_list_long_string_stop(music_list_ctrl_t *this);


__s32 music_list_set_item_scene(music_list_ctrl_t *this, __listbar_scene_t *scene);
__s32 music_list_get_item_scene(music_list_ctrl_t *this, __listbar_scene_t *scene);

__s32 MusicGetListItemFileFullPath(HRAT rat_handle, __s32 ItemIndex, char *FilePath);

__s32  music_rat_get_search_path(music_list_ctrl_t *list_ctrl);
__s32 music_list_long_string_start_roll(music_list_ctrl_t *this);


#endif



