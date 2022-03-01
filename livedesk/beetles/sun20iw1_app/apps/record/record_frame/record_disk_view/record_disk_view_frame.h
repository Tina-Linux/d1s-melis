#ifndef __RECORD_DISK_VIEW_FRAME_H__
#define __RECORD_DISK_VIEW_FRAME_H__

#include "app_record.h"


typedef struct tag_rec_disk_view_frame
{
    H_WIN           layer;
    GUI_FONT        *sys_font;

    __s32           curRootType;
    __s32           recTimeID;

    precord_ctrl_t  prec_ctrl;

} rec_disk_view_frame_t, *prec_disk_view_frame_t;






extern H_WIN rec_disk_view_frm_create(H_WIN h_parent, precord_ctrl_t para);











#endif//__RECORD_DISK_VIEW_FRAME_H__
