#ifndef __EXPLORE_DIALOG_FRAME_H__
#define __EXPLORE_DIALOG_FRAME_H__

#include "Explorer.h"

#include "Explorer_UI.h"


typedef struct explr_dialog_para_s
{
    H_LYR dialog_layer;
    H_WIN h_dialog_framewin;

    H_WIN h_dialog_yesno;

    __s32 focus_id;

    GUI_FONT *font;
} explr_dialog_para_t;



H_WIN explorer_dialog_frame_win_create(H_WIN h_parent, explr_dialog_para_t *para) ;






#endif

