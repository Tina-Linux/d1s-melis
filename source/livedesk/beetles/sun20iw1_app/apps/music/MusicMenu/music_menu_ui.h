

#include "beetles_app.h"

/*typedef enum DIGITAL_SONG_BUTTON
{
    DIGITAL_SONG_CONFIRM = 0,
    DIGITAL_SONG_RETURN ,
    DIGITAL_SONG_NEXTPAGE,
    DIGITAL_SONG_PREPAGE,

    DIGITAL_SONG_BUTTON_MAX
}DIGITAL_SONG_BUTTON_BMP;


typedef enum DIGITAL_SONG_NUM_ICON
{
    DIGITAL_SONG_NUM0 = 0,
    DIGITAL_SONG_NUM1,
    DIGITAL_SONG_NUM2,
    DIGITAL_SONG_NUM3,
    DIGITAL_SONG_NUM4,
    DIGITAL_SONG_NUM5,
    DIGITAL_SONG_NUM6,
    DIGITAL_SONG_NUM7,
    DIGITAL_SONG_NUM8,
    DIGITAL_SONG_NUM9,

    DIGITAL_SONG_NUM_MAX,

}DIGITAL_SONG_NUM_BMP;

typedef enum DIGITAL_SONG_OTHER
{
    DIGITAL_SONG_INPUT = 0,
    DIGITAL_SONG_DEL,
    DIGITAL_SONG_OTHER_MAX,

}DIGITAL_SONG_OTHER_BMP;


typedef enum FOCUSE_STATE
{
    FOCUSE_NORMAL,
    FOCUSE_OVER,
    FOCUSE_PRESS,
    FOCUSE_UP,

    FOCUSE_STATE_MAX
}FOCUSE_STATE;


typedef struct tag_res_id_info
{
    __s32   lang;
    __s32   unfocus;
    __s32   focus;
}res_info_t, *pres_info_t;

typedef struct tag_res_button_bmp_info
{
    HTHEME  unfocus;
    HTHEME  focus;
}res_bmp_info_t, *pres_bmp_info_t;


typedef struct tag_digital_song_ui
{
    char                lang[DIGITAL_SONG_BUTTON_MAX][GUI_NAME_MAX];
    res_bmp_info_t      btn_bmp[DIGITAL_SONG_BUTTON_MAX];
    res_bmp_info_t      num_bmp[DIGITAL_SONG_NUM_MAX];
    res_bmp_info_t      other_bmp[DIGITAL_SONG_OTHER_MAX];
    HTHEME              h_frm_bg;
    HTHEME              h_listbar_bg;
    HTHEME              h_listbar_unfcs;
    HTHEME              h_listbar_fcs;
}digital_song_ui_t, *pdigital_song_ui_t;

extern __s32 init_digital_song_res( pdigital_song_ui_t pui );
extern __s32 uninit_digital_song_res( pdigital_song_ui_t pui );
void draw_digital_song_btn ( pdigital_song_ui_t pui , __s32 index, FOCUSE_STATE focus );
void draw_digital_song_num ( pdigital_song_ui_t pui, __s32 index, FOCUSE_STATE focus );
void draw_digital_input_icon ( pdigital_song_ui_t pui );
void draw_digital_input_del ( pdigital_song_ui_t pui, __s32 index, FOCUSE_STATE focus );*/
//#endif//__MULTI_SCREEN_UI_H__
