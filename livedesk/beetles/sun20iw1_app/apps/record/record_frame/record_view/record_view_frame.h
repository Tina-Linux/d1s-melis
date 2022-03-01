#ifndef __RECORD_VIEW_FRAME_H__
#define __RECORD_VIEW_FRAME_H__

#include "app_record.h"

#define REC_TIMER_TIMES             50      //0.3s

typedef struct tag_rec_view_frame
{
    H_WIN           layer;
    GUI_FONT        *sys_font;

    __s32           recTimeID;

    precord_view_t  pview;

    precord_doc_t   precordDoc;
} rec_view_frame_t, *prec_view_frame_t;






extern H_WIN rec_view_frm_create(H_WIN h_parent, precord_ctrl_t para);











#endif//__RECORD_VIEW_FRAME_H__
