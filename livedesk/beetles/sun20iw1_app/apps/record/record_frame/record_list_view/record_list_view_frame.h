#ifndef __RECORD_LIST_VIEW_FRAME_H__
#define __RECORD_LIST_VIEW_FRAME_H__

#include "app_record.h"


//#define RECORD_LIST_VIEW_FRAME_DELETE 0x10

typedef struct tag_rec_list_view_frame
{
    H_WIN               layer;
    GUI_FONT            *sys_font;

    H_WIN               delDlg;

    __u32               root_type;
    precord_list_view_t plistView;
    precord_list_doc_t  plistDoc;

    __bool              isTouchPlayingBar;
    __bool              isTouchMusicList;
    __bool              isTouchMusicListMove;

} rec_list_view_frame_t, *prec_list_view_frame_t;



extern H_WIN rec_list_view_frm_create(H_WIN h_parent, precord_ctrl_t para);













#endif//__RECORD_LIST_VIEW_FRAME_H__

