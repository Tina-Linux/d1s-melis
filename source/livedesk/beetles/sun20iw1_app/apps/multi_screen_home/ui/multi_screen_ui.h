#ifndef __MULTI_SCREEN_UI_H__
#define __MULTI_SCREEN_UI_H__

#include "beetles_app.h"
/*home icon*/
enum
{
    //固定应用
    HOME_UNFOCUS_ICON_START = 0,

    HOME_DTV_UNFOCUS_ICON = HOME_UNFOCUS_ICON_START,
    HOME_MOVIE_UNFOCUS_ICON,
    HOME_MUSIC_UNFOCUS_ICON,
    HOME_PICTURE_UNFOCUS_ICON,
    HOME_EBOOK_UNFOCUS_ICON,
    HOME_FM_UNFOCUS_ICON,
    HOME_RECORD_UNFOCUS_ICON,
    HOME_SETTING_UNFOCUS_ICON,
    HOME_UNFOCUS_ICON_NUM
};
enum
{
    HOME_FOCUS_ICON_START = 0,
    HOME_DTV_FOCUS_ICON = HOME_FOCUS_ICON_START,
    HOME_MOVIE_FOCUS_ICON,
    HOME_MUSIC_FOCUS_ICON,
    HOME_PICTURE_FOCUS_ICON,
    HOME_EBOOK_FOCUS_ICON,
    HOME_FM_FOCUS_ICON,
    HOME_RECORD_FOCUS_ICON,
    HOME_SETTING_FOCUS_ICON,
    HOME_FOCUS_ICON_NUM
};

/*home string*/
enum
{
    //固定应用
    HOME_STRING_START = 0,

    HOME_DTV_STRING = HOME_STRING_START,
    HOME_MOVIE_STRING,
    HOME_MUSIC_STRING,
    HOME_PICTURE_STRING,
    HOME_EBOOK_STRING,
    HOME_NEW_FM_STRING,
    HOME_RECORD_STRING,
    HOME_SETTING_STRING,

    MULTI_SCREEN_STRING_MAX
};

typedef struct tag_multi_screen_ui
{
    char    lang[MULTI_SCREEN_STRING_MAX][GUI_NAME_MAX];
    HTHEME  bmp_uf[HOME_UNFOCUS_ICON_NUM];
    HTHEME  bmp_f[HOME_FOCUS_ICON_NUM];
} multi_screen_ui_t, *pmulti_screen_ui_t;


extern __s32 init_multi_screen_res(pmulti_screen_ui_t pui, __s16 current_focus);
extern __s32 uninit_multi_screen_res(pmulti_screen_ui_t pui);
extern void draw_multi_screen_icon_coor(pmulti_screen_ui_t pui, __s32 index, __s32 X, __s32 Y);
extern void draw_multi_screen_icon_coor_rect(pmulti_screen_ui_t pui, __s32 index, __s32 bIs_focus, GUI_RECT *rect);
extern void draw_multi_screen_string_center(pmulti_screen_ui_t pui, __s32 index, __s32 fnt_color, GUI_RECT *rect);
//extern void draw_multi_screen_string_bottom( pmulti_screen_ui_t pui, __s32 index, __s32 fnt_color, GUI_RECT* rect );

#endif//__MULTI_SCREEN_UI_H__
