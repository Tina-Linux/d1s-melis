#ifndef __RECORD_LIST_VIEW_H__
#define __RECORD_LIST_VIEW_H__

#include "record_ui.h"
#include "record_list_view_doc.h"


typedef enum RECORD_LIST_FOCUSE
{
    RECORD_MUSIC_LIST,
    RECORD_MUSIC_MENU,
    RECORD_MUSIC_DELETE,
    RECORD_MUSIC_VOLUME

} RECORD_LIST_FOCUSE;

typedef enum RECORD_DELETE_STATE
{
    RECORD_DEL_NONE,
    RECORD_DEL_OK,
    RECORD_DEL_CAL
} RECORD_DELETE_STATE;

/*typedef struct tag_recordlist_theme_res_s//....
{
    __s32   res_id;
    HTHEME  h_bmp_handle;
    void*   res_buffer;

}recordlist_theme_res_t;
typedef enum tag_recordlist_nbmp_theme_res_s//..
{
     NMENU_BACK,
     NMENU_PRE,
     NMENU_PLAY,
     NMENU_PAUSE,
     NMENU_NEXT,
     NMENU_DEL,
     NMENU_MAX,
}recordlist_NBMP_theme_res_e;

typedef enum tag_recordlist_nbmp_theme_res_s//..
{
     FMENU_BACK,
     FMENU_PRE,
     FMENU_PLAY,
     FMENU_PAUSE,
     FMENU_NEXT,
     FMENU_DEL,
     FMENU_MAX,
}recordlist_FBMP_theme_res_e;


typedef struct tag_recordlist_menu_ctrl//..
{

  recordlist_theme_res_t   recordlist_nmenu[NMENU_MAX];
  recordlist_theme_res_t   recordlist_fmenu[FMENU_MAX];
}recordlist_menu_ctrl_t;*/


typedef struct tag_record_list_view
{
    precord_ui_t        pui;

    GUI_FONT            *sys_font;
    H_WIN               layer;

    MUSIC_LIST_MENU     curMenu;
    MUSIC_LIST_MENU     playMenu;
    FOCUSE_STATE        curMenuSt;

    RECORD_LIST_FOCUSE  oldPart;
    RECORD_LIST_FOCUSE  curPart;
    __u32               timeCnt;

    RECORD_DELETE_STATE delState;

    precord_list_doc_t  precord_list_doc;

    H_LBAR      listbar_handle;

    __hdle      long_str_handle;
    __show_info_move_t  show_info;
    //..recordlist_menu_ctrl_t   rlmenu_ctrl;//..

} record_list_view_t, *precord_list_view_t;





extern __s32 record_list_view_musicList_init(precord_list_view_t prec_list_view, H_WIN framewin);

extern __s32 record_list_view_musicList_UnInit(precord_list_view_t prec_list_view);

extern void record_list_view_start_show(precord_list_view_t prec_list_view, H_WIN layer);
extern void record_list_view_show(precord_list_view_t prec_list_view, H_WIN layer);
extern void record_list_icon_view_show(precord_list_view_t prec_list_view, H_WIN layer);


















#endif//__RECORD_LIST_VIEW_H__

